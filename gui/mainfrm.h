// MainFrm.h : interface of the ArasanMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__9F335348_3F30_11D5_9727_00B0D0A186AB__INCLUDED_)
#define AFX_MAINFRM_H__9F335348_3F30_11D5_9727_00B0D0A186AB__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif                                            // _MSC_VER >= 1000

class ArasanMainFrame : public CFrameWnd
{

   protected:                                     // create from serialization only
      ArasanMainFrame();
      DECLARE_DYNCREATE(ArasanMainFrame)

      // Attributes
      public:

      enum SearchStatus {Ready, Searching};

      // Operations
   public:
      void update(int ply, long nodes, int score,
         LPCSTR predicted);

      void update(int nodes);

      void clearStatusBar();

      void invalidateStatusBar();

      void setStatus(SearchStatus status);

      SearchStatus getStatus() const
      {
         return my_status;
      }

      // Overrides
      // ClassWizard generated virtual function overrides
      //{{AFX_VIRTUAL(ArasanMainFrame)
   public:
      virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
      virtual BOOL DestroyWindow();
      virtual void ActivateFrame(int nCmdShow = -1);
      virtual void WinHelp (DWORD dwData, UINT nCmd=HELP_CONTEXT);
      //}}AFX_VIRTUAL

      // Implementation
   public:
      virtual ~ArasanMainFrame();
#ifdef _DEBUG
      virtual void AssertValid() const;
      virtual void Dump(CDumpContext& dc) const;
#endif

   protected:                                     // control bar embedded members
      CStatusBar  m_wndStatusBar, m_pvStatusBar;

      CToolBar    m_wndToolBar;

      // status bar data
      long mNodes;
      int mDepth;
      CString mScore;
      CString mPv;
      SearchStatus my_status;
      BOOL firstActivation;

      // Generated message map functions
   protected:

      //{{AFX_MSG(ArasanMainFrame)
      afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
      afx_msg void OnFileNew();
      afx_msg void OnFileOpen();
      afx_msg void OnUpdateNodes(CCmdUI *pCmdUI);
      afx_msg void OnUpdateDepth(CCmdUI *pCmdUI);
      afx_msg void OnUpdateScore(CCmdUI *pCmdUI);
      afx_msg void OnUpdatePv(CCmdUI *pCmdUI);
      afx_msg void OnClose();
      afx_msg void OnSizing(UINT fwSide, LPRECT pRect);
      //}}AFX_MSG
      DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.
#endif                                            // !defined(AFX_MAINFRM_H__9F335348_3F30_11D5_9727_00B0D0A186AB__INCLUDED_)
