// Copyright 1994, 1995, 2008, 2012-2014, 2017-2018  by Jon Dart.
// All Rights Reserved.

#include "chessio.h"
#include "boardio.h"
#include "epdrec.h"
#include "debug.h"
#include "constant.h"

#include <algorithm>
#include <cstring>
#include <sstream>

using namespace std;

#define PGN_MARGIN 70
#define MAX_TAG 255
#define MAX_VAL 255

// This module handles reading and writing board and
// game info.

static int skip_space(istream &game_file)
{
    int c = EOF;
    while (!game_file.eof()) {
       c = game_file.get();
       if (!isspace(c) && (c!='\n'))
       {
           break;
       }
    }
    return c;
}

void ChessIO::get_game_description(const vector<Header> &hdrs, string &descr, long id)
{
      stringstream s;
      string tmp;
      get_header(hdrs, "White", tmp);
      string::size_type comma = tmp.find(',',0);
      if (comma != string::npos)
      {
          tmp = tmp.substr(0,comma);
      }
      s << tmp << '-';
      get_header(hdrs, "Black", tmp);
      comma = tmp.find(',',0);
      if (comma != string::npos)
      {
          tmp = tmp.substr(0,comma);
      }
      s << tmp;
      get_header(hdrs, "Site", tmp);
      s << ", " << tmp;
      get_header(hdrs, "Round", tmp);
      if (tmp.length() >0 && tmp[0] != '?')
      {
        s << '(' << tmp << ')';
      }
      s << ' ';
      get_header(hdrs, "Date", tmp);
      s << tmp.substr(0,4);
      get_header(hdrs, "Result", tmp);
      if (tmp != "*")
      {
        s << ' ' << tmp;
      }
      s << '\t' << id;
      descr = s.str();
}

int ChessIO::scan_pgn(istream &game_file, vector<string> &contents)
{
    Board board;
    int c;

    while (game_file.good() && !game_file.eof())
    {
        // Collect the header:
        long first;
        vector<Header> hdrs;
        string eventStr;
        collect_headers(game_file, hdrs, first);
        if (get_header(hdrs,"Event",eventStr))
        {
           // We have the headers, munge them into a single-line game
           // description. Append the index to the game so the GUI
           // can navigate to the game when the user clicks on the
           // description.
           string descr;
           get_game_description(hdrs, descr, first);
           contents.push_back(descr);
        }
        hdrs.clear();

        while (game_file.good() && !game_file.eof())
        {
           if ((c = game_file.get()) == '[')
           {
               game_file.putback(c);
               break;
           }
        }
    }
    return 1;
}


int ChessIO::get_header(const vector<Header> &hdrs,
  const string &key, string &val)
{
    val = "";
    for (auto it = hdrs.begin(); it != hdrs.end(); it++)
    {
         if ((*it).tag() == key)
         {
             val = (*it).value();
             return 1;
         }
    }
    return 0;
}

void ChessIO::add_header(vector <Header> &hdrs,
  const string &key, const string & val)
{
   hdrs.push_back(Header(key,val));
}

int ChessIO::load_fen(istream &ifs, Board &board)
{
    ifs >> board;
    board.state.moveCount = 0;
    return ifs.good();
}

int ChessIO::store_fen( ostream &ofile, const Board &board)
{
    ofile << board << endl;
    return ofile.good();
}

