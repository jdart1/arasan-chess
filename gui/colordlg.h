#if !defined(AFX_COLORDLG_H__C93C8991_9EF6_4339_A1B9_37D79BD8A7F3__INCLUDED_)
#define AFX_COLORDLG_H__C93C8991_9EF6_4339_A1B9_37D79BD8A7F3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif                                            // _MSC_VER > 1000
// colordlg.h : header file
//
#include "colorbut.h"

/////////////////////////////////////////////////////////////////////////////
// BoardColorDialog dialog

class BoardColorDialog : public CDialog
{
   // Construction
   public:
      BoardColorDialog(CWnd* pParent = NULL);     // standard constructor

      // Dialog Data
      //{{AFX_DATA(BoardColorDialog)
      enum { IDD = IDD_COLORS };
      CButton  m_forceMonoCheck;
      CButton  m_chooseLight;
      CButton  m_chooseDark;
      CColorButton   m_lightColor;
      CColorButton   m_darkColor;
      BOOL  m_forceMono;
      //}}AFX_DATA

      COLORREF getLightColor() const
      {
         return m_lightColor.getColor();
      }

      COLORREF getDarkColor() const
      {
         return m_darkColor.getColor();
      }

      BOOL getForceMono() const
      {
         return m_forceMono;
      }

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(BoardColorDialog)
   protected:
                                                  // DDX/DDV support
      virtual void DoDataExchange(CDataExchange* pDX);
      //}}AFX_VIRTUAL

      // Implementation
   protected:

      // Generated message map functions
      //{{AFX_MSG(BoardColorDialog)
      afx_msg void OnChoosedark();
      afx_msg void OnChooselight();
      afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnForceMono();
      virtual BOOL OnInitDialog();
      afx_msg void OnHelp();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#endif                                            // !defined(AFX_COLORDLG_H__C93C8991_9EF6_4339_A1B9_37D79BD8A7F3__INCLUDED_)
