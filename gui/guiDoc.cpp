// guiDoc.cpp : implementation of the ArasanGuiDoc class
// Copyright 2001-2014, 2024 by Jon Dart. All Rights Reserved.

#include "stdafx.h"
#include "arasan.h"
#include "EngineCo.h"

#include "guiDoc.h"
#include "chessio.h"
#include "selectga.h"
#include "notation.h"
#include "boardio.h"
#include "bhash.h"
#include "legal.h"
#include "guiview.h"
#include "mainfrm.h"
#include "disphint.h"
#include <fstream>
#include <sstream>

using namespace std;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ArasanGuiDoc

IMPLEMENT_DYNCREATE(ArasanGuiDoc, CDocument)

BEGIN_MESSAGE_MAP(ArasanGuiDoc, CDocument)
//{{AFX_MSG_MAP(ArasanGuiDoc)
ON_COMMAND(ID_FILE_NEW, OnFileNew)
ON_COMMAND(ID_FILE_SAVE, OnFileSave)
ON_COMMAND_EX_RANGE(ID_FILE_MRU_FILE1, ID_FILE_MRU_FILE16, OnRecentFile)
ON_UPDATE_COMMAND_UI(ID_BROWSE_NEXTGAME, OnUpdateBrowseNextgame)
ON_COMMAND(ID_BROWSE_NEXTGAME, OnBrowseNextgame)
ON_COMMAND(ID_BROWSE_PREVIOUSGAME, OnBrowsePreviousgame)
ON_UPDATE_COMMAND_UI(ID_BROWSE_PREVIOUSGAME, OnUpdateBrowsePreviousgame)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

// Wrapper around ChessIO::get_game_description to remove extra chars at end
static void getPlayersDesc(const vector< ChessIO::Header > &hdrs, string &descr, long id) {
   ChessIO::get_game_description(hdrs,descr,1);
   string::size_type lasttab = descr.rfind("\t");
   if (lasttab != string::npos) {
     descr = descr.substr(0,lasttab);
   }
}

/////////////////////////////////////////////////////////////////////////////
// ArasanGuiDoc construction/destruction

ArasanGuiDoc::ArasanGuiDoc()
: currentMove(0), pgn_file(NULL),
  lastPing(0), lastPong(0), computerSide(Black), startOfGame(1), drawOffer(false) {
   InitializeCriticalSection(&docCriticalSection);
   ecoCoder = new ECO();
   m_bAutoDelete = FALSE;                         // solves problems crashing on exit
}


ArasanGuiDoc::~ArasanGuiDoc()
{
   delete ecoCoder;
   DeleteCriticalSection(&docCriticalSection);
}


BOOL ArasanGuiDoc::OnNewDocument()
{
   if (!CDocument::OnNewDocument())
      return FALSE;

   currentMove = 0;
   sendNew();
   reset();

   return TRUE;
}


void ArasanGuiDoc::getECO(string &eco,string &openingMove)
{
   ecoCoder->classify(moveList,eco,openingMove);
}


// Make the move, inform the engine, and update the board 
void ArasanGuiDoc::updateBoard(const Move m)
{
    lock();
    if (!IsNull(m)) {
      startOfGame = 0;
      string image;
	  Notation::image(currentBoard,m,Notation::OutputFormat::SAN,image);
      DisplayHint *dHint = new DisplayHint(currentBoard,m);
      moveList.resize(currentMove);
      moveList.add_move(currentBoard,m,image,false);
      currentBoard.doMove(m);
      currentMove = numMoves();
      UpdateAllViews(NULL,0L,dHint);
      delete dHint;
      stringstream msg;
      msg << "ping " << ++lastPing;
      writeToEngine(msg.str().c_str());
   }
   // No need to mark us as modified since we already updated the view
   SetModifiedFlag(FALSE);
   unlock();
}


