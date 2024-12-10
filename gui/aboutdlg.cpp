// AboutDlg.cpp : implementation file
// Copyright 2004 by Jon Dart. All Rights Reserved.

#include "stdafx.h"
#include "arasan.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog dialog

CAboutDialog::CAboutDialog(CWnd* pParent /*=NULL*/)
: CDialog(CAboutDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(CAboutDialog)
   //}}AFX_DATA_INIT
}


void CAboutDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(CAboutDialog)
   DDX_Control(pDX, IDC_HYPERLINK, m_hyperlink);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDialog, CDialog)
//{{AFX_MSG_MAP(CAboutDialog)
ON_BN_CLICKED(IDC_HYPERLINK, OnHyperlink)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog message handlers

BOOL CAboutDialog::OnInitDialog()
{
   CDialog::OnInitDialog();
   m_hyperlink.setHyperlink("http://www.arasanchess.org");

   // TODO: Add extra initialization here

   return TRUE;                                   // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


void CAboutDialog::OnHyperlink()
{
   ::ShellExecute(0, "open", m_hyperlink.getHyperlink(), 0, 0, SW_SHOWNORMAL);
}
