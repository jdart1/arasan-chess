// guiView.h : interface of the ArasanGuiView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIVIEW_H__9F33534C_3F30_11D5_9727_00B0D0A186AB__INCLUDED_)
#define AFX_GUIVIEW_H__9F33534C_3F30_11D5_9727_00B0D0A186AB__INCLUDED_

#include "display.h"
#include "stats.h"
#include "timectrl.h"
#include "clock.h"
#include "guiopts.h"
#include "options.h"
#include "guiDoc.h"
#include "mainfrm.h"
#include "stats.h"
#include <fstream>
#include <string>
#if _MSC_VER >= 1000
#pragma once
#endif                                            // _MSC_VER >= 1000

using namespace std;

class ArasanGuiView : public CView
{
   friend class ArasanGuiDoc;

   protected:                                     // create from serialization only
      ArasanGuiView();
      DECLARE_DYNCREATE(ArasanGuiView)

      // Attributes
      public:

      // Operations
   public:

      ArasanGuiDoc* GetDocument();
      void initClock();
      void showTime(time_t displayTime, ColorType side_to_move, int active);

      void saveOptions();

      void showResult(const string &result) {
         CDC *pDC = GetDC();
         disp->showResult(pDC,result);
         ReleaseDC(pDC);
      }

      enum ResetType {Initial, NewGame, NewPgn, NewPosition };

      void reset( ResetType type = Initial);

      void setTimeControl(const Search_Limit_Options &new_options);

      void fileLoaded();

      Display *getDisplay() {
         return disp;
      }

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(ArasanGuiView)
   public:
      virtual void OnDraw(CDC* pDC);              // overridden to draw this view
      virtual void OnInitialUpdate();
      virtual void OnUpdate(CView* pSender, LPARAM lHint, CObject* pHint);
   protected:
      virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
      virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
      virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
      //}}AFX_VIRTUAL

      // Implementation
      virtual ~ArasanGuiView();

#ifdef _DEBUG
      virtual void AssertValid() const;
      virtual void Dump(CDumpContext& dc) const;
#endif

   protected:

      // Generated message map functions
   protected:
      //{{AFX_MSG(ArasanGuiView)
      afx_msg void OnPaint();
      afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
      afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
      afx_msg void OnForward();
      afx_msg void OnEndofgame();
      afx_msg void OnTakebackMove();
      afx_msg void OnBeginGame();
      afx_msg void OnUpdateBeginGame(CCmdUI* pCmdUI);
      afx_msg void OnUpdateForward(CCmdUI* pCmdUI);
      afx_msg void OnUpdateTakebackMove(CCmdUI* pCmdUI);
      afx_msg void OnUpdateEndofgame(CCmdUI* pCmdUI);
      afx_msg void OnOptionsSearchLimits();
      afx_msg void OnTimer(UINT_PTR nIDEvent);
      afx_msg void OnUpdateCompute(CCmdUI* pCmdUI);
      afx_msg void OnCompute();
      afx_msg void OnUpdateBrowseSelectgame(CCmdUI* pCmdUI);
      afx_msg void OnBrowseSelectgame();
      afx_msg void OnUpdateClockPause(CCmdUI* pCmdUI);
      afx_msg void OnClockPause();
      afx_msg void OnUpdateClockReset(CCmdUI* pCmdUI);
      afx_msg void OnClockReset();
      afx_msg void OnUpdateGameRotateBoard(CCmdUI* pCmdUI);
      afx_msg void OnGameRotateBoard();
      afx_msg void OnUpdateEditCopyPosition(CCmdUI* pCmdUI);
      afx_msg void OnEditCopyPosition();
      afx_msg void OnUpdateEditCopyGame(CCmdUI* pCmdUI);
      afx_msg void OnEditCopyGame();
      afx_msg void OnUpdateGameShowMoves(CCmdUI* pCmdUI);
      afx_msg void OnGameShowMoves();
      afx_msg void OnUpdatePlayWhite(CCmdUI* pCmdUI);
      afx_msg void OnPlayWhite();
      afx_msg void OnMakeUserMove();
      afx_msg void OnLossOnTime();
      afx_msg void OnEngineReply();
      afx_msg void OnReset();
      afx_msg void OnUpdateHint(CCmdUI* pCmdUI);
      afx_msg void OnHint();
      afx_msg void OnUpdateOptionsPreferences(CCmdUI* pCmdUI);
      afx_msg void OnOptionsPreferences();
      afx_msg void OnDropFiles(HDROP hDropInfo);
      afx_msg void OnOptionsAppearance();
      afx_msg void OnUpdateOptionsAppearance(CCmdUI* pCmdUI);
      afx_msg void OnResize();
      afx_msg void OnUpdateBoardColors(CCmdUI* pCmdUI);
      afx_msg void OnBoardColors();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

      private:
      Move last_move;

      void show_last_move();
      void update_time();
      const Board &getCurrentBoard() {
         return GetDocument()->getCurrentBoard();
      }
      void setup_clock(const ColorType side);
      void free_pgn();
      void free_pgn_headers();
      int load_pgn(Board &board,ifstream &game_file, unsigned long offset,
         vector< pair<string,string> > &hdrs);
      int load_fen(istream &ifs, Board &board);

      string players;                             // Player string for current game
      vector< pair<string,string> > headers;      // PGN headers for current game

      int searching() const
      {
         return ((ArasanMainFrame*)AfxGetMainWnd())->getStatus();
      }

      int usersMove() const
      {
         return doc->getComputerSide() != doc->getCurrentBoard().sideToMove();
      }

      Display *disp;
      Square start_square;
      struct Statistics *stats;
      Time_Info ti[2];                            // Holds current time control for each side
      char engineReply[20];
      Clock *theClock;
      ArasanGuiDoc *doc;
      BOOL did_startup;
      GuiOptions::BoardSize currentBoardSize;
};

#ifndef _DEBUG                                    // debug version in guiView.cpp
inline ArasanGuiDoc* ArasanGuiView::GetDocument()
{ return (ArasanGuiDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif                                            // !defined(AFX_GUIVIEW_H__9F33534C_3F30_11D5_9727_00B0D0A186AB__INCLUDED_)