void ArasanGuiDoc::goToMove(int index)
{
   ASSERT(index >= 0 && index < moveList.num_moves());
   if (index == currentMove) {
      return;
   }
   lock();
   setForce(1);
   // Note: we can't use "setboard" here because the engine
   // needs to have the previous game context. So we force
   // the engine to step back/forward through the moves:
   if (index > currentMove) {
      do {
         const MoveRecord &rec = moveList[++currentMove];
         BoardIO::readFEN(currentBoard, rec.fen);
         std::stringstream cmd;
         cmd << "usermove ";
		 Notation::image(currentBoard,rec.move,Notation::OutputFormat::SAN,cmd);
         writeToEngine(cmd.str().c_str());
      } while (index != currentMove);
   }
   else {
      while (index != currentMove) {
         const MoveRecord &rec = moveList[--currentMove];
         BoardIO::readFEN(currentBoard,rec.fen);
         writeToEngine("undo");
      }
   }
   unlock();
   ArasanMainFrame* pMainFrm = (ArasanMainFrame*)AfxGetMainWnd();
   if (pMainFrm) pMainFrm->clearStatusBar();
   SetModifiedFlag();
   UpdateAllViews(NULL);
}


void ArasanGuiDoc::goForward(const Move m)
{
   // This is called when browsing forward through a game.
   // Can't call updateBoard because it truncates the move list.
   if (doTrace) {
      traceFile << "goForward: ";
      MoveImage(m,cout);
      cout << " side=" << ColorImage(currentBoard.sideToMove()) << endl;
   }
   lock();
   startOfGame = 0;
   if (!IsNull(m)) {
      stringstream buf;
      // make sure we're in "force" mode
      setForce(1);
      // send the move to the engine
      buf << "usermove ";
	  Notation::image(currentBoard,m,Notation::OutputFormat::SAN,buf);
      writeToEngine(buf.str().c_str());
      DisplayHint *dHint = new DisplayHint(currentBoard,m);
      currentBoard.doMove(m);
      currentMove++;
      UpdateAllViews(NULL,0L,dHint);
      delete dHint;
   }
   unlock();
   ArasanMainFrame* pMainFrm = (ArasanMainFrame*)AfxGetMainWnd();
   if (pMainFrm) pMainFrm->clearStatusBar();
   // No need to mark us as modified since we already updated the view
   SetModifiedFlag(FALSE);
}


void ArasanGuiDoc::undoMove()
{
   // get last record
   if (currentMove==0) return;
   lock();
   MoveRecord rec = moveList[currentMove-1];
   if (doTrace) {
      traceFile << "ArasanGuiDoc::undoMove() ";
      MoveImage(rec.move,cout);
      cout << " side = " <<
         ColorImage(currentBoard.sideToMove()) << endl;
   }
   // apply a sanity check - move record side to move has to
   // match current board side to move
   // apply a sanity check - move record side to move has to
   // match side to move in board previous to the the current one.
   const ColorType sideToMove = currentBoard.oppositeSide();
   const ColorType recSideToMove = BoardHash::sideToMove(rec.hashcode);
   if (sideToMove != recSideToMove) {
      unlock();
      return;
   }

   setForce(1);

   char pingMsg[30];
   snprintf(pingMsg,30,"undo\nping %d\n",++lastPing);

   writeToEngine(pingMsg);

   const Move &m = rec.move;
   BoardIO::readFEN(currentBoard, rec.fen);
   currentMove--;
   result = "*";                                  // means "incomplete"
   ArasanMainFrame* pMainFrm = (ArasanMainFrame*)AfxGetMainWnd();
   if (pMainFrm) pMainFrm->clearStatusBar();
   DisplayHint *dHint = new DisplayHint(currentBoard,m);
   UpdateAllViews(NULL,0L,dHint);
   delete dHint;
   unlock();
}


/////////////////////////////////////////////////////////////////////////////
// ArasanGuiDoc serialization

void ArasanGuiDoc::Serialize(CArchive& ar)
{
   if (ar.IsStoring()) {
      // TODO: add storing code here
   }
   else {
      // TODO: add loading code here
   }
}


