// guiView.cpp : implementation of the ArasanGuiView class
// Copyright 2001-2013, 2024 by Jon Dart. All Rights Reserved.

#include "stdafx.h"
#include "arasan.h"

#include "guiDoc.h"
#include "guiView.h"
#include "chessio.h"
#include "clock.h"
#include "legal.h"
#include "promotio.h"
#include "notation.h"
#include "colordlg.h"
#include "eco.h"
#include "scoring.h"
#include "movegen.h"
#include "selectga.h"
#include "srclimit.h"
#include "stats.h"
#include "scoring.h"
#include "EngineCo.h"
#include "MainFrm.h"
#include "options.h"
#include "boardio.h"
#include "ShowGame.h"
#include "apprdlg.h"
#include "prefdlg.h"
#include "disphint.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const UINT MAIN_TIMER = 1;

/////////////////////////////////////////////////////////////////////////////
// ArasanGuiView

IMPLEMENT_DYNCREATE(ArasanGuiView, CView)

BEGIN_MESSAGE_MAP(ArasanGuiView, CView)
//{{AFX_MSG_MAP(ArasanGuiView)
ON_WM_PAINT()
ON_WM_LBUTTONDOWN()
ON_WM_LBUTTONUP()
ON_COMMAND(ID_FORWARD, OnForward)
ON_COMMAND(ID_GAME_ENDOFGAME, OnEndofgame)
ON_COMMAND(ID_TAKEBACK_MOVE, OnTakebackMove)
ON_COMMAND(ID_BEGIN_GAME, OnBeginGame)
ON_UPDATE_COMMAND_UI(ID_BEGIN_GAME, OnUpdateBeginGame)
ON_UPDATE_COMMAND_UI(ID_FORWARD, OnUpdateForward)
ON_UPDATE_COMMAND_UI(ID_TAKEBACK_MOVE, OnUpdateTakebackMove)
ON_UPDATE_COMMAND_UI(ID_GAME_ENDOFGAME, OnUpdateEndofgame)
ON_COMMAND(ID_OPTIONS_SEARCH_LIMITS, OnOptionsSearchLimits)
ON_WM_TIMER()
ON_UPDATE_COMMAND_UI(ID_COMPUTE, OnUpdateCompute)
ON_COMMAND(ID_COMPUTE, OnCompute)
ON_UPDATE_COMMAND_UI(ID_BROWSE_SELECTGAME, OnUpdateBrowseSelectgame)
ON_COMMAND(ID_BROWSE_SELECTGAME, OnBrowseSelectgame)
ON_UPDATE_COMMAND_UI(ID_CLOCK_PAUSE, OnUpdateClockPause)
ON_COMMAND(ID_CLOCK_PAUSE, OnClockPause)
ON_UPDATE_COMMAND_UI(ID_CLOCK_RESET, OnUpdateClockReset)
ON_COMMAND(ID_CLOCK_RESET, OnClockReset)
ON_UPDATE_COMMAND_UI(ID_GAME_ROTATE_BOARD, OnUpdateGameRotateBoard)
ON_COMMAND(ID_GAME_ROTATE_BOARD, OnGameRotateBoard)
ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_POSITION, OnUpdateEditCopyPosition)
ON_COMMAND(ID_EDIT_COPY_POSITION, OnEditCopyPosition)
ON_UPDATE_COMMAND_UI(ID_EDIT_COPY_GAME, OnUpdateEditCopyGame)
ON_COMMAND(ID_EDIT_COPY_GAME, OnEditCopyGame)
ON_UPDATE_COMMAND_UI(ID_GAME_SHOW_MOVES, OnUpdateGameShowMoves)
ON_COMMAND(ID_GAME_SHOW_MOVES, OnGameShowMoves)
ON_UPDATE_COMMAND_UI(ID_PLAYWHITE, OnUpdatePlayWhite)
ON_COMMAND(ID_PLAYWHITE, OnPlayWhite)
ON_COMMAND(ID_MAKEUSERMOVE, OnMakeUserMove)
ON_COMMAND(ID_LOSSONTIME, OnLossOnTime)
ON_COMMAND(ID_ENGINE_REPLY, OnEngineReply)
ON_COMMAND(ID_RESET, OnReset)
ON_UPDATE_COMMAND_UI(ID_HINT, OnUpdateHint)
ON_COMMAND(ID_HINT, OnHint)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_PREFERENCES, OnUpdateOptionsPreferences)
ON_COMMAND(ID_OPTIONS_PREFERENCES, OnOptionsPreferences)
ON_WM_DROPFILES()
ON_COMMAND(ID_OPTIONS_APPEARANCE, OnOptionsAppearance)
ON_UPDATE_COMMAND_UI(ID_OPTIONS_APPEARANCE, OnUpdateOptionsAppearance)
ON_COMMAND(ID_RESIZE,OnResize)
ON_UPDATE_COMMAND_UI(ID_BOARD_COLORS, OnUpdateBoardColors)
ON_COMMAND(ID_BOARD_COLORS, OnBoardColors)
ON_WM_SIZE()
//}}AFX_MSG_MAP
// Standard printing commands

