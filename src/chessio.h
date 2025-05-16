// Copyright 1996-2008, 2013, 2017, 2021, 2024 by Jon Dart. All Rights Reserved
#ifndef __CHESSIO_H__
#define __CHESSIO_H__

#include "board.h"
#include "epdrec.h"
#include "movearr.h"
#include "types.h"
#include <algorithm>
#include <fstream>
#include <queue>
#include <regex>
#include <string>
#include <utility>
#include <vector>

// This class handles I/O of PGN, FEN and EPD files.
// Note: some functions are only used in the GUI.
//
class ChessIO {
  public:
    class Header : public std::pair<std::string, std::string> {
      public:
        Header() : std::pair<std::string, std::string>() {}
        Header(const std::string &tag, const std::string &val)
            : std::pair<std::string, std::string>(tag, val) {}
        const std::string &tag() const { return first; }
        const std::string &value() const { return second; }
    };

    enum TokenType {
        Number,
        GameMove,
        Result,
        OpenVar,
        CloseVar,
        NAG,
        Comment,
        Unknown,
        Ignore,
        Eof
    };

    struct Token {
        TokenType type;
        std::string val;
        Token() : type(Unknown), val("") {}
        Token(TokenType t, std::string v) : type(t), val(v) {}
    };

    class PGNReader {
        friend class TokenReader;

      public:
        PGNReader(std::istream &s)
            : stream(s), HeaderPattern("^\\s*\\[([^\\s]+)\\s+\"([^\"]*)\"\\][\\s\\r\\n]*$") {}

        virtual ~PGNReader() = default;

        // collect headers from the file, return true if found
        // and also return the file offset of the first header.
        bool collectHeaders(std::vector<Header> &hdrs, long &first);

        // scan a PGN file, fill "contents" with strings.  Each string contains
        // a descriptive 1-line header followed by a tab and the file offset
        // of the game in decimal digits.
        bool scanPGN(std::vector<std::string> &contents);

        // get line from file, return false if EOL or error
        bool getline(std::string &line);

        void putback(const std::string &line) { buffer.push(line); }

        std::istream &stream;
        std::queue<std::string> buffer;
        std::regex HeaderPattern;
    };

    class TokenReader {
      public:
        TokenReader(PGNReader &f);

        virtual ~TokenReader() = default;

        // read the next token from the "body" of a PGN game.
        Token nextToken();

      private:
        PGNReader &file;
        size_t index;
        std::string buf;
        std::regex CastlePattern, MovePattern, DigitsPattern;

        int get() {
            bool ok = true;
            while (index >= buf.size() && (ok = file.getline(buf)))
                index = 0;
            if (!ok)
                return EOF;
            else
                return static_cast<int>(buf[index++]);
        }

        void getTextToNextDelim(std::string &s);
    };

    // Store a PGN file with the specified headers
    static bool store_pgn(std::ostream &ofs, MoveArray &moves, const ColorType computer_side,
                          const std::string &result, std::vector<Header> &headers);

    static bool store_pgn(std::ostream &ofs, MoveArray &moves, const std::string &result,
                          std::vector<Header> &headers);

    static bool load_fen(std::istream &ifs, Board &board);
    static bool store_fen(std::ostream &ofs, const Board &board);

    // read a PGN game header from the file.
    static void collect_headers(std::istream &game_file, std::vector<Header> &hdrs, long &first);

    // get a specific header out of a collection of headers (returned
    // from collect_headers)
    static bool get_header(const std::vector<Header> &hdrs, const std::string &key,
                           std::string &val);

    // remove a header, return true if removed
    static bool remove_header(std::vector<Header> &hdrs, const std::string &key);

    // replace a header, if present
    static void replace_header(std::vector<Header> &hdrs, const std::string &key,
                               const std::string &val);

    // read an EPD record from a stream, return true if OK
    static bool readEPDRecord(std::istream &ifs, Board &board, EPDRecord &out);

    // write an EPD record
    static void writeEPDRecord(std::ostream &ofs, Board &board, const EPDRecord &rec);

    // return (in descr) a single-line description of a game, based on
    // its PGN headers (for game list display).
    static void get_game_description(const std::vector<Header> &hdrs, std::string &descr, long id);

  private:
    static void add_header(std::vector<Header> &hdrs, const std::string &key,
                           const std::string &val);
};

#endif
