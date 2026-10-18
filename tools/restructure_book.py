#!/usr/bin/env python3
"""
restructure_book_eco.py - tidy the annotated games of an Arasan-style opening
book PGN, one ECO code at a time.

    pip install chess
    python restructure_book_eco.py basic.pgn                       # every ECO code
    python restructure_book_eco.py basic.pgn --eco C54             # just C54
    python restructure_book_eco.py basic.pgn --eco C54 C55,C56     # several
    python restructure_book_eco.py basic.pgn -o tidy.pgn

For each ECO code the games with that code are treated as one group:

1. They are merged into one position graph keyed by Zobrist hash, so
   transpositions are found, not just identical move sequences.
2. Every position is annotated in exactly ONE place: the first place it occurs
   in the original text.  A later move order that reaches an already-annotated
   position is terminated there (just the arriving move, with its own move-level
   marks); its continuation, comments and evaluation are merged into the
   earlier position.  Exact repeats (same position + same move) collapse.
3. Quoted text symbols such as  {'!'}  {'!?'}  are converted to NAGs.
4. A group that would be longer than --max-nodes positions is split into
   several games (never more than --max-games per group).  A split-out game
   replays the path from move 1 as bare moves; all marks for that path stay
   where the position is expanded.
5. The result is re-parsed and compared with the input.

Conflicting NAGs
----------------
When the same move (or position) carries different NAGs in different places,
a Stockfish search decides (Stockfish must be on the PATH, or give --sf-path):
  * position evaluations ($10-$21): the search score of the position becomes
    $11 (=), $14/$15 (+/= , =/+), $16/$17 (+/-, -/+) or $18/$19 (+-, -+);
  * move quality ($1-$6), by these rules (! !? !! are positive, ?! ? ?? negative):
      - !! or ?? anywhere for the position is retained, with no search, unless
        another NAG of the opposite sign is also present (!! with ?! ? ??, or
        ?? with ! !? !!): that case is mixed, see below;
      - !? and !: search whether ! is warranted (the played move is the best by
        at least 0.75 over the next, never a capture); if so !, otherwise !? is
        retained - unless the engine finds the move bad (loses >= 0.30), then
        the engine's own NAG (?!, ?, ??) is used;
      - ?! and ?: search whether ? is warranted (loses >= 0.75); if so ?,
        otherwise ?! is retained - unless the engine finds the move good (! or
        !!), then that NAG is used;
      - positive and negative NAGs mixed (e.g. !? and ?, !! and ?, ?? and !?):
        the search verdict is used as is; with no verdict, --neutral decides
        (default: no mark).
    Engine verdict: loss against the best move of >= 0.30 / 0.75 / 1.50 gives
    ?! / ? / ??; best move by >= 0.75 / 3.00 over the next gives ! / !!.
Defaults: depth 30, 4096 MB hash.  Searches are cached in <input>.sfcache.json
so a re-run does not repeat them.  --no-engine skips the search (first kept).
{weight:N} conflicts are not searched (first kept).

Safeguards
----------
* ChessBase [%evp ...] evaluation-profile comments are removed everywhere:
  each describes one particular main line, so it is meaningless once lines
  are merged or split.
* A group that needs no change (one game, no repeats, no transpositions, no
  text symbols, no split) is copied through unchanged.
* One game in, one game out, with the same main line: the original headers and
  result are kept.  Otherwise the group's games get generated headers (the
  originals are listed in the report).
* A group that fails to parse, or whose output fails the check, is copied
  through unchanged and listed in the report.
* The first game in the file is never touched or merged with others (in
  basic.pgn it is the A80 game that carries the weights for the top-level
  moves and variations); --include-first overrides this.
* Games with no ECO tag (or ECO "?") and ECO codes you did not select are
  copied through unchanged (apart from the %evp removal).  Groups are independent: nothing is merged across
  different ECO codes.

Output: ONE pgn (default <input>.restructured.pgn; the input is never
overwritten) plus a report <output>.report.md.
"""
import argparse
import collections
import datetime
import io
import json
import os
import re
import shutil
import sys
import time

import chess
import chess.pgn
import chess.polyglot

sys.setrecursionlimit(20000)

# --------------------------------------------------------------------------
# Conventions of the Arasan book builder (src/util/makebook.cpp)
# --------------------------------------------------------------------------
BUILDER_MAX_NESTING = 18      # MAX_VAR = 20 in makebook.cpp -> 18 nested '('

POS_NAGS = range(10, 22)      # $10..$21 position evaluations (per line)
MOVE_QUALITY = {1, 2, 3, 4, 5, 6}
SYMBOL_TO_NAG = {'!': 1, '?': 2, '!!': 3, '??': 4, '!?': 5, '?!': 6}
QUOTED_SYM = re.compile(r"'(!!|\?\?|!\?|\?!|!|\?)'")
BARE_SYM_ONLY = re.compile(r"^\s*(?:(?:!!|\?\?|!\?|\?!|!|\?)\s*)+$")
BARE_SYM_TOKENS = re.compile(r"!!|\?\?|!\?|\?!|!|\?")
WEIGHT_RE = re.compile(r"^weight:\d+")
EVP_RE = re.compile(r"\[%evp[^\]]*\]")
START_HASH = chess.polyglot.zobrist_hash(chess.Board())

# Stockfish-based resolution of conflicting NAGs (centipawns)
MATE_CP = 100000
DEFAULT_EVAL_CP = (30, 90, 200)    # '=' up to 30, +/= up to 90, +/- up to 200, else +-
DEFAULT_LOSS_CP = (30, 75, 150)    # ?! from 0.30, ? from 0.75, ?? from 1.50 lost vs best
DEFAULT_ONLY_CP = (75, 300)        # best move this much better than 2nd: ! (0.75) / !!
NAG_NAME = {1: "!", 2: "?", 3: "!!", 4: "??", 5: "!?", 6: "?!", 10: "=", 11: "=",
            12: "=", 13: "unclear", 14: "+/=", 15: "=/+", 16: "+/-", 17: "-/+",
            18: "+-", 19: "-+"}


def zob(board):
    return chess.polyglot.zobrist_hash(board)


# --------------------------------------------------------------------------
# Parsing (one Occ per move occurrence, in PGN text order)
# --------------------------------------------------------------------------
class Occ:
    __slots__ = ("idx", "parent", "h_before", "uci", "h_after",
                 "nags", "comments", "pre", "text_nags")

    def __init__(self, idx, parent, h_before, uci, h_after):
        self.idx, self.parent = idx, parent
        self.h_before, self.uci, self.h_after = h_before, uci, h_after
        self.nags, self.comments, self.pre = [], [], []
        self.text_nags = set()     # NAGs that came from converted text symbols


