// Copyright 1996-2008, 2013, 2017, 2021 by Jon Dart. All Rights Reserved
#ifndef __CHESSIO_H__
#define __CHESSIO_H__

#include "types.h"
#include "epdrec.h"
#include "board.h"
#include "movearr.h"
#include <fstream>
#include <string>
#include <utility>
#include <vector>

// This class handles I/O of PGN, FEN and EPD files.
// Note: some functions are only used in the GUI.
//
class ChessIO
{
public:
    class Header : public std::pair<std::string, std::string> {
    public:
        Header() : std::pair<std::string, std::string>() {
        }
        Header(const std::string &tag, const std::string &val) :
            std::pair<std::string, std::string>(tag,val) {
        }
        const std::string &tag() const {
            return first;
        }
        const std::string &value() const {
            return second;
        }
    };

    // scan a PGN file, fill "contents" with strings.  Each string contains
    // a descriptive 1-line header followed by a tab and the file offset
    // of the game in decimal digits.
    static int scan_pgn(std::istream &ifs, std::vector<std::string> &contents);

    // Store a PGN file with the specified headers
    static int store_pgn(std::ostream &ofs, MoveArray &moves,
                         const ColorType computer_side,
                         const std::string &result,
	                 std::vector<Header> &headers);

    static int store_pgn(std::ostream &ofs, MoveArray &moves,
                         const std::string &result,
	                 std::vector<Header> &headers);

    static int load_fen(std::istream &ifs, Board &board);
    static int store_fen(std::ostream &ofs, const Board &board);

    // read a PGN game header from the file.
    static void collect_headers(std::istream &game_file,
                                std::vector<Header> &hdrs, long &first);

    // get a specific header out of a collection of headers (returned
    // from collect_headers)
    static int get_header(const std::vector<Header> &hdrs,
                          const std::string &key, std::string &val);

    // read an EPD record from a stream, return 1 if ok, 0 if EOF
    static int readEPDRecord(std::istream &ifs, Board &board, EPDRecord &out);

    // write an EPD record
    static void writeEPDRecord(std::ostream &ofs, Board &board, const EPDRecord &rec);

    enum TokenType { Number, GameMove, Result,
                     OpenVar,
                     CloseVar,
                     NAG,
                     BlackMove,
                     Comment, Unknown, Ignore, Eof };

    struct Token {
        TokenType type;
        std::string val;
        Token() :
            type(Unknown),val("") {
        }
        Token(TokenType t, std::string v) :
            type(t),val(v) {
        }
    };

    // read the next token from the "body" of a PGN game.
    static Token get_next_token(std::istream &game_file);

    // return (in descr) a single-line description of a game, based on
    // its PGN headers (for game list display).
    static void get_game_description(const std::vector<Header> &hdrs, std::string &descr, long id);
private:
    static void add_header(std::vector <Header> &hdrs,
                           const std::string &key, const std::string & val);


};

#endif
