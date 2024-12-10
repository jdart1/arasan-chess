#ifndef __COLORBUT_H__
#define __COLORBUT_H__

// Copyright 1995 by Jon Dart.  All Rights Reserved.
// colorbut.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorButton.  This implements a button in the Color options page.

class CColorButton : public CButton
{
   // Construction
   public:
      CColorButton();

      // Attributes
   public:

      // Operations
   public:

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(CColorButton)
   public:
      virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
      //}}AFX_VIRTUAL

      // Implementation
   public:
      virtual ~CColorButton();

      COLORREF getColor() const;

      void setColor(COLORREF);

      // Generated message map functions
   protected:
      //{{AFX_MSG(CColorButton)
      // NOTE - the ClassWizard will add and remove member functions here.
      //}}AFX_MSG

      DECLARE_MESSAGE_MAP()

         COLORREF myColor;
};

/////////////////////////////////////////////////////////////////////////////
#endif