class _Visitor(chess.pgn.BaseVisitor):
    def __init__(self):
        self.headers = collections.OrderedDict()
        self.occs = []
        self.main = []             # edges of the game's own main line
        self.cur = None            # last occurrence in the current line
        self.line_parent = -1      # parent occ idx for the first move of the line
        self.stack = []
        self.pending_pre = []
        self.fen = None
        self.game_result = "*"

    def visit_header(self, tag, value):
        self.headers[tag] = value
        if tag in ("FEN", "SetUp") and not (tag == "SetUp" and value == "0"):
            self.fen = value

    def begin_variation(self):
        self.stack.append((self.cur, self.line_parent))
        # a variation replaces the move just played: same parent as that move
        self.line_parent = self.cur.parent if self.cur is not None else -1
        self.cur = None

    def end_variation(self):
        self.cur, self.line_parent = self.stack.pop()

    def visit_move(self, board, move):
        h0 = zob(board)
        b2 = board.copy(stack=False)
        b2.push(move)
        parent = self.cur.idx if self.cur is not None else self.line_parent
        o = Occ(len(self.occs), parent, h0, move.uci(), zob(b2))
        o.pre, self.pending_pre = self.pending_pre, []
        self.occs.append(o)
        if not self.stack:
            self.main.append((h0, o.uci))
        self.cur = o

    def visit_nag(self, nag):
        if self.cur is not None:
            self.cur.nags.append(nag)

    def visit_comment(self, comment):
        c = EVP_RE.sub("", comment).strip()      # [%evp ...] profiles are dropped
        if not c:
            return
        (self.pending_pre if self.cur is None else self.cur.comments).append(c)

    def visit_result(self, result):
        self.game_result = result

    def result(self):
        return self


def split_blocks(text):
    """(start, end) of each game block; blocks start at lines '[Event ' (a stray
    leading ')' before the first one, as in upstream basic.pgn, is tolerated)."""
    starts = [m.start() for m in re.finditer(r"(?m)^\)?\[Event ", text)]
    if not starts:
        return []
    starts[0] = 0
    return list(zip(starts, starts[1:] + [len(text)]))


def block_eco(block):
    m = re.search(r'(?m)^\[ECO "([^"]*)"\]', block)
    return m.group(1).strip() if m else None


def parse_block(block):
    vis = _Visitor()
    chess.pgn.read_game(io.StringIO(block.lstrip(")")), Visitor=lambda: vis)
    return vis


def header_text(block):
    """the tag-pair lines of a block, exactly as written."""
    m = re.match(r"(?:\)?\[[^\n]*\n)+", block)
    return m.group(0) if m else ""


def strip_evp_text(block):
    """remove [%evp ...] from the movetext of a block that is copied through."""
    if "[%evp" not in block:
        return block
    hdr = header_text(block)

    def fix(m):
        inner = m.group(1)
        if "[%evp" not in inner:
            return m.group(0)
        rest = EVP_RE.sub("", inner).strip()
        return ("{%s}" % rest + m.group(2)) if rest else ""
    return hdr + re.sub(r"\{([^}]*)\}( ?)", fix, block[len(hdr):])


def convert_text_nags(o, report):
    """Turn quoted symbols in an occurrence's comments into NAGs."""
    out = []
    for c in o.comments:
        if WEIGHT_RE.match(c):
            out.append(c)
            continue
        syms = QUOTED_SYM.findall(c)
        rest = QUOTED_SYM.sub("", c)
        if not syms and BARE_SYM_ONLY.match(c):
            syms, rest = BARE_SYM_TOKENS.findall(c), ""
        if not syms:
            out.append(c)
            continue
        nags = []
        for s in syms:
            if SYMBOL_TO_NAG[s] not in nags:
                nags.append(SYMBOL_TO_NAG[s])
        rest = re.sub(r"\s+", " ", rest).strip()
        report.append(dict(h_before=o.h_before, uci=o.uci, original=c,
                           nags=nags, remaining=rest))
        for n in nags:
            if n not in o.nags:
                o.nags.append(n)
                o.text_nags.add(n)
        if rest:
            out.append(rest)
    o.comments = out


# --------------------------------------------------------------------------
# Position graph
# --------------------------------------------------------------------------
class Graph:
    def __init__(self):
        self.children = {}     # h_before -> [uci] first-seen order
        self.after = {}        # (h_before, uci) -> h_after
        self.order = {}        # edge -> sequence number (first seen)
        self.owner = {}        # h_after -> first edge that reached it
        self.e_nags = {}       # edge -> [nag]    (move-level)
        self.e_weight = {}     # edge -> "weight:N"
        self.e_pre = {}        # edge -> [comment before the move (variation labels)]
        self.p_nags = {}       # h -> [nag]       ($10..$21)
        self.p_comments = {}   # h -> [comment]
        self.conflicts = []    # dicts, see add()
        self.repeated_edges = 0
        self.dup_comments = 0
        self.occurrences = 0

    def add(self, o):
        self.occurrences += 1
        e = (o.h_before, o.uci)
        if e in self.after:
            self.repeated_edges += 1
        else:
            self.after[e] = o.h_after
            self.order[e] = len(self.order)
            self.children.setdefault(o.h_before, []).append(o.uci)
            if o.h_after not in self.owner and o.h_after != START_HASH:
                self.owner[o.h_after] = e
        en = self.e_nags.setdefault(e, [])
        for n in o.nags:
            if n in POS_NAGS:
                pn = self.p_nags.setdefault(o.h_after, [])
                if n in pn:
                    continue
                if pn:
                    self.conflicts.append(dict(kind="position evaluation", edge=e,
                                               kept=pn[0], dropped=n,
                                               from_text=n in o.text_nags))
                    continue
                pn.append(n)
            elif n not in en:
                if n in MOVE_QUALITY and any(x in MOVE_QUALITY for x in en):
                    self.conflicts.append(dict(
                        kind="move quality", edge=e,
                        kept=[x for x in en if x in MOVE_QUALITY][0], dropped=n,
                        from_text=n in o.text_nags))
                    continue
                en.append(n)
        pc = self.p_comments.setdefault(o.h_after, [])
        for c in o.comments:
            if WEIGHT_RE.match(c):
                if e in self.e_weight and self.e_weight[e] != c:
                    self.conflicts.append(dict(kind="weight", edge=e,
                                               kept=self.e_weight[e], dropped=c,
                                               from_text=False))
                else:
                    self.e_weight.setdefault(e, c)
            elif c in pc:
                self.dup_comments += 1
            else:
                pc.append(c)
        pre = self.e_pre.setdefault(e, [])
        for c in o.pre:
            if c not in pre:
                pre.append(c)

    def finalize(self):
        """A label written before a variation's first move cannot stay on a move
        that has become the first (main-line) move of its position: it would be
        read as a comment on the previous position.  Say so explicitly by
        moving it there.  (Comments before the very first move of a game stay.)
        Returns the number of labels moved."""
        moved = 0
        for h, kids in self.children.items():
            if h == START_HASH or not kids:
                continue
            pre = self.e_pre.get((h, kids[0]))
            if pre:
                pc = self.p_comments.setdefault(h, [])
                for c in pre:
                    if c not in pc:
                        pc.append(c)
                    moved += 1
                self.e_pre[(h, kids[0])] = []
        return moved

    # ownership tree ------------------------------------------------------
    def tchildren(self, h):
        """[(uci, target, expanded_here)] for position h, first-seen order."""
        out = []
        for u in self.children.get(h, []):
            t = self.after[(h, u)]
            out.append((u, t, self.owner.get(t) == (h, u)))
        return out

    def path_to(self, h):
        """edges from the start position to h along owner edges."""
        path = []
        while h != START_HASH:
            e = self.owner[h]
            path.append(e)
            h = e[0]
        path.reverse()
        return path

    def san_path(self, edges):
        moves = [chess.Move.from_uci(u) for _, u in edges]
        return chess.Board().variation_san(moves) if moves else "(start)"

    def board_at(self, h):
        b = chess.Board()
        for _, u in self.path_to(h):
            b.push(chess.Move.from_uci(u))
        return b

    def edge_label(self, e):
        """'17...Bb6'-style label of an edge."""
        h, u = e
        b = self.board_at(h)
        san = b.san(chess.Move.from_uci(u))
        return ("%d.%s" if b.turn else "%d...%s") % (b.fullmove_number, san)

    def conflict_info(self, c):
        """(move label, FEN before, FEN after) for a conflict's edge."""
        h, u = c["edge"]
        b = self.board_at(h)
        mv = chess.Move.from_uci(u)
        san = b.san(mv)
        label = ("%d.%s" if b.turn else "%d...%s") % (b.fullmove_number, san)
        before = b.fen()
        b.push(mv)
        return label, before, b.fen()

    def edge_context(self, e):
        return self.san_path(self.path_to(e[0]) + [e])

    def main_line(self):
        """edges of the main line the merged tree will have (no splitting)."""
        out, h = [], START_HASH
        while True:
            kids = self.tchildren(h)
            if not kids:
                return out
            u, t, ex = kids[0]
            out.append((h, u))
            if not ex:
                return out
            h = t


