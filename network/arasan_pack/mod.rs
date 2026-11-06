//! Compression of the feature weight blocks (piece-square, pair and threat) of Arasan network
//! files (version 11), used by the trainer when it saves a network. The decoder is in
//! src/nnue/features/weightcodec.h: keep the two in sync.
//!
//! A compressed block holds rows x cols weights of 1 or 2 bytes each. Rows that are
//! entirely zero are omitted. The layout (all integers little-endian) is:
//!   - a row bitmap, (rows+7)/8 bytes: bit (i%8) of byte (i/8) is set if row i is non-zero
//!   - for each byte plane (plane 0 = low byte of a weight, plane 1 = high byte), 256 bytes
//!     giving the Huffman code length of each byte value (0 = value not used)
//!   - a u32 giving the length in bytes of the bitstream
//!   - the bitstream: for each non-zero row, for each weight, one code per byte plane in
//!     plane order. Codes are packed most significant bit first, and the last byte is
//!     padded with zero bits.
//! Codes are canonical: assigned in order of increasing length, and within a length by
//! increasing byte value, and are at most MAX_CODE_LEN bits long.

use std::cmp::Reverse;
use std::collections::BinaryHeap;

/// Header version byte of the packed layout
pub const PACKED_VERSION: u8 = 11;
pub const MAX_CODE_LEN: usize = 12;
const SYMBOLS: usize = 256;

// Code lengths of an optimal prefix code for the symbol counts (unlimited length)
fn unlimited_lengths(counts: &[u64; SYMBOLS]) -> [u8; SYMBOLS] {
    let mut lengths = [0u8; SYMBOLS];
    let used: Vec<usize> = (0..SYMBOLS).filter(|&s| counts[s] > 0).collect();
    match used.len() {
        0 => return lengths,
        1 => {
            lengths[used[0]] = 1;
            return lengths;
        }
        _ => {}
    }
    // nodes 0..SYMBOLS are leaves, internal nodes follow: children[n - SYMBOLS] = (left, right)
    let mut children: Vec<(usize, usize)> = Vec::new();
    let mut heap = BinaryHeap::new();
    for &s in &used {
        heap.push(Reverse((counts[s], s)));
    }
    while heap.len() > 1 {
        let Reverse((c1, a)) = heap.pop().unwrap();
        let Reverse((c2, b)) = heap.pop().unwrap();
        children.push((a, b));
        heap.push(Reverse((c1 + c2, SYMBOLS + children.len() - 1)));
    }
    let root = heap.pop().unwrap().0 .1;
    let mut stack = vec![(root, 0usize)];
    while let Some((node, depth)) = stack.pop() {
        if node < SYMBOLS {
            lengths[node] = depth as u8;
        } else {
            let (l, r) = children[node - SYMBOLS];
            stack.push((l, depth + 1));
            stack.push((r, depth + 1));
        }
    }
    lengths
}

/// Code lengths limited to MAX_CODE_LEN: if the optimal code is too long, halve the counts
/// (keeping them non-zero) and try again. This costs very little in compression.
fn code_lengths(counts: &[u64; SYMBOLS]) -> [u8; SYMBOLS] {
    let mut c = *counts;
    loop {
        let lengths = unlimited_lengths(&c);
        if lengths.iter().all(|&l| usize::from(l) <= MAX_CODE_LEN) {
            return lengths;
        }
        for x in c.iter_mut().filter(|x| **x > 0) {
            *x = (*x + 1) / 2;
        }
    }
}

// Canonical codes: by increasing length, then increasing symbol
fn canonical_codes(lengths: &[u8; SYMBOLS]) -> [u32; SYMBOLS] {
    let mut codes = [0u32; SYMBOLS];
    let mut code = 0u32;
    for len in 1..=MAX_CODE_LEN {
        for sym in 0..SYMBOLS {
            if usize::from(lengths[sym]) == len {
                codes[sym] = code;
                code += 1;
            }
        }
        code <<= 1;
    }
    codes
}

struct BitWriter {
    out: Vec<u8>,
    acc: u64,
    nbits: u32,
}

impl BitWriter {
    fn new() -> Self {
        Self { out: Vec::new(), acc: 0, nbits: 0 }
    }

