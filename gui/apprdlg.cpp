// Copyright 2002-2007, 2013 by Jon Dart. All Rights Reserved.
//

#include "stdafx.h"
#include "arasan.h"
#include "apprdlg.h"
#include "display.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AppearanceDialog dialog

#define DIALOG_HELP_INDEX 14

// Fonts we support.
static const char *fontNames[] =
{
   "Chess Alpha",
   "Chess Berlin",
   "Chess Maya",
   "Chess Marroquin",
   "Chess Merida",
   "Chess Usual",
   NULL
};

AppearanceDialog::AppearanceDialog(CWnd* pParent, LPCSTR currentFont)
: CDialog(AppearanceDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(AppearanceDialog)
   m_FontSelection = _T("");
   m_FontPreviewValue = _T("o");
   m_boardSize = -1;
   m_Coordinates = FALSE;
   //}}AFX_DATA_INIT
   m_FontSelection = currentFont;
   m_boardSize = (int)guiOptions->getBoardSize();
   m_Coordinates = guiOptions->show_coordinates();
}


void AppearanceDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(AppearanceDialog)
   DDX_Control(pDX, IDCANCEL, m_FontPreviewText);
   DDX_Control(pDX, IDC_FONT_COMBO, m_ComboBox);
   DDX_CBString(pDX, IDC_FONT_COMBO, m_FontSelection);
   DDX_Text(pDX, IDC_FONT_PREVIEW, m_FontPreviewValue);
   DDX_CBIndex(pDX, IDC_BOARD_SIZE_COMBO, m_boardSize);
   DDX_Check(pDX, IDC_SHOW_COORD_CHECK, m_Coordinates);
   //}}AFX_DATA_MAP

   const CString &newFontName = getFontName();
   CDC *pDC = GetDC();
   currentFont.Detach();
   if (!currentFont.CreatePointFont(240,newFontName,pDC)) {
      MessageBox("Cannot create font!","Error",MB_ICONEXCLAMATION);
      return;
   }
   ReleaseDC(pDC);
   Invalidate();
}


BEGIN_MESSAGE_MAP(AppearanceDialog, CDialog)
//{{AFX_MSG_MAP(AppearanceDialog)
ON_CBN_SELCHANGE(IDC_FONT_COMBO, OnSelchangeFontCombo)
ON_WM_DRAWITEM()
ON_BN_CLICKED(ID_HELP, OnHelp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// AppearanceDialog message handlers

BOOL AppearanceDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   for (int i=0;fontNames[i];i++)
      m_ComboBox.AddString(fontNames[i]);
   m_ComboBox.SelectString(0,m_FontSelection);
   OnSelchangeFontCombo();
   return TRUE;                                   // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


void AppearanceDialog::OnSelchangeFontCombo()
{
   UpdateData();
}


void AppearanceDialog::OnDrawItem(int nIDCtl, LPDRAWITEMSTRUCT lpDrawItemStruct)
{

   if (nIDCtl == IDC_FONT_PREVIEW) {
      CDC* controlDC = CDC::FromHandle( lpDrawItemStruct->hDC );
      controlDC->SelectObject(currentFont);
      controlDC->TextOut(0,0,"o");
   }
   else
      CDialog::OnDrawItem(nIDCtl, lpDrawItemStruct);
}


void AppearanceDialog::OnHelp()
{
   ::HtmlHelp(NULL, AfxGetApp()->m_pszHelpFilePath ,HH_HELP_CONTEXT, DIALOG_HELP_INDEX );
}