# --------------------------------------------------------------------------
# Splitting
# --------------------------------------------------------------------------
def plan_cuts(g, limit, min_cut):
    """Bottom-up greedy: whenever the part of a subtree still in the same game
    exceeds `limit`, split off the largest child subtree (>= min_cut)."""
    order, st = [], [START_HASH]
    while st:
        h = st.pop()
        order.append(h)
        st.extend(t for _, t, ex in g.tchildren(h) if ex)
    cut, open_size = set(), {}
    for h in reversed(order):
        kids = [(u, t) for u, t, ex in g.tchildren(h) if ex]
        o = 1 + sum(open_size[t] for u, t in kids if (h, u) not in cut)
        while o > limit:
            cands = [(open_size[t], u, t) for u, t in kids
                     if (h, u) not in cut and open_size[t] >= min_cut]
            if not cands:
                break
            s, u, t = max(cands)
            cut.add((h, u))
            o -= s
        open_size[h] = o
    return cut, open_size


def choose_cuts(g, max_nodes, max_games, min_cut):
    """Split at `max_nodes` positions per game; if that needs more than
    `max_games` games, use the smallest larger limit that fits."""
    cut, _ = plan_cuts(g, max_nodes, min_cut)
    if len(cut) + 1 <= max_games:
        return max_nodes, cut
    lo, hi = max_nodes, len(g.owner) + 600          # hi: always a single game
    while hi - lo > 50:
        mid = (lo + hi) // 2
        c, _ = plan_cuts(g, mid, min_cut)
        if len(c) + 1 <= max_games:
            hi = mid
        else:
            lo = mid
    cut, _ = plan_cuts(g, hi, min_cut)
    return hi, cut


# --------------------------------------------------------------------------
# Writing (style of the existing file: one movetext line, tight () and {})
# --------------------------------------------------------------------------
class Out:
    def __init__(self):
        self.parts = []

    def add(self, tok):
        if self.parts and not self.parts[-1].endswith("(") and tok != ")":
            self.parts.append(" ")
        self.parts.append(tok)

    def text(self):
        return "".join(self.parts)


class GameWriter:
    def __init__(self, g, cut):
        self.g, self.cut = g, cut
        self.max_depth = 0
        self.nodes = 0

    def _expanded(self, h, u, t, ex):
        return ex and (h, u) not in self.cut

    def move_tokens(self, out, board, h, u, t, ex, force_num, annotate=True):
        """write one move with its annotations; returns True if a comment was
        written (a following black move then needs its number)."""
        g = self.g
        mv = chess.Move.from_uci(u)
        wrote_comment = False
        if annotate:
            for c in g.e_pre.get((h, u), []):
                out.add("{%s}" % c)
                wrote_comment = True
        if board.turn == chess.WHITE:
            out.add("%d." % board.fullmove_number)
        elif force_num or wrote_comment:
            out.add("%d..." % board.fullmove_number)
        out.add(board.san(mv))
        if not annotate:
            return False
        for n in g.e_nags.get((h, u), []):
            out.add("$%d" % n)
        expanded = self._expanded(h, u, t, ex)
        if expanded:
            for n in g.p_nags.get(t, []):
                out.add("$%d" % n)
        w = g.e_weight.get((h, u))
        if w:
            out.add("{%s}" % w)
            wrote_comment = True
        if expanded:
            for c in g.p_comments.get(t, []):
                out.add("{%s}" % c)
                wrote_comment = True
        return wrote_comment

    def emit_from(self, out, h, board, need_num, depth, count_plies=None):
        g = self.g
        self.max_depth = max(self.max_depth, depth)
        while True:
            kids = g.tchildren(h)
            if not kids:
                return
            u, t, ex = kids[0]
            if g.e_pre.get((h, u)) and h != START_HASH:
                raise AssertionError("variation label on a main-line move")
            wrote = self.move_tokens(out, board, h, u, t, ex, need_num)
            if count_plies is not None:
                count_plies[0] += 1
            b_after = board.copy(stack=False)
            b_after.push(chess.Move.from_uci(u))
            if self._expanded(h, u, t, ex):
                self.nodes += 1
            for (u2, t2, ex2) in kids[1:]:
                out.add("(")
                w2 = self.move_tokens(out, board, h, u2, t2, ex2, True)
                if self._expanded(h, u2, t2, ex2):
                    self.nodes += 1
                    b2 = board.copy(stack=False)
                    b2.push(chess.Move.from_uci(u2))
                    self.emit_from(out, t2, b2, w2, depth + 1)
                out.add(")")
            need_num = wrote or len(kids) > 1
            if not self._expanded(h, u, t, ex):
                return
            board, h = b_after, t

    def write_root(self):
        out, plies = Out(), [0]
        self.emit_from(out, START_HASH, chess.Board(), True, 0, plies)
        return out.text(), plies[0]

    def write_cut(self, edge):
        """game whose own territory starts with the edge `edge` (a cut edge)."""
        g = self.g
        path = g.path_to(edge[0]) + [edge]
        out, board = Out(), chess.Board()
        plies = 0
        for (h, u) in path:                      # bare prefix, annotations stay
            self.move_tokens(out, board, h, u, None, False, False, annotate=False)
            board.push(chess.Move.from_uci(u))
            plies += 1
        t = g.after[edge]
        wrote = False
        for n in g.p_nags.get(t, []):
            out.add("$%d" % n)
        for c in g.p_comments.get(t, []):
            out.add("{%s}" % c)
            wrote = True
        self.nodes += 1
        cnt = [0]
        self.emit_from(out, t, board, wrote, 0, cnt)
        return out.text(), plies + cnt[0], len(path)


