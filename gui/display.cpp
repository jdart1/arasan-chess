// Copyright 1994-2008, 2023 by Jon Dart. All Rights Reserved.

#include "stdafx.h"
#include "chess.h"
#include "display.h"
#include "resource.h"
#include "arasan.h"
#include "drawutil.h"

#define TEXT_OFFSET             30                /* from right edge */
#define COORD_OFFSET            5
#define TIME_Y                  COORD_OFFSET+5
#define MOVE_Y                  158
#define STATUS_Y                76
#define PLAYERS_Y               92
#define RESULT_Y                175
#define ECO_Y                   230

// Piece map for "Chess Alpha" font:
static const char *whiteSquareMapAlpha = " phbrqk  ojntwl ";
static const char *blackSquareMapAlpha = "+PHBRQK++OJNTWL+";
static const char *whitePieceMapAlpha = " phbrkq";
static const char *blackPieceMapAlpha = " ojntwl";

// Piece map for "Marroquin", "Maya" and similar fonts:
static const char *whiteSquareMapMaya = " pnbrqk  omvtwl ";
static const char *blackSquareMapMaya = "+PNBRQK++OMVTWL+";
static const char *whitePieceMapMaya = "+OMVTWL";
static const char *blackPieceMapMaya = " omvtwl";

// Piece map for "Berlin" font:
static const char *whiteSquareMapBerlin = " phbrqk  ojntwl ";
static const char *blackSquareMapBerlin = "+PHBRQK++OJNTWL+";
static const char *whitePieceMapBerlin = "+OJNTWL";
static const char *blackPieceMapBerlin = " ojntwl";

// used for black & white display:
static const char **blackSquareMap;
static const char **whiteSquareMap;

// used for color display:
static const char **blackPieceMap;
static const char **whitePieceMap;

static CFont *textFont = NULL;
static CFont *coordFont = NULL;

static DWORD square_width,square_height;
DWORD Display::spacing;

void Display::clearRect(CDC *pDC, CRect &rect)
{
   CRgn rgn;
   rgn.CreateRectRgnIndirect(rect);
   CBrush brush;
   brush.CreateSolidBrush(messageAreaColor);
   pDC->FillRgn(&rgn,&brush);
}


Display::Display( CWnd *pWin, const CRect &initialSize )
: turned(FALSE)
{
   myWin = pWin;
   CDC *pDC = pWin->GetDC();
   int fontSize = calcFontSize(guiOptions->getBoardSize());
   if (!pieceFont.CreatePointFont(fontSize,guiOptions->getPieceFontName(),pDC)) {
      AfxMessageBox("failed to load chess font");
      pWin->ReleaseDC(pDC);
      return;
   }
   textFont = new CFont();
   NONCLIENTMETRICS ncm;
   TEXTMETRIC tm;

   SystemParametersInfo(SPI_GETNONCLIENTMETRICS,sizeof(NONCLIENTMETRICS),&ncm,0);
   textFont->CreatePointFont(90,ncm.lfMessageFont.lfFaceName);
   CFont *oldFont = (CFont*)pDC->SelectObject(textFont);
   pDC->GetOutputTextMetrics(&tm);

   char_width = tm.tmAveCharWidth;
   spacing = tm.tmHeight + tm.tmExternalLeading;
   messageAreaColor = COLORREF(0xffffff);

   coordFont = new CFont();
   coordFont->CreatePointFont(80,ncm.lfMessageFont.lfFaceName);

   CRect timeRect;
   pDC->DrawText("00:00:00", &timeRect, DT_CALCRECT);
   timeWidth = timeRect.Width();

   UINT nColors = pDC->GetDeviceCaps(NUMCOLORS);
   mono = (nColors <= 2);
   if (mono) {
      // 16- and 32-bit color modes do not seem
      // to report NUMCOLORS correctly.
      mono = pDC->GetDeviceCaps(BITSPIXEL) == 1 &&
         pDC->GetDeviceCaps(PLANES) == 1;
   }
   usePalette = pDC->GetDeviceCaps(RASTERCAPS) & RC_PALETTE;

   // now select piece font to get data from that
   updatePieceFont(pDC);
   pDC->SelectObject(oldFont);
   pWin->ReleaseDC(pDC);
   setSize(initialSize);

   activeTimeColor = COLORREF(0x1f1f1f);
   activeTimeBrush.CreateSolidBrush(activeTimeColor);
   messageAreaBrush.CreateSolidBrush(messageAreaColor);
}


