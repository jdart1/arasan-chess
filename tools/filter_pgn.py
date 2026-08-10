#!/usr/bin/env python3
# Copyright 2026 by Jon Dart. All Rights Reserved.

"""Filter PGN files, keeping only standard, non-blitz, normally terminated
games with a long enough time control.

Like util/sortpgn.cpp, this does no real PGN parsing: it only examines the
header lines of each game and copies the game text through verbatim.

Criteria for skipping a game:
  - a "Variant" header whose value is not "Standard"
  - an "Event" header containing "Blitz", "blitz", "Titled" or "3-0"
  - an "EventType" header containing "Blitz" or "blitz"
  - a "Termination" header whose value is not "Normal"
  - a "Result" header whose value is "*"
  - a "TimeControl" header of the form "base[+increment]" for which
    base + increment/3 < 600 seconds
"""

import argparse
import re
import sys

# Same header pattern used by ChessIO (see src/chessio.h).
HEADER_PATTERN = re.compile(r'^\s*\[([^\s]+)\s+"([^"]*)"\]\s*$')

EVENT_EXCLUDE = ("Blitz", "blitz", "Titled", "3-0")

EVENT_TYPE_EXCLUDE = ("Blitz", "blitz")

DEFAULT_MIN_TIME = 600.0


def read_games(fh):
    """Yield (headers, header_lines, move_lines) for each game in fh.

    Line terminators are stripped; headers is a dict of tag -> value for
    the last occurrence of each tag.
    """
    headers = {}
    header_lines = []
    move_lines = []
    in_moves = False

    def flush():
        return (headers, header_lines, move_lines)

    for raw in fh:
        line = raw.rstrip("\r\n")
        stripped = line.strip()
        match = HEADER_PATTERN.match(line) if stripped.startswith("[") else None
        if match:
            if in_moves or (move_lines and not header_lines):
                # a new game starts without an intervening blank line
                yield flush()
                headers, header_lines, move_lines, in_moves = {}, [], [], False
            headers[match.group(1)] = match.group(2)
            header_lines.append(line)
        elif not stripped:
            if in_moves:
                # blank line after the moves ends the game
                yield flush()
                headers, header_lines, move_lines, in_moves = {}, [], [], False
            # blank lines before or between the header and move sections
            # are separators and are not retained
        else:
            in_moves = True
            move_lines.append(line)

    if header_lines or move_lines:
        yield flush()


def parse_time_control(value):
    """Return (base, increment) in seconds, or None if not parseable.

    Handles the common forms "600", "600+5", "40/7200", "40/7200:1800"
    and "*180". Only the first period of a multi-period control is used.
    """
    value = value.strip()
    if not value or value in ("-", "?"):
        return None
    period = value.split(":")[0].strip()
    if period.startswith("*"):  # sandclock
        period = period[1:]
    if "/" in period:  # moves/seconds
        period = period.split("/", 1)[1]
    parts = period.split("+")
    if len(parts) > 2:
        return None
    try:
        base = float(parts[0])
        inc = float(parts[1]) if len(parts) == 2 else 0.0
    except ValueError:
        return None
    return (base, inc)


def skip_reason(headers, min_time, skip_unknown_tc):
    """Return a short reason string if the game should be skipped, else None."""
    variant = headers.get("Variant")
    if variant is not None and variant.strip().lower() != "standard":
        return "variant"

    event = headers.get("Event", "")
    if any(s in event for s in EVENT_EXCLUDE):
        return "event"

    event_type = headers.get("EventType", "")
    if any(s in event_type for s in EVENT_TYPE_EXCLUDE):
        return "event type"

    termination = headers.get("Termination")
    if termination is not None and termination.strip().lower() != "normal":
        return "termination"

    if headers.get("Result", "").strip() == "*":
        return "result"

    tc = headers.get("TimeControl")
    if tc is not None:
        parsed = parse_time_control(tc)
        if parsed is None:
            if skip_unknown_tc:
                return "unparsed time control"
        else:
            base, inc = parsed
            if base + inc / 3 < min_time:
                return "time control"

    return None


def write_game(out, header_lines, move_lines):
    for line in header_lines:
        out.write(line + "\n")
    if header_lines and move_lines:
        out.write("\n")
    for line in move_lines:
        out.write(line + "\n")
    out.write("\n")


def filter_file(fh, out, args, counts):
    for headers, header_lines, move_lines in read_games(fh):
        if not header_lines and not move_lines:
            continue
        counts["read"] += 1
        reason = skip_reason(headers, args.min_time, args.skip_unknown_tc)
        if reason:
            counts[reason] = counts.get(reason, 0) + 1
            counts["skipped"] += 1
        else:
            counts["kept"] += 1
            write_game(out, header_lines, move_lines)


def main():
    parser = argparse.ArgumentParser(
        description="Filter PGN files by Variant, Event, EventType, "
        "Termination, Result and TimeControl headers.")
    parser.add_argument("files", nargs="*", metavar="pgn_file",
                        help="input PGN file(s); stdin if none given")
    parser.add_argument("-o", "--output", metavar="FILE",
                        help="output file (default: stdout)")
    parser.add_argument("-m", "--min-time", type=float,
                        default=DEFAULT_MIN_TIME, metavar="SECONDS",
                        help="minimum base + increment/3, in seconds "
                        "(default: %(default)s)")
    parser.add_argument("--skip-unknown-tc", action="store_true",
                        help="skip games whose TimeControl header cannot be "
                        "parsed (default: keep them)")
    parser.add_argument("-q", "--quiet", action="store_true",
                        help="do not write a summary to stderr")
    args = parser.parse_args()

    counts = {"read": 0, "kept": 0, "skipped": 0}
    out = sys.stdout
    outfile = None
    try:
        if args.output:
            outfile = open(args.output, "w", encoding="utf-8", newline="\n")
            out = outfile
        if args.files:
            for name in args.files:
                # utf-8-sig discards a byte order mark if present
                with open(name, "r", encoding="utf-8-sig",
                          errors="replace") as fh:
                    filter_file(fh, out, args, counts)
        else:
            filter_file(sys.stdin, out, args, counts)
    except BrokenPipeError:
        return 0
    except OSError as ex:
        sys.stderr.write("error: %s\n" % ex)
        return 1
    finally:
        if outfile:
            outfile.close()

    if not args.quiet:
        sys.stderr.write("games read: %d, kept: %d, skipped: %d\n" %
                         (counts["read"], counts["kept"], counts["skipped"]))
        for reason in sorted(k for k in counts
                             if k not in ("read", "kept", "skipped")):
            sys.stderr.write("  skipped (%s): %d\n" % (reason, counts[reason]))
    return 0


if __name__ == "__main__":
    sys.exit(main())
