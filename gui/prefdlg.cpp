// prefdlg.cpp : implementation file
// Copyright 2002, 2012, 2013, 2024 by Jon Dart. All Rights Reserved.

#include "stdafx.h"
#include "prefdlg.h"

#include "arasan.h"
#include "constant.h"

#include <stdio.h>

/////////////////////////////////////////////////////////////////////////////
// PreferenceDialog dialog

#define DIALOG_HELP_INDEX 16

PreferenceDialog::PreferenceDialog(CWnd* pParent /*=NULL*/)
: CDialog(PreferenceDialog::IDD, pParent)
{
   //{{AFX_DATA_INIT(PreferenceDialog)
   m_AutoSizeHashTable = FALSE;
   m_HashSize = 0;
   m_Ponder = FALSE;
   m_BeepOnError = FALSE;
   m_ShowCoord = FALSE;
   m_BeepAfterMove = FALSE;
   m_CanResign = FALSE;
   m_PositionLearning = FALSE;
   //}}AFX_DATA_INIT
   m_AutoSizeHashTable = guiOptions->auto_size_hash_table();
   m_HashSize = guiOptions->hash_table_size();
   m_Ponder = guiOptions->think_when_idle();
   m_BeepOnError = guiOptions->beep_on_error();
   m_BeepAfterMove = guiOptions->beep_after_move();
   m_CanResign = guiOptions->can_resign();
   m_PositionLearning = guiOptions->position_learning();
   m_Cores = guiOptions->cores();
   m_Strength = guiOptions->strength();
   m_TbPath = guiOptions->tbPath();
   m_BookVariety = guiOptions->bookVariety();
}


void PreferenceDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(PreferenceDialog)
    DDX_Control(pDX, IDC_AUTO_SIZE, m_AutoSizeCheck);
    DDX_Control(pDX, IDC_HASH_SIZE_EDIT, m_HashSizeEdit);
    DDX_Check(pDX, IDC_AUTO_SIZE, m_AutoSizeHashTable);
    DDX_Text(pDX, IDC_HASH_SIZE_EDIT, m_HashSize);
    DDV_MinMaxUInt(pDX, m_HashSize, 0, 1500);
    DDX_Check(pDX, IDC_PONDER_CHECK, m_Ponder);
    DDX_Check(pDX, IDC_BEEP_ON_ERROR_CHECK, m_BeepOnError);
    DDX_Check(pDX, IDC_BEEP_AFTER_MOVE_CHECK, m_BeepAfterMove);
    DDX_Check(pDX, IDC_CAN_RESIGN_CHECK, m_CanResign);
    DDX_Check(pDX, IDC_POSITION_LEARNING_CHECK, m_PositionLearning);
    DDX_Control(pDX, IDC_CORES_EDIT, m_CoresEdit);
    DDX_Text(pDX, IDC_CORES_EDIT, m_Cores);
    DDX_Control(pDX, IDC_CORES_SPIN, m_CoresSpin);
    DDX_Control(pDX, IDC_STRENGTH_EDIT, m_StrengthEdit);
    DDX_Text(pDX, IDC_STRENGTH_EDIT, m_Strength);
    DDX_Control(pDX, IDC_STRENGTH_SPIN, m_StrengthSpin);
    DDX_Control(pDX, IDC_TB_PATH_EDIT, m_TbPathEdit);
    DDX_Text(pDX, IDC_TB_PATH_EDIT, m_TbPath);
    DDX_Control(pDX, IDC_BOOK_VARIETY_EDIT, m_BookVarietyEdit);
    DDX_Control(pDX, IDC_BOOK_VARIETY_SPIN, m_BookVarietySpin);
    DDX_Text(pDX, IDC_BOOK_VARIETY_EDIT, m_BookVariety);
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(PreferenceDialog, CDialog)
//{{AFX_MSG_MAP(PreferenceDialog)
ON_BN_CLICKED(IDC_AUTO_SIZE, OnAutoSize)
ON_NOTIFY(UDN_DELTAPOS, IDC_HASH_SIZE_SPIN, OnDeltaposHashSizeSpin)
ON_BN_CLICKED(ID_HELP, OnHelp)
//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void PreferenceDialog::save()
{
   GeneralPreferences prefs;
   guiOptions->getGeneralPreferences(prefs);
   prefs.beep_on_error = m_BeepOnError;
   prefs.beep_after_move = m_BeepAfterMove;
   prefs.think_when_idle = m_Ponder;
   prefs.hash_table_size = m_HashSize;
   prefs.auto_size_hash_table = m_AutoSizeHashTable;
   prefs.can_resign = m_CanResign;
   prefs.position_learning = m_PositionLearning;
   prefs.cores = m_Cores;
   prefs.strength = m_Strength;
   prefs.tbPath = m_TbPath;
   prefs.bookVariety = m_BookVariety;
   guiOptions->setGeneralPreferences(prefs);
}


/////////////////////////////////////////////////////////////////////////////
// PreferenceDialog message handlers

void PreferenceDialog::OnAutoSize()
{
   UpdateData();
   if (m_AutoSizeHashTable) {
      int val = calc_hash_size();
      char valStr[20];
      sprintf(valStr,"%d",val);
      m_HashSizeEdit.EnableWindow(FALSE);
      m_HashSizeEdit.SetWindowText(valStr);
   }
   else
      m_HashSizeEdit.EnableWindow(TRUE);
}


void PreferenceDialog::OnDeltaposHashSizeSpin(NMHDR* pNMHDR, LRESULT* pResult)
{
   NM_UPDOWN* pNMUpDown = (NM_UPDOWN*)pNMHDR;
   // TODO: Add your control notification handler code here

   *pResult = 0;
}


BOOL PreferenceDialog::OnInitDialog()
{
   CDialog::OnInitDialog();

   OnAutoSize();
   
   m_CoresSpin.SetRange32(1,Constants::MaxCPUs);

   m_TbPathEdit.SetLimitText(1000);

   return TRUE;                                   // return TRUE unless you set the focus to a control
   // EXCEPTION: OCX Property Pages should return FALSE
}


void PreferenceDialog::OnHelp()
{
   ::HtmlHelp(NULL, AfxGetApp()->m_pszHelpFilePath ,HH_HELP_CONTEXT, DIALOG_HELP_INDEX );
}