# --------------------------------------------------------------------------
# Verification
# --------------------------------------------------------------------------
def verify(g, games):
    """games: list of dicts(block=str, prefix=int).  Returns list of problems."""
    problems = []
    g2 = Graph()
    edge_count = collections.Counter()
    comment_sites = collections.Counter()
    nag_sites = collections.Counter()
    parents = collections.defaultdict(set)
    for gi, gm in enumerate(games):
        v = parse_block(gm["block"])
        for o in v.occs:
            g2.add(o)
            if o.idx < gm["prefix"]:
                continue                       # bare replay of the path
            edge_count[(o.h_before, o.uci)] += 1
            parents[o.h_before].add((gi, o.parent))
        for o in v.occs:
            if any(not WEIGHT_RE.match(c) for c in o.comments):
                comment_sites[o.h_after] += 1
            if any(n in POS_NAGS for n in o.nags):
                nag_sites[o.h_after] += 1
    if g2.after != g.after:
        problems.append("edge set differs: %d missing, %d extra" % (
            len(set(g.after) - set(g2.after)), len(set(g2.after) - set(g.after))))
    for name in ("e_nags", "p_nags"):
        a, b = getattr(g, name), getattr(g2, name)
        for k in set(a) | set(b):
            if set(a.get(k, [])) != set(b.get(k, [])):
                problems.append("%s differ" % name)
                break
    for k in set(g.p_comments) | set(g2.p_comments):
        if set(g.p_comments.get(k, [])) != set(g2.p_comments.get(k, [])):
            problems.append("position comments differ")
            break
    a, b = g.e_weight, g2.e_weight
    if {k: v for k, v in a.items() if v} != {k: v for k, v in b.items() if v}:
        problems.append("weights differ")
    a = {k: v for k, v in g.e_pre.items() if v}
    b = {k: v for k, v in g2.e_pre.items() if v}
    if a != b:
        problems.append("variation labels differ")
    if any(c > 1 for c in edge_count.values()):
        problems.append("some moves are still written more than once")
    if any(len(p) > 1 for p in parents.values()):
        problems.append("some positions are still expanded in more than one place")
    if any(c > 1 for c in comment_sites.values()):
        problems.append("a position carries comments in more than one place")
    if any(c > 1 for c in nag_sites.values()):
        problems.append("a position carries an evaluation in more than one place")
    return problems


# --------------------------------------------------------------------------
# Stockfish: resolving conflicting NAGs
# --------------------------------------------------------------------------
class Searcher:
    """Depth-limited Stockfish searches with an on-disk cache."""

    def __init__(self, path, depth, hash_mb, threads, cache_path):
        self.path, self.depth, self.hash_mb, self.threads = path, depth, hash_mb, threads
        self.cache_path = cache_path
        self.engine = None
        self.cache = {}
        self.searches = self.hits = self.decisions = 0
        if cache_path and os.path.exists(cache_path):
            try:
                with open(cache_path, encoding="utf-8") as f:
                    self.cache = json.load(f)
            except (OSError, ValueError):
                self.cache = {}

    def _start(self):
        import chess.engine
        self.engine = chess.engine.SimpleEngine.popen_uci(self.path)
        opts = {"Hash": self.hash_mb}
        if self.threads:
            opts["Threads"] = self.threads
        self.engine.configure(opts)

    def selftest(self):
        """start the engine and run a trivial search, so a bad --sf-hash or --sf-path
        fails now rather than after hours of work."""
        import chess.engine
        self._start()
        self.engine.analyse(chess.Board(), chess.engine.Limit(depth=1))

    def _save(self):
        if not self.cache_path:
            return
        tmp = self.cache_path + ".tmp"
        with open(tmp, "w", encoding="utf-8") as f:
            json.dump(self.cache, f)
        os.replace(tmp, self.cache_path)

    def lines(self, board, multipv=1, root_move=None):
        """[[uci, centipawns from White's side], ...] best first."""
        import chess.engine
        key = "%s|%d|%d|%s" % (board.epd(), self.depth, multipv, root_move or "")
        if key in self.cache:
            self.hits += 1
            return self.cache[key]
        if self.engine is None:
            self._start()
        kw = {}
        if root_move:
            kw["root_moves"] = [chess.Move.from_uci(root_move)]
        try:
            infos = self.engine.analyse(board, chess.engine.Limit(depth=self.depth),
                                        multipv=multipv, **kw)
        except chess.engine.EngineTerminatedError:
            self.engine = None                    # restart on the next search
            raise
        if isinstance(infos, dict):
            infos = [infos]
        out = []
        for inf in infos:
            pv, sc = inf.get("pv"), inf.get("score")
            if pv and sc is not None:
                out.append([pv[0].uci(), sc.white().score(mate_score=MATE_CP)])
        self.cache[key] = out
        self.searches += 1
        self._save()
        return out

    def close(self):
        if self.engine is not None:
            try:
                self.engine.quit()
            except Exception:
                pass
            self.engine = None


def fmt_cp(cp):
    if abs(cp) >= MATE_CP - 1000:
        return ("+" if cp > 0 else "-") + "M"
    return "%+.2f" % (cp / 100.0)


def nag_name(n):
    return "$%d (%s)" % (n, NAG_NAME.get(n, "?"))


def eval_to_nag(cp, t):
    """position evaluation NAG for a score in centipawns from White's side."""
    a, white = abs(cp), cp > 0
    if a <= t[0]:
        return 11
    if a <= t[1]:
        return 14 if white else 15
    if a <= t[2]:
        return 16 if white else 17
    return 18 if white else 19


NEUTRAL_RANK = {4: 0, 2: 1, 6: 2, 5: 3, 1: 4, 3: 5}   # ?? ? ?! !? ! !!


def nearest_to_neutral(cands):
    """the candidate closest to 'no clear verdict' (!?); ties: the earlier one."""
    return min(cands, key=lambda n: abs(NEUTRAL_RANK.get(n, 3) - 3))


def move_to_nag(loss, margin, loss_t, only_t, capture=False):
    """Returns (NAG or None, reason).  `loss`: centipawns lost against the best
    move; `margin`: when the played move is the best one, how much better it is
    than the second-best.  Good-move marks are never given to captures."""
    c = lambda v: "%.2f" % (v / 100.0)
    if loss >= loss_t[2]:
        return 4, "loses %s, at least %s (??)" % (c(loss), c(loss_t[2]))
    if loss >= loss_t[1]:
        return 2, "loses %s, at least %s (?)" % (c(loss), c(loss_t[1]))
    if loss >= loss_t[0]:
        return 6, "loses %s, at least %s (?!)" % (c(loss), c(loss_t[0]))
    if margin is not None:
        if margin >= only_t[0] and capture:
            return None, "best move by %s, but it is a capture (no ! for captures)" % c(margin)
        if margin >= only_t[1]:
            return 3, "best move, %s better than the next, at least %s (!!)" % (
                c(margin), c(only_t[1]))
        if margin >= only_t[0]:
            return 1, "best move, %s better than the next, at least %s (!)" % (
                c(margin), c(only_t[0]))
        return None, "best move, but only %s better than the next (! needs %s)" % (
            c(margin), c(only_t[0]))
    if loss > 0:
        return None, "loses %s, below the %s needed for ?!" % (c(loss), c(loss_t[0]))
    return None, "as good as the best move; no alternative line to compare"


