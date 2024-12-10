// Copyright 1994-2024 by Jon Dart. All Rights Reserved.
// arasan.h : main header file for the GUI application
//
#include "afxpriv.h"                              // for CRecentFileList

#if !defined(AFX_GUI_H__9F335344_3F30_11D5_9727_00B0D0A186AB__INCLUDED_)
#define AFX_GUI_H__9F335344_3F30_11D5_9727_00B0D0A186AB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif                                            // _MSC_VER >= 1000

#ifndef __AFXWIN_H__
#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"                             // main symbols
#include "guiopts.h"
#include <fstream>

/////////////////////////////////////////////////////////////////////////////
// ArasanGuiApp:
// See arasan.cpp for the implementation of this class
//
class ArasanGuiView;
class ArasanGuiDoc;

extern ofstream traceFile;
extern bool doTrace;

class ArasanGuiApp : public CWinApp
{
   public:
      ArasanGuiApp();

      void setEngineHandle(HANDLE engine) {
         m_engineProcess = engine;
      }

      ArasanGuiView* ArasanGuiApp::getView() const;

      ArasanGuiDoc* ArasanGuiApp::getDoc() const;

      CString &getRecentFile(int index) const
      {
         return (*m_pRecentFileList)[index];
      }

      void init();

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(ArasanGuiApp)
   public:
      virtual BOOL InitInstance();
      virtual int ExitInstance();
      //}}AFX_VIRTUAL

      // Implementation

      //{{AFX_MSG(ArasanGuiApp)
      afx_msg void OnAppAbout();
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()

         CWinThread *m_searchThread;
      HANDLE m_engineProcess;
      CWnd *theView;
      CSingleDocTemplate* pDocTemplate;

};

extern GuiOptions *guiOptions;
extern char initialFilename[MAX_PATH];

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif                                            // !defined(AFX_GUI_H__9F335344_3F30_11D5_9727_00B0D0A186AB__INCLUDED_)
