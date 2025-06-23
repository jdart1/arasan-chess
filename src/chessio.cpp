// Copyright 1994, 1995, 2008, 2012-2014, 2017-2018, 2021-2024 by Jon Dart.
// All Rights Reserved.

#include "chessio.h"
#include "boardio.h"
#include "constant.h"
#include "epdrec.h"

#include <algorithm>
#include <cstring>
#include <iomanip>
#include <regex>
#include <sstream>
#include <unordered_set>

static constexpr unsigned PGN_MARGIN = 70;

static std::unordered_set<std::string> RESULTS = {"0-1", "1-0", "1/2-1/2"};

// This module handles reading and writing board and game info.

void ChessIO::get_game_description(const std::vector<Header> &hdrs, std::string &descr, long id) {
    std::stringstream s;
    std::string tmp;
    get_header(hdrs, "White", tmp);
    std::string::size_type comma = tmp.find(',', 0);
    if (comma != std::string::npos) {
        tmp = tmp.substr(0, comma);
    }
    s << tmp << '-';
    get_header(hdrs, "Black", tmp);
    comma = tmp.find(',', 0);
    if (comma != std::string::npos) {
        tmp = tmp.substr(0, comma);
    }
    s << tmp;
    get_header(hdrs, "Site", tmp);
    s << ", " << tmp;
    get_header(hdrs, "Round", tmp);
    if (tmp.length() > 0 && tmp[0] != '?') {
        s << '(' << tmp << ')';
    }
    s << ' ';
    get_header(hdrs, "Date", tmp);
    s << tmp.substr(0, 4);
    get_header(hdrs, "Result", tmp);
    if (tmp != "*") {
        s << ' ' << tmp;
    }
    s << '\t' << id;
    descr = s.str();
}

bool ChessIO::PGNReader::scanPGN(std::vector<std::string> &contents) {
    while (stream.good() && !stream.eof()) {
        long first;
        std::vector<Header> hdrs;
        std::string eventStr;
        collectHeaders(hdrs, first);
        if (get_header(hdrs, "Event", eventStr)) {
            // We have the headers, munge them into a single-line game
            // description. Append the index to the game so the GUI
            // can navigate to the game when the user clicks on the
            // description.
            std::string descr;
            get_game_description(hdrs, descr, first);
            contents.push_back(descr);
        }
    }
    return stream.good();
}

bool ChessIO::PGNReader::getline(std::string &line) {
    if (!buffer.empty()) {
        line = buffer.front();
        buffer.pop();
        return 1;
    } else {
        if (stream.eof())
            return false;
        std::getline(stream, line);
        return stream.good();
    }
}

bool ChessIO::get_header(const std::vector<Header> &hdrs, const std::string &key,
                         std::string &val) {
    auto it =
        std::find_if(hdrs.begin(), hdrs.end(), [&key](const Header &x) { return x.tag() == key; });
    if (it == hdrs.end()) {
        return false;
    } else {
        val = it->value();
        return true;
    }
}

bool ChessIO::remove_header(std::vector<Header> &hdrs, const std::string &key) {
    auto it = std::remove_if(hdrs.begin(), hdrs.end(),
                             [&key](const Header &x) { return x.tag() == key; });
    return it != hdrs.end();
}

void ChessIO::replace_header(std::vector<Header> &hdrs, const std::string &key,
                             const std::string &val) {
    std::replace_if(
        hdrs.begin(), hdrs.end(), [&key](const Header &x) { return x.tag() == key; },
        Header(key, val));
}

void ChessIO::add_header(std::vector<Header> &hdrs, const std::string &key,
                         const std::string &val) {
    hdrs.push_back(Header(key, val));
}

bool ChessIO::load_fen(std::istream &ifs, Board &board) {
    ifs >> board;
    board.state.moveCount = 0;
    return ifs.good();
}

bool ChessIO::store_fen(std::ostream &ofile, const Board &board) {
    ofile << board << std::endl;
    return ofile.good();
}