POSITIVE_NAGS = {1, 3, 5}      # ! !! !?
NEGATIVE_NAGS = {2, 4, 6}      # ? ?? ?!


def retained_extreme(cset):
    """!! ($3) or ?? ($4) is always retained, unless a NAG of the opposite sign
    is also present.  Returns 3, 4 or None (None: the search decides)."""
    if 3 in cset and not (cset & NEGATIVE_NAGS):
        return 3
    if 4 in cset and not (cset & POSITIVE_NAGS):
        return 4
    return None


def choose_move_nag(cset, verdict, neutral, cands):
    """Final move-quality NAG for a conflict.  `cset`: the conflicting NAGs;
    `verdict`: the search verdict from move_to_nag (None = no verdict).
    Returns (nag or None, why).  A retained !! / ?? is decided by the caller
    without a search."""
    n = retained_extreme(cset)
    if n is not None:
        return n, "%s is specified and no NAG of the opposite sign conflicts, so it is " \
                  "retained" % NAG_NAME[n]
    has_pos, has_neg = bool(cset & POSITIVE_NAGS), bool(cset & NEGATIVE_NAGS)
    if has_pos and not has_neg:                       # ! and !?
        if verdict in NEGATIVE_NAGS:
            return verdict, "the engine finds the move bad, so its verdict replaces ! / !?"
        if verdict in (1, 3):
            return 1, "! is warranted"
        return 5, "! is not warranted, so !? is retained"
    if has_neg and not has_pos:                       # ?! and ?
        if verdict in POSITIVE_NAGS:
            return verdict, "the engine finds the move good, so its verdict replaces ?! / ?"
        if verdict in (2, 4):
            return 2, "? is warranted"
        return 6, "? is not warranted, so ?! is retained"
    if verdict is not None:                           # positive and negative mixed
        return verdict, "positive and negative NAGs conflict, so the search verdict is used"
    if neutral == "nearest":
        return nearest_to_neutral(cands), "mixed NAGs, no verdict: the one closest to neutral"
    if neutral == "first":
        return cands[0], "mixed NAGs, no verdict: the first seen"
    return None, "mixed NAGs and no engine verdict"


def eval_reason(cp, t):
    a = abs(cp) / 100.0
    if abs(cp) <= t[0]:
        return "|eval| %.2f is within %.2f of equal" % (a, t[0] / 100.0)
    lo, hi = (t[0], t[1]) if abs(cp) <= t[1] else (
        (t[1], t[2]) if abs(cp) <= t[2] else (t[2], None))
    if hi is None:
        return "|eval| %.2f is above %.2f" % (a, lo / 100.0)
    return "|eval| %.2f is between %.2f and %.2f" % (a, lo / 100.0, hi / 100.0)


def resolve_conflicts(g, eco, searcher, args, log):
    """Decide conflicting NAGs by search and rewrite g accordingly.  Every
    conflict dict gets 'resolution' and 'evidence' strings.  Returns the
    decision records."""
    decisions, groups = [], collections.OrderedDict()
    for c in g.conflicts:
        if c["kind"] == "move quality":
            key = ("mq", c["edge"])
        elif c["kind"] == "position evaluation":
            key = ("pe", g.after[c["edge"]])
        else:
            c["resolution"] = "first kept (weight comments are not searched)"
            continue
        grp = groups.setdefault(key, dict(cands=[c["kept"]], items=[], edge=c["edge"]))
        if c["dropped"] not in grp["cands"]:
            grp["cands"].append(c["dropped"])
        grp["items"].append(c)

    for (kind, k), grp in groups.items():
        t0 = time.time()
        searcher.decisions += 1
        d = dict(eco=eco, kind=kind, cands=grp["cands"], changed=False, failed=False,
                 result=None)
        evidence = outcome = ""
        label = "?"
        try:
            if kind == "pe":
                label = g.edge_label(g.owner[k]) + " (position)"
                board = g.board_at(k)
                if board.is_game_over():
                    cp = 0 if not board.is_checkmate() else (
                        -MATE_CP if board.turn == chess.WHITE else MATE_CP)
                    evidence = "game over in this position"
                else:
                    top = searcher.lines(board, 1)[0]
                    cp = top[1]
                    evidence = "eval %s (White's view), engine's move here: %s" % (
                        fmt_cp(cp), board.san(chess.Move.from_uci(top[0])))
                new = eval_to_nag(cp, args.eval_cp)
                old = g.p_nags.get(k, [None])[0]
                g.p_nags[k] = [new]
                d["changed"], d["result"] = new != old, new
                outcome = "%s - %s" % (nag_name(new), eval_reason(cp, args.eval_cp))
            else:
                h, u = k
                label = g.edge_label(k)
                cset = set(grp["cands"])
                first = grp["cands"][0]
                if retained_extreme(cset) is not None:    # !! or ?? retained: no search
                    new, why = choose_move_nag(cset, None, args.neutral, grp["cands"])
                    evidence = "(no search needed)"
                    outcome = "%s - %s" % (nag_name(new), why)
                else:
                    board = g.board_at(h)
                    white = board.turn == chess.WHITE
                    mover = (lambda cp: cp) if white else (lambda cp: -cp)
                    lines = searcher.lines(board, 2)
                    best_uci, best_w = lines[0]
                    best_cp = mover(best_w)
                    if u == best_uci:
                        played_w = best_w
                    elif len(lines) > 1 and u == lines[1][0]:
                        played_w = lines[1][1]
                    else:
                        played_w = searcher.lines(board, 1, root_move=u)[0][1]
                    played_cp = mover(played_w)
                    loss = max(0, best_cp - played_cp)
                    margin = None
                    if u == best_uci and len(lines) > 1:
                        margin = best_cp - mover(lines[1][1])
                    capture = board.is_capture(chess.Move.from_uci(u))
                    verdict, reason = move_to_nag(loss, margin, args.loss_cp, args.only_cp,
                                                  capture)
                    san = board.san(chess.Move.from_uci(u))
                    parts = ["best %s %s" % (board.san(chess.Move.from_uci(best_uci)),
                                             fmt_cp(best_w))]
                    if len(lines) > 1:
                        parts.append("2nd %s %s" % (
                            board.san(chess.Move.from_uci(lines[1][0])), fmt_cp(lines[1][1])))
                    parts.append("played %s (= best)" % san if u == best_uci
                                 else "played %s %s" % (san, fmt_cp(played_w)))
                    tail = "loss %.2f" % (loss / 100.0)
                    if margin is not None:
                        tail += ", margin over 2nd %.2f" % (margin / 100.0)
                    evidence = " | ".join(parts) + " | " + tail + "   (evals: White's view)"
                    new, why = choose_move_nag(cset, verdict, args.neutral, grp["cands"])
                    outcome = "%s - %s (engine: %s)" % (
                        nag_name(new) if new is not None else "no mark", why, reason)
                nags = g.e_nags[k]
                idx = [i for i, n in enumerate(nags) if n in MOVE_QUALITY]
                at = idx[0] if idx else len(nags)
                nags = [n for n in nags if n not in MOVE_QUALITY]
                if new is not None:
                    nags.insert(at, new)
                g.e_nags[k] = nags
                d["changed"], d["result"] = new != first, new
        except Exception as exc:                  # keep the first-seen NAG
            d["failed"] = True
            outcome = "search failed (%s: %s): first kept" % (
                type(exc).__name__, str(exc)[:120])
        d["evidence"], d["outcome"] = evidence, outcome
        for it in grp["items"]:
            it["resolution"] = "Stockfish d%d: %s" % (searcher.depth, outcome)
            it["evidence"] = evidence
        decisions.append(d)
        log("#%d %s %s  [%s]  %.0fs\n    %s\n    -> %s" % (
            searcher.decisions, eco, label,
            " vs ".join(nag_name(n) for n in grp["cands"]), time.time() - t0,
            evidence or "(no search)", outcome))
    return decisions


