// Copyright 2026 by Jon Dart. All Rights Reserved.
#ifndef _WEIGHT_CODEC_H
#define _WEIGHT_CODEC_H

#include <array>
#include <cstdint>
#include <cstring>
#include <istream>
#include <type_traits>
#include <vector>

#include "util.h"

// Decoder for the compressed feature weight blocks in network files (version 11). The
// encoder is in the trainer (network/arasan_pack/mod.rs): keep the two in sync.
//
// A block holds rows x cols weights of 1 or 2 bytes each. Rows that are entirely zero
// are omitted. The block layout (all integers little-endian) is:
//   - a row bitmap, (rows+7)/8 bytes: bit (i%8) of byte (i/8) is set if row i is non-zero
//   - for each byte plane (plane 0 = low byte of a weight, plane 1 = high byte), 256
//     bytes giving the Huffman code length of each byte value (0 = value not used)
//   - a uint32 giving the length in bytes of the bitstream
//   - the bitstream: for each non-zero row, for each weight, one code per byte plane in
//     plane order. Codes are packed most significant bit first, and the last byte is
//     padded with zero bits.
// Codes are canonical: assigned in order of increasing length, and within a length by
// increasing byte value, and are at most MAX_CODE_LEN bits long.
class WeightCodec {
  public:
    static constexpr unsigned MAX_CODE_LEN = 12;
    static constexpr size_t SYMBOLS = 256;

    // Read a compressed block from the stream. rowPtr(i) must return a pointer to the
    // storage for the cols weights of row i; every row is written (zero rows as zeros).
    // The stream's failbit is set if the data is invalid or truncated.
    template <typename T, typename RowFunc>
    static std::istream &read(std::istream &s, size_t rows, size_t cols, RowFunc rowPtr) {
        static_assert(sizeof(T) == 1 || sizeof(T) == 2, "unsupported weight size");
        static_assert(std::is_integral<T>::value, "weights must be integers");
        constexpr size_t PLANES = sizeof(T);
        using U = std::make_unsigned_t<T>;

        std::vector<uint8_t> bitmap((rows + 7) / 8);
        s.read(reinterpret_cast<char *>(bitmap.data()), static_cast<std::streamsize>(bitmap.size()));
        size_t nonZeroRows = 0;
        for (size_t i = 0; i < rows; ++i) {
            nonZeroRows += rowIsSet(bitmap, i) ? 1 : 0;
        }

        std::array<DecodeTable, PLANES> tables;
        for (auto &table : tables) {
            uint8_t lengths[SYMBOLS];
            s.read(reinterpret_cast<char *>(lengths), sizeof(lengths));
            if (s.fail() || !table.build(lengths)) {
                s.setstate(std::ios::failbit);
                return s;
            }
        }

        const uint32_t dataLen = static_cast<uint32_t>(read_little_endian<int32_t>(s));
        const uint64_t maxLen =
            (static_cast<uint64_t>(nonZeroRows) * cols * PLANES * MAX_CODE_LEN + 7) / 8;
        if (s.fail() || dataLen > maxLen) {
            s.setstate(std::ios::failbit);
            return s;
        }
        std::vector<uint8_t> data(dataLen);
        s.read(reinterpret_cast<char *>(data.data()), static_cast<std::streamsize>(dataLen));
        if (s.fail()) return s;

        uint64_t acc = 0;    // bit accumulator: the next code bits are the low nbits
        unsigned nbits = 0;  // number of valid bits in acc
        size_t pos = 0;      // bytes loaded into acc, including zero bytes past the end
        bool ok = true;
        for (size_t i = 0; i < rows && ok; ++i) {
            T *row = rowPtr(i);
            if (!rowIsSet(bitmap, i)) {
                std::memset(row, 0, cols * sizeof(T));
                continue;
            }
            for (size_t j = 0; j < cols && ok; ++j) {
                U v = 0;
                for (size_t p = 0; p < PLANES; ++p) {
                    if (nbits < MAX_CODE_LEN) {
                        while (nbits <= 56) {
                            acc = (acc << 8) | (pos < dataLen ? data[pos] : 0);
                            ++pos;
                            nbits += 8;
                        }
                    }
                    const uint16_t e = tables[p].entries[(acc >> (nbits - MAX_CODE_LEN)) &
                                                         (TABLE_SIZE - 1)];
                    const unsigned len = e >> 8;
                    if (len == 0) {
                        ok = false;
                        break;
                    }
                    nbits -= len;
                    v = static_cast<U>(v | (static_cast<U>(e & 0xFF) << (8 * p)));
                }
                row[j] = static_cast<T>(v);
            }
        }
        // The stream must be consumed exactly (apart from the padding bits of the last byte)
        const uint64_t consumed = static_cast<uint64_t>(pos) * 8 - nbits;
        if (!ok || consumed > static_cast<uint64_t>(dataLen) * 8 ||
            (consumed + 7) / 8 != dataLen) {
            s.setstate(std::ios::failbit);
        }
        return s;
    }

  private:
    static constexpr size_t TABLE_SIZE = size_t(1) << MAX_CODE_LEN;

    static bool rowIsSet(const std::vector<uint8_t> &bitmap, size_t row) noexcept {
        return (bitmap[row >> 3] >> (row & 7)) & 1;
    }

    // Lookup table indexed by the next MAX_CODE_LEN bits of the stream: each entry is
    // (code length << 8) | byte value, or 0 if no code matches.
    struct DecodeTable {
        std::array<uint16_t, TABLE_SIZE> entries;

        // Returns false if the lengths are not valid (over-subscribed or too long)
        bool build(const uint8_t *lengths) {
            std::array<unsigned, MAX_CODE_LEN + 1> count{};
            for (size_t sym = 0; sym < SYMBOLS; ++sym) {
                if (lengths[sym] > MAX_CODE_LEN) return false;
                ++count[lengths[sym]];
            }
            unsigned kraft = 0;
            for (unsigned len = 1; len <= MAX_CODE_LEN; ++len) {
                kraft += count[len] << (MAX_CODE_LEN - len);
            }
            if (kraft > TABLE_SIZE) return false;
            entries.fill(0);
            unsigned code = 0;
            for (unsigned len = 1; len <= MAX_CODE_LEN; ++len) {
                const size_t span = size_t(1) << (MAX_CODE_LEN - len);
                for (size_t sym = 0; sym < SYMBOLS; ++sym) {
                    if (lengths[sym] != len) continue;
                    const size_t start = static_cast<size_t>(code) << (MAX_CODE_LEN - len);
                    for (size_t k = 0; k < span; ++k) {
                        entries[start + k] = static_cast<uint16_t>((len << 8) | sym);
                    }
                    ++code;
                }
                code <<= 1;
            }
            return true;
        }
    };
};

#endif