void Display::getRect( DisplayRegion region,CRect &rect)
{
   switch(region) {
      case SideToMove:
         rect.left = textX;
         rect.top = 12;
         rect.right = width-5;
         rect.bottom = 52;
         break;
      case ECO:
         rect.left=textX;
         rect.top=ECO_Y;
         rect.right=width-5;
         rect.bottom=ECO_Y+5*spacing;
         break;
      case Players:
         rect.left=textX;
         rect.top=PLAYERS_Y;
         rect.right=width-5;
         rect.bottom=PLAYERS_Y+4*spacing;
         break;
      case Result:
         rect.left=textX;
         rect.top=RESULT_Y;
         rect.right=width-5;
         rect.bottom=RESULT_Y+spacing;
         break;
   }
}


void Display::setPieceFont(CDC *pDC, LPCSTR fontName, GuiOptions::BoardSize boardSize)
{
   int fontSize = calcFontSize(boardSize);
   CFont newFont;
   if (!newFont.CreatePointFont(fontSize,fontName)) {
      ::MessageBox(NULL,"Cannot create chess font!","Error",MB_ICONEXCLAMATION);
      return;
   }
   LOGFONT lf;
   newFont.GetLogFont(&lf);
   setPieceFont(pDC,&lf);
}


void Display::resize(CDC *pDC, GuiOptions::BoardSize size)
{
   LOGFONT lf;
   pieceFont.GetLogFont(&lf);
   setPieceFont(pDC,lf.lfFaceName,size);
}


void Display::updatePieceFont(CDC *pDC)
{
   TEXTMETRIC piece_tm;
   CFont *oldFont = (CFont*)pDC->SelectObject(pieceFont);
   pDC->GetTextMetrics(&piece_tm);

   square_width = piece_tm.tmMaxCharWidth;
   square_height = piece_tm.tmHeight;
   board_right_edge = piece_tm.tmMaxCharWidth*8 + 25;

   textX = 8*square_width+TEXT_OFFSET;
   coordX = 8*square_width+COORD_OFFSET;
   LOGFONT lf;
   pieceFont.GetLogFont(&lf);
   if (strcmp(lf.lfFaceName,"Chess Alpha")==0) {
      blackSquareMap = (const char**)&blackSquareMapAlpha;
      whiteSquareMap = (const char**)&whiteSquareMapAlpha;
      blackPieceMap = (const char**)&blackPieceMapAlpha;
      whitePieceMap = (const char**)&blackPieceMapAlpha;
   }
   else if (strcmp(lf.lfFaceName,"Chess Berlin")==0) {
      blackSquareMap = (const char**)&blackSquareMapBerlin;
      whiteSquareMap = (const char**)&whiteSquareMapBerlin;
      blackPieceMap = (const char**)&blackPieceMapBerlin;
      whitePieceMap = (const char**)&blackPieceMapBerlin;
   }
   else {
      blackSquareMap = (const char**)&blackSquareMapMaya;
      whiteSquareMap = (const char**)&whiteSquareMapMaya;
      blackPieceMap = (const char**)&blackPieceMapMaya;
      whitePieceMap = (const char**)&blackPieceMapMaya;
   }
}


Display::~Display()
{
   delete textFont;
   delete coordFont;
}


void Display::drawMessageArea(CDC *pDC, CRgn *pRgn)
{
   CBrush brush;
   brush.CreateSolidBrush(COLORREF(0x000000));
   pDC->FrameRect(&messageRect,&brush);
   showTimeArea(pDC);
}


void Display::drawBoard( CDC *pDC, const Board &board, const CRect &drawArea, BOOL turned)
{

   pDC->SelectObject(pieceFont);
   if (usePalette) {
      pDC->SelectPalette(&palette,FALSE);
      pDC->RealizePalette();
   }

   int vert = 0;
   for (int i = 0; i < 8; i++) {
      for (int j = 0; j < 8; j++) {
         Square sq;
         if (turned)
            sq = MakeSquare(8-j,8-i,Black);
         else
            sq = MakeSquare(j+1,i+1,Black);
         drawPiece(pDC,sq,board[sq]);
      }
      vert += square_height;
   }
   showSide(pDC,board.sideToMove());
}


