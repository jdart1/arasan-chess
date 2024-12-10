// Copyright 1994, 2017 by Jon Dart
#ifndef __SELECTGA_H__
#define __SELECTGA_H__
// selectga.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SelectGame dialog

#include "resource.h"
#include "chessio.h"

class SelectGame : public CDialog
{
   // Construction
   public:
      SelectGame(const std::vector<std::string> &contents, CWnd* pParent = NULL);

      // Dialog Data
      //{{AFX_DATA(SelectGame)
      enum { IDD = IDD_SELECT_GAME };
      CListBox        m_GameList;
      //}}AFX_DATA

      DWORD get_offset() const;

      DWORD get_selection() const
      {
         return game_selection;
      }

      // Implementation
   protected:
                                                  // DDX/DDV support
      virtual void DoDataExchange(CDataExchange* pDX);

      // Generated message map functions
      //{{AFX_MSG(SelectGame)
      afx_msg void OnDblclkGamelist();
      virtual BOOL OnInitDialog();
      afx_msg void OnSelchangeGamelist();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

      const std::vector<std::string> &my_contents;
      DWORD my_offset, game_selection;
};
#endif