int ChessIO::store_pgn(ostream &ofile,MoveArray &moves,
                        const ColorType computer_side,
                        const string &result,
                        vector<Header> &headers)
{
    // Write standard PGN header.

    string gameResult = result;
    if (result.length() == 0)
       gameResult = "*";
    string val;
    vector <Header> newHeaders;
    if (!get_header(headers, "Event", val))
       add_header(newHeaders,"Event","?");
    else
       add_header(newHeaders,"Event",val);
    if (!get_header(headers, "Site", val))
       add_header(newHeaders,"Site","?");
    else
       add_header(newHeaders,"Site",val);
    if (!get_header(headers, "Date", val))
    {
       char dateStr[15];
       time_t tm = time(NULL);
       struct tm *t = localtime(&tm);
       sprintf(dateStr,"%4d.%02d.%02d",t->tm_year+1900,t->tm_mon+1,
                        t->tm_mday);
       add_header(newHeaders,"Date",dateStr);
    }
    else
       add_header(newHeaders,"Date",val);
    if (!get_header(headers, "Round", val))
       add_header(newHeaders,"Round","?");
    else
       add_header(newHeaders,"Round",val);

    string name("Arasan ");
    name += Arasan_Version;
    if (computer_side == White)
    {
       if (!get_header(headers, "White",val))
       {
         add_header(newHeaders, "White", name);
       }
       else
         add_header(newHeaders,"White",val);
       if (!get_header(headers, "Black",val))
         add_header(newHeaders,"Black","?");
       else
         add_header(newHeaders,"Black",val);
    }
    else
    {
       if (!get_header(headers, "White",val))
         add_header(newHeaders,"White","?");
       else
         add_header(newHeaders,"White",val);
       if (!get_header( headers, "Black",val))
         add_header(newHeaders, "Black", name);
       else
         add_header(newHeaders,"Black",val);
    }
    // "Result" may contain a comment. Don't put this in the
    // PGN header.
    string shortResult = gameResult;
    string longResult = gameResult;
    string::size_type space = gameResult.find(" ");
    if (space >0 && space < MAX_PATH)
       shortResult = gameResult.erase(space);
    if (!get_header(headers,"Result",val))
       add_header(newHeaders,"Result",shortResult);
    else
       add_header(newHeaders,"Result",val);

    // We have now written all the mandatory headers.
    // Add any more headers that were passed into us.

    for (auto it = headers.begin(); it != headers.end(); it++)
    {
        Header p = *it;
        if (p.tag() !="Event" &&
            p.tag() !="Site" &&
            p.tag() != "Date" &&
            p.tag() != "Round" &&
            p.tag() != "White" &&
            p.tag() != "Black" &&
            p.tag() != "Result")
            add_header(newHeaders,p.tag(),p.value());
    }

    // write headers and cleanup
    return store_pgn(ofile,moves,longResult,newHeaders);
}

int ChessIO::store_pgn(ostream &ofile, MoveArray &moves,const string &result,
                         vector<Header> &headers)
{
    for (auto it = headers.begin(); it != headers.end(); it++) {
       Header p = *it;
       ofile << "[" << p.tag() << " \"" << p.value() << "\"]" << endl;
    }
    ofile << endl;

    // Write game moves.
    unsigned len = moves.num_moves();
    stringstream buf;
    for (unsigned i = 0; i < len; i++) {
        const MoveRecord &e = moves[i];
        stringstream numbuf;
        if (i % 2 == 0) {
            numbuf << (i/2)+1 << ". ";
        }
        const int image_size = (int)e.image().length();
        if ((int)buf.tellp() + image_size + numbuf.str().length() + 1 >= PGN_MARGIN) {
            ofile << buf.str() << endl;
            buf.str("");
        }
        if (buf.tellp() != (streampos)0) {
           buf << ' ';
        }
        buf << numbuf.str() << e.image();
    }
    if ((int)buf.tellp() + result.length() + 1 >= PGN_MARGIN) {
        ofile << buf.str() << endl;
        buf.str("");
        buf << result;
    } else {
        buf << ' ' << result;
    }
    ofile << buf.str() << endl;
    if (!ofile) {
        return 0;
    }
    return 1;
}

int ChessIO::readEPDRecord(istream &ifs, Board &board, EPDRecord &rec)
{
    rec.clear();
    // read FEN description
    ifs >> board;
    if (ifs.eof())
       return 0;
    if (!ifs.good())
    {
        rec.setError("Bad EPD record: FEN board description missing or invalid");
        ifs.clear();
        ifs.ignore(4096,'\n');
        return 1;
    }
    // read EPD commands
    string commands, command;
    getline(ifs,commands);
    stringstream s(commands);
    while (getline(s,command,';')) {
       // each command should contain a verb and a value
       string cmd,val;
       auto it = command.begin();
       auto end = command.end();
       // command does not have embedded spaces
       while (it != end && isspace(*it)) it++;
       while (it != end && !isspace(*it)) cmd += *it++;
       while (it != end && isspace(*it)) it++;
       // value may have spaces (multiple values) or be quoted
       while (it != end) val += *it++;
       rec.add(cmd,val);
    }
    return 1;
}

void ChessIO::writeEPDRecord(ostream &ofs, Board &board, const EPDRecord &rec)
{
   BoardIO::writeFEN(board,ofs,0);
   for (unsigned i = 0; i < rec.getSize(); i++) {
      string key,val;
      (void)rec.getData(i,key,val);
      ofs << ' ' << key << ' ' << val << ';';
   }
   ofs << endl;
}