END_MESSAGE_MAP()

static Move text_to_move(const Board &board,char *buf)
{
   return Notation::value(board,board.sideToMove(),Notation::InputFormat::WB,buf);
}


int ArasanGuiView::load_fen(istream &ifs, Board &board)
{
   ifs >> board;
   board.repListHead = board.repList;
   board.state.moveCount = 0;
   return ifs.good();
}


void ArasanGuiView::reset( ResetType type)
{
   initClock();
   if (type != NewPgn) {
      players = "";
   }
   else
      theClock->stop();
   ArasanMainFrame *mainFrm = (ArasanMainFrame*)AfxGetMainWnd();
   if (mainFrm) mainFrm->setStatus(ArasanMainFrame::Ready);
   start_square = InvalidSquare;
   last_move = NullMove;
}


/////////////////////////////////////////////////////////////////////////////
// ArasanGuiView construction/destruction

ArasanGuiView::ArasanGuiView()
: disp(NULL), start_square(InvalidSquare),
theClock(NULL),did_startup(FALSE)
{
   TRACE("creating view");
   initClock();
   // tell the app to use HTML Help
}


ArasanGuiView::~ArasanGuiView()
{
   if (disp) delete disp;
   if (theClock) {
      delete theClock;
   }
}


void ArasanGuiView::OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint)
{
   // allows selective update of the board
   DisplayHint *dispHint = (DisplayHint*)pHint;
   CRect clientArea;
   GetClientRect(&clientArea);
   if (!disp) {
      disp = new Display(this,clientArea);
   }
   else
      disp->setSize(clientArea);
   ArasanGuiDoc *pDoc = (ArasanGuiDoc*)GetDocument();
   const Board &board = pDoc->getCurrentBoard();
   CRect loc;
   for (int sq=0;sq<64;sq++) {
      if (dispHint == NULL || dispHint->isDirty(sq)) {
         // This square needs repainting. Calculate
         // its rectangle.
         disp->getSquareRect(sq,disp->isTurned(),loc);
         // mark affected rectangle for painting
         InvalidateRect(loc);
      }
   }
   // Make sure ECO is updated
   disp->getRect(Display::ECO,loc);
   InvalidateRect(loc);
   // And the side to move
   disp->getRect(Display::SideToMove,loc);
   InvalidateRect(loc);
   // And the status bar
   ArasanMainFrame *mainFrm = (ArasanMainFrame*)AfxGetMainWnd();
   if (mainFrm) mainFrm->invalidateStatusBar();
   if (pHint == NULL) {
      // non-selective update, show players
      disp->getRect(Display::Players,loc);
      InvalidateRect(loc);
   }
   // always invalidate result
   disp->getRect(Display::Result,loc);
   InvalidateRect(loc);
   theClock->start(getCurrentBoard().sideToMove());
}


void ArasanGuiView::OnResize()
{
   currentBoardSize = guiOptions->getBoardSize();
   CDC *pDC = GetDC();
   disp->resize(pDC,currentBoardSize);
   ReleaseDC(pDC);
   int w,h;
   Display::calcWindowSize(currentBoardSize,w,h);
   AfxGetMainWnd()->SetWindowPos( NULL, 0, 0, w, h, SWP_NOMOVE  | SWP_NOZORDER | SWP_DRAWFRAME );
   Invalidate();
}


/////////////////////////////////////////////////////////////////////////////
// ArasanGuiView drawing

void ArasanGuiView::OnDraw(CDC* pDC)
{
   CRect clientArea;
   GetClientRect(&clientArea);
   if (!disp) {
      disp = new Display(this,clientArea);
   }
   else
      disp->setSize(clientArea);

   CPaintDC dc(this);                             // device context for painting

   // Do not call CView::OnPaint() for painting messages
   CRgn rgn;
   int ret = GetUpdateRgn(&rgn);

   ArasanGuiDoc *pDoc = (ArasanGuiDoc*)GetDocument();
   disp->drawBoard(&dc,pDoc->getCurrentBoard(),&clientArea,disp->isTurned());
   disp->drawMessageArea(&dc,NULL);
   ColorType side = pDoc->getCurrentBoard().sideToMove();
   disp->showSide(&dc,side);

   theClock->show_time(White,side == White && theClock->is_running());
   theClock->show_time(Black,side == Black && theClock->is_running());
   string opening_name;
   string eco;
   ArasanGuiDoc *doc = (ArasanGuiDoc*)GetDocument();
   doc->getECO(eco,opening_name);
   disp->showECO(&dc,eco.c_str(),opening_name.c_str());

   disp->showHeader(&dc,doc->getPlayers());
   disp->showResult(&dc,doc->getResult());
   if (guiOptions->show_coordinates())
      disp->showCoordinates(&dc);
   show_last_move();
}


/////////////////////////////////////////////////////////////////////////////
// ArasanGuiView printing

BOOL ArasanGuiView::OnPreparePrinting(CPrintInfo* pInfo)
{
   // default preparation
   return DoPreparePrinting(pInfo);
}


void ArasanGuiView::OnBeginPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
   // TODO: add extra initialization before printing
}