void Display::drawPiece(CDC *pDC, Square sq, Piece p)
{
   CRect loc;
   getSquareRect(sq,turned,loc);
   // Do not draw this piece if its square is not visible
   //if (!pDC->RectVisible(&loc)) return;
   char text;
   if (mono || guiOptions->getForceMono()) {
      // use "hatched" background for dark squares, and
      // don't try to set colors.
      if (SquareColor(sq) == White)
         text = (*whiteSquareMap)[p];
      else
         text = (*blackSquareMap)[p];
      pDC->TextOut(loc.left,loc.top,&text,1);
   }
   else {
      PieceType typeOfPiece = TypeOfPiece(p);
      COLORREF light = guiOptions->getLightSquareColor();
      COLORREF dark = guiOptions->getDarkSquareColor();
      if (usePalette) {
         // Select our own palette here
         pDC->SelectPalette(&palette,FALSE);
         UINT result = pDC->RealizePalette();
      }

      COLORREF oldBkColor = pDC->SetBkColor(SquareColor(sq) == White ? light : dark);
      COLORREF oldTextColor = (PieceColor(p) == White) ?
         pDC->SetTextColor(RGB(255,255,255)) :
      pDC->SetTextColor(RGB(0,0,0));

      CBrush squareBrush(SquareColor(sq) == White ? light : dark);
      CBrush *oldBrush = (CBrush*)pDC->SelectObject(&squareBrush);
      pDC->Rectangle(loc);
      pDC->SelectObject(oldBrush);
      if (typeOfPiece != EmptyPiece) {
         text = PieceColor(p) == White ?
            (*whitePieceMap)[typeOfPiece] : (*blackPieceMap)[typeOfPiece];
         CString str="";
         str += text;
         // for some reason drawing the text in OPAQUE mode does not work well on
         // palette displays. Set the mode TRANSPARENT, since we have already drawn
         // the square background with the Rectangle call, above.
         pDC->SetBkMode(TRANSPARENT);
         pDC->TextOut(loc.left,loc.top,str);
         // Draw outline around White pieces (unfortunately this
         // does not do anti-aliasing).
         if (PieceColor(p) == White) DrawUtil::Draw(pDC,loc,&text);
      }
      pDC->SetBkColor(oldBkColor);
      pDC->SetTextColor(oldTextColor);

   }
}


void Display::getSquareRect(Square sq,BOOL turned,CRect &loc)
{
   int f = File(sq);
   int r = Rank(sq,Black);
   if (turned) {
      r = 9-r;
      f = 9-f;
   }

   int vert = square_height*(r-1);
   int horiz = square_width*(f-1);
   CRect sqloc(horiz,vert,horiz+square_width,vert+square_height);
   loc = sqloc;
}


void Display::setSize(const CRect &size)
{
   this->size = size;
   width = size.Width();
   messageRect = CRect(board_right_edge,10,size.Width()-10,size.Height()-10);
   int hpad = messageRect.Width()/6;
   whiteTimeRegion.DeleteObject();
   blackTimeRegion.DeleteObject();
   CRect wRect(board_right_edge + hpad,20,
      board_right_edge+hpad+timeWidth,20+2*spacing);
   wRect.InflateRect(CSize(2,2));
   whiteTimeRegion.CreateRectRgn(wRect.right,wRect.top,wRect.left,wRect.bottom);
   CRect bRect(messageRect.right-hpad-timeWidth,20,
      messageRect.right-hpad,20+2*spacing);
   bRect.InflateRect(CSize(2,2));
   blackTimeRegion.CreateRectRgn(bRect.right,bRect.top,bRect.left,bRect.bottom);
}


void Display::showSide(CDC *pDC, ColorType side)
{
   sideToMove = side;
}


void Display::showTimeArea(CDC *pDC)
{
   if (!textFont)                                 // not initialized yet
      return;

   pDC->FrameRgn(&whiteTimeRegion,
      CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)),1,1);
   pDC->FrameRgn(&blackTimeRegion,
      CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)),1,1);
}


