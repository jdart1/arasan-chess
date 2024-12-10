// Copyright 1993 by Jon Dart.  All Rights Reserved.

#ifndef _PROMOTION_H
#define _PROMOTION_H

#include "resource.h"
#ifndef __AFXWIN_H__
#include <afxwin.h>
#endif

/////////////////////////////////////////////////////////////////////////////
// PromotionDialog dialog

class PromotionDialog : public CDialog
{
   // Construction
   public:
      PromotionDialog(CWnd* pParent = NULL);      // standard constructor

      // Dialog Data
      //{{AFX_DATA(PromotionDialog)
      enum { IDD = IDD_PROMOTION };
      int             m_PieceType;
      //}}AFX_DATA

      // Implementation
   protected:
                                                  // DDX/DDV support
      virtual void DoDataExchange(CDataExchange* pDX);

      // Generated message map functions
      //{{AFX_MSG(PromotionDialog)
      // NOTE: the ClassWizard will add member functions here
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()
};
#endif
