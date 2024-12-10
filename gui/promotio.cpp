// Copyright 1993 by Jon Dart.  All Rights Reserved.

#include "stdafx.h"
#include "promotio.h"

/////////////////////////////////////////////////////////////////////////////
// PromotionDialog dialog

PromotionDialog::PromotionDialog(CWnd* pParent /*=NULL*/)
: CDialog(PromotionDialog::IDD, pParent)
{
   // default is promotion to queen
   //{{AFX_DATA_INIT(PromotionDialog)
   m_PieceType = 0;
   //}}AFX_DATA_INIT
}


void PromotionDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(PromotionDialog)
   DDX_Radio(pDX, IDC_QUEEN, m_PieceType);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PromotionDialog, CDialog)
//{{AFX_MSG_MAP(PromotionDialog)
// NOTE: the ClassWizard will add message map macros here
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// PromotionDialog message handlers
