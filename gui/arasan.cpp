// arasan.cpp: Application class for Arasan Windows GUI.
// Copyright 1994, 2001, 2002, 2024 by Jon Dart. All Rights Reserved.

#include "stdafx.h"
#include "arasan.h"
#include "globals.h"

#include "MainFrm.h"
#include "guiDoc.h"
#include "guiView.h"
#include "EngineCo.h"
#include "attacks.h"
#include "aboutdlg.h"

#include <cstring>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Global variables
GuiOptions *guiOptions;
char initialFilename[MAX_PATH];
ofstream traceFile;
bool doTrace;

/////////////////////////////////////////////////////////////////////////////
// ArasanGuiApp

BEGIN_MESSAGE_MAP(ArasanGuiApp, CWinApp)
    //{{AFX_MSG_MAP(ArasanGuiApp)
    ON_COMMAND(ID_ABOUTDIALOG, OnAppAbout)
    //}}AFX_MSG_MAP
    // Standard file based document commands
    ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
    ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
    // Standard print setup command
    ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

static ThreadComm *comm = NULL;
char *programPath;

static int terminated = 0;

/////////////////////////////////////////////////////////////////////////////
// ArasanGuiApp construction

ArasanGuiApp::ArasanGuiApp()
{
  // EnableHtmlHelp();
}


/////////////////////////////////////////////////////////////////////////////
// The one and only ArasanGuiApp object

ArasanGuiApp theApp;

/////////////////////////////////////////////////////////////////////////////
// ArasanGuiApp initialization

BOOL ArasanGuiApp::InitInstance()
{
    // must do this first, many things depend on it:
    Bitboard::init();
    Board::init();
    // set up attack bitmaps, etc.
    Attacks::init();

    // Set the registry key.  This makes MFC store option data in
    // the Registry rather than in an .INI file.  Normally this
    // key is a company name - we don't have one, we use "Arasan".
    SetRegistryKey(_T("Arasan"));

    LoadStdProfileSettings();                     // Load standard INI file options (including MRU)

    // load program-specific options
    guiOptions = new GuiOptions(this);

    // First free the string allocated by MFC at CWinApp startup.
    free((void*)m_pszHelpFilePath);

    char moduleName[MAX_PATH];
    GetModuleFileName(AfxGetInstanceHandle(),moduleName,MAX_PATH-1);

    // Assume help file is where arasan.exe is located.
    // Change the name of the help file (default is .hlp, we use
    // the somewhat newer .chm format now).
    m_pszHelpFilePath = _tcsdup(globals::derivePath("arasan.chm").c_str());

    // Register the application's document templates.  Document templates
    //  serve as the connection between documents, frame windows and views.

    pDocTemplate = new CSingleDocTemplate(
        IDR_MAINFRAME,
        RUNTIME_CLASS(ArasanGuiDoc),
        RUNTIME_CLASS(ArasanMainFrame),           // main SDI frame window
        RUNTIME_CLASS(ArasanGuiView));
    AddDocTemplate(pDocTemplate);

    // Parse command line for standard shell commands, DDE, file open
    CCommandLineInfo cmdInfo;
    ParseCommandLine(cmdInfo);

    // command-line processing.
    std::string cmdLine(m_lpCmdLine);
    doTrace = cmdLine.find("-trace") != string::npos;
    if (doTrace) {
        traceFile.open(globals::derivePath("trace.log"),ios::out|ios::trunc);
    }

    // Set up a separate thread to handle communication with the
    // chess engine.
    comm = new ThreadComm;
    comm->app = this;
    comm->moduleName = _strdup(moduleName);
    m_searchThread = AfxBeginThread(EngineThreadProc,comm);

    // Wait for the engine to start up.
    while (!engineReady()) WaitForSingleObject(m_searchThread,2000);

    // Send the init string to the engine:
    init();

    // The one and only window has been initialized, so show and update it.
    //m_pMainWnd->ShowWindow(SW_SHOW);
    //m_pMainWnd->UpdateWindow();
    OnFileNew();

    theView = getView();

    // send time control string to engine:
    Search_Limit_Options options = guiOptions->get_src_limits();
    ((ArasanGuiView*)theView)->setTimeControl(options);

    // Dispatch commands specified on the command line
    if (!ProcessShellCommand(cmdInfo))
        return FALSE;
    return TRUE;
}


ArasanGuiView* ArasanGuiApp::getView() const
{
    POSITION pos = pDocTemplate->GetFirstDocPosition();
    if (!pos) return NULL;
    CDocument* firstDoc = pDocTemplate->GetNextDoc(pos);
    if (!firstDoc) return NULL;
    pos = firstDoc->GetFirstViewPosition();
    if (!pos) return NULL;
    return (ArasanGuiView*)firstDoc->GetNextView(pos);
}

ArasanGuiDoc* ArasanGuiApp::getDoc() const
{
    POSITION pos = pDocTemplate->GetFirstDocPosition();
    if (!pos) return NULL;
    return (ArasanGuiDoc*)pDocTemplate->GetNextDoc(pos);
}

void ArasanGuiApp::init()
{
    // The engine communication thread calls back to here
    // once the link is set up.
    writeToEngine("xboard\nprotover 2");
    writeToEngine("post");
}


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
    public:
        CAboutDlg();

    // Dialog Data
    //{{AFX_DATA(CAboutDlg)
        enum { IDD = IDD_ABOUTBOX };
    //}}AFX_DATA

    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CAboutDlg)
    protected:

    // DDX/DDV support
    virtual void DoDataExchange(CDataExchange* pDX);
    //}}AFX_VIRTUAL

    // Implementation
    protected:
    //{{AFX_MSG(CAboutDlg)
    // No message handlers
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
    //{{AFX_DATA_INIT(CAboutDlg)
    //}}AFX_DATA_INIT
}


void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAboutDlg)
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
    //{{AFX_MSG_MAP(CAboutDlg)
    // No message handlers
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()

// App command to run the dialog
void ArasanGuiApp::OnAppAbout()
{
    CAboutDialog aboutDlg;
    aboutDlg.DoModal();
}


/////////////////////////////////////////////////////////////////////////////
// ArasanGuiApp commands

int ArasanGuiApp::ExitInstance()
{
    Bitboard::cleanup();
    terminated++;
    return CWinApp::ExitInstance();
}
