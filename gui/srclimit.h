// Copyright 1995, 1997, 2001 by Jon Dart.  All Rights Reserved.
// srclimit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// SearchLimitsDialog dialog

#include "searchop.h"

class SearchLimitsDialog : public CDialog
{
   //	DECLARE_DYNCREATE(SearchLimitsDialog)

   // Construction
   public:
      SearchLimitsDialog(CWnd *pParent, const Search_Limit_Options &options);
      virtual ~SearchLimitsDialog();

      // Dialog Data
      //{{AFX_DATA(SearchLimitsDialog)
      enum { IDD = IDD_SEARCH_LIMITS };
      CEdit m_Var2Edit;
      CEdit m_Var1Edit;
      CStatic  m_Label1;
      CStatic  m_Label2;
      int      m_SearchType;
      int     m_Var1;
      int     m_Var2;
      //}}AFX_DATA

      // Overrides
      // ClassWizard generate virtual function overrides
      //{{AFX_VIRTUAL(SearchLimitsDialog)
   protected:
                                                  // DDX/DDV support
      virtual void DoDataExchange(CDataExchange* pDX);
      //}}AFX_VIRTUAL

      // Implementation
   protected:
      void new_type(int);

      // Generated message map functions
      //{{AFX_MSG(SearchLimitsDialog)
      afx_msg void OnFixedTime();
      afx_msg void OnTournament();
      afx_msg void OnIncremental();
      virtual BOOL OnInitDialog();
      afx_msg void OnFixedDepth();
      afx_msg void OnHelp();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

      private:
      const Search_Limit_Options &opts;
public:
    afx_msg void OnStnClickedStrengthLabel();
};
