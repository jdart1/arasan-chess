// Copyright 1994, 2017 by Jon Dart
// selectga.cpp : implementation file
//

#include "stdafx.h"
#include "selectga.h"

#include <vector>

#ifdef _DEBUG
#undef THIS_FILE
static char BASED_CODE THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// SelectGame dialog

SelectGame::SelectGame(const std::vector<std::string> &contents, CWnd* pParent /*=NULL*/)
: CDialog(SelectGame::IDD, pParent),my_contents(contents),
game_selection(0)
{
   //{{AFX_DATA_INIT(SelectGame)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}


void SelectGame::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(SelectGame)
   DDX_Control(pDX, IDC_GAMELIST, m_GameList);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(SelectGame, CDialog)
//{{AFX_MSG_MAP(SelectGame)
ON_LBN_DBLCLK(IDC_GAMELIST, OnDblclkGamelist)
ON_LBN_SELCHANGE(IDC_GAMELIST, OnSelchangeGamelist)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// SelectGame message handlers

void SelectGame::OnDblclkGamelist()
{
   int sel = m_GameList.GetCurSel();
   if (sel == LB_ERR)
      return;
   else
      my_offset = m_GameList.GetItemData(sel);
   game_selection = sel;
   OnOK();
}


BOOL SelectGame::OnInitDialog()
{
   CDialog::OnInitDialog();
   m_GameList.SetHorizontalExtent(400);
   for (auto it = my_contents.begin(); it != my_contents.end(); it++) {
      CString str = (*it).c_str();
      int tab = str.Find('\t');
      if (tab >= 0) {
         int newIndex = m_GameList.AddString(str.Left(tab));
         CString num(str.Right(str.GetLength()-tab-1));
         DWORD offset;
         sscanf((LPCSTR)num,"%ld",&offset);
         m_GameList.SetItemData(newIndex,offset);
      }
   }

   return TRUE;                                   // return TRUE  unless you set the focus to a control
}


void SelectGame::OnSelchangeGamelist()
{
   int sel = m_GameList.GetCurSel();
   if (sel == LB_ERR)
      return;
   else {
      my_offset = m_GameList.GetItemData(sel);
      game_selection = sel;
   }
}


DWORD SelectGame::get_offset() const
{
   return my_offset;
}