void Display::showTime(CDC *pDC, time_t time, ColorType side, int active)
{
   if (!textFont)                                 // not initialized yet
      return;

   // internal time is in 1/100 second increments
   unsigned hours = (unsigned)(time/360000U);
   unsigned minutes = (unsigned) ((time - (hours*360000U))/6000U);
   unsigned seconds = (unsigned)(time - (hours*360000U) - (minutes*6000U))/100;
   // convert time to ASCII:
   char time_str[50];
   wsprintf(time_str,"%02d:%02d:%02d",
      (int)hours,(int)minutes,(int)seconds);
   CRect box;
   CRgn *rgn;
   CString text;
   if (side == White) {
      whiteTimeRegion.GetRgnBox(&box);
      rgn = &whiteTimeRegion;
      text.LoadString(IDS_WHITE);
   }
   else {
      blackTimeRegion.GetRgnBox(&box);
      rgn = &blackTimeRegion;
      text.LoadString(IDS_BLACK);
   }
   box.DeflateRect(CSize(2,2));
   CFont *oldFont = (CFont*)pDC->SelectObject(textFont);
   COLORREF oldBkColor, oldTextColor;
   if (active) {
      pDC->FillRgn(rgn,&activeTimeBrush);
      oldBkColor = pDC->SetBkColor(activeTimeColor);
      oldTextColor = pDC->SetTextColor((COLORREF)0xffffff);
   }
   else {
      pDC->FillRgn(rgn,&messageAreaBrush);
      oldBkColor = pDC->SetBkColor(messageAreaColor);
      oldTextColor = pDC->SetTextColor((COLORREF)0x000000);
   }
   pDC->FrameRgn(rgn,
      CBrush::FromHandle((HBRUSH)GetStockObject(BLACK_BRUSH)),1,1);
   CRect rcText;
   rcText.SetRect(box.left,box.top,box.right,box.top+spacing);
   pDC->DrawText(text,rcText,DT_CENTER);
   rcText.SetRect(box.left,box.top+spacing,box.right,box.top+2*spacing);
   pDC->DrawText(time_str,rcText,DT_CENTER);
   pDC->SelectObject(oldFont);
   pDC->SetTextColor(oldTextColor);
   pDC->SetBkColor(oldBkColor);
}


void Display::showECO(CDC *pDC, const char *eco, const char *openingName)
{
   if (!textFont)                                 // not initialized yet
      return;
   int right = messageRect.right-5;
   int left = messageRect.left+5;
   COLORREF oldColor = pDC->SetBkColor(messageAreaColor);
   pDC->SelectObject(textFont);
   CRect rcText(left,ECO_Y,right,ECO_Y+spacing);
   clearRect(pDC,rcText);
   rcText.DeflateRect(CSize(2,0));
   char msg[20];
   *msg = '\0';
   if (*eco)
      sprintf(msg,"ECO: %s",eco);
   pDC->DrawText(msg,strlen(msg),&rcText,DT_LEFT);
   rcText.SetRect(left,ECO_Y+spacing,right,ECO_Y+4*spacing);
   clearRect(pDC,rcText);
   rcText.DeflateRect(CSize(2,0));

   pDC->DrawText(openingName,strlen(openingName),
      &rcText, DT_LEFT | DT_WORDBREAK);
   pDC->SetBkColor(oldColor);
}


void Display::showMove(class CDC *pDC,const string &image,int moveCount,ColorType side)
{

   int right = messageRect.right-5;
   int left = messageRect.left+5;
   CFont *oldFont = (CFont*)pDC->SelectObject(textFont);
   CRect rcText(left,MOVE_Y,right,MOVE_Y+spacing);
   char text[80];
   // erase any previous text:
   clearRect(pDC,rcText);
   rcText.DeflateRect(CSize(2,0));
   if (image.length()==0) {
      pDC->SelectObject(oldFont);
      return;
   }
   int move_num = (moveCount-1)/2;
   if (side == White)
      wsprintf(text,"%d  %s",move_num+1,image.c_str());
   else
      wsprintf(text,"%d ... %s",move_num+1,image.c_str());
   COLORREF oldColor = pDC->SetBkColor(messageAreaColor);
   pDC->DrawText(text,strlen(text), &rcText, DT_LEFT | DT_WORDBREAK);
   pDC->SelectObject(oldFont);
   pDC->SetBkColor(oldColor);
}


void Display::showResult(CDC *pDC, const string &result)
{
   int right = messageRect.right-5;
   int left = messageRect.left+5;
   pDC->SelectObject(textFont);
   CRect rcText(left,RESULT_Y,right,(int)RESULT_Y+spacing);
   // erase any previous text:
   clearRect(pDC,rcText);
   rcText.DeflateRect(CSize(2,0));
   if (result.length()==0 || (result.compare("*")==0)) {
      return;
   }
   COLORREF oldColor = pDC->SetBkColor(messageAreaColor);
   pDC->DrawText(result.c_str(),result.length(), &rcText, DT_LEFT | DT_WORDBREAK);
   pDC->SetBkColor(oldColor);
}


