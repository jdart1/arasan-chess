// Copyright 2002 by Jon Dart. All Rights Reserved.
//
#if !defined(AFX_SHOWGAME_H__6BF30220_24A5_11D6_A3B7_00022D06A28C__INCLUDED_)
#define AFX_SHOWGAME_H__6BF30220_24A5_11D6_A3B7_00022D06A28C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif                                            // _MSC_VER >= 1000
// ShowGame.h : header file
//
#include "movearr.h"
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// ShowGame dialog

class ShowGame : public CDialog
{
   // Construction
   public:
                                                  // standard constructor
      ShowGame(const MoveArray &moves, CWnd* pParent = NULL);

      // Dialog Data
      //{{AFX_DATA(ShowGame)
      enum { IDD = IDD_SHOW_GAME_MOVES };
      CListBox m_List;
      //}}AFX_DATA

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(ShowGame)
   protected:
                                                  // DDX/DDV support
      virtual void DoDataExchange(CDataExchange* pDX);
      //}}AFX_VIRTUAL

      // Implementation
   protected:
      const MoveArray &m_moves;

      // Generated message map functions
      //{{AFX_MSG(ShowGame)
      virtual BOOL OnInitDialog();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif                                            // !defined(AFX_SHOWGAME_H__6BF30220_24A5_11D6_A3B7_00022D06A28C__INCLUDED_)
