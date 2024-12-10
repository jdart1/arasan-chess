#if !defined(AFX_ABOUTDLG_H__DC1C2E1D_5FED_430E_95D9_2D2EBAE9036A__INCLUDED_)
#define AFX_ABOUTDLG_H__DC1C2E1D_5FED_430E_95D9_2D2EBAE9036A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif                                            // _MSC_VER > 1000
// AboutDlg.h : header file
//
#include "HyperlinkButton.h"

/////////////////////////////////////////////////////////////////////////////
// CAboutDialog dialog

class CAboutDialog : public CDialog
{
   // Construction
   public:
      CAboutDialog(CWnd* pParent = NULL);         // standard constructor

      // Dialog Data
      //{{AFX_DATA(CAboutDialog)
      enum { IDD = IDD_ABOUTBOX };
      HyperlinkButton   m_hyperlink;
      //}}AFX_DATA

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(CAboutDialog)
   protected:
                                                  // DDX/DDV support
      virtual void DoDataExchange(CDataExchange* pDX);
      //}}AFX_VIRTUAL

      // Implementation
   protected:

      // Generated message map functions
      //{{AFX_MSG(CAboutDialog)
      virtual BOOL OnInitDialog();
      afx_msg void OnHyperlink();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif                                            // !defined(AFX_ABOUTDLG_H__DC1C2E1D_5FED_430E_95D9_2D2EBAE9036A__INCLUDED_)
