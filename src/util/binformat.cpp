// Copyright 2021-2024 by Jon Dart. All Rights Reserved.
#include "binformat.h"

#include "boardio.h"
#include "chessio.h"
#include "stdendian.h"
#include <algorithm>
#include <array>
#include <sstream>
#include <unordered_map>

using PackedPosition = std::array<uint8_t, 32>;

static const std::array<unsigned, 6> HUFFMAN_TABLE = {0, 1, 3, 5, 7, 9};

#if _BYTE_ORDER == _BIG_ENDIAN
static uint16_t to_little_endian16(uint16_t x) { return bswap16(x); }
static uint32_t to_little_endian32(uint32_t x) { return bswap32(x); }
static uint64_t to_little_endian64(uint64_t x) { return bswap64(x); }
#else
static uint16_t to_little_endian16(uint16_t x) { return (x); }
static uint32_t to_little_endian32(uint32_t x) { return (x); }
static uint64_t to_little_endian64(uint64_t x) { return (x); }
#endif

// borrowed from nnue reading code
template <typename T> T read_little_endian(std::istream &s) {
    char buf[sizeof(T)];
    s.read(buf, sizeof(T));
    T input = *(reinterpret_cast<T *>(buf));
    switch (sizeof(T)) {
    case 1:
        return static_cast<T>(input);
    case 2:
        return static_cast<T>(to_little_endian16(input));
    case 4:
        return static_cast<T>(to_little_endian32(input));
    case 8:
        return static_cast<T>(to_little_endian64(input));
    default:
        throw std::invalid_argument("unsupported size");
    }
}

static inline unsigned encode_bit(PackedPosition &out, unsigned bit, unsigned pos) {
    if (bit) {
        out[pos / 8] |= (1 << (pos % 8));
    }
    return pos + 1;
}

static inline unsigned encode_bits(PackedPosition &out, unsigned bits, unsigned size,
                                   unsigned pos) {
    for (unsigned i = 0; i < size; i++) {
        pos = encode_bit(out, bits & (1 << i), pos);
    }
    return pos;
}

static inline unsigned decode_bit(const PackedPosition &in, unsigned &bit, unsigned pos) {
    bit = (in[pos / 8] & (1 << (pos % 8))) != 0;
    return pos + 1;
}

static inline unsigned decode_bits(const PackedPosition &in, unsigned &bits, unsigned size,
                                   unsigned pos) {
    bits = 0;
    for (unsigned i = 0; i < size; i++) {
        unsigned bit;
        pos = decode_bit(in, bit, pos);
        bits |= bit << i;
    }
    return pos;
}

