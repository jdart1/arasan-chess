// Copyright 2001. 2017 by Jon Dart. All Rights Reserved.
//
// guiDoc.h : interface of the ArasanGuiDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_GUIDOC_H__9F33534A_3F30_11D5_9727_00B0D0A186AB__INCLUDED_)
#define AFX_GUIDOC_H__9F33534A_3F30_11D5_9727_00B0D0A186AB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif                                            // _MSC_VER >= 1000

#include "board.h"
#include "moveArr.h"
#include "eco.h"
#include "stats.h"
#include "chessio.h"
#include <string>
#include <fstream>
#include <utility>
#include <vector>

using namespace std;

// This encapsulates the state of the board and the moves made
// in the game so far.
//
class ArasanGuiDoc : public CDocument
{

   protected:                                     // create from serialization only
      ArasanGuiDoc();
      DECLARE_DYNCREATE(ArasanGuiDoc)

         void sendNew();

      // Attributes
   public:
      // Operations
   public:

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(ArasanGuiDoc)
   public:
      virtual BOOL OnNewDocument();
      virtual void Serialize(CArchive& ar);
      virtual void OnCloseDocument();
      virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
   protected:
      virtual BOOL SaveModified();
      //}}AFX_VIRTUAL

      // Implementation
   public:
      virtual ~ArasanGuiDoc();
#ifdef _DEBUG
      virtual void AssertValid() const;
      virtual void Dump(CDumpContext& dc) const;
#endif
      const Board &getCurrentBoard() const
      {
         return currentBoard;
      }
      const Piece &operator[]( const Square sq ) const
      {
         return currentBoard[sq];
      }

      void updateBoard(Move m);

      void goToMove(const int index);

      void goForward(const Move m);

      // undo the last move
      void undoMove();

      void reset();

      void loadBoard(const Board &newBoard);

      void storeGame(ostream &);

      // number of half-moves in the game
      int numMoves() const
      {
         return moveList.num_moves();
      }

      Move getMove(int n) {
         return moveList.move(n);
      }

      void getECO(string &eco,string &openingName);

      BOOL open_file(LPCSTR path);

      int getCurrentMove() const
      {
         return currentMove;
      }

      const vector <string> *getPGNContents() const
      {
         return (pgnSelection == -1) ? NULL : &pgnContents;
      }

      const DWORD getPGNSelection() const
      {
         return pgnSelection;
      }

      const int getPGNListSize() const
      {
         return pgnContents.size();
      }

      void nextGame();

      void previousGame();

      void selectGame(DWORD index);

      const string &getPlayers() const
      {
         return players;
      }

      const vector<ChessIO::Header> &getHeaders() const
      {
         return headers;
      }

      const ColorType getComputerSide() const
      {
         return computerSide;
      }

      void changeComputerSide();

      const MoveArray &getMoveList() const
      {
         return moveList;
      }

      const string &getResult() const
      {
         return result;
      }

      void setResult(const string &res);

      int pingCurrent() const
      {
         return lastPing == lastPong;
      }

      void setPong(int pong) {
         lastPong = pong;
      }

      BOOL load_pgn(Board &board,ifstream &game_file, DWORD offset,
                    vector< ChessIO::Header > &hdrs);

      const char *lastMoveImage() const;

      int atStartOfGame() const
      {
         return startOfGame;
      }

      inline void lock() {
         EnterCriticalSection(&docCriticalSection);
      }

      inline void unlock() {
         LeaveCriticalSection(&docCriticalSection);
      }

      bool getDrawOffer() const {
          return drawOffer;
      }

      void setDrawOffer() {
          drawOffer = true;
      }

      void clearDrawOffer() {
          drawOffer = false;
      }

   protected:
      Board currentBoard;
      MoveArray moveList;
      ECO *ecoCoder;
      int currentMove;
      string result;

      ifstream *pgn_file;
      vector<string> pgnContents;
      DWORD pgnSelection;
      vector< ChessIO::Header > headers;
      string players;
      string eco;
      ColorType computerSide;
      int lastPing, lastPong;
      int startOfGame;
      bool drawOffer;
      CRITICAL_SECTION docCriticalSection;

      // Generated message map functions
   protected:
      //{{AFX_MSG(ArasanGuiDoc)
      afx_msg void OnFileNew();
      afx_msg void OnFileSave();
      afx_msg BOOL OnRecentFile(UINT nID);
      afx_msg void OnUpdateBrowseNextgame(CCmdUI* pCmdUI);
      afx_msg void OnBrowseNextgame();
      afx_msg void OnBrowsePreviousgame();
      afx_msg void OnUpdateBrowsePreviousgame(CCmdUI* pCmdUI);
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif                                            // !defined(AFX_GUIDOC_H__9F33534A_3F30_11D5_9727_00B0D0A186AB__INCLUDED_)
