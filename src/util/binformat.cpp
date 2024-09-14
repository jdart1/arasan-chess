#include "binformat.h"

#include "boardio.h"
#include "stdendian.h"
#include <array>

using PosOutputType = std::array<uint8_t, 32>;

static inline unsigned encode_bit(PosOutputType &out, unsigned bit, unsigned pos) {
    if (bit) {
        out[pos / 8] |= (1 << (pos % 8));
    }
    return pos + 1;
}

static inline unsigned encode_bits(PosOutputType &out, unsigned bits, unsigned size, unsigned pos) {
    for (unsigned i = 0; i < size; i++) {
        pos = encode_bit(out, bits & (1 << i), pos);
    }
    return pos;
}

static unsigned encode_board(PosOutputType &out, const Board &b, unsigned pos) {
    static const std::array<unsigned, 6> HUFFMAN_TABLE = {0, 1, 3, 5, 7, 9};
    for (unsigned rank = 0; rank < 8; ++rank) {
        for (unsigned file = 0; file < 8; ++file) {
            Square sq = 56 - rank * 8 + file;
            const Piece &p = b[sq];
            if (IsEmptyPiece(p)) {
                ++pos; // 1 bit
            } else if (TypeOfPiece(p) != King) {
                assert(HUFFMAN_TABLE[TypeOfPiece(p)] <= 9);
                pos = encode_bits(out, HUFFMAN_TABLE[TypeOfPiece(p)], 4, pos);
                pos = encode_bit(out, PieceColor(p) == Black, pos);
            }
        }
    }
    return pos;
}

static uint16_t encode_move(ColorType side, const Move move) {
    Square from = StartSquare(move);
    Square to = DestSquare(move);
    uint16_t data = 0;
    switch (TypeOfMove(move)) {
    case KCastle:
        to = (side == White) ? chess::H1 : chess::H8;
        data |= 3 << 14;
        break;
    case QCastle:
        to = (side == White) ? chess::A1 : chess::A8;
        data |= 3 << 14;
        break;
    case Promotion:
        data |= (PromoteTo(move) - 2) << 12;
        data |= 1 << 14;
        break;
    case EnPassant:
        data |= 2 << 14;
        break;
    case Normal:
        break;
    default:
        break;
    }
    data |= to;
    data |= (from << 6);
    assert(from != to);
    assert(from < 64 && to < 64);
    return data;
}

// serialize as little-endian
template <typename T> static void serialize(std::ostream &o, const T &data) {
    switch (sizeof(T)) {
    case 1:
        o.write((char *)(&data), sizeof(T));
        break;
    case 2: {
        uint16_t out = swapEndian16(reinterpret_cast<const uint16_t *>(&data));
        o.write((char *)(&out), sizeof(T));
        break;
    }
    case 4: {
        uint32_t out = swapEndian32(reinterpret_cast<const uint32_t *>(&data));
        o.write((char *)(&out), sizeof(T));
        break;
    }
    default:
        std::cerr << "unsuppored size for .bin seralization" << std::endl;
        break;
    } // end switch
}

bool BinFormats::writeBin(const BinFormats::PositionData &data, int result, std::ostream &out) {
    PosOutputType posData;
    unsigned pos = 0;
    posData.fill(0);
    pos = 0; // bit count
    Board board;
    BoardIO::readFEN(board, data.fen);
    pos = encode_bit(posData, board.sideToMove() == Black, pos);
    pos = encode_bits(posData, board.kingSquare(White), 6, pos);
    pos = encode_bits(posData, board.kingSquare(Black), 6, pos);
    // Encode board (minus Kings)
    pos = encode_board(posData, board, pos);
    CastleType wcs = board.castleStatus(White);
    CastleType bcs = board.castleStatus(Black);
    pos = encode_bit(posData, wcs == CanCastleKSide || wcs == CanCastleEitherSide, pos);
    pos = encode_bit(posData, wcs == CanCastleQSide || wcs == CanCastleEitherSide, pos);
    pos = encode_bit(posData, bcs == CanCastleKSide || bcs == CanCastleEitherSide, pos);
    pos = encode_bit(posData, bcs == CanCastleQSide || bcs == CanCastleEitherSide, pos);
    Square epsq = board.enPassantSq();
    if (epsq == InvalidSquare) {
        ++pos;
    } else {
        // Arasan's internal en passant square is not what Stockfish/FEN
        // expect, correct here:
        Square target = (board.sideToMove() == White) ? epsq + 8 : epsq - 8;
        pos = encode_bit(posData, 1, pos);
        pos = encode_bits(posData, target, 6, pos);
    }
    // 6 bits for Stockfish compatibility:
    pos = encode_bits(posData, data.move50Count, 6, pos);
    // fullmove counter
    pos = encode_bits(posData, 2 * (data.ply / 2), 8, pos);
    // next 8 bits of fullmove counter
    pos = encode_bits(posData, (2 * (data.ply / 2)) >> 8, 8, pos);
    // 7th bit of 50-move counter
    pos = encode_bit(posData, data.move50Count >> 6, pos);
    assert(pos <= 256);
    // output position
    out.write(reinterpret_cast<const char *>(posData.data()), 32);
    // score. Note: Arasan scores are always from side to moves's POV.
    serialize<int16_t>(out, static_cast<int16_t>((data.score)));
    serialize<uint16_t>(out, encode_move(board.sideToMove(), data.move));
    serialize<uint16_t>(out, static_cast<uint16_t>(data.ply));
    serialize<int8_t>(out, static_cast<int8_t>(result));
    serialize<uint8_t>(out, static_cast<uint8_t>(0xff));
    if (out.bad()) {
        return false;
    }
    assert(out.tellp() % 40 == 0);
    return true;
}