bool ChessIO::store_pgn(std::ostream &ofile, MoveArray &moves, const ColorType computer_side,
                        const std::string &result, std::vector<Header> &headers) {
    // Write standard PGN header.

    std::string gameResult = result;
    if (result.length() == 0)
        gameResult = "*";
    std::string val;
    std::vector<Header> newHeaders;
    if (!get_header(headers, "Event", val))
        add_header(newHeaders, "Event", "?");
    else
        add_header(newHeaders, "Event", val);
    if (!get_header(headers, "Site", val))
        add_header(newHeaders, "Site", "?");
    else
        add_header(newHeaders, "Site", val);
    if (!get_header(headers, "Date", val)) {
        time_t tm = time(NULL);
        struct tm *t = localtime(&tm);
        std::stringstream dateStr;
        dateStr << std::setw(4) << t->tm_year + 1900 << std::setw(1) << '.' << std::setw(2)
                << std::setfill('0') << t->tm_mon + 1 << std::setw(1) << '.' << std::setw(2)
                << t->tm_mday;
        add_header(newHeaders, "Date", dateStr.str());
    } else
        add_header(newHeaders, "Date", val);
    if (!get_header(headers, "Round", val))
        add_header(newHeaders, "Round", "?");
    else
        add_header(newHeaders, "Round", val);

    std::string name("Arasan ");
    name += Arasan_Version;
    if (computer_side == White) {
        if (!get_header(headers, "White", val)) {
            add_header(newHeaders, "White", name);
        } else
            add_header(newHeaders, "White", val);
        if (!get_header(headers, "Black", val))
            add_header(newHeaders, "Black", "?");
        else
            add_header(newHeaders, "Black", val);
    } else {
        if (!get_header(headers, "White", val))
            add_header(newHeaders, "White", "?");
        else
            add_header(newHeaders, "White", val);
        if (!get_header(headers, "Black", val))
            add_header(newHeaders, "Black", name);
        else
            add_header(newHeaders, "Black", val);
    }
    // "Result" may contain a comment. Don't put this in the
    // PGN header.
    std::string shortResult = gameResult;
    std::string longResult = gameResult;
    std::string::size_type space = gameResult.find(" ");
    if (space > 0 && space < MAX_PATH)
        shortResult = gameResult.erase(space);
    if (!get_header(headers, "Result", val))
        add_header(newHeaders, "Result", shortResult);
    else
        add_header(newHeaders, "Result", val);

    // We have now written all the mandatory headers.
    // Add any more headers that were passed into us.

    for (auto &it : headers) {
        Header p = it;
        if (p.tag() != "Event" && p.tag() != "Site" && p.tag() != "Date" && p.tag() != "Round" &&
            p.tag() != "White" && p.tag() != "Black" && p.tag() != "Result")
            add_header(newHeaders, p.tag(), p.value());
    }

    // write headers and cleanup
    return store_pgn(ofile, moves, longResult, newHeaders);
}

bool ChessIO::store_pgn(std::ostream &ofile, MoveArray &moves, const std::string &result,
                        std::vector<Header> &headers) {
    for (auto it = headers.begin(); it != headers.end(); it++) {
        Header p = *it;
        ofile << "[" << p.tag() << " \"" << p.value() << "\"]" << std::endl;
    }
    ofile << std::endl;

    // Write game moves.
    unsigned len = moves.num_moves();
    std::stringstream buf;
    for (unsigned i = 0; i < len; i++) {
        const MoveRecord &e = moves[i];
        std::stringstream numbuf;
        if (i % 2 == 0) {
            numbuf << (i / 2) + 1 << ". ";
        }
        const int image_size = (int)e.image.size();
        if ((int)buf.tellp() + image_size + numbuf.str().length() + 1 >= PGN_MARGIN) {
            ofile << buf.str() << std::endl;
            buf.str("");
        }
        if (buf.tellp() != (std::streampos)0) {
            buf << ' ';
        }
        buf << numbuf.str() << e.image;
    }
    if ((int)buf.tellp() + result.length() + 1 >= PGN_MARGIN) {
        ofile << buf.str() << std::endl;
        buf.str("");
        buf << result;
    } else {
        buf << ' ' << result;
    }
    ofile << buf.str() << std::endl << std::endl;
    return (bool)ofile;
}

bool ChessIO::readEPDRecord(std::istream &ifs, Board &board, EPDRecord &rec) {
    rec.clear();
    // read FEN description
    ifs >> board;
    if (ifs.eof())
        return false;
    if (!ifs.good()) {
        rec.setError("Bad EPD record: FEN board description missing or invalid");
        ifs.clear();
        ifs.ignore(4096, '\n');
        return true;
    }
    // read EPD commands
    std::string commands, command;
    getline(ifs, commands);
    std::stringstream s(commands);
    while (getline(s, command, ';')) {
        // each command should contain a verb and a value
        std::string cmd, val;
        auto it = command.begin();
        auto end = command.end();
        // command does not have embedded spaces
        while (it != end && isspace(*it))
            it++;
        while (it != end && !isspace(*it))
            cmd += *it++;
        while (it != end && isspace(*it))
            it++;
        // value may have spaces (multiple values) or be quoted
        while (it != end)
            val += *it++;
        rec.add(cmd, val);
    }
    return true;
}

