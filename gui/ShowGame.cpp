// Copyright 2002, 2021 by Jon Dart. All Rights Reserved.
//

#include "stdafx.h"
#include "ShowGame.h"

#include <sstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ShowGame dialog

ShowGame::ShowGame(const MoveArray &moves, CWnd* pParent /*=NULL*/)
: CDialog(ShowGame::IDD, pParent),m_moves(moves)
{
   //{{AFX_DATA_INIT(ShowGame)
   // NOTE: the ClassWizard will add member initialization here
   //}}AFX_DATA_INIT
}


void ShowGame::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(ShowGame)
   DDX_Control(pDX, IDC_GAME_LIST, m_List);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ShowGame, CDialog)
//{{AFX_MSG_MAP(ShowGame)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ShowGame message handlers

BOOL ShowGame::OnInitDialog()
{
   CDialog::OnInitDialog();

   unsigned i = 0;
   while (i < m_moves.num_moves()) {
      std::stringstream s;
      s << (i/2)+1 << ". " << m_moves[i].image;
      ++i;
      if (i<m_moves.num_moves()) {
         if (s.tellg() <= 8) s << '\t';
         s << m_moves[i].image;
      }
      m_List.AddString(s.str().c_str());
      ++i;
   }

   return TRUE;                                   // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}
