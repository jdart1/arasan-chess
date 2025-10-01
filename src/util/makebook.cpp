// Copyright 1996-2004, 2012-2025 by Jon Dart.  All Rights Reserved.

// Stand-alone executable to build the binary opening book from
// one or more PGN input files.

// The book file is a binary file consisting of a header followed
// by one or more "index pages" and then by one or more "data pages."
// These data structures are defined in bookdefs.h.

#include "board.h"
#include "boardio.h"
#include "bookdefs.h"
#include "bookwrit.h"
#include "bhash.h"
#include "chessio.h"
#include "movegen.h"
#include "notation.h"
#include "attacks.h"
#include "globals.h"
#include "scoring.h"

#include <algorithm>
#include <cassert>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <unordered_map>
#include <vector>

extern "C"{
#include <ctype.h>
#include <math.h>
};

enum class OutputType { Binary, Json };
enum ResultType {White_Win, Black_Win, DrawResult, UnknownResult};
ResultType tmp_result;

// number of pages in the book.
static unsigned indexPages = 1;                    // default
// max ply depth processed for PGN games
static bool verbose = false;
static OutputType output_type = OutputType::Binary;
static unsigned maxPly = 70;
// do not use indirectly computed position evaluations (by
// using the ends of lines and minimaxing) below this ply.
static constexpr unsigned POSITION_EVAL_MIN_PLY = 10;
// also do not use indirectly computed position evaluations
// on positions that have >= this number of occurances.
static constexpr unsigned POSITION_EVAL_MAX_POSITIONS = 100;

enum MoveEval {NO_MOVE_EVAL,
               BLUNDER_MOVE,POOR_MOVE,QUESTIONABLE_MOVE,NEUTRAL_EVAL,
               INTERESTING_MOVE,GOOD_MOVE,VERY_GOOD_MOVE};

enum PositionEval {NO_POSITION_EVAL,
                   BLACK_WINNING_ADVANTAGE,
                   BLACK_STRONG_ADVANTAGE,
                   BLACK_MODERATE_ADVANTAGE,
                   BLACK_ADVANTAGE,
                   EQUAL_POSITION,
                   WHITE_ADVANTAGE,
                   WHITE_MODERATE_ADVANTAGE,
                   WHITE_STRONG_ADVANTAGE,
                   WHITE_WINNING_ADVANTAGE};

static std::unordered_map<std::string, MoveEval> moveEvals;
static std::unordered_map<std::string, PositionEval> positionEvals;

static const uint8_t MoveEvalValues[] = { book::NO_RECOMMEND, 0, 0, book::MAX_WEIGHT/3,
                                       book::MAX_WEIGHT/2, 4*book::MAX_WEIGHT/6,
                                       5*book::MAX_WEIGHT/6, book::MAX_WEIGHT };

static std::string output_name;

class BookEntry;

struct MoveListEntry {
    Move move;
    int index;
    unsigned rec;
    MoveEval moveEval;
    // store state so moves are undoable
    BoardState state;
    MoveListEntry(): move(NullMove),
                     index(0),
                     rec(book::NO_RECOMMEND),
                     moveEval(NO_MOVE_EVAL) {
    }
};

struct Variation {
    Board save;
    std::vector<MoveListEntry> moves;
    ResultType result;
    PositionEval eval;
    int move_num; // at start of variation
    Variation() :  result(UnknownResult), eval(NO_POSITION_EVAL), move_num(0) {
    }
    Variation(const Board &b, int n) : save(b), result(UnknownResult), eval(NO_POSITION_EVAL), move_num(n) {
    }
    Move lastMove() const {
        return moves.size() == 0 ? NullMove : moves[moves.size()-1].move;
    }
};

static unsigned minFrequency = 0;

class BookEntry {
   public:
    BookEntry( ColorType side, unsigned ply,
               unsigned recommend, PositionEval ev,
               MoveEval mev, ResultType result,
               uint8_t mv_indx,
               BookEntry *nxt, bool isFirst);

