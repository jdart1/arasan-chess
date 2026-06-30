// Copyright 2026 by Jon Dart. All Rights Reserved.

// Utility to sort PGN files by date and round.
// This assumes the file fits in memory, and does no parsing
// for correctness of the PGN.

#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iostream>
#include <list>
#include <memory>
#include <regex>
#include <sstream>

#include "chessio.h"

struct ListItem {
    ListItem() : date(0), round(0), startIndex(0), endIndex(0) {}
    int32_t date, round;
    uint64_t startIndex, endIndex;
};

std::list<ListItem> list;

static const inline std::regex DatePattern("(\\?{4,4}|\\d{4,4})\\.(\\?{2,2}|\\d{2,2})\\.(\\?{2,2}|\\d{2,2})");

static void show_usage() {
    std::cerr << "Usage: sortpgn pgn_file" << std::endl;
}

static int32_t dateCode(const std::string &dateStr) {
    auto toInt = [](const std::string &s) {
        if (!s.size() || s[0] == '?') return 0;
        else return std::atoi(s.c_str());
    };
    std::smatch match;
    if (std::regex_match(dateStr, match, DatePattern)) {
        auto it = match.begin() + 1;
        std::string year = *it++;
        std::string month = *it++;
        std::string day = *it;
        return toInt(year) * 13 * 32 +
            toInt(month) * 32 +
            toInt(day);
    }
    else {
        std::cerr << "Could not parse date string: " << dateStr << std::endl;
        return 0;
    }
}

static int32_t roundCode(const std::string &roundStr) {
    if (roundStr.empty()) {
        return 0;
    }
    std::stringstream s(roundStr);
    std::string part;
    int parts[2] = {0, 0};
    int i = 0;
    // handle round strings like "6.3":
    while (i < 2 && std::getline(s, part, '.')) {
        if (part.size() && part[0] == '?') continue;
        std::stringstream p(part);
        p >> parts[i++];
        if (p.fail()) std::cerr << "Warning: invalid Round string: " << part << std::endl;
    }
    return parts[0]*1000 + parts[1];
}

// Read a line, stripping a trailing '\r' so that CRLF-terminated files
// parse correctly (blank separator lines are otherwise seen as "\r").
static std::istream &readLine(std::istream &pgn_file, std::string &line) {
    std::getline(pgn_file, line);
    if (!line.empty() && line.back() == '\r')
        line.pop_back();
    return pgn_file;
}

static void processHeaders(std::ifstream &pgn_file, char *buffer, size_t &index, ListItem &item) {
    item.date = item.round = 0;
    item.startIndex = index;
    std::string line;
    // skip blank lines
    while (pgn_file.good()) {
        readLine(pgn_file, line);
        if (!line.empty())
            break;
    }
    while (pgn_file.good()) {
        if (line.empty()) {
            // end of headers, start of game moves
            buffer[index++] = '\n';
            break;
        }
        if (line[0] == '[') {
            std::smatch match;
            if (std::regex_match(line, match, ChessIO::HeaderPattern)) {
                std::string tag, value;
                auto it = match.begin() + 1;
                if (it != match.end())
                    tag = *it++;
                if (it != match.end())
                    value = *it;
                if (tag == "Event") {
                    item.startIndex = index;
                } else if (tag == "Date") {
                    item.date = dateCode(value);
                } else if (tag == "Round") {
                    item.round = roundCode(value);
                }
            }
        }
        line.copy(buffer + index, line.size());
        buffer[index + line.size()] = '\n';
        index += line.size() + 1;
        if (!readLine(pgn_file, line)) break;
    }
}

static void processMoves(std::ifstream &pgn_file, char *buffer, size_t &index, ListItem &item) {
    std::string line;
    while (pgn_file.good()) {
        readLine(pgn_file, line);
        if (pgn_file.eof() || line.empty()) {
            // end of game moves
            buffer[index++] = '\n';
            item.endIndex = index;
            break;
        }
        line.copy(buffer + index, line.size());
        buffer[index + line.size()] = '\n';
        index += line.size() + 1;
    }
    if (item.startIndex == item.endIndex) {
        std::cerr << "warning: empty game moves section" << std::endl;
    }
}

int CDECL main(int argc, char **argv) {

    if (argc < 2) {
        show_usage();
        exit(-1);
    }

    std::ifstream pgn_file(argv[1], std::ios::in | std::ios::binary);
    if (!pgn_file.good()) {
        std::cerr << "could not open file " << argv[1] << std::endl;
        exit(-1);
    }

    pgn_file.seekg(0, std::ios::end);
    std::streamsize size = pgn_file.tellg();
    pgn_file.seekg(0, std::ios::beg);

    // allocate buffer
    std::unique_ptr<char []> buffer;
    try {
        buffer = std::make_unique<char[]>(static_cast<size_t>(size));
    } catch (std::bad_alloc &ex) {
        std::cerr << "out of memory" << std::endl;
        exit(-1);
    }

    // discard UTF-8 BOM if present
    const int BOM = 0xef;
    int x = pgn_file.get();
    if (x == BOM) {
        x = pgn_file.get();
        x = pgn_file.get();
    } else {
        pgn_file.putback(x);
    }

    size_t index = 0;

    // Read the file into the buffer, collecting data for the sort
    while (pgn_file.good()) {
        ListItem item;
        processHeaders(pgn_file, buffer.get(), index, item);
        if (item.startIndex == index) {
            // no game data read (end of file)
            break;
        }
        processMoves(pgn_file, buffer.get(), index, item);
        list.push_back(item);
    }

    // perform the sort
    list.sort( []( const ListItem &a, const ListItem &b ) {
        return (a.date != b.date) ? (a.date < b.date) : (a.round < b.round); } );

    // ouput in sorted order
    for (const auto &it : list) {
        std::cout.write(buffer.get() + it.startIndex, it.endIndex - it.startIndex);
    }

    return 0;
}