void ArasanGuiDoc::loadBoard(const Board &newBoard)
{
   lock();
   currentBoard = newBoard;
   moveList.reset();
   currentMove = 0;
   unlock();
   SetModifiedFlag(FALSE);
   UpdateAllViews(NULL);
}


void ArasanGuiDoc::sendNew()
{
   startOfGame = 1;
   if (engineReady()) {
      // Send options from GUI to engine. Do this before sending
      // "new," which will perform final intialization in the engine.
      stringstream s;
      s << "memory " << guiOptions->hash_table_size() << endl;
      s << "cores " << guiOptions->cores() << endl;
      if (guiOptions->tbPath() != "") {
         writeToEngine("egtpath syzygy " + guiOptions->tbPath());
      }
      s << "option Strength=" << guiOptions->strength() << endl;
      s << "option Book variety=" << guiOptions->bookVariety() << endl;
      s << "option Can resign=" << guiOptions->can_resign() << endl;
      s << "option Position learning=" << guiOptions->position_learning() << endl;
      s << "option Store games=" << guiOptions->saveGames() << endl;
      s << "option Game pathname=" << guiOptions->gamePathname() << endl;
      s << endl;

      writeToEngine(s.str().c_str());

      // We may get incoming stuff from the engine, including a move,
      // between the time we issue the "new" command here and the
      // time it's executed. So use the "ping" mechanism to enable
      // us to reject extra commands.
      ++lastPing;

      if (guiOptions->think_when_idle())
         writeToEngine("new\nhard\n");
      else
         writeToEngine("new\neasy\n");

      // "new" will reset force mode in the engine, so reset our flag too
      extern int force;
      force = 0;
      char msg[30];
      sprintf(msg,"ping %d\n",lastPing);
      writeToEngine(msg);
   }
}


void ArasanGuiDoc::reset()
{
   lock();
   Board cleanBoard;
   loadBoard(cleanBoard);
   pgnContents.clear();
   headers.clear();
   currentMove = 0;
   computerSide = Black;
   players = "";
   result = "*";                                  // means "incomplete"
   unlock();
   POSITION pos = GetFirstViewPosition();
   // Note: there is only one view (SDI)
   ArasanGuiView* pFirstView = (ArasanGuiView*)GetNextView( pos );
   SetModifiedFlag(FALSE);
   ArasanMainFrame* pMainFrm = (ArasanMainFrame*)AfxGetMainWnd();
   if (pMainFrm) pMainFrm->clearStatusBar();
   pFirstView->reset();
   pFirstView->Invalidate();
}