    fn push(&mut self, code: u32, len: u32) {
        self.acc = (self.acc << len) | u64::from(code);
        self.nbits += len;
        while self.nbits >= 8 {
            self.out.push((self.acc >> (self.nbits - 8)) as u8);
            self.nbits -= 8;
        }
    }

    fn finish(mut self) -> Vec<u8> {
        if self.nbits > 0 {
            self.out.push((self.acc << (8 - self.nbits)) as u8);
        }
        self.out
    }
}

/// Compress a block of rows x cols weights of elem (1 or 2) little-endian bytes each.
pub fn pack_block(data: &[u8], rows: usize, cols: usize, elem: usize) -> Vec<u8> {
    assert!(elem == 1 || elem == 2);
    let row_len = cols * elem;
    assert_eq!(data.len(), rows * row_len);
    let row_set: Vec<bool> = (0..rows).map(|i| data[i * row_len..(i + 1) * row_len].iter().any(|&b| b != 0)).collect();

    let mut counts = vec![[0u64; SYMBOLS]; elem];
    for i in (0..rows).filter(|&i| row_set[i]) {
        for (k, &b) in data[i * row_len..(i + 1) * row_len].iter().enumerate() {
            counts[k % elem][usize::from(b)] += 1;
        }
    }
    let lengths: Vec<[u8; SYMBOLS]> = counts.iter().map(code_lengths).collect();
    let codes: Vec<[u32; SYMBOLS]> = lengths.iter().map(canonical_codes).collect();

    let mut out = vec![0u8; (rows + 7) / 8];
    for i in (0..rows).filter(|&i| row_set[i]) {
        out[i / 8] |= 1 << (i % 8);
    }
    for l in &lengths {
        out.extend_from_slice(l);
    }
    let mut writer = BitWriter::new();
    for i in (0..rows).filter(|&i| row_set[i]) {
        for (k, &b) in data[i * row_len..(i + 1) * row_len].iter().enumerate() {
            let (p, s) = (k % elem, usize::from(b));
            writer.push(codes[p][s], u32::from(lengths[p][s]));
        }
    }
    let stream = writer.finish();
    out.extend_from_slice(&(stream.len() as u32).to_le_bytes());
    out.extend_from_slice(&stream);
    out
}

fn take<'a>(input: &'a [u8], pos: &mut usize, n: usize) -> Result<&'a [u8], String> {
    let end = pos.checked_add(n).filter(|&e| e <= input.len()).ok_or("truncated block")?;
    let s = &input[*pos..end];
    *pos = end;
    Ok(s)
}

/// Decompress a block starting at input[*pos], advancing *pos past it. This is a simple
/// reference decoder (bit at a time) used to verify the encoder.
pub fn unpack_block(input: &[u8], pos: &mut usize, rows: usize, cols: usize, elem: usize) -> Result<Vec<u8>, String> {
    let bitmap = take(input, pos, (rows + 7) / 8)?.to_vec();
    let mut tables: Vec<[u8; SYMBOLS]> = Vec::new();
    for _ in 0..elem {
        tables.push(take(input, pos, SYMBOLS)?.try_into().unwrap());
    }
    let stream_len = u32::from_le_bytes(take(input, pos, 4)?.try_into().unwrap()) as usize;
    let stream = take(input, pos, stream_len)?;

    // per plane: first code and count at each length, and the symbols in canonical order
    struct Table {
        first: [u32; MAX_CODE_LEN + 1],
        count: [u32; MAX_CODE_LEN + 1],
        offset: [usize; MAX_CODE_LEN + 1],
        symbols: Vec<u8>,
    }
    let mut dec = Vec::new();
    for lengths in &tables {
        let mut t = Table { first: [0; MAX_CODE_LEN + 1], count: [0; MAX_CODE_LEN + 1], offset: [0; MAX_CODE_LEN + 1], symbols: Vec::new() };
        for len in 1..=MAX_CODE_LEN {
            t.offset[len] = t.symbols.len();
            for sym in 0..SYMBOLS {
                if usize::from(lengths[sym]) == len {
                    t.symbols.push(sym as u8);
                    t.count[len] += 1;
                }
            }
            t.first[len] = if len == 1 { 0 } else { (t.first[len - 1] + t.count[len - 1]) << 1 };
            if u64::from(t.first[len]) + u64::from(t.count[len]) > (1u64 << len) {
                return Err("over-subscribed code".into());
            }
        }
        if lengths.iter().any(|&l| usize::from(l) > MAX_CODE_LEN) {
            return Err("code too long".into());
        }
        dec.push(t);
    }

    let row_len = cols * elem;
    let mut out = vec![0u8; rows * row_len];
    let mut bit = 0usize;
    for i in 0..rows {
        if bitmap[i / 8] >> (i % 8) & 1 == 0 {
            continue;
        }
        for k in 0..row_len {
            let t = &dec[k % elem];
            let mut code = 0u32;
            let mut sym = None;
            for len in 1..=MAX_CODE_LEN {
                let byte = *stream.get(bit / 8).ok_or("stream overrun")?;
                code = (code << 1) | u32::from((byte >> (7 - bit % 8)) & 1);
                bit += 1;
                if t.count[len] > 0 && code >= t.first[len] && code - t.first[len] < t.count[len] {
                    sym = Some(t.symbols[t.offset[len] + (code - t.first[len]) as usize]);
                    break;
                }
            }
            out[i * row_len + k] = sym.ok_or("invalid code")?;
        }
    }
    if (bit + 7) / 8 != stream_len {
        return Err("stream length mismatch".into());
    }
    Ok(out)
}