void ChessIO::collect_headers(istream &game_file,vector <Header>&hdrs, long &first)
{
   first = -1L;
   int c;
   bool firstTag = true;
   while (!game_file.eof())
   {
      char tag[MAX_TAG+1];
      char val[MAX_VAL+1];
      c = skip_space(game_file);
      if (c!='[')
      {
         game_file.putback(c);
         break;
      }
      else
      {
         if (first == -1)
            first = (long)game_file.tellg()-1;
         int t = 0;
         while (!game_file.eof())
         {
            c = game_file.get();
            if (!isspace(c) && c != '"' && t < MAX_TAG)
               tag[t++] = c;
            else
               break;
         }
         tag[t] = '\0';
         if (firstTag)
         {
            if (strcmp(tag,"vent") == 0)
            {
               // It appears that there is a bug in iostream.
               // Calling tellg (above) sometimes causes
               // a missing char when the next read occurs.
               // So we get a partial tag. This is a workaround.
               strcpy(tag,"Event");
            }
         }
         firstTag = false;
         if (isspace(c)) {
            c = skip_space(game_file);
         }
         val[0] = '\0';
         if (c=='"')
         {
            int v = 0;
            while (!game_file.eof())
            {
               c = game_file.get();
               if (c != '"' && v < MAX_VAL)
                  val[v++] = c;
               else
                  break;
            }
            val[v] = '\0';
         }
         hdrs.push_back(Header(tag,val));
         while (!game_file.eof() && c != ']')
            c = game_file.get();
      }
   }
}

ChessIO::Token ChessIO::get_next_token(istream &game_file) {
    TokenType tok = Unknown;
    int c = EOF;
    string value;
    while (!game_file.eof()) {
      c = game_file.get();
      if (!isspace(c)) break;
    }
    if (c == EOF) {
      return Token(Eof,value);
    }
    else if (c=='[') {
       // Not expected within a game since we should have already
       // read the headers. Probably the start of the next game.
       game_file.putback(c);
       return Token(Eof,value);
    }
    else if (c=='{') {
        value += c;
        while (game_file.good() && c != '}') {
            c = game_file.get();
            if (c == EOF) break;
            value += c;
        }
        return Token(Comment,value);
    }
    else if (c == '(') {
        value += '(';
        return Token(OpenVar,value);
    }
    else if (c == ')') {
        value += ')';
        return Token(CloseVar,value);
    }
    else if (c == '$') {
        value += c;
        while (game_file.good()) {
            c = game_file.get();
            if (!game_file.good()) {
                break;
            }
            else if (!isdigit(c)) {
                game_file.putback(c);
                break;
            }
            value += c;
        }
        return Token(NAG,value);
    }
    else if (c == '.') {
        value += c;
        if (!game_file.eof() && game_file.good()) {
             c = game_file.get();
             if (c == '.') {
                 value += c;
                 tok = BlackMove;
             } else {
                 game_file.putback(c);
             }
        }
    }
    else if (isdigit(c)) {
        // peek at next char.
        int nextc = game_file.get();
        if (c == '0') {
           if (nextc == '-')
           {
               // some so-called PGN files have 0-0 or 0-0-0 for
               // castling.  To handle this, we need to peek ahead
               // one more character.
               int nextc2 = game_file.peek();
               if (toupper(nextc2) == 'O' || nextc2 == '0') {
                  // castling, we presume
                  value += c;
                  c = nextc;
                  while (game_file.good() && (c == '-' ||
                    c == '0' || toupper(c) == 'O' || c == '+')) {
                      value += c;
                      c = game_file.get();
                  }
                  game_file.putback(c);
                  return Token(GameMove,value);
               }
           }
        }
        if (nextc == '-' || nextc == '/') { // assume result
            value += c;
            c = nextc;
            while (!game_file.eof() && game_file.good() && !isspace(c)) {
                value += c;
                c = game_file.get();
            }
            tok = Result;
        }
        else {
            // Assume we have a move number.
            value += c;
            c = nextc;
            while (game_file.good() && isdigit(c))
            {
               value += c;
               c = game_file.get();
            }
            if (game_file.good() && c == '.') {
                value+=c;
            } else {
                game_file.putback(c);
            }
            tok = Number;
        }
   }
   else if (isalpha(c)) {
       while (game_file.good() && (isalnum(c)
              || c == '-' || c == '=' || (c == '+'))) {
           value += c;
           c = game_file.get();
       }
       game_file.putback(c);
       tok = GameMove;
   }
   else if (c == '#') { // "Checkmate"
       value += c;
       tok = Ignore;
   }
   else if (c == '*') {
       value += c;
       tok = Result;
   }
   else {
       value += c;
       tok = Unknown;
   }
   return Token(tok,value);
}