BOOL ArasanGuiDoc::load_pgn(Board &board,ifstream &game_file, DWORD offset,
                            vector<ChessIO::Header> &hdrs)
{
   // Make sure engine removes old game moves
   // and is at start of game.
   lock();
   sendNew();
   setForce(1);
   board.reset();
   ColorType side = White;
   game_file.clear();
   game_file.seekg(offset);

   // Collect the header:
   long first;
   {
       ChessIO::PGNReader rdr(game_file);
       rdr.collectHeaders(hdrs, first);
   }

   // See if the file contained a "FEN" tag.
   string FENString;
   if (ChessIO::get_header(hdrs,"FEN",FENString)) {
      const char *buf = FENString.c_str();

      istringstream FENstream(buf,256);
      if (!ChessIO::load_fen(FENstream,board)) {
         CString msg;
         msg.LoadString(IDS_INVALID_FEN);
         ::MessageBox(NULL,(LPCSTR)msg,"",MB_OK);
         unlock();
         return FALSE;
      }
      side = board.sideToMove();
   }

   // read the moves
   currentMove = 0;
   moveList.reset();

   Board tmp_board(board);
   bool more = true;
   int var_depth = 0;
   ChessIO::PGNReader rdr(game_file);
   ChessIO::TokenReader tokReader(rdr);
   while (more) {
       ChessIO::Token tok = tokReader.nextToken();
       switch (tok.type) {
       case ChessIO::Eof:
		   more = false;
           break;
       case ChessIO::Number:
           break;
       case ChessIO::GameMove: {
           // parse the move (but only in main variation)
		   if (!var_depth) {
			   Move m = Notation::value(tmp_board,side,Notation::InputFormat::SAN,tok.val);
			   if (IsNull(m) || !legalMove(tmp_board,m)) {
				   CString msg;
				   AfxFormatString1(msg,IDS_ILLEGAL_MOVE,tok.val.c_str());
				   ::MessageBox(NULL,(LPCSTR)msg,"",MB_OK);
				   break;
			   }
			   else {
				   std::string image;
				   // Don't use the current move string as the input
				   // parser is forgiving and will accept incorrect
				   // SAN. Convert it here to the correct form:
				   Notation::image(tmp_board,m,Notation::OutputFormat::SAN,image);
				   moveList.add_move(tmp_board,m,image.c_str(),false);
				   tmp_board.doMove(m);
			   }
			   side = OppositeColor(side);
		   }
           break;
       }
       case ChessIO::Unknown: {
           CString msg;
           AfxFormatString1(msg,IDS_UNREC_TEXT,tok.val.c_str());
           ::MessageBox(NULL,(LPCSTR)msg,"",MB_OK);
           break;
       }
       case ChessIO::Comment: {
           // ignored for now
           break;
       }
       case ChessIO::Result:
		   more = false;
           break;
	   case ChessIO::OpenVar:
		   // GUI does not handle variations (yet), so just keep track
		   // of the variation depth
		   ++var_depth;
		   break;
	   case ChessIO::CloseVar:
		   --var_depth;
		   break;
       } // end switch
   }
   // position us at the start of the game
   currentMove = 0;
   unlock();
   UpdateAllViews(NULL);
   return TRUE;
}


const char *ArasanGuiDoc::lastMoveImage() const
{
   if (currentMove == 0) return "";
   return moveList[currentMove-1].image.c_str();
}


void ArasanGuiDoc::changeComputerSide()
{
   computerSide = OppositeColor(computerSide);
   if (computerSide == White)
      writeToEngine("white\n");
   else
      writeToEngine("black\n");
}


void ArasanGuiDoc::setResult(const string &res)
{
   result = res;
   char buf[200];
   strcpy(buf,"result ");
   strcat(buf,res.c_str());
   writeToEngine(buf);
   POSITION pos = GetFirstViewPosition();
   ArasanGuiView* pFirstView = (ArasanGuiView*)GetNextView( pos );
   pFirstView->showResult(result);
}


BOOL ArasanGuiDoc::open_file(LPCSTR fileName)
{
   // framework may call this with fileName == NULL, in
   // response to OnFileNew
   if (!fileName) {
      // TBD OnNewGame();
      return FALSE;
   }
   BOOL status = FALSE;
   // get the extension:
   CString ext;
   CString name(fileName);
   int i = name.GetLength()-1;
   while (i>=0 && name[i] != '.') --i;
   if (name[i] == '.')
      ext = name.Right(name.GetLength()-i-1);
   //   free_pgn();
   pgn_file = new ifstream();

   pgn_file->open(fileName,ios::in | ios::binary);
   if (!pgn_file->is_open() || !pgn_file->good()) {
      CString msg;
      AfxFormatString1(msg,IDS_CANT_OPEN,fileName);
      AfxMessageBox(msg);
      return FALSE;
   }

   ext.MakeLower();
   BOOL multipleGameFile = FALSE;
   if (ext == "pgn") {
      SetPathName(fileName);

      currentBoard.reset();
	  pgnContents.clear();

      // might take awhile, put up a wait cursor. A
      // progress bar would be nicer ...
      BeginWaitCursor();
      // scan just the PGN headers, so we can tell the
      // user what's in the file:
      ChessIO::PGNReader rdr(*pgn_file);
      rdr.scanPGN(pgnContents);
      EndWaitCursor();

      DWORD pgnOffset = 0L;
      if (pgnContents.size() > 1) {
         // let the user choose a game to view
         POSITION pos = GetFirstViewPosition();
         CView* pFirstView = GetNextView( pos );

         SelectGame dlg(pgnContents,pFirstView);
         if (dlg.DoModal() == IDCANCEL) {
            return FALSE;
         }
         selectGame(dlg.get_selection());
         return TRUE;
      }
	  headers.clear();
      status = load_pgn(currentBoard,*pgn_file,pgnOffset,headers);
	  getPlayersDesc(headers, players, 1);
      UpdateAllViews(NULL);
   }
   else if (ext == "fen") {
      status = ChessIO::load_fen(*pgn_file,currentBoard);
      if (!status) {
         CString msg;
         AfxFormatString2(msg,IDS_INVALID_BOARD,fileName,ext);
         AfxMessageBox(msg);
         reset();
      }
      else {
         char buf[256];
         // Sync engine with new board position
         ostringstream s(buf,256);
         s << "setboard " << currentBoard;
         writeToEngine(s.str().c_str());
         moveList.reset();
         setResult("*");
         currentMove = 0;
         SetPathName(fileName);
         SetModifiedFlag();
         UpdateAllViews(NULL);
      }
   }
   else {
      CString msg;
      msg.LoadString(IDS_UNKNOWN_FILE_TYPE);
      AfxMessageBox((LPCSTR)msg);
      return FALSE;
   }
   // If we read a pgn file consisting of >1 game, keep the file
   // open:
   if (!multipleGameFile) {
      if (pgn_file) pgn_file->close();
      delete pgn_file;
      pgn_file = NULL;
   }
   POSITION pos = GetFirstViewPosition();
   ArasanGuiView* theView = (ArasanGuiView*)GetNextView( pos );
   theView->fileLoaded();
   theView->Invalidate();
   // We have updated the view
   return TRUE;
}


