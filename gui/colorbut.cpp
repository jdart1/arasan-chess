// Copyright 1995 by Jon Dart.  All Rights Reserved.
// colorbut.cpp : implementation file
//

#include "stdafx.h"
#include "colorbut.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CColorButton

CColorButton::CColorButton()
{
}


CColorButton::~CColorButton()
{
}


COLORREF CColorButton::getColor() const
{
   return myColor;
}


void CColorButton::setColor(COLORREF color)
{
   myColor = color;
   Invalidate();
}


BEGIN_MESSAGE_MAP(CColorButton, CButton)
//{{AFX_MSG_MAP(CColorButton)
// NOTE - the ClassWizard will add and remove mapping macros here.
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CColorButton message handlers

void CColorButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
   COLORREF color = myColor;
   CDC pDC;
   pDC.Attach(lpDrawItemStruct->hDC);
   CBrush colorBrush, backBrush;
   backBrush.CreateSolidBrush(RGB(192,192,192));  // dialog background
   CBrush *oldBrush = pDC.SelectObject(&backBrush);
   pDC.Rectangle(&lpDrawItemStruct->rcItem);

   // create a brush of the appropriate color
   colorBrush.CreateSolidBrush(color);

   pDC.SelectObject(&colorBrush);
   // draw the button area
   CRect colorRect;
   colorRect =   lpDrawItemStruct->rcItem;
   pDC.FillRect(colorRect,&colorBrush);
   // draw a black border
   CBrush blackBrush;
   blackBrush.CreateSolidBrush(RGB(0,0,0));
   pDC.FrameRect(colorRect,&blackBrush);
   pDC.SelectObject(oldBrush);

}
