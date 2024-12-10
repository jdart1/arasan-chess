#if !defined(AFX_HYPERLINKBUTTON_H__2ECAFA91_DA7B_4959_91A8_D26A3145A0F0__INCLUDED_)
#define AFX_HYPERLINKBUTTON_H__2ECAFA91_DA7B_4959_91A8_D26A3145A0F0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// HyperlinkButton.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// HyperlinkButton window

class HyperlinkButton : public CButton
{
// Construction
public:
	HyperlinkButton();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HyperlinkButton)
	protected:
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL
        virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

// Implementation
public:
	virtual ~HyperlinkButton();

        const CString &getHyperlink() const {
           return m_hyperlink;
        }
        void setHyperlink(const CString &str) {
           m_hyperlink = str;
        }

	// Generated message map functions
protected:
	//{{AFX_MSG(HyperlinkButton)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

        CString m_hyperlink;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HYPERLINKBUTTON_H__2ECAFA91_DA7B_4959_91A8_D26A3145A0F0__INCLUDED_)