/////////////////////////////////////////////////////////////////////////////
// ArasanGuiDoc diagnostics

#ifdef _DEBUG
void ArasanGuiDoc::AssertValid() const
{
   CDocument::AssertValid();
}


void ArasanGuiDoc::Dump(CDumpContext& dc) const
{
   CDocument::Dump(dc);
}
#endif                                            //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ArasanGuiDoc commands

void ArasanGuiDoc::OnCloseDocument()
{
   POSITION pos = GetFirstViewPosition();
   ArasanGuiView* pView = (ArasanGuiView*)GetNextView( pos );
   pView->saveOptions();

   CDocument::OnCloseDocument();
}


void ArasanGuiDoc::OnFileNew()
{
   OnNewDocument();
}


void ArasanGuiDoc::nextGame()
{

   selectGame(++pgnSelection);
}


void ArasanGuiDoc::previousGame()
{
   ASSERT(pgnSelection > 0);
   selectGame(--pgnSelection);
}


void ArasanGuiDoc::selectGame(DWORD index)
{
   pgnSelection = index;
   const string &header = pgnContents[pgnSelection];
   string::size_type indx = header.find('\t');
   DWORD offset;
   string num = header.substr(indx+1,header.length()-indx-1);
   sscanf(num.c_str(),"%ld",&offset);
   vector< ChessIO::Header > hdrs;

   sendNew();
   Board cleanBoard;
   loadBoard(cleanBoard);

   load_pgn(currentBoard,*pgn_file,offset,hdrs);
   getPlayersDesc(hdrs, players, 1);

   currentMove = 0;
   computerSide = Black;
   POSITION pos = GetFirstViewPosition();
   // Note: there is only one view (SDI)
   ArasanGuiView* pFirstView = (ArasanGuiView*)GetNextView( pos );
   SetModifiedFlag(FALSE);
   ArasanMainFrame* pMainFrm = (ArasanMainFrame*)AfxGetMainWnd();
   if (pMainFrm) pMainFrm->clearStatusBar();
   // clear result since we are at the start of the game (result
   // is shown after navigation to game end)
   setResult("*");
   pFirstView->reset(ArasanGuiView::NewPgn);
}

