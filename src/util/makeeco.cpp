// Copyright 1995, 2007, 2008, 2012-2014, 2021 by Jon Dart
// Stand-alone console executable to build "ecodata.cpp" file
// from "eco" text file.

#include "board.h"
#include "notation.h"
#include "legal.h"
#include "globals.h"
#include "scoring.h"
#include <cctype>
#include <iostream>
#include <fstream>
#include <regex>
#include <sstream>
#include <unordered_map>

void charout(unsigned char c)
{
    if (c == '"')
    {
        std::cout << '\\' << '"';
    }
    else if (c == '\\')
    {
        std::cout << '\\' << '\\';
    }
    else if (c >= ' ' && (int)c < 127)
    {
        std::cout << c;
    }
    else
    {
        std::cout << '\\';
        std::ios_base::fmtflags original_flags = std::cout.flags();
	std::cout.fill('0');
        std::streamsize oldwidth = std::cout.width();
	std::cout.width(3);
	std::cout << std::oct << (unsigned)c;
	std::cout.width(oldwidth);
	std::cout.fill(' ');
        std::cout.flags(original_flags);
    }
}

static void write_64(hash_t h, std::ostream &out)
{
   std::ios_base::fmtflags original_flags = out.flags();
   out << "0x" << std::hex << h << "ULL";
   out.flags(original_flags);
}

static std::unordered_map<hash_t,unsigned> hashes;

static const auto pattern = std::regex("^([A-Z\\d]+)\\s+(.+[\\w\\d\\-\\+#])(\\s+\\\"([^\"]*).*)?$");

int do_eco(const std::string &eco_line, unsigned line)
{
    if (eco_line.empty()) return 0;

    ColorType side = White;
    Board board;
    int movecount = 0;

    // Parse the line
    std::string code, name, moves, text;
    std::smatch match;
    if (std::regex_match(eco_line,match,pattern)) {
        assert(match.size()>=3);
        name = ""; // may be missing
        code = match[1]; // first match is whole string
        moves = match[2];
        if (match.size() > 4) {
            name = match[4]; // inner group
        }
        // parse moves
        std::stringstream s(moves);
        while (s >> text) {
            // skip numbers
            if (isdigit(text[0])) continue;
            // parse the move
            Move m = Notation::value(board,side,Notation::InputFormat::SAN,text);
            if (IsNull(m) ||
                !legalMove(board,StartSquare(m),DestSquare(m)))
            {
                std::cerr << "Illegal or invalid move: " << text << std::endl;
                return -1;
            }
            else {
                ++movecount;
                board.doMove(m);
            }
            side = OppositeColor(side);
        }
    } else {
        std::cerr << "Failed to parse line " << line << std::endl;
        return -1;
    }
    auto hashMatch = hashes.find(board.hashCode());
    if (hashMatch != hashes.end()) {
       std::cerr << "warning: duplicate hash, lines " << (*hashMatch).second << " & " << line << std::endl;
    }
    hashes.emplace(std::pair<hash_t,unsigned>(board.hashCode(),line));
    std::cout << '{' << '"' << code << '"' << ", ";
    write_64(board.hashCode(),std::cout);
    std::cout << ", ";
    if (name.length())
        std::cout << '"' << name << '"';
    else
        std::cout << '"' << '"';
    std::cout << "},";
    std::cout << std::endl;
    return 0;
}

int CDECL main(int argc, char **argv)
{
   if (argc ==1)
   {
	 std::cerr << "Usage: makeeco eco_file" << std::endl;
	 return -1;
   }
   Bitboard::init();
   Board::init();
   globals::initOptions();
   Attacks::init();
   Scoring::init();
   if (!globals::initGlobals(false)) {
       globals::cleanupGlobals();
       exit(-1);
   }
   atexit(globals::cleanupGlobals);

   std::ifstream eco_file( argv[argc-1], std::ios::in);
   if (!eco_file.good())
   {
         std::cerr << "Cannot open file: " << argv[argc-1] << std::endl;
	 return -1;
   }
   Board b;
   std::cout << "// This is a machine-generated file.  Do not edit." << std::endl;
   std::cout << std::endl;
   std::cout << "#include \"ecodata.h\"" << std::endl << std::endl;
   std::cout << "const struct ECOData eco_codes[] =" << std::endl;
   std::cout << "{{" << '"' << "A00" << '"' << ", ";
   write_64(b.hashCode(),std::cout);
   std::cout << ", " << '"' << '"' << "}," << std::endl;
   unsigned lines = 1;
   while (eco_file.good() && !eco_file.eof())
   {
       std::string eco_line;
       getline(eco_file,eco_line);
       if (do_eco(eco_line,lines)) {
           std::cerr << "error in ECO file, line " << lines << std::endl;
       }
       ++lines;
   }
   std::cout << "{0,0," << '"' << '"' << "}};" << std::endl;
   return 0;
}