    BookEntry *next;
    bool first;
    unsigned ply;
    PositionEval eval;
    MoveEval moveEval;
    unsigned rec; // explicit weight if any
    uint32_t win,loss,draw;
    uint8_t move_index;

    uint32_t count() const
    {
       return win + loss + draw;
    }

    void updateWinLoss( ColorType side, ResultType result );

    bool omitMove() const noexcept;

    uint8_t computeWeight() const noexcept;
};

class BookEntryJson : public BookEntry {
  public:
    BookEntryJson(Board b, unsigned p /*ply*/, ColorType side, unsigned recommend, PositionEval ev,
                  MoveEval mev, ResultType result, uint8_t idx, const std::string &move,
                  BookEntryJson *nxt, bool isFirst)
        : BookEntry(side, p, recommend, ev, mev, result, idx, nxt, isFirst) {
        strncpy(movestr, move.c_str(), 8);
        std::stringstream s;
        BoardIO::writeFEN(b, s, 1);
        fen = s.str();
    }

    char movestr[9];
    std::string fen;
};

BookEntry::BookEntry(ColorType side, unsigned p /*ply*/, unsigned recommend, PositionEval ev,
                     MoveEval mev, ResultType result, uint8_t mv_indx, BookEntry *nxt, bool isFirst)
    : next(nxt), first(isFirst), ply(p), eval(ev), moveEval(mev), rec(recommend), win(0), loss(0),
      draw(0), move_index(mv_indx) {
    updateWinLoss(side, result);
}

void BookEntry::updateWinLoss(ColorType side, ResultType result) {
    if (side == White) {
        if (result == White_Win) {
            ++win;
        } else if (result == Black_Win) {
            ++loss;
        } else {
            ++draw;
        }
    } else {
        if (result == Black_Win) {
            ++win;
        } else if (result == White_Win) {
            ++loss;
        } else {
            ++draw;
        }
    }
}

bool BookEntry::omitMove() const noexcept
{
    // omit moves that have no explicit weight, and are
    // marked as blunders or lead only to losses
    return (rec == book::NO_RECOMMEND && (moveEval == BLUNDER_MOVE ||
                                          loss == count()));
}

uint8_t BookEntry::computeWeight() const noexcept
{
   if (rec != book::NO_RECOMMEND) {
      return rec*book::MAX_WEIGHT/100;
   }
   else if (moveEval != NO_MOVE_EVAL) {
      return MoveEvalValues[moveEval];
   }
   else if (eval != NO_POSITION_EVAL && ply >= POSITION_EVAL_MIN_PLY &&
            count() <= POSITION_EVAL_MAX_POSITIONS) {
      int w = int(book::MAX_WEIGHT)/2;
      int intEval = (int)eval - (int)EQUAL_POSITION;
      int wmod = int(w*(1.0+intEval/4.0));
      return (uint8_t)std::max<int>(0,std::min<int>((int)book::MAX_WEIGHT,wmod));
   }
   else {
      return book::NO_RECOMMEND;
   }
}

static std::unordered_map <uint64_t, BookEntry *>* hashTable = nullptr;