void ArasanGuiDoc::storeGame(ostream &ofs)
{
   if (headers.size() > 0) {
      int ecoFound = 0;
      for (auto it = headers.begin(); it != headers.end(); it++) {
		 const ChessIO::Header &p = *it;
         if (p.tag() == "ECO") {
            ++ecoFound;
         }
      }
      if (!ecoFound) {
          headers.push_back(ChessIO::Header("ECO",eco.c_str()));
      }
   }
   else {
       headers.push_back(ChessIO::Header("ECO",eco.c_str()));
   }

   ChessIO::store_pgn(ofs,moveList,computerSide,result,headers);
}


void ArasanGuiDoc::OnFileSave()
{
   CString filter;
   filter.LoadString(IDS_FILE_OPEN_FILTER);
   CFileDialog dlg(FALSE,
      "pgn",                                      // default file extension
      NULL,                                       // no default filename
      OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      (LPCSTR)filter,
      AfxGetMainWnd());
   if (dlg.DoModal() == IDOK) {
      // check the file extension
      CString str = dlg.GetFileExt();
      CString fileName = dlg.GetPathName();
      // Don't allow trailing spaces. Under NT these are part of the file name.
      // It is too easy to get these in due to typos.
      const int last = fileName.GetLength()-1;
      int j = 0;
      while (j<=last) {
         if (fileName[j] != ' ')
            break;
         ++j;
      }
      int i = last;
      while (i) {
         if (fileName[i] != ' ')
            break;
         i--;
      }
      fileName = fileName.Mid(j,i+1-j);
      ofstream ofs(fileName,ios::out);
      if (!ofs.good()) {
         CString msg;
         AfxFormatString1(msg,IDS_CANT_OPEN,fileName);
         AfxMessageBox(msg);
         return;
      }
      str.MakeLower();
      str = str.Left(3);
      if (str == "pgn") {
         int ecoFound = 0;
         for (auto it = headers.begin(); it != headers.end(); it++) {
			const ChessIO::Header &p = *it;
            if (p.tag() == "ECO") {
               ++ecoFound;
               break;
            }
         }
         // TBD if (!ecoFound)
         // TBD   headers.append(StringPair("ECO",eco));
         ChessIO::store_pgn(ofs,moveList,computerSide,result,headers);
      }
      else if (str == "fen" || str == "epd") {
         ChessIO::store_fen(ofs,currentBoard);
      }
      else {
         CString msg;
         AfxFormatString1(msg,IDS_UNKNOWN_FILE_TYPE,(LPCSTR)str);
         AfxMessageBox((LPCSTR)msg);
      }
      ofs.close();
   }

}


BOOL ArasanGuiDoc::OnRecentFile(UINT nID)
{
   ArasanGuiApp* pApp = (ArasanGuiApp*)AfxGetApp();
   if (OnOpenDocument(pApp->getRecentFile(nID - ID_FILE_MRU_FILE1)))
      setForce(1);
   else
      return FALSE;
   return TRUE;                                   // We processed this message
}


BOOL ArasanGuiDoc::OnOpenDocument(LPCTSTR lpszPathName)
{
   // We don't use the default MFC method
   return open_file(lpszPathName);
}


BOOL ArasanGuiDoc::SaveModified()
{
   // We don't generally edit game files .. so don't
   // prompt to change a saved document. The user can
   // still save manually, if desired (this is how
   // Arasan 5.4 and previous versions operated).

   return TRUE;
}


void ArasanGuiDoc::OnUpdateBrowseNextgame(CCmdUI* pCmdUI)
{
   const vector <string> *pgnContents = getPGNContents();
   pCmdUI->Enable(pgnContents && (getPGNListSize() > 1) &&
      (getPGNSelection() < getPGNListSize()-1));
}


void ArasanGuiDoc::OnBrowseNextgame()
{
   nextGame();
}


void ArasanGuiDoc::OnBrowsePreviousgame()
{
   previousGame();
}


void ArasanGuiDoc::OnUpdateBrowsePreviousgame(CCmdUI* pCmdUI)
{
   const vector <string> *pgnContents = getPGNContents();
   pCmdUI->Enable(pgnContents && (getPGNListSize() > 1) &&
      (getPGNSelection() > 0));
}