/// For use in a bullet SavedFormat transform: quantise the weights (multiply by scale and round,
/// to an int8 if elem is 1 or an int16 if it is 2), pack them as a block of rows x cols, verify
/// the result, and return the packed bytes as f32 values in [-128, 127]. Quantising these as
/// int8 with scale 1 (the next step in the SavedFormat) writes the packed bytes unchanged.
pub fn pack_weights(values: &[f32], scale: i16, rows: usize, cols: usize, elem: usize) -> Vec<f32> {
    assert_eq!(values.len(), rows * cols, "unexpected number of weights");
    let mut bytes = Vec::with_capacity(values.len() * elem);
    for &v in values {
        let q = (f64::from(scale) * f64::from(v)).round();
        match elem {
            1 => {
                let x = q as i8;
                assert!(q == f64::from(x), "failed quantisation from f32 to i8");
                bytes.push(x as u8);
            }
            2 => {
                let x = q as i16;
                assert!(q == f64::from(x), "failed quantisation from f32 to i16");
                bytes.extend_from_slice(&x.to_le_bytes());
            }
            _ => panic!("unsupported weight size"),
        }
    }
    let packed = pack_block(&bytes, rows, cols, elem);
    let mut pos = 0;
    assert!(unpack_block(&packed, &mut pos, rows, cols, elem).unwrap() == bytes, "packed weights do not unpack to the original");
    packed.iter().map(|&b| f32::from(b as i8)).collect()
}

#[cfg(test)]
mod tests {
    use super::*;

    fn roundtrip(data: &[u8], rows: usize, cols: usize, elem: usize) -> Vec<u8> {
        let block = pack_block(data, rows, cols, elem);
        let mut pos = 0;
        assert_eq!(unpack_block(&block, &mut pos, rows, cols, elem).unwrap(), data);
        assert_eq!(pos, block.len());
        block
    }

    fn table(entries: &[(usize, u8)]) -> Vec<u8> {
        let mut t = vec![0u8; SYMBOLS];
        for &(s, l) in entries {
            t[s] = l;
        }
        t
    }

    // The same blocks as the hand-built cases in the C++ unit tests (nnuetest.cpp)
    #[test]
    fn matches_cpp_test_blocks() {
        let mut expected = vec![0x01];
        expected.extend(table(&[(0, 1), (5, 2), (0xFB, 2)]));
        expected.extend([1, 0, 0, 0, 0x58]);
        assert_eq!(roundtrip(&[0, 5, 0xFB, 0, 0, 0, 0, 0], 2, 4, 1), expected);

        let mut expected = vec![0x01];
        expected.extend(table(&[(0x34, 1), (0x78, 1)]));
        expected.extend(table(&[(0x12, 1), (0xFF, 1)]));
        expected.extend([1, 0, 0, 0, 0x30]);
        assert_eq!(roundtrip(&[0x34, 0x12, 0x78, 0xFF], 1, 2, 2), expected);

        let mut expected = vec![0x01];
        expected.extend(table(&[(7, 1)]));
        expected.extend([1, 0, 0, 0, 0x00]);
        assert_eq!(roundtrip(&[7], 1, 1, 1), expected);

        let mut expected = vec![0x00];
        expected.extend(table(&[]));
        expected.extend([0, 0, 0, 0]);
        assert_eq!(roundtrip(&[0; 15], 3, 5, 1), expected);
    }