// Add move to our in-memory data structure
static void
add_move(const Board & board, unsigned ply, const MoveListEntry &m, bool is_first_file,
         const Variation &var)
{
#ifdef _TRACE
   std::cout << "adding move ";
   Notation::image(board,m.move,Notation::OutputFormat::SAN,std::cout);
   std::cout << std::endl;
#endif
   const int move_index = m.index;
   const int recommend = m.rec;
   auto it = hashTable->find(board.hashCode());
   BookEntry *be;
   if (it == hashTable->end())
      be = nullptr;
   else
      be = (*it).second;
   if (be == nullptr) {
       // position not found in hashtable
       BookEntry *new_entry;
       try {
           if (output_type == OutputType::Json) {
               std::string movestr;
               Notation::image(board,m.move,Notation::OutputFormat::SAN,movestr);
               new_entry = new BookEntryJson(board, ply, board.sideToMove(), recommend,
                                    var.eval, m.moveEval, var.result,
                                    move_index, movestr, nullptr, is_first_file);
           } else {
               new_entry = new BookEntry(board.sideToMove(), ply, recommend,
                                    var.eval, m.moveEval, var.result,
                                    move_index, nullptr, is_first_file);
           }
       } catch(std::bad_alloc const &ex) {
          std::cerr << "out of memory!" << std::endl;
          exit(-1);
       }
      (*hashTable)[board.hashCode()] = new_entry;
#ifdef _TRACE
      std::cout << "inserting position: " <<
         " h:" << std::hex << Bitboard(board.hashCode()).hivalue() <<
         Bitboard(board.hashCode()).lovalue() << std::dec <<
         " index = " << (int)move_index << " from bk = " << (int)is_first_file <<
          " pos. eval=" << (int)var.eval << " moveEval=" << (int)m.moveEval << " recommend=" << (int)recommend << std::endl;
#endif
   }
   else {
      // update existing entry
      BookEntry *p = be;
      int found = 0;
      while (p && !found) {
         if (p->move_index == move_index) {
            ++found; break;
         }
         p = p->next;
      }
      if (found) {
         // Move already in hash table
         p->updateWinLoss(board.sideToMove(),var.result);
         if (m.rec != book::NO_RECOMMEND &&
             p->rec == book::NO_RECOMMEND) {
            // set explicit weight if not set already
            p->rec = m.rec;
         }
         if (m.moveEval != NO_MOVE_EVAL && p->moveEval == NO_MOVE_EVAL) {
            // set move eval if not set already
            p->moveEval = m.moveEval;
         }
#ifdef _TRACE
         std::cout << "updating: " <<
            " h:" << std::hex << Bitboard(board.hashCode()).hivalue() <<
            Bitboard(board.hashCode()).lovalue() << std::dec <<
            " index = " << (int)move_index <<
            " first = " << (int)is_first_file <<
            " win=" << p->win <<
            " loss=" << p->loss <<
            " draw=" << p->draw <<
            " rec=" << p->rec <<
            " eval=" << (int)p->eval <<
            " moveEval=" << (int)p->moveEval <<
            " count=" << p->count() << std::endl;
#endif
      }
      else {
         // Position is in hashtable, but not with this move.
#ifdef _TRACE
         std::cout << "inserting move: " <<
            " h:" << std::hex << Bitboard(board.hashCode()).hivalue() <<
            Bitboard(board.hashCode()).lovalue() << std::dec <<
            " index = " << (int)move_index << " first = " << (int)is_first_file <<
             " pos. eval =" << (int)var.eval << " move eval=" << (int)m.moveEval << " recommend=" << (int)recommend << std::endl;
#endif
         BookEntry *new_entry;
         try {
           if (output_type == OutputType::Json) {
               std::string movestr;
               Notation::image(board,m.move,Notation::OutputFormat::SAN,movestr);
               new_entry = new BookEntryJson(board, ply, board.sideToMove(), recommend,
                                             var.eval, m.moveEval, var.result,
                                             move_index, movestr, (BookEntryJson*)be, is_first_file);
           } else {
               new_entry = new BookEntry(board.sideToMove(), ply, recommend,
                                    var.eval, m.moveEval, var.result,
                                    move_index, be, is_first_file);
           }
         } catch(std::bad_alloc const &) {
            std::cerr << "out of memory!" << std::endl;
            exit(-1);
         }
         (*hashTable)[board.hashCode()] = new_entry;
      }
   }
}

// convert a move to an index based on the order the move generator
// provides. Return -1 if move is not in generated list, hence is illegal.
static int get_move_indx(const Board &board, Move move) {
    // check (pseudo) legality:
    MoveGenerator mg(board);
    int move_indx = -1;
    Move moves[Constants::MaxMoves];
    int n = mg.generateAllMoves(moves, 1 /* repeatable */);
    for (int i = 0; i < n; i++) {
      if (MovesEqual(moves[i],move)) {
          move_indx = i;
          break;
       }
    }
    return move_indx;
}

