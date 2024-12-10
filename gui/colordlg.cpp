// Copyright 1995, 2013 by Jon Dart. All Rights Reserved.
// colordlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "colordlg.h"
#include "guiopts.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// BoardColorDialog dialog

extern GuiOptions *guiOptions;

#define DIALOG_HELP_INDEX 15

BoardColorDialog::BoardColorDialog(CWnd* pParent /*=NULL*/)
: CDialog(BoardColorDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(BoardColorDialog)
   m_forceMono = FALSE;
   //}}AFX_DATA_INIT
}


void BoardColorDialog::DoDataExchange(CDataExchange* pDX)
{
   CDialog::DoDataExchange(pDX);
   //{{AFX_DATA_MAP(BoardColorDialog)
   DDX_Control(pDX, IDC_FORCE_MONO, m_forceMonoCheck);
   DDX_Control(pDX, IDC_CHOOSELIGHT, m_chooseLight);
   DDX_Control(pDX, IDC_CHOOSEDARK, m_chooseDark);
   DDX_Control(pDX, IDC_LIGHTCOLOR, m_lightColor);
   DDX_Control(pDX, IDC_DARKCOLOR, m_darkColor);
   DDX_Check(pDX, IDC_FORCE_MONO, m_forceMono);
   //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(BoardColorDialog, CDialog)
//{{AFX_MSG_MAP(BoardColorDialog)
ON_BN_CLICKED(IDC_CHOOSEDARK, OnChoosedark)
ON_BN_CLICKED(IDC_CHOOSELIGHT, OnChooselight)
ON_WM_CREATE()
ON_BN_CLICKED(IDC_FORCE_MONO, OnForceMono)
ON_BN_CLICKED(ID_HELP, OnHelp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// BoardColorDialog message handlers

int BoardColorDialog::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
   if (CDialog::OnCreate(lpCreateStruct) == -1)
      return -1;

   return 0;
}


void BoardColorDialog::OnChoosedark()
{
   CColorDialog clrDlg(getDarkColor(),0,this);
   if (clrDlg.DoModal() == IDOK) {
      COLORREF darkColor = clrDlg.GetColor();
      m_darkColor.setColor(darkColor);
   }
}


void BoardColorDialog::OnChooselight()
{
   CColorDialog clrDlg(getLightColor(),0,this);
   if (clrDlg.DoModal() == IDOK) {
      COLORREF lightColor = clrDlg.GetColor();
      m_lightColor.setColor(lightColor);
   }
}


void BoardColorDialog::OnForceMono()
{
   UpdateData();
   m_chooseDark.EnableWindow(!m_forceMono);
   m_chooseLight.EnableWindow(!m_forceMono);
}


BOOL BoardColorDialog::OnInitDialog()
{
   CDialog::OnInitDialog();
   // set the color of the buttons that show the current colors:
   m_lightColor.setColor(guiOptions->getLightSquareColor());
   m_darkColor.setColor(guiOptions->getDarkSquareColor());
   m_forceMono = guiOptions->getForceMono();
   m_forceMonoCheck.SetCheck(m_forceMono);
   m_chooseDark.EnableWindow(!m_forceMono);
   m_chooseLight.EnableWindow(!m_forceMono);

   return TRUE;                                   // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


void BoardColorDialog::OnHelp()
{
   ::HtmlHelp(NULL, AfxGetApp()->m_pszHelpFilePath ,HH_HELP_CONTEXT, DIALOG_HELP_INDEX );
}
