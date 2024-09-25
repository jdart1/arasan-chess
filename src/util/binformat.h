// Copyright 2021-2024 by Jon Dart. All Rights Reserverd
#ifndef _BIN_FORMATS_H

#include "chess.h"
#include <fstream>

// support for binary position formats
class BinFormats {

public:    

    struct PositionData {
        std::string fen;
        score_t score;
        unsigned ply;
        unsigned move50Count;
        ColorType stm;
        Move move;
    };

    // "Bullet" is the "ChessBoard" format used in https://github.com/jw1912/bulletformat
    enum class Format {StockfishBin, Marlin, Bullet};

    static bool fromString(const std::string &str, Format &fmt);

    static std::string toString(const Format fmt);

    // Read data. Result is -1 for Black win, 0 for draw, 1 for White win.
    template<Format fmt>
    inline static bool write(const PositionData &pos, int resultVal, std::ostream &out) {
        switch(fmt) {
        case Format::StockfishBin:
            return writeBin(pos, resultVal, out);
        case Format::Marlin:
            return writeMarlin(pos, resultVal, out);
        case Format::Bullet:
            return writeBullet(pos, resultVal, out);
        default:
            assert(0);
            return false;
        }
    }

    // Write data. Result is -1 for Black win, 0 for draw, 1 for White win.
    template<Format fmt>
    inline static bool read(std::istream &in, int &result, PositionData &pos) {
        switch(fmt) {
        case Format::StockfishBin:
            return readBin(in, result, pos);
        default:
            assert(0);
            return false;
        }
    }

private:
    static bool writeBin(const PositionData &pos, int resultVal, std::ostream &out);

    static bool readBin(std::istream &in, int &result, PositionData &pos);

    static bool writeMarlin(const PositionData &pos, int result, std::ostream &out);

    static bool writeBullet(const PositionData &pos, int result, std::ostream &out);
};

#endif