static void processVar(const Variation &var, bool first) {
    // Game has been processed, now add moves to book (we do this
    // only after seeing the whole variation, because we want the
    // end of line eval or result, if any).
#ifdef _TRACE
    std::cout << "process var: board=" << var.save << " hash=" << std::hex << var.save.hashCode() << std::dec << " eval=";
    if (var.eval == NO_POSITION_EVAL) {
        std::cout << "none";
    } else {
        std::cout << (int)var.eval-(int)EQUAL_POSITION;
    }
    std::cout << std::endl;
#endif
    if (var.moves.size()) {
        Board board(var.save);
        std::vector<MoveListEntry>::const_iterator it = var.moves.begin();
        unsigned ply = var.move_num*2;
        while (it != var.moves.end()) {
            const MoveListEntry &m = *it++;
            if (ply < maxPly || first) {
                add_move(board, ply, m, first, var);
            }
            ++ply;
            board.doMove(m.move);
        }
    }
}

static int do_pgn(std::ifstream &infile, const std::string &book_name, bool firstFile)
{
   std::vector<ChessIO::Header> hdrs;
   long games = 0L;
   ColorType side = White;
   ChessIO::PGNReader pgnReader(infile);
   while (!infile.eof() && infile.good()) {
      long first;
      side = White;
      ResultType last_result = UnknownResult;
      hdrs.clear();
      pgnReader.collectHeaders(hdrs,first);
      if (hdrs.size() == 0) break;
      ++games;
#ifdef _TRACE
      std::cout << "game " << games << std::endl;
#endif
      int move_num = 0;
      // process a game
      Board board;
      int var = 0;
      const int MAX_VAR = 20;
      Variation varStack[MAX_VAR];
      varStack[var++] = Variation(board,0);
      Variation &topVar = varStack[0];

      const auto commentRegex = std::regex("^\\{[\\n\\r\\s]*([\\S]*)[\\n\\r\\s]*\\}$");
      const auto weightRegex = std::regex("^weight:([\\d]+).*$");

      Board p1,p2;
      ChessIO::TokenReader tokenReader(pgnReader);
      for (;;) {
         std::string num;
         ChessIO::Token tok = tokenReader.nextToken();
         if (tok.type == ChessIO::Eof)
            break;
         else if (tok.type == ChessIO::OpenVar) {
             if (var >= MAX_VAR-1) {
                 std::cerr << "error: variation nesting limit reached" << std::endl;
                 continue;
             }
             // New variation is starting. Its start position is the current
             // board, -1 halfmove
             Move lastMove = varStack[var-1].lastMove();
             board.undoMove(lastMove,varStack[var-1].moves[
                                                           varStack[var-1].moves.size()-1].state);

             Variation newVar(board,move_num); // start of variation
             varStack[var++] = newVar;
             varStack[var-1] = varStack[var-1]; // new top of stack
             side = OppositeColor(side);
         }
         else if (var && tok.type == ChessIO::CloseVar) {
             const Variation &branchPoint = varStack[var-1];
             processVar(branchPoint,firstFile);
             if (var >= 2) {
                 Variation &parent = varStack[var-2];
                 // minimax child variation evals back to parent
                 if (parent.save.sideToMove() == branchPoint.save.sideToMove()) {
                     // maximize
                     parent.eval = (PositionEval)std::max<int>(
                                                           (int)parent.eval,
                                                           (int)branchPoint.eval);
                 } else {
                     // minimize
                     parent.eval = (PositionEval)std::min<int>(
                                                           (int)parent.eval,
                                                           (int)branchPoint.eval);
                 }
             }
             // reset board to start of variation
             board = branchPoint.save;
             --var;
             // now reapply last move from the parent variation
             // (main line move)
             assert(var);
             Move mainLine = varStack[var-1].moves[varStack[var-1].moves.size()\
-1].move;
             board.doMove(mainLine);
             side = board.sideToMove();
         }
         else if (tok.type == ChessIO::NAG) {
             // applies to the previous move or line
             auto it = moveEvals.find(tok.val);
             if (it != moveEvals.end()) {
                 // get last move and set its eval
                 varStack[var-1].moves[varStack[var-1].moves.size()-1].moveEval = (*it).second;
             }
             //check for position eval
             auto it2 = positionEvals.find(tok.val);
             if (it2 != positionEvals.end()) {
                 // associate it with the current variation
                 varStack[var-1].eval = (PositionEval)((*it2).second);
             }
         }
         else if (tok.type == ChessIO::Comment) {
            // strip braces and leading/trailing whitespace
            std::string comment = std::regex_replace(tok.val,commentRegex,"$1");
            // extract weight value if any
            std::smatch match;
            if (std::regex_match(comment,match,weightRegex) && match.size()) {
               std::stringstream s(match[1].str());
               int weight;
               s >> weight;
               if (s.bad() || s.fail()) {
                  std::cerr << "Warning: failed to parse weight comment: " << tok.val << ", ignored" << std::endl;
               }
               else {
                  // get last move and set its recommendation
                  if (weight >=0 && weight <=100) {
                      if (varStack[var-1].moves.size()) {
                          varStack[var-1].moves[varStack[var-1].moves.size()-1].rec = weight;
                      }
                      else {
                          std::cerr << "warning: misplaced weight comment, ignored" << std::endl;
                      }
                  }
                  else {
                     std::cerr << "Warning: invalid move weight: " << weight << ", ignored" << std::endl;
                  }
               }
            }
            else if (comment.length()>=3) {
                if (comment.substr(0,7) == "1/2-1/2" ||
                     comment.substr(0,3) == "\xBD-\xBD") {
                    varStack[var-1].result = DrawResult;
                }
                else if (comment.substr(0,3) == "1-0") {
                    varStack[var-1].result = White_Win;
                }
                else if (comment.substr(0,3) == "0-1") {
                    varStack[var-1].result = Black_Win;
                }
            }
         }
         else if (tok.type == ChessIO::Number) {
             num = tok.val;
             std::stringstream s(tok.val);
             s >> move_num;
         }
         else if (tok.type == ChessIO::GameMove) {
            // parse the move
             Move move = Notation::value(board,side,Notation::InputFormat::SAN,tok.val);
            if (IsNull(move)) {
                std::cerr << "Illegal move: " << tok.val <<
                   " in game " << games << ", file " <<
                   book_name << std::endl;
                return -1;
            }
            else {
                int move_indx = get_move_indx(board,move);
                if (move_indx == -1) {
                    std::cerr << "Illegal move: " << tok.val <<
                        " in game " << games << ", file " <<
                        book_name << std::endl;
                    return -1;
                } else {
                    MoveListEntry m;
                    m.move = move;
                    m.index = move_indx;
                    m.state = board.state;
#ifdef _TRACE
                    std::cout << "adding to stack " << var-1 << " ";
                    MoveImage(move,std::cout);
                    std::cout << std::endl;
#endif
                    varStack[var-1].moves.push_back(m);
#ifdef _TRACE
                    std::cout << " size=" << varStack[var-1].moves.size() << std::endl;

#endif
                }
            }
            p2 = p1;
            p1 = board;
            board.doMove(move);
            side = OppositeColor(side);
         }
         else if (tok.type == ChessIO::Unknown) {
            std::cerr << "Unrecognized text: " << tok.val <<
                         " in game " << games << ", file " <<
                          book_name << std::endl;
         }
         else if (tok.type == ChessIO::Result) {
            if (tok.val == "1/2-1/2")
               last_result = DrawResult;
            else if (tok.val=="0-1" || tok.val=="0:1")
               last_result = Black_Win;
            else if (tok.val == "1-0" || tok.val == "1:0")
               last_result = White_Win;
            else
                last_result = UnknownResult;
            topVar.result = last_result;
            break;
         }
      }
      processVar(topVar,firstFile);
      --var;
      assert(var == 0);
   }
   return 0;
}