void ArasanGuiView::OnEndPrinting(CDC* /*pDC*/, CPrintInfo* /*pInfo*/)
{
   // TODO: add cleanup after printing
}


/////////////////////////////////////////////////////////////////////////////
// ArasanGuiView diagnostics

#ifdef _DEBUG
void ArasanGuiView::AssertValid() const
{
   CView::AssertValid();
}


void ArasanGuiView::Dump(CDumpContext& dc) const
{
   CView::Dump(dc);
}


ArasanGuiDoc* ArasanGuiView::GetDocument()        // non-debug version is inline
{
   ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(ArasanGuiDoc)));
   return (ArasanGuiDoc*)m_pDocument;
}
#endif                                            //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ArasanGuiView message handlers

void ArasanGuiView::OnPaint()
{
   CPaintDC dc(this);                             // device context for painting

   CRect clientArea;
   GetClientRect(&clientArea);
   if (!disp) {
      disp = new Display(this,clientArea);
   }
   else
      disp->setSize(clientArea);

   // Do not call CView::OnPaint() for painting messages

   ArasanGuiDoc *pDoc = (ArasanGuiDoc*)GetDocument();
   disp->drawBoard(&dc,pDoc->getCurrentBoard(),&clientArea,disp->isTurned());
   disp->drawMessageArea(&dc,NULL);
   ColorType side = pDoc->getCurrentBoard().sideToMove();
   disp->showSide(&dc,side);
   if (theClock) {
      theClock->show_time(White,side == White && theClock->is_running());
      theClock->show_time(Black,side == Black && theClock->is_running());
   }
   string opening_name;
   string eco;
   GetDocument()->getECO(eco,opening_name);
   disp->showECO(&dc,eco.c_str(),opening_name.c_str());
   disp->showHeader(&dc,doc->getPlayers());
   if (guiOptions->show_coordinates())
      disp->showCoordinates(&dc);
   disp->showResult(&dc,doc->getResult());
   show_last_move();

   if (*initialFilename && !did_startup) {
      PostMessage(WM_COMMAND,ID_FILE_OPEN,0);
      did_startup = TRUE;
   }
}


void ArasanGuiView::OnLButtonDown(UINT nFlags, CPoint point)
{
   if (!disp)
      return;
   if (!usersMove()) {
      // not the user's move
      if (guiOptions->beep_on_error())
         ::MessageBeep(MB_ICONEXCLAMATION);
      start_square = InvalidSquare;
      return;
   }
   // find out where we are on the board:
   start_square = disp->mouseLocation(point);
   if (start_square == InvalidSquare)
      return;
   if (IsEmptyPiece(GetDocument()->getCurrentBoard()[start_square])) {
      start_square = InvalidSquare;
      return;                                     // no piece on this square
   }
   else {
      CDC *pDC = GetDC();
      // TBD disp->highlight_square(pDC,start_square);
      ReleaseDC(pDC);
   }

   CView::OnLButtonDown(nFlags, point);
}


void ArasanGuiView::OnLButtonUp(UINT nFlags, CPoint point)
{
   if (!disp)
      return;
   Square dest = disp->mouseLocation(point);
   TRACE("dest square = %d, users move=%d\n",dest,usersMove());
   const Board& board = GetDocument()->getCurrentBoard();
   Move move = CreateMove(board, start_square, dest, Empty);
   if (!legalMove(board, move)) {
      if (guiOptions->beep_on_error())
         ::MessageBeep(MB_ICONEXCLAMATION);
   }
   else {
      last_move = move;
      PostMessage(WM_COMMAND,ID_MAKEUSERMOVE,0L);
   }
   CView::OnLButtonUp(nFlags, point);
}


void ArasanGuiView::OnMakeUserMove()
{
   start_square = StartSquare(last_move);
   Square dest = DestSquare(last_move);
   Move emove;
   ArasanGuiDoc *doc = (ArasanGuiDoc*)GetDocument();
   const Board &current_board = doc->getCurrentBoard();
   if (TypeOfPiece(current_board[start_square]) == Pawn &&
   Rank(dest,current_board.sideToMove()) == 8) {
      // promotion, prompt for piece to promote to
      PromotionDialog pdlg(this);
      if (pdlg.DoModal() == IDCANCEL) {
         return;
      }
      Move m;
      switch (pdlg.m_PieceType) {
         case 0: m = CreateMove(current_board,start_square,dest,Queen); break;
         case 1: m = CreateMove(current_board,start_square,dest,Rook); break;
         case 2: m = CreateMove(current_board,start_square,dest,Bishop); break;
         case 3: m = CreateMove(current_board,start_square,dest,Knight); break;
      }
      emove = m;
   }
   else
      emove = CreateMove(current_board,start_square,dest,Empty);

   if (searching() && usersMove()) {
       // we are interrupting a "Go" command issued while it is the
       // user's turn. Temporarily enter force mode to cancel the current
       // search.
       writeToEngine("force\n");
       setForce(1);
   }

   string image;
   // generate move image before making move
   Notation::image(current_board,emove,Notation::OutputFormat::SAN,image);

   update_time();
   GetDocument()->updateBoard(emove);

   stringstream buf;

   // send time control info to the engine
   if (ti[getCurrentBoard().sideToMove()].get_search_type() != Time_Limit) {
      ColorType cside = doc->getComputerSide();
      buf << "time " << theClock->time_left(cside) << '\n';
      buf << "otim " << theClock->time_left(OppositeColor(cside)) << '\n';
   }

   // send the move to the engine
   buf << "usermove " << image;

   writeToEngine(buf.str().c_str());
   // If we have entered force mode, issue a "go" so searching begins:
   if (force) {
      startSearch();
   }
   ArasanMainFrame *mainFrm = (ArasanMainFrame*)AfxGetMainWnd();
   mainFrm->clearStatusBar();
   mainFrm->setStatus(ArasanMainFrame::Searching);
}


