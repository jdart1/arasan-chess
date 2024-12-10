// MainFrm.cpp : implementation of the ArasanMainFrame class
// Copyright 2002, 2008, 2013, 2018 by Jon Dart. All Rights Reserved.

#include "stdafx.h"
#include "arasan.h"

#include "MainFrm.h"
#include "GuiDoc.h"
#include "EngineCo.h"
#include "guiview.h"
#include "constant.h"
#include <sstream>
#include <iomanip>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static void printNodes(uint64_t nodes, ostream &str)
{
   str.setf(ios::fixed);
   if (nodes >= 1000000) {
      str << setprecision(2) << (float)(nodes)/1000000.0 << "M";
   }
   else if (nodes >= 1000) {
      str << setprecision(2) << (float)(nodes)/1000.0 << "K";
   }
   else
      str << setprecision(2) << nodes;
}


// toolbar buttons - IDs are command buttons
static UINT BASED_CODE buttons[] =
{
   // same order as in the bitmap 'toolbar.bmp'
   ID_FILE_NEW,
   ID_FILE_OPEN,
   ID_FILE_SAVE,
   /** LATER
                   ID_SEPARATOR,
           ID_FILE_PRINT,
   **/
   ID_SEPARATOR,
   ID_BROWSE_PREVIOUSGAME,
   ID_BEGIN_GAME,
   ID_TAKEBACK_MOVE,
   ID_COMPUTE,
   ID_FORWARD,
   ID_GAME_ENDOFGAME,
   ID_BROWSE_NEXTGAME,
   ID_SEPARATOR,
   ID_HINT
};

static UINT BASED_CODE indicators[] =
{
   ID_SEPARATOR,                                  // status line indicator
   ID_INDICATOR_PLYLABEL,
   ID_INDICATOR_PLY,
   ID_INDICATOR_SCORELABEL,
   ID_INDICATOR_SCORE,
   ID_INDICATOR_NODELABEL,
   ID_INDICATOR_NODES
};

static UINT BASED_CODE pv_indicators[] =
{
   //        ID_SEPARATOR,           // status line indicator
   ID_INDICATOR_PVLABEL,
   ID_INDICATOR_PV
};

/////////////////////////////////////////////////////////////////////////////
// ArasanMainFrame

IMPLEMENT_DYNCREATE(ArasanMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(ArasanMainFrame, CFrameWnd)
//{{AFX_MSG_MAP(ArasanMainFrame)
ON_WM_CREATE()
ON_COMMAND(ID_FILE_NEW, OnFileNew)
ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
ON_UPDATE_COMMAND_UI(ID_INDICATOR_NODES, OnUpdateNodes)
ON_UPDATE_COMMAND_UI(ID_INDICATOR_PLY, OnUpdateDepth)
ON_UPDATE_COMMAND_UI(ID_INDICATOR_SCORE, OnUpdateScore)
ON_UPDATE_COMMAND_UI(ID_INDICATOR_PV, OnUpdatePv)
ON_WM_CLOSE()
ON_WM_SIZING()
ON_UPDATE_COMMAND_UI(ID_VIEW_PV, OnUpdateControlBarMenu)
ON_COMMAND_EX(ID_VIEW_PV, OnBarCheck)
ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
ON_COMMAND( ID_HELP_INDEX, CFrameWnd::OnHelpIndex )
ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()
/////////////////////////////////////////////////////////////////////////////

static const char* print_score( int score )
{
   static char buf[20];
   // score is in centipawns, adjust to Arasan internal score
   const int adj_score = (128*score)/100;
   if (adj_score <= -Constants::MATE_RANGE) {
      int movesToMate = (Constants::MATE+adj_score+1)/2;
      sprintf(buf,"-Mate%d",movesToMate);
   }
   else if (adj_score >= Constants::MATE_RANGE) {
      int movesToMate = (Constants::MATE-adj_score+1)/2;
      sprintf(buf,"+Mate%d",movesToMate);
   }
   else if (adj_score == Constants::TABLEBASE_WIN) {
	   strcpy(buf, "TbWin");
   }
   else if (adj_score == -Constants::TABLEBASE_WIN) {
	   strcpy(buf, "TbLoss");
   }
   else
      sprintf(buf,"%+6.2f",0.01F*score);
   return buf;
}


// ArasanMainFrame construction/destruction

ArasanMainFrame::ArasanMainFrame()
{
   mNodes = 0L;
   mDepth = 0;
   mScore = "";
   mPv = "";
   my_status = Ready;
   firstActivation = TRUE;
}


ArasanMainFrame::~ArasanMainFrame()
{
}


int ArasanMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
      return -1;

   if (!m_wndToolBar.Create(this,WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS) ||
      !m_wndToolBar.LoadToolBar(IDR_MAINFRAME) ||
      !m_wndToolBar.SetButtons(buttons,
   sizeof(buttons)/sizeof(UINT))) {
      TRACE("Failed to create toolbar\n");
      return -1;                                  // fail to create
   }

   // Create the regular status bar
   if (!m_wndStatusBar.Create(this) ||
      !m_wndStatusBar.SetIndicators(indicators,
   sizeof(indicators)/sizeof(UINT))) {
      TRACE("Failed to create status bar 1\n");
      return -1;                                  // fail to create
   }
   // Create the p.v. status bar.  Note that the bar has the same
   // control ID as the menu item that controls it.
   if (!m_pvStatusBar.Create(this,WS_CHILD | WS_VISIBLE | CBRS_BOTTOM,
      ID_VIEW_PV) ||
      !m_pvStatusBar.SetIndicators(pv_indicators,
   sizeof(pv_indicators)/sizeof(UINT))) {
      TRACE("Failed to create status bar 2\n");
      return -1;                                  // fail to create
   }
   m_wndStatusBar.SetPaneInfo(0,ID_SEPARATOR,SBPS_NOBORDERS |
      SBPS_STRETCH, 150);
   m_wndStatusBar.SetPaneInfo(1,ID_INDICATOR_PLYLABEL,SBPS_NOBORDERS |
      SBPS_NORMAL, 42);
   m_wndStatusBar.SetPaneInfo(2,ID_INDICATOR_PLY, SBPS_NORMAL, 30);
   m_wndStatusBar.SetPaneInfo(3,ID_INDICATOR_SCORELABEL,SBPS_NOBORDERS |
      SBPS_NORMAL, 42);
   m_wndStatusBar.SetPaneInfo(4,ID_INDICATOR_SCORE, SBPS_NORMAL, 50);
   m_wndStatusBar.SetPaneInfo(5,ID_INDICATOR_NODELABEL,SBPS_NOBORDERS |
      SBPS_NORMAL, 42);
   m_wndStatusBar.SetPaneInfo(6,ID_INDICATOR_NODES, SBPS_NORMAL, 60);
   m_pvStatusBar.SetPaneInfo(0,ID_INDICATOR_PVLABEL,SBPS_NOBORDERS |
      SBPS_NORMAL, 60);
   m_pvStatusBar.SetPaneInfo(1,ID_INDICATOR_PV, SBPS_STRETCH, 100);
   LoadBarState("ControlBarState");

   return 0;
}


