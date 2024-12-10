// HyperlinkButton.cpp : implementation file
// Copyright 2004 by Jon Dart. All Rights Reserved.

#include "stdafx.h"
#include "arasan.h"
#include "HyperlinkButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// HyperlinkButton

HyperlinkButton::HyperlinkButton()
{
}

HyperlinkButton::~HyperlinkButton()
{
}

BEGIN_MESSAGE_MAP(HyperlinkButton, CButton)
	//{{AFX_MSG_MAP(HyperlinkButton)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void HyperlinkButton::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct) 
{
    DWORD style = GetStyle();
    UINT align = 0;
    if (style & SS_CENTER)
       align = DT_CENTER;
    else if (style & SS_RIGHT)
        align = DT_RIGHT;
    else if (style & SS_LEFT)
        align = DT_LEFT;
    COLORREF crOldColor = ::SetTextColor(lpDrawItemStruct->hDC, RGB(0,0,255));
    ::DrawText(lpDrawItemStruct->hDC, m_hyperlink, m_hyperlink.GetLength(), 
      &lpDrawItemStruct->rcItem, DT_SINGLELINE|DT_VCENTER|align);
    ::SetTextColor(lpDrawItemStruct->hDC, crOldColor);
}

/////////////////////////////////////////////////////////////////////////////
// HyperlinkButton message handlers

void HyperlinkButton::PreSubclassWindow() 
{
	GetWindowText(m_hyperlink);
	CButton::PreSubclassWindow();
}