void ArasanGuiView::OnLossOnTime()
{
   char msg[80];

   sprintf(msg,"%s loses on time!",ColorImage(getCurrentBoard().sideToMove()));
   AfxMessageBox(msg,MB_OK | MB_ICONINFORMATION);
   string result;
   if (getCurrentBoard().sideToMove() == White)
      result = "0-1 {time}";
   else
      result = "1-0 {time}";
   GetDocument()->setResult(result);
}


static int parseMove(LPCSTR cmd,char *moveText)
{
   char move[20];
   if (isdigit(*cmd)) {
      const char *p = cmd;
      int dots = 0;
      while (*p && !isalpha(*p)) {
         if (*p == '.') ++dots;
         ++p;
      }
      if (dots<2) return 0;                       // a move we made, not a reply
      if (*p) {
         char *q = move;
         while (*p && !isspace(*p) && (*p != '\n')) *q++=*p++;
         *q = '\0';
      }
   }
   else
      return 0;                                   // not a move

   TRACE("move text = %s\n", move);
   strcpy(moveText,move);
   return 1;
}


void ArasanGuiView::OnEngineReply()
{
   // This method handles all incoming commands from the engine.
   while (commandCount()) {
      char *cmd = readCommand();
      char moveText[20];
      TRACE("processing cmd %s\n",cmd);
      ArasanGuiDoc *doc = GetDocument();
      if (strncmp(cmd,"pong",4) == 0) {
         int pong;
         sscanf(cmd+4,"%d",&pong);
         doc->setPong(pong);
      }
      else if (strncmp(cmd,"move",4) == 0) {
         // engine move
         if (!doc->pingCurrent()) {
            if (doc->atStartOfGame()) {
               TRACE("ping not current, move ignored\n");
               if (doTrace) traceFile << "ping not current, move ignored" << endl;
            }
            else {
               // We didn't expect this
               if (doTrace) traceFile << "ping not current, undoing extra move" << endl;
               TRACE("ping not current, undoing extra move");
               writeToEngine("undo\n");
            }
         }
         else {
            Move engineMove = text_to_move(getCurrentBoard(),cmd + 5);
#ifdef _DEBUG
            if (IsNull(engineMove)) TRACE("move parse failed!");
#endif
            if (!usersMove() && guiOptions->beep_after_move())
               ::MessageBeep(MB_OK);
            update_time();
            GetDocument()->updateBoard(engineMove);
            ArasanMainFrame *mainFrm = (ArasanMainFrame*)AfxGetMainWnd();
            mainFrm->setStatus(ArasanMainFrame::Ready);
            const Board &b = doc->getCurrentBoard();
            if (doc->getDrawOffer()) {
                // Engine claimed draw after current move.
                CString text;
                if (b.repCount() >= 2) {
                    text = "Repetition";
                } else if (b.materialDraw()) {
                    text = "Insufficient material";
                } else {
                    MoveGenerator mg(b);
                    Move moves[Constants::MaxMoves];
                    int count = mg.generateAllMoves(moves,0);
                    if (!count) {
                        if (b.checkStatus() == InCheck) {
                            return; // this is checkmate.
                        } else {
                            text = "Stalemate";
                        }
                    } else if (b.state.moveCount > 100) {
                        text = "50-move rule";
                    } else {
                        doc->clearDrawOffer();
                        return; // reject draw claim
                    }
                }
                doc->setResult((LPCTSTR)text);
                char buf[80];
                // Tell the engine the game is really over
                strcpy(buf,"result 1/2-1/2 {");
                strcat(buf,text);
                strcat(buf,"}");
                writeToEngine(buf);
                CString boxText = "Draw (";
                boxText += text;
                boxText += ")";
                AfxMessageBox(boxText);
                doc->clearDrawOffer();
                theClock->stop();
            }
         }
      }
      else if (strncmp(cmd,"Hint:",5)==0 || strncmp(cmd,"Book move",9)==0) {
         MessageBox(cmd,"Hint",MB_ICONINFORMATION);
      }
      else if (parseMove(cmd,moveText)) {
         Move engineMove = text_to_move(getCurrentBoard(),moveText);
#ifdef _DEBUG
         if (IsNull(engineMove)) TRACE("move parse failed!");
#endif
         if (!usersMove() && guiOptions->beep_after_move())
            ::MessageBeep(MB_OK);
         if (force) {
            // TBD check game state like xboard does
            TRACE("force mode - ignoring unexpected move from engine %s\n",cmd+5);
         }
         else {
            GetDocument()->updateBoard(engineMove);
            ArasanMainFrame *mainFrm = (ArasanMainFrame*)AfxGetMainWnd();
            mainFrm->setStatus(ArasanMainFrame::Ready);
         }
      } else if (strncmp(cmd,"1-0",3)==0 || strncmp(cmd,"0-1",3) == 0 ||
                 strncmp(cmd,"1/2-1/2",7)==0) {
         // engine asserts end of game
         doc->setResult(cmd);
         // We believe what the engine tells us. Send it a result command
         // (like ICC does) so it knows the game is really over.
         char buf[200];
         strcpy(buf,"result ");
         strcat(buf,cmd);
         writeToEngine(buf);
         theClock->stop();
         // Report the game status to the user
         CString text;
         if (strncmp(cmd,"1/2-1/2",7)==0) {
            text = "Draw";
            char *p = cmd;
            // Add any comment that followed the result
            while (*p != '\0' && *p != '{') p++;
            if (*p != '\0') {
               p++;
               text += " (";
               while (*p != '\0' && *p != '}') text += *p++;
               text += ")";
            }
         }
         else {
            // Extract any comment that followed the result
            text = "";
            char *p = cmd;
            while (*p != '\0' && *p != '{') p++;
            if (*p != '\0') {
               p++;
               while (*p != '\0' && *p != '}') text += *p++;
            }
         }
         AfxMessageBox(text);
      } else if (strncmp(cmd,"offer draw",10)==0) {
         // Engine will send this to claim a draw by rule (Arasan does
         // not currently offer draws in other situations). Just remember
         // that we got the command, since we have not yet gotten the
         // engine move
         doc->setDrawOffer();
      }
      else if (searching() && (*cmd == ' ' || isdigit(*cmd)) && strchr(cmd,'.')==NULL) {
         // assume it's "post" output
         stringstream post(cmd);
         int depth, value;
         long time, nodes;
         string pvMoveText;
         post >> depth;
         post >> value;
         post >> time;
         post >> nodes;
         int i = 0;
         while (i < 127) {
            char c;
            post.get(c);
            if (post.eof() || post.bad()) break;
            if (i != 0) pvMoveText += c; // skip first char, should be ' '
            i++;
         }
         ArasanMainFrame *mainFrm = (ArasanMainFrame*)AfxGetMainWnd();
         mainFrm->update(depth,nodes,value, pvMoveText.c_str());
      }
      else {
         // it's some other output from the engine, not a move
         TRACE("unparsed command from engine: %s\n",cmd);
      }
      free(cmd);                                  // was dynamically allocated
   }
}


