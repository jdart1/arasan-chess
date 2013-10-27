// Copyright 1995, 2007, 2008, 2012, 2013 by Jon Dart
// Stand-alone console executable to build "ecodata.cpp" file
// from "eco" text file.

#include "board.h"
#include "notation.h"
#include "legal.h"
#include "globals.h"
#include <iostream>
#include <fstream>
#include <ctype.h>

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
	cout.fill('0');
        std::streamsize oldwidth = cout.width();
	cout.width(3);
	cout << (oct) << (unsigned)c;
	cout.width(oldwidth);
	cout.fill(' ');
    }
}

static void write_64(hash_t h, ostream &out)
{
  out << "0x" << (hex) << h << "ULL";
}

static int skip_space(ifstream &eco_file)
{
    char c = EOF;
    while (!eco_file.eof())
    {
       c = eco_file.get();
       if (!isspace(c) && (c!='\n'))
       {
	   break;
       }
    }
    return c;
}

void do_eco(ifstream &eco_file)
{
    ColorType side = White;
    Board board;
    int movecount = 0;
    char c;
    // Collect the ECO code
    c = skip_space(eco_file);
    static char code[20];
    int i = 0;
    if (isalpha(c) && isupper(c))
    {
        while (!eco_file.eof() && !isspace(c) && i < 19)
	{
           code[i++] = c;
	   c = eco_file.get();
	}
    }
    code[i] = '\0';
    if (i == 0)
        return;
    cout << '{' << '"' << code << '"' << ", ";
    static char name[255];
    char *name_ptr = name;
    *name_ptr = '\0';
    // Follow the opening line
    while (eco_file.good() && !eco_file.eof())
    {
       // skip to next move
       while (!eco_file.eof())
       {
	  if (c == '\n' || c == '"')
	      break;
	  else if (isalpha(c))
	  {
	     break;
	  }
          c = eco_file.get();
       }
       if (c == '"')
       {
          while (!eco_file.eof())
          {
	     c = eco_file.get();
	     if (c == '"')
                 break;
             else
                 *name_ptr++ = c;
          }
	  *name_ptr = '\0';
          while (!eco_file.eof() && c != '\n')
             c = eco_file.get();
       }
       if (c == '\n')
          break;
       // collect the move text
       char movebuf[20];
       int i = 0;
       while (i < 19 && !eco_file.eof() && 
       	      !isspace(c) && (c != '\n'))
       {
          movebuf[i] = c; ++i;
	  c = eco_file.get();
       }
       if (i == 0)
       	  break;
       movebuf[i] = '\0';
       // parse the move
       Move m = Notation::value(board,side,Notation::SAN_IN,movebuf);
       if (IsNull(m) ||
       	   !legalMove(board,StartSquare(m),DestSquare(m)))
       {
           cerr << "Illegal or invalid move: " << movebuf << endl;
           cout << "Illegal or invalid move: " << movebuf << endl;
	   break;
       }
       else
       {
           ++movecount;
	   board.doMove(m);
       }
       side = OppositeColor(side);
    }
    write_64(board.hashCode(),cout);
    cout << " ,";
    if (*name)
       cout << '"' << name << '"';
    else
       cout << '"' << '"';
    cout << "},";
    cout << endl;
}

int CDECL main(int argc, char **argv)
{
   if (argc ==1)
   {
	 cerr << "Usage: makeeco eco_file" << endl;
	 return -1;
   }
   Bitboard::init();
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
   while (eco_file.good() && !eco_file.eof())
   {
       do_eco(eco_file);
   }
   cout << "{0,0," << '"' << '"' << "}};" << endl;
   return 0;
}