BOOL ArasanMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
   int x,y;

   guiOptions->get_app_location(x,y);
   Display::calcWindowSize(guiOptions->getBoardSize(),cs.cx,cs.cy);
   cs.x = x;
   cs.y = y;
   cs.style &= ~FWS_ADDTOTITLE;

   return CFrameWnd::PreCreateWindow(cs);
}


void ArasanMainFrame::update(int ply, long nodes, int score,
LPCSTR predicted)
{
   mNodes = nodes;
   mDepth = ply;
   mScore = print_score(score);
   if (strlen(predicted) != 0) {
      mPv = predicted;
      m_pvStatusBar.SetPaneText(1,mPv);
   }
   if (m_wndStatusBar.IsWindowVisible()) {
      // could just call m_wndStatusBar.Invalidate() but
      // that doesn't show updates right away.
      char text[20];
      sprintf(text,"%d",mDepth);
      m_wndStatusBar.SetPaneText(2,text );
      m_wndStatusBar.SetPaneText(4,mScore);
      stringstream nodeStr;
      printNodes(mNodes,nodeStr);
      m_wndStatusBar.SetPaneText(6,nodeStr.str().c_str());
   }
}


void ArasanMainFrame::OnUpdateNodes(CCmdUI *pCmdUI)
{
   pCmdUI->Enable();
   stringstream nodeStr;
   printNodes(mNodes,nodeStr);
   pCmdUI->SetText( nodeStr.str().c_str() );
}


void ArasanMainFrame::OnUpdateDepth(CCmdUI *pCmdUI)
{
   pCmdUI->Enable();
   char text[20];
   sprintf(text,"%d",mDepth);
   pCmdUI->SetText( text );
}


void ArasanMainFrame::OnUpdatePv(CCmdUI *pCmdUI)
{
   pCmdUI->Enable();
   pCmdUI->SetText( (LPCSTR)mPv );
}


void ArasanMainFrame::OnUpdateScore(CCmdUI *pCmdUI)
{
   pCmdUI->Enable();
   pCmdUI->SetText( mScore );
}


void ArasanMainFrame::update(int nodes)
{
   mNodes = nodes;
}


void ArasanMainFrame::clearStatusBar()
{
   mNodes = 0L;
   mDepth = 0;
   mScore = "";
   mPv = "";
   m_pvStatusBar.SetPaneText(1,mPv);
   m_wndStatusBar.Invalidate();
}


void ArasanMainFrame::invalidateStatusBar()
{
   if (m_wndToolBar) {
      m_wndToolBar.Invalidate();
      m_pvStatusBar.Invalidate();
   }
}