void ArasanGuiView::OnReset()
{
   reset(Initial);
}


void ArasanGuiView::show_last_move()
{
   CDC *pDC = GetDC();
   disp->showMove(pDC,
      GetDocument()->lastMoveImage(),
      GetDocument()->getCurrentMove(),
      OppositeColor(GetDocument()->getCurrentBoard().sideToMove()));
   ReleaseDC(pDC);
}


// handle updating to the next time control, if necessary
// call before updating the board.
void ArasanGuiView::update_time()
{
   if (theClock->time_is_up())
      return;
   // We already made the move and changed the side to move,
   // so use "oppside" for the data we need:
   const Board &current_board = GetDocument()->getCurrentBoard();
   const ColorType oppside = current_board.oppositeSide();
   Time_Info &my_ti = ti[oppside];
   if (my_ti.get_search_type() == Tournament) {
      // see if we made time control
      Search_Limit limit = my_ti.get_search_limit();
      int num_moves = GetDocument()->numMoves();
      if (((num_moves-1)/2) - my_ti.get_last_time_control()
      == limit.limit.moves) {
         // we made it, update to next time control
         int period = my_ti.get_period();
         time_t bonus = theClock->get_limit(oppside)
            - theClock->elapsed_time(oppside);
         my_ti.set_bonus((unsigned long)bonus);
         my_ti.set_last_time_control(1+((GetDocument()->numMoves()-1)/2));
         my_ti.set_period(period);
         setup_clock(oppside);
      }
   }
   else if (my_ti.get_search_type() == Incremental) {
      Search_Limit lim = my_ti.get_search_limit();
      if (lim.limit.increment > 0) {
         theClock->add_increment(100*lim.limit.increment,
            current_board.sideToMove());
      }
   }
   theClock->start(current_board.oppositeSide());
}


// Call this when a time control is reached to reset clock options for one
// side
void ArasanGuiView::setup_clock(const ColorType side)
{
   const Search_Limit limits = ti[side].get_search_limit();
   CDC *pDC = GetDC();
   switch (ti[side].get_search_type()) {
      case Fixed_Ply:
      case Time_Limit:
         theClock->count_up();
         disp->showTime(pDC,0,White,theClock->is_running() && side==White);
         disp->showTime(pDC,0,Black,theClock->is_running() && side==Black);
         break;
      case Game:
      case Tournament:
      case Incremental:
      {
         time_t time = 100*(limits.limit.minutes*60L)+ti[side].get_bonus();
         theClock->count_down(time, side);
         disp->showTime(pDC,time,White,side==White);
         disp->showTime(pDC,time,Black,side==Black);
         break;
      }
   }
   ReleaseDC(pDC);
}