    // xorshift generator, so the tests need no crates
    fn rng(state: &mut u64) -> u64 {
        *state ^= *state << 13;
        *state ^= *state >> 7;
        *state ^= *state << 17;
        *state
    }

    #[test]
    fn random_blocks() {
        let mut st = 0x9E3779B97F4A7C15u64;
        for &(rows, cols, elem) in &[(40, 77, 1), (33, 50, 2), (1, 1, 1), (9, 3, 2)] {
            let mut data = vec![0u8; rows * cols * elem];
            for (i, b) in data.iter_mut().enumerate() {
                let r = rng(&mut st);
                // skewed byte values; every 4th row is zero
                *b = if (i / (cols * elem)) % 4 == 3 { 0 } else { (r.trailing_zeros().min(40) as u8).wrapping_mul(7) };
            }
            roundtrip(&data, rows, cols, elem);
        }
    }

    #[test]
    fn code_length_limit() {
        // geometric counts need an unlimited code of length > 12; the limited code must be valid
        let mut counts = [0u64; SYMBOLS];
        for (s, c) in counts.iter_mut().enumerate().take(30) {
            *c = 1u64 << (29 - s);
        }
        assert!(unlimited_lengths(&counts).iter().any(|&l| usize::from(l) > MAX_CODE_LEN));
        let lengths = code_lengths(&counts);
        assert!(lengths.iter().all(|&l| usize::from(l) <= MAX_CODE_LEN));
        let kraft: f64 = lengths.iter().filter(|&&l| l > 0).map(|&l| 0.5f64.powi(i32::from(l))).sum();
        assert!(kraft <= 1.0 + 1e-12);
        // and data with such a distribution round trips
        let mut data = Vec::new();
        for (s, &c) in counts.iter().enumerate().take(30) {
            data.extend(std::iter::repeat(s as u8 + 1).take(((c >> 18) + 1) as usize));
        }
        let n = data.len();
        roundtrip(&data, 1, n, 1);
    }

    #[test]
    fn pack_weights_survives_quantisation() {
        // the packed bytes, returned as f32 values, must be written unchanged by bullet's int8
        // quantisation (scale 1, rounding)
        let mut st = 12345u64;
        for &(rows, cols, elem) in &[(7, 5, 1), (6, 4, 2)] {
            let values: Vec<f32> = (0..rows * cols)
                .map(|i| if i / cols % 3 == 2 { 0.0 } else { ((rng(&mut st) % 255) as i32 - 127) as f32 / 255.0 })
                .collect();
            let packed_f32 = pack_weights(&values, 255, rows, cols, elem);
            let written: Vec<u8> = packed_f32.iter().map(|&f| (f64::from(1i16) * f64::from(f)).round() as i8 as u8).collect();
            let mut pos = 0;
            let bytes = unpack_block(&written, &mut pos, rows, cols, elem).unwrap();
            assert_eq!(pos, written.len());
            let expected: Vec<u8> = values
                .iter()
                .flat_map(|&v| {
                    let q = (255.0 * f64::from(v)).round();
                    if elem == 1 { vec![q as i8 as u8] } else { (q as i16).to_le_bytes().to_vec() }
                })
                .collect();
            assert_eq!(bytes, expected);
        }
    }

    #[test]
    fn corrupt_blocks_rejected() {
        let data: Vec<u8> = (0..200).map(|i| (i * i % 11) as u8).collect();
        let block = pack_block(&data, 10, 20, 1);
        let mut pos = 0;
        assert!(unpack_block(&block[..block.len() - 1], &mut pos, 10, 20, 1).is_err());
    }
}