/////////////////////////////////////////////////////////////////////////////
// ArasanMainFrame diagnostics

#ifdef _DEBUG
void ArasanMainFrame::AssertValid() const
{
   CFrameWnd::AssertValid();
}


void ArasanMainFrame::Dump(CDumpContext& dc) const
{
   CFrameWnd::Dump(dc);
}
#endif                                            //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// ArasanMainFrame message handlers

void ArasanMainFrame::OnFileNew()
{
   ArasanGuiDoc *pDoc = (ArasanGuiDoc*)GetActiveDocument();
   mNodes = 0L;
   mDepth = 0;
   if (pDoc)
      pDoc->reset();
}


void ArasanMainFrame::OnFileOpen()
{
   CString filter;
   filter.LoadString(IDS_FILE_OPEN_FILTER);
   CFileDialog dlg(TRUE,
      ".pgn",                                     // default file extension
      NULL,                                       // no default filename
      OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
      (LPCSTR)filter,
      this);
   if (dlg.DoModal() == IDOK) {
      ArasanGuiDoc *pDoc = (ArasanGuiDoc*)GetActiveDocument();
      if (pDoc->open_file(dlg.GetPathName())) {
         setForce(1);
      }
   }
}


BOOL ArasanMainFrame::DestroyWindow()
{
   // Tell the engine process to terminate.
   writeToEngine("quit\n");
   return CFrameWnd::DestroyWindow();
}


void ArasanMainFrame::OnClose()
{
   if (doTrace) traceFile.close();
   CRect rect;
   GetWindowRect(&rect);
   guiOptions->set_app_location(rect.left,rect.top);
   WINDOWPLACEMENT wp;
   if (GetWindowPlacement(&wp)) {
	   guiOptions->setFlags(wp.flags);
	   guiOptions->setShowCmd(wp.showCmd);
   }
   CFrameWnd::OnClose();
}


void ArasanMainFrame::ActivateFrame(int nCmdShow)
{
  if (firstActivation) {
      firstActivation = FALSE;
      WINDOWPLACEMENT wp;
      wp.length = sizeof(WINDOWPLACEMENT);
      CRect rect;
      int x, y;
      guiOptions->get_app_location(x,y);
      rect.left = x;
      rect.top = y;
      int size_x,size_y;
      Display::calcWindowSize(guiOptions->getBoardSize(),size_x,size_y);
      rect.right = rect.left + size_x;
      rect.bottom = rect.top + size_y;
      wp.rcNormalPosition = rect;
      wp.flags = guiOptions->getFlags();
      wp.showCmd = guiOptions->getShowCmd();
      wp.ptMinPosition = CPoint(0,0);
      wp.ptMaxPosition = CPoint(-GetSystemMetrics(SM_CXBORDER), -GetSystemMetrics(SM_CYBORDER));                                 
      SetWindowPlacement(&wp);
   }
   else {
      CFrameWnd::ActivateFrame(nCmdShow);
   }
}

// Override to call HTML help not WinHelp
void ArasanMainFrame::WinHelp (DWORD dwData, UINT nCmd) {
   CWinApp* theApp = AfxGetApp();
   switch (nCmd) {
      case HELP_CONTEXT:
         ::HtmlHelp(NULL, theApp->m_pszHelpFilePath ,HH_HELP_CONTEXT, dwData );
         break;
      case HELP_FINDER:
         ::HtmlHelp(NULL, theApp->m_pszHelpFilePath ,HH_DISPLAY_TOPIC, 0);
         break;
     }
}

void ArasanMainFrame::OnSizing(UINT fwSide, LPRECT pRect)
{
   int dragWidth = pRect->right-pRect->left;
   int dragHeight = pRect->bottom-pRect->top;
   int w,h;
   GuiOptions::BoardSize newSize;
   Display::calcWindowSize(GuiOptions::Large,w,h);
   if (dragWidth>=w && dragHeight>=h)
      newSize = GuiOptions::Large;
   else {
      Display::calcWindowSize(GuiOptions::Medium,w,h);
      if (dragWidth>=w && dragHeight>=h)
         newSize = GuiOptions::Medium;
      else
         newSize = GuiOptions::Small;
   }
   //pRect->bottom = pRect->top + h;
   //pRect->right = pRect->left + w;
   GuiOptions::BoardSize currentSize = guiOptions->getBoardSize();
   if (currentSize != newSize) {
      guiOptions->setBoardSize(newSize);
   }
   CFrameWnd::OnSizing(fwSide, pRect);
   CView *view = GetActiveView();
   if (view)
      view->PostMessage(WM_COMMAND,ID_RESIZE,0);

}


void ArasanMainFrame::setStatus(SearchStatus status)
{
   CString msg;
   if (status == Ready) {
      SetMessageText(IDS_READY);
   }
   else if (status == Searching) {
      SetMessageText(IDS_SEARCHING);
   }
   my_status = status;
}