// Call this on startup and after clock options change
void ArasanGuiView::initClock()
{
   if (!theClock) {
      theClock = new Clock(this);
   }
   if (!guiOptions || !m_pDocument) return;
   Search_Limit_Options options = guiOptions->get_src_limits();
   setTimeControl(options);
}


void ArasanGuiView::showTime(time_t displayTime, ColorType side_to_move,
int active)
{
   CDC *pDC = GetDC();
   disp->showTime(pDC, displayTime, side_to_move, active);
   ReleaseDC(pDC);
}


void ArasanGuiView::OnInitialUpdate()
{
   doc = (ArasanGuiDoc*)GetDocument();
   CWnd *mainWnd = AfxGetMainWnd();
   if (mainWnd) {
      CRect rect;
      mainWnd->GetWindowRect(&rect);
      if (!disp) {
         disp = new Display(this,rect);
      }
      SetTimer(MAIN_TIMER,1000, NULL);
   }

   // OnNewDocument calls this, don't do it here: reset(Initial);
   DragAcceptFiles(TRUE);
}


void ArasanGuiView::OnForward()
{
   last_move = doc->getMove(doc->getCurrentMove());
   doc->goForward(last_move);
}


void ArasanGuiView::OnEndofgame()
{
   doc->goToMove(doc->numMoves());
}


void ArasanGuiView::OnTakebackMove()
{
   if (doc->getCurrentMove() == 0) {
      AfxMessageBox("No moves to take back.");
      return;
   }
   else {
      doc->undoMove();
   }
}


void ArasanGuiView::OnBeginGame()
{
   doc->goToMove(0);
}


void ArasanGuiView::OnUpdateBeginGame(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(doc->getCurrentMove() > 0);
}


void ArasanGuiView::OnUpdateForward(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(doc->getCurrentMove() < doc->numMoves());
}


void ArasanGuiView::OnUpdateTakebackMove(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(doc->getCurrentMove() > 0);

}


void ArasanGuiView::OnUpdateEndofgame(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(doc->getCurrentMove() < doc->numMoves());
}


void ArasanGuiView::OnOptionsSearchLimits()
{
   SearchLimitsDialog dlg(this,guiOptions->get_src_limits());
   Search_Limit_Options old_options(guiOptions->get_src_limits());
   if (dlg.DoModal() == IDCANCEL) {
      return;
   }

   Search_Limit_Options new_options;
   new_options.search_type = dlg.m_SearchType;
   new_options.var1 = dlg.m_Var1;
   new_options.var2 = dlg.m_Var2;
   if (new_options != old_options) {
      setTimeControl(new_options);
   }
}


void ArasanGuiView::fileLoaded()
{
   // Reset the clock and do not let it run after a file load.
   if (theClock) {
      theClock->reset();
      theClock->stop();
   }
}


void ArasanGuiView::setTimeControl(const Search_Limit_Options &new_options)
{

   guiOptions->set_src_limits(new_options);

   int var1 = new_options.var1;
   int var2 = new_options.var2;

   Search_Type src_type;
   Search_Limit src_limit;
   Time_Limits tl;

   // convert new time control to a Winboard "level" command
   char cmd[128];
   switch (new_options.search_type) {
      case 0:
         // fixed time. Send st value in seconds. Note: Arasan
         // versions before 17.2 mistakenly required a value in
         // centiseconds.
         wsprintf(cmd,"st %d",var1);
         if (theClock) {
            theClock->reset();
            theClock->count_up();
         }
         src_type = Time_Limit;
         src_limit.centiseconds = 100*new_options.var1;
         break;
      case 1:
         // incremental
         wsprintf(cmd,"level 0 %d %d\ntime %ld\notim %ld",60*var1,var2,var1*6000L,var1*6000L);
         if (theClock) {
            theClock->count_down(var1*6000L,Black);
            theClock->count_down(var1*6000L,White);
         }
         src_type = Incremental;
         src_limit.centiseconds = 100*new_options.var1;
         tl.minutes = new_options.var1;
         tl.increment = new_options.var2;
         src_limit.limit = tl;
         break;
      case 2:
         // "conventional" time control
         wsprintf(cmd,"level %d %d:0 0\ntime %ld\notim %ld",var1,var2,var2*6000L,var2*6000L);
         if (theClock) {
            theClock->count_down(var2*6000L,Black);
            theClock->count_down(var2*6000L,White);
         }
         src_type = Tournament;
         tl.moves = new_options.var1;
         tl.minutes = new_options.var2;
         src_limit.limit = tl;
         break;
      case 3:
         // fixed depth
         wsprintf(cmd,"sd %d\ntime %ld\notim %ld",var1,1000000L,1000000L);
         src_type = Fixed_Ply;
         src_limit.max_ply = var1;
   }
   writeToEngine(cmd);

   ti[White] =
      ti[Black] = Time_Info(src_type,src_limit);
   ti[White].set_period(0);
   ti[Black].set_period(0);
   ti[White].set_bonus(0);
   ti[Black].set_bonus(0);
   if (theClock) {
      theClock->reset();
      setup_clock(White);
      setup_clock(Black);
      theClock->start(getCurrentBoard().sideToMove());
   }
}


