// Copyright 1994, 2008, 2021, 2024-2025 by Jon Dart

#include "eco.h"
#include "board.h"
#include "notation.h"

#include <cctype>
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <vector>

ECO::ECO() {
    // Initialize hash table
    ht = std::make_unique<std::map<hash_t, ECOData>>();

    initDone = false;
}

void ECO::init(const std::string &path) {
    if (initDone) {
        return;
    }

    static const std::vector<std::string> files{{"a.tsv", "b.tsv", "c.tsv", "d.tsv", "e.tsv"}};

    for (const auto &filename : files) {
        std::ifstream file(path + std::filesystem::path::preferred_separator + filename, std::ios::in);
        if (!file.good()) {
            continue;
        }

        std::string line;
        // Skip header line
        std::getline(file, line);

        if (file.bad()) return;

        while (std::getline(file, line)) {
            if (line.empty()) {
                continue;
            }

            // Split line by tabs: eco<tab>name<tab>pgn
            size_t firstTab = line.find('\t');
            if (firstTab == std::string::npos) {
                continue;
            }

            size_t secondTab = line.find('\t', firstTab + 1);
            if (secondTab == std::string::npos) {
                continue;
            }

            std::string eco = line.substr(0, firstTab);
            std::string name = line.substr(firstTab + 1, secondTab - firstTab - 1);
            std::string pgn = line.substr(secondTab + 1);

            // Parse PGN moves and play them on a board
            Board board;
            std::istringstream ss(pgn);
            std::string token;
            bool parseError = false;

            // Regex to match move numbers (e.g., "1.", "2.", "10.")
            static const std::regex moveNumberRegex(R"(^\d+\.+$)");

            while (ss >> token) {
                // Skip move numbers using regex
                if (std::regex_match(token, moveNumberRegex)) {
                    continue;
                }

                // Parse and make the move
                Move move = Notation::value(board, board.sideToMove(),
                                          Notation::InputFormat::SAN, token);
                if (IsNull(move)) {
                    parseError = true;
                    break;
                }
                board.doMove(move);
            }

            if (!parseError) {
                // Add strings to pools and get stable pointers
                auto ecoIt = ecoPool.insert(eco).first;
                auto nameIt = namePool.insert(name).first;

                // Store ECOData with pointers to pooled strings
                ECOData data(&(*ecoIt), &(*nameIt));
                ht->insert(std::make_pair(board.hashCode(), data));
            }
        }
    }
    if (ht->size()) initDone = true;
}

void ECO::classify(const MoveArray &moves, std::string &result, std::string &name) {
    // We take kind of a simple-minded approach to ECO classification.
    // Each ECO code is associated with a chess position.  We follow the
    // game moves until we have found the last one that leads to a
    // hash code matching an ECO position, then report that ECO code.
    // This doesn't allow for openings that eventually transpose into
    // an ECO subline (e.g. A04/01) but don't ever hit the position
    // for the main ECO code (e.g. A04) - this is possible.
    result = "";
    name = "";
    for (unsigned i = 0; i < moves.num_moves(); i++) {
        std::map<hash_t, ECOData>::const_iterator it = ht.get()->find(moves[i].hashcode);
        if (it != ht.get()->end()) {
            const ECOData &hit = (*it).second;
            result = *(hit.eco);
            name = *(hit.opening_name);
        }
    }

    // Also check the final position after all moves (MoveRecord stores positions before each move)
    if (moves.num_moves() > 0) {
        Board board;
        for (unsigned i = 0; i < moves.num_moves(); i++) {
            board.doMove(moves[i].move);
        }
        std::map<hash_t, ECOData>::const_iterator it = ht.get()->find(board.hashCode());
        if (it != ht.get()->end()) {
            const ECOData &hit = (*it).second;
            result = *(hit.eco);
            name = *(hit.opening_name);
        }
    }
}