static unsigned encode_board(PackedPosition &out, const Board &b, unsigned pos) {
    for (unsigned rank = 0; rank < 8; ++rank) {
        for (unsigned file = 0; file < 8; ++file) {
            Square sq = 56 - rank * 8 + file;
            const Piece &p = b[sq];
            if (IsEmptyPiece(p) || TypeOfPiece(p) == King) {
                ++pos; // 1 bit
            } else {
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

static unsigned decode_piece(const PackedPosition &in, Piece &p, unsigned pos) {
    unsigned code = 0, bits = 0, bit;
    PieceType pt = Empty;
    bool found = false;
    while (!found && bits < 4) {
        assert(pos < 256);
        pos = decode_bit(in, bit, pos);
        code |= (bit << bits++);
        if (code == 0 && bits == 1) { // empty piece
            found = true;
            break;
        } else if (bits == 4) {
            // note: Kings are not encoded this way
            for (unsigned pieceCode = 0; pieceCode <= 5; ++pieceCode) {
                if (HUFFMAN_TABLE[pieceCode] == code) {
                    pt = static_cast<PieceType>(pieceCode);
                    found = true;
                    break;
                }
            }
        }
    }
    assert(found);
    unsigned color = 0;
    if (pt != Empty) {
        pos = decode_bit(in, color, pos);
    }
    p = MakePiece(pt, color == 1 ? Black : White);
    return pos;
}

static unsigned decode_board(const PackedPosition &in, Board &board, unsigned pos) {
    for (unsigned rank = 0; rank < 8; ++rank) {
        for (unsigned file = 0; file < 8; ++file) {
            Square sq = 56 - rank * 8 + file;
            Piece p;
            pos = decode_piece(in, p, pos);
            board.setContents(p, sq);
        }
    }
    return pos;
}

static void decode_move(uint16_t encodedMove, const Board &board, Move &m) {
    Square to = static_cast<Square>(encodedMove & 0b111111);
    Square from = static_cast<Square>((encodedMove >> 6) & 0b111111);
    PieceType promotion = Empty;
    if (TypeOfPiece(board[from]) == Pawn && Rank(to, board.sideToMove()) == 8) {
        promotion = static_cast<PieceType>(((encodedMove >> 12) & 0x3) + 2);
    }
    if (TypeOfPiece(board[from]) == King) {
        // Stockfish encodes castling differently
        if (from == chess::E1 && to == chess::H1)
            to = chess::G1;
        else if (from == chess::E1 && to == chess::A1)
            to = chess::C1;
        else if (from == chess::E8 && to == chess::H8)
            to = chess::G8;
        else if (from == chess::E8 && to == chess::A8)
            to = chess::C8;
    }
    m = CreateMove(board, from, to, promotion);
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
    case 8: {
        uint64_t out = swapEndian64(reinterpret_cast<const uint64_t *>(&data));
        o.write((char *)(&out), sizeof(T));
        break;
    }
    default:
        std::cerr << "unsuppored size for .bin seralization" << std::endl;
        break;
    } // end switch
}

// deserialize from little-endian
template <typename T> static void deserialize(std::istream &in, T &data) {
    char *target = reinterpret_cast<char *>(&data);
    (void)in.read(target, sizeof(T));
    switch (sizeof(T)) {
    case 1:
        break;
    case 2: {
        data = swapEndian16(reinterpret_cast<const T *>(&data));
        break;
    }
    case 4: {
        data = swapEndian32(reinterpret_cast<const T *>(&data));
        break;
    }
    case 8: {
        data = swapEndian64(reinterpret_cast<const T *>(&data));
        break;
    }
    default:
        std::cerr << "unsupported size for .bin seralization" << std::endl;
        break;
    } // end switch
}

static const std::array<std::string,5> format_names = {"bin", "marlin", "bullet", "text", "viri"};

bool BinFormats::fromString(const std::string &s, Format &format) {
    size_t i = 0;
    for (const std::string &name : format_names) {
        if (name == s) {
            format = static_cast<BinFormats::Format>(i);
            return true;
        }
        ++i;
    }
    return false;
}

std::string BinFormats::toString(const BinFormats::Format fmt) {
    return format_names[static_cast<int>(fmt)];
}

bool BinFormats::writeBin(const BinFormats::PositionData &data, int result, std::ostream &out) {
    PackedPosition posData;
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
    pos = encode_bit(posData, board.canCastleKSide(White), pos);
    pos = encode_bit(posData, board.canCastleQSide(White), pos);
    pos = encode_bit(posData, board.canCastleKSide(Black), pos);
    pos = encode_bit(posData, board.canCastleQSide(Black), pos);
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
    // output position
    out.write(reinterpret_cast<const char *>(posData.data()), 32);
    // score. Note: Arasan scores are always from side to moves's POV.
    serialize<int16_t>(out, static_cast<int16_t>((data.score)));
    serialize<uint16_t>(out, encode_move(board.sideToMove(), data.move));
    serialize<uint16_t>(out, static_cast<uint16_t>(data.ply));
    // result is from White perspective
    serialize<int8_t>(out, static_cast<int8_t>(result));
    serialize<uint8_t>(out, static_cast<uint8_t>(0xff));
    if (out.bad()) {
        return false;
    }
    assert(out.tellp() % 40 == 0);
    return true;
}

bool BinFormats::readBin(std::istream &in, int &result, PositionData &out) {
    PackedPosition posData;
    size_t count = 0;
    for (; count < 32 && !in.fail(); ++count) {
        posData[count] = read_little_endian<uint8_t>(in);
    }
    if (in.fail())
        return false;
    assert(count == 32);
    int pos = 0;
    unsigned stm, whiteKing, blackKing;
    Board board;
    pos = decode_bit(posData, stm, pos);
    out.stm = stm ? Black : White;
    board.setSideToMove(stm ? Black : White);
    pos = decode_bits(posData, whiteKing, 6, pos);
    pos = decode_bits(posData, blackKing, 6, pos);
    pos = decode_board(posData, board, pos);
    board.setContents(WhiteKing, static_cast<Square>(whiteKing));
    board.setContents(BlackKing, static_cast<Square>(blackKing));
    board.setSecondaryVars();
    unsigned wKingCastle, bKingCastle, wQueenCastle, bQueenCastle;
    pos = decode_bit(posData, wKingCastle, pos);
    pos = decode_bit(posData, wQueenCastle, pos);
    pos = decode_bit(posData, bKingCastle, pos);
    pos = decode_bit(posData, bQueenCastle, pos);
    static CastleType CastleTypeMap[] = {CantCastleEitherSide, CanCastleKSide, CanCastleQSide,
                                         CanCastleEitherSide};
    board.setCastleStatus(CastleTypeMap[wKingCastle + (wQueenCastle << 1)], White);
    board.setCastleStatus(CastleTypeMap[bKingCastle + (bQueenCastle << 1)], Black);
    unsigned ep, epCode;
    pos = decode_bit(posData, ep, pos);
    board.state.enPassantSq = InvalidSquare;
    if (ep) {
        pos = decode_bits(posData, epCode, 6, pos);
        Square ep_candidate = static_cast<Square>(epCode) - (8 * Direction[board.sideToMove()]);
        // only actually set the e.p. square on the board if an en-passant capture
        // is truly possible:
        if (Attacks::ep_mask[File(ep_candidate) - 1][(int)board.oppositeSide()] &
            board.pawn_bits[board.sideToMove()]) {
            board.state.enPassantSq = ep_candidate;
        }
    }
    unsigned move50ExtraBit, fullMoveCounterLo, fullMoveCounterHi;
    pos = decode_bits(posData, out.move50Count, 6, pos);
    // fullmove counter (not actually used)
    pos = decode_bits(posData, fullMoveCounterLo, 8, pos);
    pos = decode_bits(posData, fullMoveCounterHi, 8, pos);
    // 7th bit of 50-move counter
    pos = decode_bit(posData, move50ExtraBit, pos);
    out.move50Count += (move50ExtraBit << 6);
    assert(pos < 256);
    int16_t score;
    uint16_t encodedMove, ply;
    int8_t res;
    deserialize<int16_t>(in, score);
    if (in.fail())
        return false;
    deserialize<uint16_t>(in, encodedMove);
    if (in.fail())
        return false;
    deserialize<uint16_t>(in, ply);
    if (in.fail())
        return false;
    deserialize<int8_t>(in, res);
    if (in.fail())
        return false;
    // read and discard the padding byte
    uint8_t padding;
    deserialize<uint8_t>(in, padding);
    assert(in.eof() || in.tellg() % 40 == 0);
    out.score = static_cast<score_t>(score);
    decode_move(encodedMove, board, out.move);
    out.ply = static_cast<unsigned>(ply);
    result = static_cast<int>(res);
    std::stringstream s;
    BoardIO::writeFEN(board, s, false);
    out.fen = s.str();
    return true;
}

// marlinformat "PackedBoard", shared by Format::Marlin and Format::Viri.
// 'whiteScore' and 'wdl' are from White's perspective, 'wdl' being 0 for a
// Black win, 1 for a draw and 2 for a White win. Bytes are emitted explicitly
// little-endian.
static void packMarlinBoard(const Board &board, int16_t whiteScore, uint8_t wdl, unsigned ply,
                            std::array<uint8_t, 32> &out) {
    out.fill(0);
    const uint64_t occupied = static_cast<uint64_t>(board.allOccupied);
    for (unsigned i = 0; i < 8; ++i) {
        out[i] = static_cast<uint8_t>(occupied >> (8 * i));
    }
    // 32 4-bit piece codes, in ascending square order
    Bitboard occ(board.allOccupied);
    Square sq;
    unsigned idx = 0;
    while ((sq = occ.firstOne()) != InvalidSquare) {
        occ.clear(sq);
        const Piece p = board[sq];
        assert(TypeOfPiece(p) != Empty);
        unsigned pieceCode = static_cast<unsigned>(TypeOfPiece(p)) - 1;
        // This allows encoding castling status.
        static constexpr unsigned UnMovedRook = 6;
        if (TypeOfPiece(p) == Rook) {
            if (ColorOfPiece(p) == White) {
                if (((sq == chess::A1) && board.canCastleQSide(White)) ||
                    ((sq == chess::H1) && board.canCastleKSide(White))) {
                    pieceCode = UnMovedRook;
                }
            } else {
                if (((sq == chess::A8) && board.canCastleQSide(Black)) ||
                    ((sq == chess::H8) && board.canCastleKSide(Black))) {
                    pieceCode = UnMovedRook;
                }
            }
        }
        pieceCode |= static_cast<unsigned>(ColorOfPiece(p)) << 3;
        out[8 + idx / 2] |= static_cast<uint8_t>(pieceCode << (4 * (idx & 1)));
        ++idx;
    }
    // 7-bit enpassant target square (64 if none) + side to move
    unsigned target = 64;
    const Square epsq = board.state.enPassantSq;
    if (epsq != InvalidSquare) {
        // Arasan's internal en passant square is the square of the pawn that
        // just moved two squares, not the square behind it.
        target = (board.sideToMove() == White) ? epsq + 8 : epsq - 8;
    }
    out[24] = static_cast<uint8_t>((target & 0x7f) |
                                   (board.sideToMove() == White ? 0 : 0x80));
    out[25] = static_cast<uint8_t>(board.state.moveCount);
    // The reader recovers the game ply from this, so it must be accurate and
    // nonzero.
    const uint16_t fullMove = static_cast<uint16_t>(ply / 2 + 1);
    out[26] = static_cast<uint8_t>(fullMove);
    out[27] = static_cast<uint8_t>(fullMove >> 8);
    const uint16_t score = static_cast<uint16_t>(whiteScore);
    out[28] = static_cast<uint8_t>(score);
    out[29] = static_cast<uint8_t>(score >> 8);
    out[30] = wdl;
    // extra byte
    out[31] = 0;
}

// viriformat move: 6-bit from | 6-bit to | 2-bit promotion piece | 2-bit type,
// where the promotion piece is knight=0, bishop=1, rook=2, queen=3 and the type
// is 0 for a normal move, 1 for en passant, 2 for castling and 3 for promotion.
// Castling is encoded king-takes-rook, for Chess960 compatibility.
static uint16_t encodeViriMove(const Board &board, Move m) {
    unsigned from = static_cast<unsigned>(StartSquare(m));
    unsigned to = static_cast<unsigned>(DestSquare(m));
    unsigned promotion = 0, type = 0;
    switch (TypeOfMove(m)) {
    case Normal:
        break;
    case EnPassant:
        type = 1;
        break;
    case KCastle:
        type = 2;
        to = (board.sideToMove() == White) ? chess::H1 : chess::H8;
        break;
    case QCastle:
        type = 2;
        to = (board.sideToMove() == White) ? chess::A1 : chess::A8;
        break;
    case Promotion:
        type = 3;
        promotion = static_cast<unsigned>(PromoteTo(m)) - static_cast<unsigned>(Knight);
        break;
    }
    // the all-zero bit pattern terminates a game record
    assert(from != to);
    return static_cast<uint16_t>(from | (to << 6) | (promotion << 12) | (type << 14));
}

void BinFormats::ViriGame::restart(const Board &board, unsigned ply) {
    moves.clear();
    // The header score is unused (each ply carries its own) and the result byte
    // is filled in by write().
    packMarlinBoard(board, 0, 1, ply, header);
    begun = true;
}

void BinFormats::ViriGame::addMove(const Board &board, Move m, score_t score) {
    assert(begun);
    // scores are White relative; clamp so that mate scores do not wrap
    static constexpr score_t MAX_SCORE = 32000;
    const score_t whiteScore = (board.sideToMove() == White) ? score : -score;
    moves.emplace_back(encodeViriMove(board, m),
                       static_cast<int16_t>(std::clamp(whiteScore, -MAX_SCORE, MAX_SCORE)));
}

bool BinFormats::ViriGame::write(int result, std::ostream &out) const {
    if (!begun || moves.empty()) {
        return true;
    }
    std::array<uint8_t, 32> hdr = header;
    hdr[30] = static_cast<uint8_t>(result + 1);
    out.write(reinterpret_cast<const char *>(hdr.data()), hdr.size());
    for (const auto &[move, score] : moves) {
        const uint16_t s = static_cast<uint16_t>(score);
        const uint8_t buf[4] = {static_cast<uint8_t>(move), static_cast<uint8_t>(move >> 8),
                                static_cast<uint8_t>(s), static_cast<uint8_t>(s >> 8)};
        out.write(reinterpret_cast<const char *>(buf), sizeof(buf));
    }
    static const uint8_t terminator[4] = {0, 0, 0, 0};
    out.write(reinterpret_cast<const char *>(terminator), sizeof(terminator));
    return !out.fail();
}

bool BinFormats::writeMarlin(const BinFormats::PositionData &data, int result, std::ostream &out) {
    Board board;
    if (!BoardIO::readFEN(board, data.fen)) {
        std::cerr << "bad fen" << std::endl;
        return false;
    }
    std::array<uint8_t, 32> packed;
    packMarlinBoard(board,
                    static_cast<int16_t>(board.sideToMove() == White ? data.score : -data.score),
                    static_cast<uint8_t>(result + 1), data.ply, packed);
    out.write(reinterpret_cast<const char *>(packed.data()), packed.size());
    return !out.fail();
}

bool BinFormats::writeBullet(const BinFormats::PositionData &data, int result, std::ostream &out) {
    Board board;
    if (!BoardIO::readFEN(board, data.fen)) {
        std::cerr << "bad fen" << std::endl;
        return false;
    }
    // see bulletformat/src/chess.rs
    Bitboard occ(board.allOccupied);
    Square sq, ksq = InvalidSquare, opp_ksq = InvalidSquare;
    unsigned idx = 0;
    std::array<uint8_t, 16> pos;
    pos.fill(0);
    std::unordered_map<Square, unsigned> pieces;
    Bitboard newOcc;
    while ((sq = occ.lastOne()) != InvalidSquare) {
        occ.clear(sq);
        unsigned pieceCode = static_cast<unsigned>(board[sq]) - 1;
        if (data.stm == Black) {
            pieceCode ^= 8;
            sq ^= 0x38;
        }
        if (pieceCode == 5) {
            ksq = sq;
        } else if (pieceCode == 13) {
            opp_ksq = sq ^ 0x38;
        }
        pieces[sq] = pieceCode;
        newOcc.set(sq);
        ++idx;
    }
    assert(ksq != InvalidSquare && opp_ksq != InvalidSquare);
    serialize<uint64_t>(out, newOcc);
    idx = 0;
    // pack position with modified piece codes
    while ((sq = newOcc.firstOne()) != InvalidSquare) {
        newOcc.clear(sq);
        unsigned pieceCode = pieces[sq];
        pos[idx / 2] |= (pieceCode << (4 * (idx & 1)));
        ++idx;
    }
    out.write(reinterpret_cast<const char *>(pos.data()), 16);
    // score is stm relative
    serialize<int16_t>(out, static_cast<int16_t>(data.score));
    // result is stm relative
    if (data.stm == Black) result = -result;
    serialize<uint8_t>(out, static_cast<uint8_t>(result + 1));
    serialize<uint8_t>(out, static_cast<uint8_t>(ksq));
    serialize<uint8_t>(out, static_cast<uint8_t>(opp_ksq));
    // add extra bytes
    static char extra[3] = {0, 0, 0};
    out.write(extra, 3);
    return !out.fail();
}

bool BinFormats::writeText(const BinFormats::PositionData &data, int result, std::ostream &out) {
    int16_t whiteScore = data.stm == White ? data.score : -data.score;
    out << data.fen << " | " << whiteScore << " | " << (result+1)/2.0 << std::endl;
    return true;
}