void ArasanGuiView::OnTimer(UINT_PTR nIDEvent)
{
   if (nIDEvent == MAIN_TIMER && theClock != NULL) {
      theClock->update();
   }
   CView::OnTimer(nIDEvent);
}


void ArasanGuiView::saveOptions()
{
   guiOptions->save();
}


void ArasanGuiView::OnUpdateCompute(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(TRUE);
}


void ArasanGuiView::OnCompute()
{
    ArasanMainFrame *mainFrm = (ArasanMainFrame*)AfxGetMainWnd();
    ArasanMainFrame::SearchStatus status = mainFrm->getStatus();
    if (status == ArasanMainFrame::Searching)
        writeToEngine("?");      // Winboard "move now" command
    else {
        // The user has asked us to compute while it is the user's turn
        // to move. 
        // Temporarily enter force mode to terminate any search in progress
        // (such as a ponder search).
        writeToEngine("force\n");
        setForce(1);
        mainFrm->clearStatusBar();
        // Now search. We can still be interrupted by the user making a
        // move before the search is complete.
        startSearch();
        mainFrm->setStatus(ArasanMainFrame::Searching);
    }
}


void ArasanGuiView::OnUpdateBrowseSelectgame(CCmdUI* pCmdUI)
{
   DWORD offset = 0L;
   pCmdUI->Enable(doc->getPGNListSize() > 1);

}


void ArasanGuiView::OnBrowseSelectgame()
{
   DWORD index;
   if (doc->getPGNListSize() > 1) {
      SelectGame dlg(*(doc->getPGNContents()),this);
      if (dlg.DoModal() == IDCANCEL) {
         return;
      }
      index = dlg.get_selection();
      doc->selectGame(index);
   }
}


void ArasanGuiView::OnUpdateClockPause(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(!theClock->is_running() && !theClock->is_stopped());
   pCmdUI->Enable(!theClock->is_stopped());
}


void ArasanGuiView::OnClockPause()
{
   if (theClock->is_running())
      theClock->pause();
   else
      theClock->resume();

}


void ArasanGuiView::OnUpdateClockReset(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}


void ArasanGuiView::OnClockReset()
{
   if (theClock) {
      theClock->reset();
      theClock->start(getCurrentBoard().sideToMove());
   }

}


void ArasanGuiView::OnUpdateGameRotateBoard(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}


void ArasanGuiView::OnGameRotateBoard()
{
   disp->setTurned(!disp->isTurned());
   Invalidate();
}


void ArasanGuiView::OnUpdateEditCopyPosition(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();

}


void ArasanGuiView::OnEditCopyPosition()
{
   if (OpenClipboard()) {
      ::EmptyClipboard();
      HGLOBAL hBuf = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,256);
      char *buf = (char*)GlobalLock(hBuf);
      ZeroMemory(buf,256);
      ArasanGuiDoc *doc = (ArasanGuiDoc*)GetDocument();
      stringstream str;
      BoardIO::writeFEN(doc->getCurrentBoard(),str,1);
      str << '\015' << '\012';                    // CR/LF
      strncpy(buf,str.str().c_str(),255);
      HANDLE hndl = ::SetClipboardData(CF_TEXT,hBuf);
      GlobalUnlock(hBuf);
      // N.b. do not free the memory
      CloseClipboard();
   }

}


void ArasanGuiView::OnUpdateEditCopyGame(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}


void ArasanGuiView::OnEditCopyGame()
{
   if (OpenClipboard()) {
      ::EmptyClipboard();
      const int BUF_SIZE = 16384;
      HGLOBAL hBuf = GlobalAlloc(GMEM_MOVEABLE | GMEM_DDESHARE,BUF_SIZE);
      char *buf = (char*)GlobalLock(hBuf);
      ZeroMemory(buf,BUF_SIZE);
      static char tmp[BUF_SIZE];
      // Create memory stream for output
      stringstream ofs(ios::out | ios::in);
      doc->storeGame(ofs);
      strncpy(tmp,ofs.str().c_str(),BUF_SIZE-4);
      // CF_TEXT clipboard format requires carriage return and
      // line feed at line end. So ensure this:
      char *p = tmp;
      char *q = buf;
      while (*p && q-buf<BUF_SIZE-4) {
         if (*p == '\n')
            *q++ = '\015';                        // carriage return
         *q++ = *p++;
      }
      *q = '\0';
      HANDLE hndl = ::SetClipboardData(CF_TEXT,hBuf);
      GlobalUnlock(hBuf);
      // N.b. do not free the memory
      CloseClipboard();
   }

}


void ArasanGuiView::OnUpdateGameShowMoves(CCmdUI* pCmdUI)
{
   ArasanGuiDoc *doc = (ArasanGuiDoc*)GetDocument();
   pCmdUI->Enable(doc->numMoves() > 0);
}