def tally_lines(results):
    """summary of the search decisions, as a list of lines."""
    mq, pe, failed = collections.Counter(), collections.Counter(), 0
    for r in results:
        for d in r.sf_decisions:
            if d["failed"]:
                failed += 1
            else:
                (mq if d["kind"] == "mq" else pe)[d["result"]] += 1

    def fmt(c):
        order = sorted(c.items(), key=lambda kv: (kv[0] is None, kv[0] or 0))
        return ", ".join("%s %d" % ("no mark" if k is None else NAG_NAME.get(k, "$%d" % k), v)
                         for k, v in order) or "none"
    return ["Stockfish decisions: %d (move quality %d, position evaluation %d, failed %d)" % (
                sum(mq.values()) + sum(pe.values()) + failed, sum(mq.values()),
                sum(pe.values()), failed),
            "  move quality: %s" % fmt(mq),
            "  position evaluation: %s" % fmt(pe)]


# --------------------------------------------------------------------------
# One ECO group
# --------------------------------------------------------------------------
class GroupResult:
    def __init__(self, eco, n_in):
        self.eco, self.n_in = eco, n_in
        self.status = "unchanged"     # unchanged | rewritten | skipped
        self.reason = ""
        self.text = None              # replacement text (rewritten only)
        self.n_out = n_in
        self.limit = None
        self.stats = []               # (no, title, positions, nesting, bytes)
        self.g = None
        self.conv = []
        self.old_headers = []
        self.kept_headers = False
        self.occurrences = self.distinct = self.repeats = 0
        self.transpositions = []
        self.max_nesting = 0
        self.labels_moved = 0
        self.sf_decisions = []


def process_group(eco, blocks_text, args, nl, today, searcher=None):
    """blocks_text: the group's game blocks, in file order."""
    res = GroupResult(eco, len(blocks_text))
    try:
        g, conv, parsed = Graph(), [], []
        for bt in blocks_text:
            v = parse_block(bt)
            if v.fen:
                res.status, res.reason = "skipped", "has a FEN/SetUp header"
                return res
            parsed.append(v)
            res.old_headers.append(v.headers)
            for o in v.occs:
                convert_text_nags(o, conv)
                g.add(o)
        res.labels_moved = g.finalize()
        if g.conflicts and searcher is not None:
            res.sf_decisions = resolve_conflicts(
                g, eco, searcher, args, lambda m: print(m, flush=True))
        res.g, res.conv = g, conv
        res.occurrences, res.distinct, res.repeats = g.occurrences, len(g.after), g.repeated_edges
        if g.occurrences == 0:
            res.reason = "no moves"
            return res
        trans = collections.Counter(g.after.values())
        res.transpositions = [h for h, c in trans.items() if c > 1]

        res.limit, cut = choose_cuts(g, args.max_nodes, args.max_games, args.min_cut)
        cut_edges = sorted(cut, key=lambda e: g.order[e])
        n_games = len(cut_edges) + 1

        needs_change = (len(blocks_text) > 1 or conv or g.repeated_edges
                        or res.transpositions or g.dup_comments or g.conflicts
                        or res.labels_moved or n_games > 1)
        if not needs_change:
            res.reason = "nothing to change"
            return res

        ann = collections.Counter(h.get("Annotator", "") for h in res.old_headers)
        annotator = ann.most_common(1)[0][0] or "?"

        def gen_block(event, rnd, plies, movetext):
            hdr = [("Event", event), ("Site", "?"), ("Date", today), ("Round", rnd),
                   ("White", "?"), ("Black", "?"), ("Result", "*"),
                   ("Annotator", annotator), ("ECO", eco), ("PlyCount", str(plies))]
            return (nl.join('[%s "%s"]' % kv for kv in hdr) + nl + nl
                    + movetext + " *" + nl + nl)

        games, stats = [], []
        w = GameWriter(g, cut)
        mt, plies = w.write_root()
        keep = (len(blocks_text) == 1 and n_games == 1
                and g.main_line() == parsed[0].main)
        if keep:
            hdrs = header_text(blocks_text[0])
            blk = hdrs + nl + mt + " " + parsed[0].game_result + nl + nl
            res.kept_headers = True
            title = "(original game kept)"
        else:
            title = "stem and minor lines" if n_games > 1 else "merged"
            event = ("%s [1/%d] %s" % (eco, n_games, title)) if n_games > 1 \
                else "%s %s" % (eco, title)
            blk = gen_block(event, "1", plies, mt)
        games.append(dict(block=blk, prefix=0))
        stats.append(("1", title, w.nodes, w.max_depth, len(mt)))
        for k, e in enumerate(cut_edges, start=2):
            w = GameWriter(g, cut)
            mt, plies, prefix = w.write_cut(e)
            title = g.edge_context(e)
            games.append(dict(block=gen_block("%s [%d/%d] %s" % (eco, k, n_games, title),
                                              str(k), plies, mt), prefix=prefix))
            stats.append((str(k), title, w.nodes, w.max_depth, len(mt)))

        problems = verify(g, games)
        if problems:
            res.status, res.reason = "skipped", "check failed: " + "; ".join(problems)
            return res
        res.status = "rewritten"
        res.text = "".join(gm["block"] for gm in games)
        res.n_out = n_games
        res.stats = stats
        res.max_nesting = max(s[3] for s in stats)
        return res
    except Exception as exc:                      # leave the group as it was
        res.status = "skipped"
        res.reason = "%s: %s" % (type(exc).__name__, str(exc)[:200])
        res.text = None
        return res


# --------------------------------------------------------------------------
def _ints(text, n):
    try:
        v = tuple(int(x) for x in text.split(","))
    except ValueError:
        v = ()
    if len(v) != n or list(v) != sorted(v):
        raise argparse.ArgumentTypeError("need %d increasing integers, e.g. 30,90,200" % n)
    return v


