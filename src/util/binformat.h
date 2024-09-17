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

    // TBD: other formats
    enum class Format {StockfishBin};

    template<Format fmt>
    inline static bool write(const PositionData &pos, int resultVal, std::ostream &out) {
        switch(fmt) {
        case Format::StockfishBin:
            return writeBin(pos, resultVal, out);
        default:
            assert(0);
            return false;
        }
    }

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
};

#endif
