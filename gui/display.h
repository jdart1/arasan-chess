// Copyright 1994-2002, 2008 by Jon Dart. All Rights Reserved.
#ifndef _DISPLAY_H
#define _DISPLAY_H

#include <afxwin.h>
#include "board.h"
#include "stats.h"
#include "guiopts.h"

class Display
{

   public:
      enum DisplayRegion { SideToMove, ECO, Result, Players };

      void getRect(DisplayRegion region,CRect &rect);

      Display(CWnd *parent, const CRect &size);

      virtual ~Display();

      void setSize(const CRect &);

      void resize(CDC *pDC, GuiOptions::BoardSize);

      void drawMessageArea(CDC *pDC, CRgn *);

      void drawBoard( CDC *pDC, const Board &board, const CRect &drawArea, BOOL turned);

      void drawPiece( CDC *pDC, Square sq, Piece p);

      void showSide(CDC *pDC, ColorType side);

      void showTime(CDC *pDC, time_t displayTime, ColorType side, int active);

      void showTimeArea(CDC *pDC);

      void showECO(CDC *pDC, const char *eco, const char *openingName);

      void showMove(CDC *pDC, const string &moveImage, int moveCount, ColorType side);

      void showResult(CDC *pDC, const string &result);

      void showCoordinates(CDC *pDC);

      Square mouseLocation(const CPoint &point) const;

      void showHeader(CDC *pDC, const string &header);

      void setTurned(BOOL state) {
         turned = state;
      }

      BOOL isTurned() const
      {
         return turned;
      }

      CFont &getPieceFont() {
         return pieceFont;
      }

      void setPieceFont(CDC *pDC, LPCSTR fontName, GuiOptions::BoardSize boardSize);

      static void calcWindowSize(GuiOptions::BoardSize boardSize, int &x, int &y);

      static int calcFontSize(GuiOptions::BoardSize boardSize);

      int is_mono() const
      {
         return mono;
      }

      void getSquareRect(Square sq,BOOL turned,CRect &loc);

   protected:
      void setPieceFont( CDC *pDC, const LPLOGFONT font ) {
         pieceFont.Detach();
         pieceFont.CreateFontIndirect(font);
         updatePieceFont(pDC);
      }

      void updatePieceFont(CDC *pDC);

   private:
      void clearRect(CDC *pDC, CRect &rect);
      CBrush *get_brush( CDC *pDC, Square sq) const;

      DWORD char_width;
      static DWORD spacing;
      BOOL turned;
      BOOL usePalette;
      BOOL mono;
      DWORD board_right_edge;
      CRect messageRect;
      DWORD width;
      DWORD textX;
      DWORD coordX;
      CFont pieceFont;
      CWnd *myWin;
      CPalette palette;
      int fontSize;
      int xDPI;
      ColorType sideToMove;
      int timeWidth;
      COLORREF messageAreaColor;
      CRgn whiteTimeRegion, blackTimeRegion;
      CRect whiteTimeRect, blackTimeRect;
      CRect size;
      COLORREF activeTimeColor;
      CBrush activeTimeBrush, messageAreaBrush;
};
#endif