void ChessIO::writeEPDRecord(std::ostream &ofs, Board &board, const EPDRecord &rec) {
    BoardIO::writeFEN(board, ofs, 0);
    for (size_t i = 0; i < rec.getSize(); i++) {
        std::string key, val;
        (void)rec.getData(static_cast<int>(i), key, val);
        ofs << ' ' << key << ' ' << val << ';';
    }
    ofs << std::endl;
}

bool ChessIO::PGNReader::collectHeaders(std::vector<Header> &hdrs, long &first) {
    first = static_cast<long>(stream.tellg()) - 1;
    bool foundHeader = false;
    std::string line;
    while (getline(line)) {
        std::smatch match;
        std::string tag, value;
        if (std::regex_match(line, match, HeaderPattern)) {
            foundHeader = true;
            auto it = match.begin() + 1;
            if (it != match.end())
                tag = *it++;
            if (it != match.end())
                value = *it;
            hdrs.push_back(Header(tag, value));
        } else if (foundHeader &&
                   (line.empty() || std::all_of(line.begin(), line.end(), isspace))) {
            break;
        } else {
            first = static_cast<long>(stream.tellg()) - 1;
        }
    }
    return foundHeader;
}

void ChessIO::TokenReader::getTextToNextDelim(std::string &s) {
    auto delim = buf.find_first_of(" \t()\r\n", index);
    auto end = (delim == std::string::npos) ? buf.end() : buf.begin() + delim;
    for (auto it = buf.begin() + index; it != end; ++it)
        s += *it;
    index += s.size();
}

ChessIO::TokenReader::TokenReader(ChessIO::PGNReader &f)
    : file(f), index(0), CastlePattern("^([0O]\\-[0O](\\-[0O])?).*$"),
      MovePattern("^([a-zA-z1-8\\-\\+=]+).*$"), DigitsPattern("^((\\d+)?\\.*)$") {}

ChessIO::Token ChessIO::TokenReader::nextToken() {
    TokenType tok = Unknown;
    int c;
    std::string value;
    while ((c = get()) != EOF && isspace(c))
        ;
    if (c == EOF) {
        tok = Eof;
    } else if (c == '[') {
        // not expected within a PGN body, must be header of the
        // next game
        file.putback(buf);
        index = 0;
        tok = Eof;
    } else if (c == '{') {
        value += c;
        while ((c = get()) != EOF) {
            value += c;
            if (c == '}')
                break;
        }
        tok = Comment;
    } else if (c == '(') {
        value = '(';
        tok = OpenVar;
    } else if (c == ')') {
        value = ')';
        tok = CloseVar;
    } else if (c == '$') {
        value += c;
        std::string s;
        getTextToNextDelim(s);
        std::smatch match;
        if (std::regex_match(s, match, DigitsPattern)) {
            tok = NAG;
            value += *(match.begin() + 1);
        }
    } else if (isdigit(c)) {
        value += c;
        std::string s;
        getTextToNextDelim(s);
        value += s;
        std::smatch match;
        if (std::regex_match(value, match, CastlePattern)) {
            tok = GameMove;
        } else if ((value.size() > 1) && (RESULTS.find(value) != RESULTS.end())) {
            // result string
            tok = Result;
            Token tok2 = nextToken();
            if (tok2.type == Comment) {
                value += ' ' + tok2.val;
            }
        } else if (std::regex_match(value, match, DigitsPattern)) {
            tok = Number;
        }
    } else if (isalpha(c)) {
        value += c;
        std::string s;
        std::smatch match;
        getTextToNextDelim(s);
        if (std::regex_match(s, match, MovePattern)) {
            value += *(match.begin() + 1);
            tok = GameMove;
        }
    } else if (c == '#') { // "Checkmate"
        value += c;
        tok = Ignore;
    } else if (c == '*') {
        value += c;
        Token tok2 = nextToken();
        if (tok2.type == Comment) {
            value += ' ' + tok2.val;
        }
        tok = Result;
    } else {
        value += c;
        tok = Unknown;
    }
    return Token(tok, value);
}