static void usage() {
    std::cerr << "Usage:" << std::endl;
    std::cerr << "makebook -n <pages> -p <max play> -h <hash size>" << std::endl;
    std::cerr << "         -t <binary | json>" << std::endl;
    std::cerr << "         -m <min frequency> -o <output file> <input file(s)>" << std::endl;
}

int CDECL main(int argc, char **argv)
{
   std::string book_name;

   BitUtils::init();
   Board::init();
   globals::initOptions();
   Attacks::init();
   if (!globals::initGlobals()) {
      globals::cleanupGlobals();
      exit(-1);
   }
   atexit(globals::cleanupGlobals);

   hashTable =  new std::unordered_map< uint64_t, BookEntry*>;
   moveEvals.insert(std::pair<std::string,MoveEval>("$1",GOOD_MOVE));
   moveEvals.insert(std::pair<std::string,MoveEval>("$2",POOR_MOVE));
   moveEvals.insert(std::pair<std::string,MoveEval>("$3",VERY_GOOD_MOVE));
   moveEvals.insert(std::pair<std::string,MoveEval>("$4",BLUNDER_MOVE));
   moveEvals.insert(std::pair<std::string,MoveEval>("$5",INTERESTING_MOVE));
   moveEvals.insert(std::pair<std::string,MoveEval>("$6",QUESTIONABLE_MOVE));

   positionEvals.insert(std::pair<std::string,PositionEval>("$21",BLACK_WINNING_ADVANTAGE));
   positionEvals.insert(std::pair<std::string,PositionEval>("$19",BLACK_STRONG_ADVANTAGE));
   positionEvals.insert(std::pair<std::string,PositionEval>("$17",BLACK_MODERATE_ADVANTAGE));
   positionEvals.insert(std::pair<std::string,PositionEval>("$15",BLACK_ADVANTAGE));
   positionEvals.insert(std::pair<std::string,PositionEval>("$10",EQUAL_POSITION));
   positionEvals.insert(std::pair<std::string,PositionEval>("$11",EQUAL_POSITION));
   positionEvals.insert(std::pair<std::string,PositionEval>("$12",EQUAL_POSITION));
   positionEvals.insert(std::pair<std::string,PositionEval>("$14",WHITE_ADVANTAGE));
   positionEvals.insert(std::pair<std::string,PositionEval>("$16",WHITE_MODERATE_ADVANTAGE));
   positionEvals.insert(std::pair<std::string,PositionEval>("$18",WHITE_STRONG_ADVANTAGE));
   positionEvals.insert(std::pair<std::string,PositionEval>("$20",WHITE_WINNING_ADVANTAGE));

   output_name = "";
   int arg = 1;
   while (arg < argc) {
      if (*argv[arg] == '-') {
         char c = argv[arg][1];
         switch(c) {
            case 'p': /* max ply */
               ++arg;
               maxPly = atoi(argv[arg]);
               if (maxPly == 0) {
                  std::cerr << "Illegal max ply (-p) value" << std::endl;
                  exit(-1);
               }
               break;
            case 'o':                             /* output file name */
               ++arg;
               output_name = argv[arg];
               break;
            case 'n':                             /* number of pages */
               ++arg;
               indexPages = atoi(argv[arg]);
               if (indexPages == 0) {
                  std::cerr << "Illegal size specified for -n" << std::endl;
                  exit(-1);
               }
               break;
            case 'm':
               ++arg;
               minFrequency = (unsigned)atoi(argv[arg]);
               break;
            case 't':
               ++arg;
               if (strcmp(argv[arg],"binary") == 0)
                   output_type = OutputType::Binary;
               else if (strcmp(argv[arg],"json") == 0)
                   output_type = OutputType::Json;
               else {
                   std::cerr << "Invalid output type: " << argv[arg] << std::endl;
                   exit(-1);
               }
               break;
            case 'v':
                verbose = true;
                break;
            default:
               std::cerr << "Illegal switch: " << c << std::endl;
               usage();
               exit(-1);
         }
         ++arg;
      }
      else
         break;
   }

   if (output_name == "") {
      output_name = "book.bin";
   }
   if (arg >= argc) {
       std::cerr << "No book input files specified." << std::endl;
       usage();
       return -1;
   }

   bool first = true;
   while (arg < argc) {
      book_name = argv[arg++];
      std::ifstream infile;

      infile.open(book_name.c_str(), std::ios::in);
      if (!infile.good()) {
         std::cerr << "Can't open input file: " << book_name << std::endl;
         return -1;
      }
      if (verbose) std::cerr << "processing " << book_name << std::endl;
      int result = do_pgn(infile, book_name, first);
      first = false;
      infile.close();
      if (result == -1) break;
   }

   // Iterate through the hash table, picking out moves that meet
   // the "minFrequency" test.
   if (verbose) std::cerr << "PGN processing complete." << std::endl;
   uint32_t total_moves = 0;
   unsigned long positions = 0;
   if (output_type == OutputType::Binary) {
       BookWriter writer(indexPages);
       for (const auto &it : *hashTable) {
           // Note: it.first is the hash code
#ifdef _TRACE
           std::cout << "h:" << std::hex << it.first << std::dec << std::endl;
#endif
           int added = 0;
           for (auto be = (BookEntry*)it.second; be != nullptr; be = be->next) {
               if ((be->count() >= minFrequency) || be->first) {
                   try {
                       if (!be->omitMove()) {
                               writer.add(it.first, be->move_index,
                                  be->computeWeight(), be->win, be->loss, be->draw);
                               ++added;
                       }
                   } catch(BookFullException &ex) {
                       std::cerr << ex.what() << std::endl;
                       return -1;
                   }
                   total_moves++;
               }
           }
           if (added) ++positions;
       }
       if (verbose) std::cerr << "writing .." << std::endl;
       if (writer.write(output_name.c_str())) {
           std::cerr << "error writing book" << std::endl;
           return -1;
       }
       std::cerr << "book capacity is about " << indexPages*book::INDEX_PAGE_SIZE << " positions." << std::endl;
   } else {
       for (const auto &it : *hashTable) {
           BookEntryJson* be = (BookEntryJson*)it.second;
           // Note: it.first is the hash code
#ifdef _TRACE
           std::cout << "h:" << std::hex << it.first << std::dec << std::endl;
#endif
           int added = 0;
           std::vector<std::string> json_moves;
           std::string fen = be->fen;
           for (be = (BookEntryJson*)it.second; be != nullptr; be = (BookEntryJson*)be->next) {
                if ((be->count() >= minFrequency) || be->first) {
                   ++added;
                   std::stringstream s;
                   s << "{\"san\":\"" <<
                       be->movestr << "\",\"win\":" <<
                       be->win << ",\"loss\":" << be->loss <<
                       ",\"draw\":" << be->draw;
                   int weight = be->computeWeight();
                   if (weight != book::NO_RECOMMEND) {
                       std::ios_base::fmtflags original_flags = std::cout.flags();
                       s << std::setprecision(2);
                       s << ",\"weight\":" << 100.0*weight/book::MAX_WEIGHT;
                       s.flags(original_flags);
                   }
                   s << "}";
                   json_moves.push_back(s.str());
                   total_moves++;
               }
           }
           if (json_moves.size()>0) {
               std::cout << "{\"fen\":\"" << fen << "\",\"moves\":[";
               std::cout << std::flush;
               for (unsigned i = 0; i<json_moves.size(); i++) {
                   std::cout << json_moves[i];
                   if (i+1 < json_moves.size()) {
                       std::cout << ',';
                   }
               }
               std::cout << "]}" << std::endl;
           }
           if (added) ++positions;
       }
   }
   std::cerr << positions << " positions, " << total_moves << " total moves in book." << std::endl;
   return 0;
}
