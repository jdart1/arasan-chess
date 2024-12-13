#if !defined(AFX_PREFDLG_H__A0E63300_3B7D_11D6_A3B7_00022D06A28C__INCLUDED_)
#define AFX_PREFDLG_H__A0E63300_3B7D_11D6_A3B7_00022D06A28C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif                                            // _MSC_VER >= 1000
// prefdlg.h : header file
//
#include "resource.h"

/////////////////////////////////////////////////////////////////////////////
// PreferenceDialog dialog

class PreferenceDialog : public CDialog
{
   // Construction
   public:
      PreferenceDialog(CWnd* pParent = NULL);     // standard constructor

      // Dialog Data
      //{{AFX_DATA(PreferenceDialog)
      enum { IDD = IDD_PREFERENCES };
      CButton  m_AutoSizeCheck;
      CEdit m_HashSizeEdit;
      BOOL  m_AutoSizeHashTable;
      UINT  m_HashSize;
      BOOL  m_Ponder;
      BOOL  m_BeepOnError;
      BOOL  m_ShowCoord;
      BOOL  m_BeepAfterMove;
      BOOL  m_CanResign;
      BOOL  m_PositionLearning;
      CEdit m_CoresEdit;
      UINT  m_Cores;
      CSpinButtonCtrl m_CoresSpin;
      CEdit m_StrengthEdit;
      UINT  m_Strength;
      CSpinButtonCtrl m_StrengthSpin;
      CEdit m_TbPathEdit;
      CString m_TbPath;
      UINT m_BookVariety;
      CEdit m_BookVarietyEdit;
      CSpinButtonCtrl m_BookVarietySpin;
      BOOL m_SaveGames;
      CEdit m_GamePathnameEdit;
      CString m_GamePathname;
      //}}AFX_DATA

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(PreferenceDialog)
   protected:
                                                  // DDX/DDV support
      virtual void DoDataExchange(CDataExchange* pDX);
      //}}AFX_VIRTUAL

      // Implementation
   public:
      void save();
   protected:

      // Generated message map functions
      //{{AFX_MSG(PreferenceDialog)
      afx_msg void OnAutoSize();
      afx_msg void OnDeltaposHashSizeSpin(NMHDR* pNMHDR, LRESULT* pResult);
      virtual BOOL OnInitDialog();
      afx_msg void OnHelp();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()
public:
    afx_msg void OnStnClickedTbPathLabel2();
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif                                            // !defined(AFX_PREFDLG_H__A0E63300_3B7D_11D6_A3B7_00022D06A28C__INCLUDED_)