void Display::showCoordinates(CDC *pDC)
{
   CFont *oldFont = pDC->SelectObject(coordFont);
   TEXTMETRIC tm;
   pDC->GetTextMetrics(&tm);
   int fontHeight = tm.tmHeight + tm.tmExternalLeading;
   CString letters;
   letters.LoadString(IDS_LETTER_COORDS);
   CString numbers;
   numbers.LoadString(IDS_NUMBER_COORDS);
   int i;
   for (i = 0; i < 8; i++) {
      CRect loc(i*square_width,square_height*8+6,(i+1)*square_width,square_height*8+6+fontHeight);
      int midY = i*square_height + square_height/2;
      CRect loc2(coordX,midY-fontHeight/2,coordX+12,midY+fontHeight/2);
      if (turned) {
         pDC->DrawText((LPCSTR)letters.Mid(7-i,1),1,&loc,DT_CENTER);
         pDC->DrawText((LPCSTR)numbers.Mid(i,1),1,&loc2,DT_CENTER | DT_VCENTER );
      }
      else {
         pDC->DrawText((LPCSTR)letters.Mid(i,1),1,&loc,DT_CENTER);
         pDC->DrawText((LPCSTR)numbers.Mid(7-i,1),1,&loc2,DT_CENTER | DT_VCENTER );
      }
   }
   pDC->SelectObject(oldFont);
}


Square Display::mouseLocation(const CPoint &p) const
{
   int x,y;
   x = (p.x/square_width)+1;
   if (turned)
      x = 9-x;
   y = (p.y/square_height)+1;
   if (x > 8 || y > 8)
      return InvalidSquare;
   else if (turned)
      return MakeSquare(x,y,White);
   else
      return MakeSquare(x,y,Black);
}


void Display::showHeader(CDC *pDC, const string &header)
{
   if (!textFont)                                 // not initialized yet
      return;
   pDC->SelectObject(textFont);
   int left=messageRect.left+5;
   int right=messageRect.right-5;
   CRect rcText(left,PLAYERS_Y,right,PLAYERS_Y+4*spacing);
   clearRect(pDC,rcText);
   rcText.DeflateRect(CSize(2,0));
   COLORREF oldColor = pDC->SetBkColor(messageAreaColor);

   pDC->DrawText(header.c_str(),header.length(),
      &rcText, DT_LEFT | DT_WORDBREAK | DT_NOPREFIX);
   pDC->SetBkColor(oldColor);
}


int Display::calcFontSize(GuiOptions::BoardSize boardSize)
{
   HDC pDC = GetDC(0);
   int fontSize;
   int xDPI = GetDeviceCaps(pDC,LOGPIXELSX);
   switch (guiOptions->getBoardSize()) {
      case GuiOptions::Small: fontSize = (240*96)/xDPI; break;
      case GuiOptions::Medium: fontSize = (360*96)/xDPI; break;
      case GuiOptions::Large: fontSize = (480*96)/xDPI; break;
      case GuiOptions::XLarge: fontSize = (600*96)/xDPI; break;
      case GuiOptions::XXLarge: fontSize = (900*96)/xDPI; break;
   }
   ReleaseDC(0,pDC);
   return fontSize;
}


void Display::calcWindowSize(GuiOptions::BoardSize boardSize, int &w, int &h)
{
   // should really base this on the board size (which is a function
   // of the font size) but the window sizing happens before the font
   // initialization, so sizes must be constants.
   // Try to compensate for higher DPI screens (120 DPI):
   HDC screen = GetDC(0);
   int xDPI = GetDeviceCaps(screen, LOGPIXELSX);
   int wextra = 0;
   if (xDPI > 96) wextra = 20;
   ReleaseDC(0, screen);
   switch (boardSize) {
      case GuiOptions::Small: w = 256+220+wextra; h = 200+256; break;
      case GuiOptions::Medium: w = 384+220+wextra; h = 175+384; break;
      case GuiOptions::Large: w = 576+220+wextra; h = 150+576; break;
      case GuiOptions::XLarge: w = 768+220+wextra; h = 125+768; break;
   }

}
