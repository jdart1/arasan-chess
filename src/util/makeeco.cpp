// Copyright 1995, 2007, 2008, 2012-2014, 2021 by Jon Dart
// Stand-alone console executable to build "ecodata.cpp" file
// from "eco" text file.

#include "board.h"
#include "notation.h"
#include "legal.h"
#include "globals.h"
#include <iostream>
#include <fstream>
#include <cctype>
#include <unordered_map>

void charout(unsigned char c)
{
    if (c == '"')
    {
        cout << '\\' << '"';
    }
    else if (c == '\\')
    {
        cout << '\\' << '\\';
    }
    else if (c >= ' ' && (int)c < 127)
    {
        cout << c;
    }
    else
    {
        cout << '\\';
        std::ios_base::fmtflags original_flags = cout.flags();
	cout.fill('0');
        std::streamsize oldwidth = cout.width();
	cout.width(3);
	cout << (oct) << (unsigned)c;
	cout.width(oldwidth);
	cout.fill(' ');
        cout.flags(original_flags);
    }
}

static void write_64(hash_t h, ostream &out)
{
   std::ios_base::fmtflags original_flags = out.flags();
   out << "0x" << (hex) << h << "ULL";
   out.flags(original_flags);
}

static std::unordered_map<hash_t,unsigned> hashes;

int do_eco(const string &eco_line, unsigned line)
{
    ColorType side = White;
    Board board;
    int movecount = 0;
    stringstream s(eco_line);
    const string &str = s.str();
    string::const_iterator it = str.begin();

    // Follow the opening line
    string token;
    string code;
    string name;
    int first_token = 1;
    while (it != str.end()) {
       // skip spaces
       while (isspace(*it) && it != str.end()) it++;
       if (it == str.end()) break;
       // extract text
       string text;
       int first = 1;
       int quoted = 0;
       while (it != str.end() && (quoted ? (*it != '"') : !isspace(*it))) {
           if (first && *it == '"') {
               quoted++;
               it++;
               first = 0;
               continue;
           }
           text += *it++;
       }
       if (first_token) {
           code = text;
           first_token = 0;
           continue;
       }
       else if (quoted) {
           name = text;
           break;
       } else if (text.length() == 0) {
           break;
       }
       // skip numbers
       if (isdigit(text[0])) continue;
       if (!isalpha(text[0])) return -1;
       // parse the move
       Move m = Notation::value(board,side,Notation::InputFormat::SAN,text);
       if (IsNull(m) ||
       	   !legalMove(board,StartSquare(m),DestSquare(m)))
       {
           cerr << "Illegal or invalid move: " << text << endl;
	   return -1;
       }
       else
       {
           ++movecount;
	   board.doMove(m);
       }
       side = OppositeColor(side);
    }
    if (code.length()) {
        auto match = hashes.find(board.hashCode());
        if (match != hashes.end()) {
          cerr << "warning: duplicate hash, lines " << (*match).second << " & " << line << endl;
        }
        hashes.emplace(std::pair<hash_t,unsigned>(board.hashCode(),line));
        cout << '{' << '"' << code << '"' << ", ";
        write_64(board.hashCode(),cout);
        cout << ", ";
        if (name.length())
            cout << '"' << name << '"';
        else
            cout << '"' << '"';
        cout << "},";
        cout << endl;
    }
    return 0;
}

int CDECL main(int argc, char **argv)
{
   if (argc ==1)
   {
	 cerr << "Usage: makeeco eco_file" << endl;
	 return -1;
   }
   Bitboard::init();
   Board::init();
   initOptions(argv[0]);
   Attacks::init();
   Scoring::init();
   if (!initGlobals(argv[0], false)) {
       cleanupGlobals();
       exit(-1);
   }
   atexit(cleanupGlobals);
   
   ifstream eco_file( argv[argc-1], ios::in);
   if (!eco_file.good())
   {
         cerr << "Cannot open file: " << argv[argc-1] << endl;
	 return -1;
   }
   Board b;
   cout << "// This is a machine-generated file.  Do not edit." << endl;
   cout << endl;   
   cout << "#include \"ecodata.h\"" << endl << endl;
   cout << "const struct ECOData eco_codes[] =" << endl;
   cout << "{{" << '"' << "A00" << '"' << ", ";
   write_64(b.hashCode(),cout);
   cout << ", " << '"' << '"' << "}," << endl;
   unsigned lines = 1;
   while (eco_file.good() && !eco_file.eof())
   {
       string eco_line;
       getline(eco_file,eco_line);
       if (do_eco(eco_line,lines)) {
           cerr << "error in ECO file, line " << lines << endl;
       }
       ++lines;
   }
   cout << "{0,0," << '"' << '"' << "}};" << endl;
   return 0;
}