def main():
    ap = argparse.ArgumentParser(description=__doc__,
                                 formatter_class=argparse.RawDescriptionHelpFormatter)
    ap.add_argument("pgn")
    ap.add_argument("--eco", nargs="+", default=None, metavar="CODE",
                    help="ECO code(s) to process, e.g. --eco C54 C55 or C54,C55 "
                         "(default: every code present)")
    ap.add_argument("-o", "--output", default=None,
                    help="output PGN (default: <input>.restructured.pgn)")
    ap.add_argument("--max-nodes", type=int, default=1800,
                    help="split a group's games above this many positions (default 1800)")
    ap.add_argument("--max-games", type=int, default=8,
                    help="most games one ECO code may end up with (default 8)")
    ap.add_argument("--min-cut", type=int, default=300,
                    help="smallest subtree worth its own game (default 300)")
    ap.add_argument("--no-engine", action="store_true",
                    help="do not search; conflicting NAGs keep the first occurrence")
    ap.add_argument("--neutral", choices=("drop", "nearest", "first"), default="drop",
                    help="mixed positive/negative move-quality conflicts where the search gives "
                         "no verdict: no mark (default), the NAG nearest to neutral, or the "
                         "first-seen NAG")
    ap.add_argument("--sf-path", default="stockfish",
                    help="Stockfish executable (default: stockfish, from the PATH)")
    ap.add_argument("--sf-depth", type=int, default=30,
                    help="search depth for conflicting NAGs (default 30)")
    ap.add_argument("--sf-hash", type=int, default=4096,
                    help="Stockfish hash in MB (default 4096 = 4 GB)")
    ap.add_argument("--sf-threads", type=int, default=0,
                    help="Stockfish threads (default: Stockfish's own default)")
    ap.add_argument("--sf-cache", default=None,
                    help="search cache file (default: <input>.sfcache.json)")
    ap.add_argument("--eval-cp", type=lambda x: _ints(x, 3), default=DEFAULT_EVAL_CP,
                    metavar="A,B,C", help="position evaluation thresholds in cp: '=' up "
                    "to A, +/= up to B, +/- up to C, else +-  (default 30,90,200)")
    ap.add_argument("--loss-cp", type=lambda x: _ints(x, 3), default=DEFAULT_LOSS_CP,
                    metavar="A,B,C", help="centipawns lost for ?!, ?, ??  (default 30,75,150)")
    ap.add_argument("--only-cp", type=lambda x: _ints(x, 2), default=DEFAULT_ONLY_CP,
                    metavar="A,B", help="margin over the second-best move for !, !!  "
                    "(default 75,300)")
    ap.add_argument("--include-first", action="store_true",
                    help="also process the first game in the file (by default it is "
                         "left untouched: it holds the top-level move weights)")
    ap.add_argument("--list-transpositions", action="store_true",
                    help="list every merged transposition in the report")
    args = ap.parse_args()

    out_pgn = args.output or os.path.splitext(args.pgn)[0] + ".restructured.pgn"
    if os.path.abspath(out_pgn) == os.path.abspath(args.pgn):
        sys.exit("refusing to overwrite the input file")
    out_rep = os.path.splitext(out_pgn)[0] + ".report.md"

    sf_path = None
    if not args.no_engine:
        sf_path = shutil.which(args.sf_path) or (
            args.sf_path if os.path.isfile(args.sf_path) else None)
        if not sf_path:
            sys.exit("Stockfish not found (%r): put it on the PATH, give --sf-path, "
                     "or use --no-engine" % args.sf_path)

    raw = open(args.pgn, "rb").read()
    text = raw.decode("utf-8", errors="surrogateescape")
    nl = "\r\n" if raw.count(b"\r\n") > raw.count(b"\n") / 2 else "\n"
    blocks = split_blocks(text)
    if not blocks:
        sys.exit("no games found")

    groups = collections.OrderedDict()            # eco -> [block index]
    first_eco = block_eco(text[blocks[0][0]:blocks[0][1]])
    for i, (s, e) in enumerate(blocks):
        if i == 0 and not args.include_first:
            continue                              # protected: copied through
        eco = block_eco(text[s:e])
        if eco and eco != "?":
            groups.setdefault(eco, []).append(i)
    if args.eco:
        wanted = []
        for tok in args.eco:
            wanted += [t.strip().upper() for t in tok.split(",") if t.strip()]
        missing = [c for c in wanted if c not in groups]
        if missing:
            print("warning: no games with ECO code(s): %s" % ", ".join(missing))
        groups = collections.OrderedDict((c, groups[c]) for c in wanted if c in groups)
    if not groups:
        sys.exit("nothing to process")

    today = datetime.date.today().strftime("%Y.%m.%d")
    results = []
    if not args.include_first:
        print("first game in the file (ECO %s) is left untouched" % (first_eco or "none"))
    print("processing %d ECO code(s), %d games ..." % (
        len(groups), sum(len(v) for v in groups.values())))
    searcher = None
    if sf_path:
        searcher = Searcher(sf_path, args.sf_depth, args.sf_hash, args.sf_threads,
                            args.sf_cache or os.path.splitext(args.pgn)[0] + ".sfcache.json")
        print("conflicting NAGs: Stockfish (%s), depth %d, hash %d MB; cache %s (%d entries)"
              % (sf_path, args.sf_depth, args.sf_hash, searcher.cache_path,
                 len(searcher.cache)), flush=True)
        try:
            searcher.selftest()
        except Exception as exc:
            searcher.close()
            sys.exit("Stockfish could not run with Hash=%d MB (%s: %s).\n"
                     "Check --sf-path, or lower --sf-hash (it must fit in memory)."
                     % (args.sf_hash, type(exc).__name__, str(exc)[:150]))
    else:
        print("conflicting NAGs: no engine, first occurrence kept")
    try:
        for n, (eco, idxs) in enumerate(groups.items(), 1):
            res = process_group(eco, [text[blocks[i][0]:blocks[i][1]] for i in idxs],
                                args, nl, today, searcher)
            res.idxs = idxs
            results.append(res)
            if n % 50 == 0:
                print("  %d/%d codes" % (n, len(groups)), flush=True)
    finally:
        if searcher is not None:
            searcher.close()

    # assemble the single output file ------------------------------------------
    replace_first, drop = {}, set()
    for r in results:
        if r.status == "rewritten":
            replace_first[r.idxs[0]] = r.text
            drop.update(r.idxs[1:])
    pieces = []
    for i, (s, e) in enumerate(blocks):
        if i in replace_first:
            pieces.append(replace_first[i])
        elif i not in drop:
            pieces.append(strip_evp_text(text[s:e]))
    out_text = "".join(pieces)
    evp_in, evp_left = text.count("[%evp"), out_text.count("[%evp")
    with open(out_pgn, "wb") as f:
        f.write(out_text.encode("utf-8", errors="surrogateescape"))

    # report ---------------------------------------------------------------------
    rew = [r for r in results if r.status == "rewritten"]
    skip = [r for r in results if r.status == "skipped"]
    unch = [r for r in results if r.status == "unchanged"]
    nm = (lambda x: ("$%d" % x) if isinstance(x, int) else x)
    R = ["# ECO restructure report\n",
         "Input `%s` -> output `%s`\n" % (args.pgn, out_pgn),
         ("- the first game in the file (ECO %s) was left untouched and not merged "
          "with other games" % (first_eco or "none")) if not args.include_first else
         "- the first game in the file was processed like any other (--include-first)",
         "- ECO codes processed: %d (rewritten %d, unchanged %d, skipped %d)" % (
             len(results), len(rew), len(unch), len(skip)),
         "- games in the rewritten groups: %d -> %d" % (
             sum(r.n_in for r in rew), sum(r.n_out for r in rew)),
         "- repeated move occurrences merged away: %d" % sum(r.repeats for r in rew),
         "- text annotations converted to NAGs: %d" % sum(len(r.conv) for r in rew),
         "- transposition positions merged: %d" % sum(len(r.transpositions) for r in rew),
         "- engine labels moved to the preceding position's comments: %d" % sum(
             r.labels_moved for r in rew),
         "- conflicting marks: %d%s" % (
             sum(len(r.g.conflicts) for r in rew),
             "" if searcher is None else
             " (searched at depth %d, hash %d MB: %d decisions, %d changed the first-seen "
             "NAG, %d failed; %d searches run, %d from cache)" % (
                 args.sf_depth, args.sf_hash,
                 sum(len(r.sf_decisions) for r in rew),
                 sum(d["changed"] for r in rew for d in r.sf_decisions),
                 sum(d["failed"] for r in rew for d in r.sf_decisions),
                 searcher.searches, searcher.hits)),
         "- [%%evp] comments removed: %d%s" % (
             evp_in - evp_left, "" if not evp_left else " (%d REMAIN)" % evp_left),
         "- limits: %d positions per game, at most %d games per ECO code\n" % (
             args.max_nodes, args.max_games)]
    if skip:
        R.append("## Skipped groups (copied through unchanged)\n")
        for r in skip:
            R.append("- **%s** (%d games): %s" % (r.eco, r.n_in, r.reason))
        R.append("")
    R.append("## Rewritten groups\n")
    R.append("| ECO | games in | games out | positions | repeats merged | "
             "transpositions | text->NAG | conflicts | max nesting | headers |")
    R.append("|---|---|---|---|---|---|---|---|---|---|")
    for r in rew:
        R.append("| %s | %d | %d | %d | %d | %d | %d | %d | %d | %s |" % (
            r.eco, r.n_in, r.n_out, r.distinct + 1, r.repeats, len(r.transpositions),
            len(r.conv), len(r.g.conflicts), r.max_nesting,
            "kept" if r.kept_headers else "generated"))
    deep = [r.eco for r in rew if r.max_nesting > BUILDER_MAX_NESTING]
    if deep:
        R.append("\nWARNING: nesting deeper than the book builder's limit (%d) in: %s" % (
            BUILDER_MAX_NESTING, ", ".join(deep)))
    R.append("\n## Games produced by splitting\n")
    for r in rew:
        if r.n_out > 1:
            R.append("**%s** (limit %d positions)\n" % (r.eco, r.limit))
            R.append("| # | positions | nesting | movetext bytes | starts after |")
            R.append("|---|---|---|---|---|")
            for s in r.stats:
                R.append("| %s | %d | %d | %d | %s |" % (s[0], s[2], s[3], s[4], s[1]))
            R.append("")
    R.append("## Text annotations converted to NAGs\n")
    if not any(r.conv for r in rew):
        R.append("- none")
    for r in rew:
        for c in r.conv:
            R.append("- %s: `%s` after the move in **%s** : `{%s}` -> %s%s" % (
                r.eco, c["uci"], r.g.edge_context((c["h_before"], c["uci"])),
                c["original"], " ".join("$%d" % n for n in c["nags"]),
                (" (comment kept as `{%s}`)" % c["remaining"]) if c["remaining"] else ""))
    if searcher is not None:
        R.append("\n## Stockfish decisions\n")
        R.extend("    " + ln for ln in tally_lines(results))
    R.append("\n## Conflicting marks\n")
    R.append("Where Stockfish decided, the resolution says so; otherwise the first "
             "occurrence was kept and the other was NOT written.\n")
    if not any(r.g.conflicts for r in rew):
        R.append("- none")
    console_conf = []
    for r in rew:
        for c in r.g.conflicts:
            label, fen0, fen1 = r.g.conflict_info(c)
            src = ("the dropped one came from a text symbol" if c["from_text"]
                   else "two explicit marks disagreed")
            R.append("- %s, %s on **%s**: first `%s`, other `%s` (%s)" % (
                r.eco, c["kind"], r.g.edge_context(c["edge"]),
                nm(c["kept"]), nm(c["dropped"]), src))
            R.append("  - move %s; FEN before: `%s`; FEN after: `%s`" % (label, fen0, fen1))
            if c.get("evidence"):
                R.append("  - evidence: %s" % c["evidence"])
            R.append("  - resolution: %s" % c.get("resolution", "first kept (--no-engine)"))
            console_conf.append((r.eco, label, fen0, c, src))
    if args.list_transpositions:
        R.append("\n## Transpositions merged\n")
        for r in rew:
            for h in r.transpositions:
                o = r.g.owner[h]
                others = [e for e, t in r.g.after.items() if t == h and e != o]
                R.append("- %s: kept at **%s**; also reached by %s" % (
                    r.eco, r.g.san_path(r.g.path_to(h)),
                    "; ".join("`%s`" % r.g.edge_context(e) for e in others)))
    R.append("\n## Headers of the original games where new headers were generated\n")
    for r in rew:
        if r.kept_headers:
            continue
        for h in r.old_headers:
            R.append("- %s: " % r.eco + ", ".join(
                "%s=%s" % kv for kv in h.items()
                if kv[0] in ("Event", "Site", "Date", "White", "Black", "Result",
                             "WhiteElo", "BlackElo")))
    with open(out_rep, "w", encoding="utf-8") as f:
        f.write("\n".join(R) + "\n")

    # console ---------------------------------------------------------------------
    for r in rew:
        print("  %-4s %3d -> %d games  repeats %d  transp %d  text->NAG %d  conflicts %d%s" % (
            r.eco, r.n_in, r.n_out, r.repeats, len(r.transpositions), len(r.conv),
            len(r.g.conflicts), "  (headers kept)" if r.kept_headers else ""))
    for r in skip:
        print("  %-4s SKIPPED (left unchanged): %s" % (r.eco, r.reason))
    if console_conf:
        print("conflicting marks - FEN before the move%s:" % (
            "" if len(console_conf) <= 25
            else " (first 25 of %d; all are in the report)" % len(console_conf)))
        for eco, label, fen0, c, src in console_conf[:25]:
            print("  %s %-10s %s  %s vs %s -> %s" % (
                eco, label, fen0, nm(c["kept"]), nm(c["dropped"]),
                c.get("resolution", "first kept (--no-engine)")))
    if searcher is not None and any(r.sf_decisions for r in results):
        print("\n".join(tally_lines(results)))
    print("[%%evp] comments removed: %d%s" % (
        evp_in - evp_left, "" if not evp_left else " (%d REMAIN)" % evp_left))
    print("codes: %d processed, %d rewritten, %d unchanged, %d skipped" % (
        len(results), len(rew), len(unch), len(skip)))
    print("wrote:\n  %s\n  %s" % (out_pgn, out_rep))
    return 1 if skip else 0


if __name__ == "__main__":
    sys.exit(main())
