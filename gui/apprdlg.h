// Copyright 2002 by Jon Dart. All Rights Reserved.
#if !defined(AFX_APPRDLG_H__5FECB180_3EF7_11D6_A3B7_00022D06A28C__INCLUDED_)
#define AFX_APPRDLG_H__5FECB180_3EF7_11D6_A3B7_00022D06A28C__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif                                            // _MSC_VER >= 1000
// fontsele.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// AppearanceDialog dialog

class AppearanceDialog : public CDialog
{
   // Construction
   public:
                                                  // standard constructor
      AppearanceDialog(CWnd* pParent,LPCSTR currentFont);

      const CString &getFontName() {
         return m_FontSelection;
      }

      const int getFontType() {
         return m_fontType;
      }

      // Dialog Data
      //{{AFX_DATA(AppearanceDialog)
      enum { IDD = IDD_APPEARANCE };
      CButton  m_FontPreviewText;
      CComboBox   m_ComboBox;
      CString  m_FontSelection;
      CString  m_FontPreviewValue;
      int      m_boardSize;
      BOOL  m_Coordinates;
      //}}AFX_DATA

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(AppearanceDialog)
   protected:
                                                  // DDX/DDV support
      virtual void DoDataExchange(CDataExchange* pDX);
      //}}AFX_VIRTUAL

      // Implementation
   protected:

      // Generated message map functions
      //{{AFX_MSG(AppearanceDialog)
      virtual BOOL OnInitDialog();
      afx_msg void OnSelchangeFontCombo();
      afx_msg void OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct);
      afx_msg void OnHelp();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

         int m_fontType;
      CFont currentFont;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif                                            // !defined(AFX_APPRDLG_H__5FECB180_3EF7_11D6_A3B7_00022D06A28C__INCLUDED_)