void ArasanGuiView::OnGameShowMoves()
{
   ShowGame dlg(doc->getMoveList());
   dlg.DoModal();
}


void ArasanGuiView::OnUpdatePlayWhite(CCmdUI* pCmdUI)
{
   pCmdUI->SetCheck(doc->getComputerSide() == White);
}


void ArasanGuiView::OnPlayWhite()
{
   setForce(1);
   doc->changeComputerSide();
   // Make computer move if it is computer's turn
   if (doc->getCurrentBoard().sideToMove() == doc->getComputerSide()) {
      OnCompute();
   }
}


void ArasanGuiView::OnUpdateHint(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(!searching());
}


void ArasanGuiView::OnHint()
{
   writeToEngine("hint");
}


void ArasanGuiView::OnUpdateOptionsPreferences(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}


void ArasanGuiView::OnOptionsPreferences()
{
   PreferenceDialog dlg(this);
   BOOL canResign = guiOptions->can_resign();
   int hashSize = guiOptions->hash_table_size();
   BOOL think_when_idle = guiOptions->think_when_idle();
   BOOL position_learning = guiOptions->position_learning();
   int cores = guiOptions->cores();
   int strength = guiOptions->strength();
   int bookVariety = guiOptions->bookVariety();
   CString tbPath = guiOptions->tbPath();
   if (dlg.DoModal() == IDOK) {
      dlg.save();
      if (guiOptions->can_resign() != canResign) {
         // inform engine via option command
         string buf("option Can resign=");
         buf += guiOptions->can_resign() ? "true" : "false";
         buf += "\n";
         writeToEngine(buf.c_str());
      }
      if (guiOptions->think_when_idle() != think_when_idle) {
         // inform engine via option command
         string buf = dlg.m_Ponder ? "hard" : "easy";
         buf += "\n";
         writeToEngine(buf.c_str());
      }
      if (guiOptions->position_learning() != position_learning) {
         // inform engine via option command
         string buf("option Position learning=");
         buf += guiOptions->position_learning() ? "true" : "false";
         buf += "\n";
         writeToEngine(buf.c_str());
      }
      if (guiOptions->hash_table_size() != hashSize) {
         stringstream s;
         s << "memory " << guiOptions->hash_table_size() << endl;
         // inform engine
         writeToEngine(s.str().c_str());
      }
      if (guiOptions->cores() != cores) {
         stringstream s;
         s << "cores " << guiOptions->cores() << endl;
         writeToEngine(s.str().c_str());
      }
      if (guiOptions->strength() != strength) {
         stringstream s;
         s << "option Strength=" << guiOptions->strength() << endl;
         writeToEngine(s.str().c_str());
      }
      if (guiOptions->bookVariety() != bookVariety) {
         stringstream s;
         s << "option Book variety=" << guiOptions->bookVariety() << endl;
         writeToEngine(s.str().c_str());
      }
      if (guiOptions->tbPath() != tbPath) {
         // inform engine of new path
         string buf("egtpath syzygy ");
         buf += guiOptions->tbPath();
         buf += "\n";
         writeToEngine(buf.c_str());
      }
   }
}


void ArasanGuiView::OnDropFiles(HDROP hDropInfo)
{
   *initialFilename = '\0';
   ::DragQueryFile(hDropInfo, 0, initialFilename, MAX_PATH);
   if (*initialFilename) {
      SendMessage(WM_COMMAND,ID_FILE_OPEN,(LPARAM)0);
   }
   ::DragFinish(hDropInfo);
}


void ArasanGuiView::OnOptionsAppearance()
{
   CString font = guiOptions->getPieceFontName();
   AppearanceDialog dlg(this,(LPCSTR)font);
   if (dlg.DoModal() == IDOK) {
      guiOptions->setShowCoordinates(dlg.m_Coordinates);
      guiOptions->setBoardSize((GuiOptions::BoardSize)dlg.m_boardSize);
      if (font.Compare(dlg.m_FontSelection)!=0) {
         guiOptions->setPieceFontName(dlg.m_FontSelection);
         CDC *pDC = GetDC();
         disp->setPieceFont(pDC,dlg.m_FontSelection,guiOptions->getBoardSize());
         ReleaseDC(pDC);
      }
      PostMessage(WM_COMMAND,ID_RESIZE,0);
   }
}


void ArasanGuiView::OnUpdateOptionsAppearance(CCmdUI* pCmdUI)
{
   pCmdUI->Enable();
}


void ArasanGuiView::OnUpdateBoardColors(CCmdUI* pCmdUI)
{
   pCmdUI->Enable(disp == NULL ? FALSE : !disp->is_mono());
}


void ArasanGuiView::OnBoardColors()
{
   BoardColorDialog dlg(this);
   if (dlg.DoModal() == IDOK) {
      guiOptions->setLightSquareColor(dlg.getLightColor());
      guiOptions->setDarkSquareColor(dlg.getDarkColor());
      guiOptions->setForceMono(dlg.getForceMono());
      Invalidate();
   }
}
