// Copyright 2021-2024 by Jon Dart. All Rights Reserverd
#ifndef _BIN_FORMATS_H

#include "board.h"
#include "chess.h"
#include <array>
#include <cstdint>
#include <fstream>
#include <utility>
#include <vector>

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
    // "Text" is the bullet text format (FEN | score | result)
    // "Viri" is the game-based format used in https://github.com/cosmobobak/viriformat
    // (see the ViriGame class below, not the per-position write() method)
    enum class Format {StockfishBin, Marlin, Bullet, Text, Viri};

    static bool fromString(const std::string &str, Format &fmt);

    static std::string toString(const Format fmt);

    // Write data. Result is -1 for Black win, 0 for draw, 1 for White win.
    template<Format fmt>
    inline static bool write(const PositionData &pos, int resultVal, std::ostream &out) {
        switch(fmt) {
        case Format::StockfishBin:
            return writeBin(pos, resultVal, out);
        case Format::Marlin:
            return writeMarlin(pos, resultVal, out);
        case Format::Bullet:
            return writeBullet(pos, resultVal, out);
        case Format::Text:
            return writeText(pos, resultVal, out);
        case Format::Viri:
            // game-scoped, see ViriGame
            assert(0);
            return false;
        default:
            assert(0);
            return false;
        }
    }

    // Read data. Result is -1 for Black win, 0 for draw, 1 for White win.
    // TBD: no reader for Format::Viri yet.
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

    // Viriformat ("viribinpack"): a 32-byte marlinformat PackedBoard holding
    // the start position and the game result, followed by one 4-byte (Move,
    // White-relative score) pair per ply, then a 4-byte zero terminator. The
    // result is in the header, so plies are buffered until the game ends.
    // The move list must be a complete legal sequence: positions cannot be
    // omitted, so filtering of non-quiet positions is done by the trainer.
    class ViriGame {
    public:
        ViriGame() { moves.reserve(512); }

        // (Re)start the record at 'board', which is at game ply 'ply'.
        void restart(const Board &board, unsigned ply);

        // Drop the record. Used after a random move, whose successors we do
        // not want associated with the game result.
        void reset() noexcept {
            begun = false;
            moves.clear();
        }

        bool started() const noexcept { return begun; }

        std::size_t plies() const noexcept { return moves.size(); }

        // 'score' is side to move relative, as in PositionData.
        void addMove(const Board &board, Move m, score_t score);

        // Result is -1 for Black win, 0 for draw, 1 for White win.
        bool write(int result, std::ostream &out) const;

    private:
        bool begun = false;
        std::array<uint8_t, 32> header{};
        std::vector<std::pair<uint16_t, int16_t>> moves;
    };

private:
    static bool writeBin(const PositionData &pos, int resultVal, std::ostream &out);

    static bool readBin(std::istream &in, int &result, PositionData &pos);

    static bool writeMarlin(const PositionData &pos, int result, std::ostream &out);

    static bool writeBullet(const PositionData &pos, int result, std::ostream &out);

    static bool writeText(const PositionData &pos, int result, std::ostream &out);
};

#endif
