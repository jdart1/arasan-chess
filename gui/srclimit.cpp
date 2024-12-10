// Copyright 1995, 1997, 2002, 2013 by Jon Dart.  All Rights Reserved.

#include "stdafx.h"
#include "arasan.h"
#include "srclimit.h"
#include "searchop.h"
#include "resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
// SearchLimitsDialog dialog

#define DIALOG_HELP_INDEX 13

SearchLimitsDialog::SearchLimitsDialog(CWnd *parent,const Search_Limit_Options &options)
: CDialog(SearchLimitsDialog::IDD), opts(options)
{
   //{{AFX_DATA_INIT(SearchLimitsDialog)
   m_SearchType = opts.search_type;
   m_Var1 = opts.var1;
   m_Var2 = opts.var2;
   //}}AFX_DATA_INIT
}


SearchLimitsDialog::~SearchLimitsDialog()
{
}


void SearchLimitsDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SearchLimitsDialog)
   DDX_Control(pDX, IDD_VAR2_EDIT, m_Var2Edit);
   DDX_Control(pDX, IDD_VAR1_EDIT, m_Var1Edit);
   DDX_Control(pDX, IDD_SEARCH_LIMITS_LABEL1, m_Label1);
   DDX_Control(pDX, IDD_SEARCH_LIMITS_LABEL2, m_Label2);
   DDX_Radio(pDX, IDD_FIXED_TIME, m_SearchType);
   DDX_Text(pDX, IDD_VAR1_EDIT, m_Var1);
   DDX_Text(pDX, IDD_VAR2_EDIT, m_Var2);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SearchLimitsDialog, CDialog)
//{{AFX_MSG_MAP(SearchLimitsDialog)
ON_BN_CLICKED(IDD_FIXED_TIME, OnFixedTime)
ON_BN_CLICKED(IDD_TOURNAMENT, OnTournament)
ON_BN_CLICKED(IDD_INCREMENTAL, OnIncremental)
ON_BN_CLICKED(IDD_FIXED_DEPTH, OnFixedDepth)
ON_BN_CLICKED(ID_HELP, OnHelp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SearchLimitsDialog message handlers

void SearchLimitsDialog::new_type(int srctype)
{
   switch (srctype) {
      case 0:
         m_Label2.ShowWindow(SW_HIDE);
         m_Var2Edit.ShowWindow(SW_HIDE);
         m_Var2Edit.EnableWindow(FALSE);
         m_Label1.SetWindowText("Seconds/Move:");
         break;
      case 1:
         m_Label2.ShowWindow(SW_SHOW);
         m_Var2Edit.ShowWindow(SW_SHOW);
         m_Label1.SetWindowText("Minutes:");
         m_Label2.SetWindowText("Increment (seconds):");
         m_Var2Edit.EnableWindow(TRUE);
         break;
      case 2:
         m_Label2.ShowWindow(SW_SHOW);
         m_Var2Edit.ShowWindow(SW_SHOW);
         m_Label1.SetWindowText("Moves:");
         m_Label2.SetWindowText("Minutes:");
         m_Var2Edit.EnableWindow(TRUE);
         break;
      case 3:
         m_Label2.ShowWindow(SW_HIDE);
         m_Var2Edit.ShowWindow(SW_HIDE);
         m_Var2Edit.EnableWindow(FALSE);
         m_Label1.SetWindowText("Depth:");
         break;
   }
   m_SearchType = srctype;
}


/////////////////////////////////////////////////////////////////////////////
// SearchLimitsDialog message handlers

void SearchLimitsDialog::OnFixedTime()
{
   new_type(0);
   m_Var1 = m_Var2 = 5;
   UpdateData(FALSE);
}


void SearchLimitsDialog::OnIncremental()
{
   new_type(1);
   m_Var1 = 10; m_Var2 = 0;
   UpdateData(FALSE);
}


void SearchLimitsDialog::OnTournament()
{
   new_type(2);
   m_Var1 = m_Var2 = 40;
   UpdateData(FALSE);
}


void SearchLimitsDialog::OnFixedDepth()
{
   new_type(3);
   m_Var1 = 1;
   UpdateData(FALSE);
}


BOOL SearchLimitsDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   new_type(m_SearchType);

   return TRUE;                                   // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


void SearchLimitsDialog::OnHelp()
{
   ::HtmlHelp(NULL, AfxGetApp()->m_pszHelpFilePath ,HH_HELP_CONTEXT, DIALOG_HELP_INDEX );
}
