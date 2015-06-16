// MainFrm.h : interface of the CMainFrame class
//


#pragma once

#include "MyEdit.h"
#include "MyListCtrl.h"
#include "SettingsBar.h"
#include "SubchannelCfg.h"
#include "ClientManager.h"
#include "ExtChildResizablePropertySheet.h"
#include "LoginCfg.h"
#include "MiscCfg.h"

class CroboServView;

typedef CArray<CClientManager*, CClientManager*> CRoomArray;

class CMainFrame : 
	public CFrameWnd,
	virtual public CClientManager
{
	
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

protected:
	enum styles{ WIN2K3=0, WINXP, WIN2K, WIN2K5 };
	int m_nStyle;
	
	CString m_strRoomSuffix;

// Attributes
public:
	CMenu	  m_cmSystray;
	CMySocket m_mServer;
	virtual void WriteText(COLORREF crText, UINT nID, ...);
	void WriteText(CString strText, COLORREF crText = RGB(0,0,128), BOOL bBold = TRUE, BOOL bAppendNewLine = TRUE);
	void PrintCmdNotUnderstood(const CString strCmd = "");
	void PrintHelp(void);
	void LoadToTray(HWND hWnd, UINT uCallbackMessage, CString strInfoTitle, CString strInfo, CString strTip, int uTimeout, HICON icon, DWORD dwIcon);
	void EchoChat(CString strName, CString strMsg);
	void HandleCommand(CString strCmd);
	void SetRoomName(CString strName, CString strIP, CString strPort);
	static UINT ServerThread(LPVOID pParam);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	virtual BOOL DestroyWindow();
	virtual void ActivateFrame(int nCmdShow = -1);
	//}}AFX_VIRTUAL

private:
	// window placement persistence
	WINDOWPLACEMENT m_dataFrameWP;
// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

public: 
	CExtMenuControlBar    m_wndMenuBar;

protected:  // control bar embedded members


	NOTIFYICONDATA m_nIconData;
	CExtStatusControlBar  m_wndStatusBar;
	CExtToolControlBar    m_wndToolBar;
	CExtToolControlBar    m_wndToolBar2;
	CExtToolControlBar	  m_wndHostBar;
	CExtToolControlBar	  m_wndLangBar;
	CExtControlBar    m_wndChatBar;
	CExtControlBar    m_wndUserBar;
	CExtControlBar    m_wndSettingsBar;
	
	CExtChildResizablePropertySheet m_wndDockedPS;
	CSettingsBar	  m_wndSettings;
	CSubChannelCfg	  m_wndSubSettings;
	CLoginCfg		  m_wndLogin;
	CMiscCfg		  m_wndMisc;
	CExtWRB<CMyEdit> m_wndInBarEdit;
	CMyListCtrl m_wndInBarListBox;

	CExtComboBox m_wndNameBox;
	CExtComboBox m_wndIP;
	CExtComboBox m_wndPort;
	CExtComboBox m_wndLang;

	CroboServView* m_pView;

	CRoomArray  m_aRooms;

// Generated message map functions
protected:
	void SaveBans(CString strFile);
	// Generated message map functions
	//{{AFX_MSG(CMainFrame)
	afx_msg LRESULT OnWriteTextSub(WPARAM w, LPARAM l);
	afx_msg void OnSystrayRestore();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg LRESULT OnTrayNotify(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT ClientCallback(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnInput(WPARAM wParam, LPARAM lParam);
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSetFocus(CWnd *pOldWnd);
	afx_msg void OnViewLikeOffice2k();
	afx_msg void OnUpdateViewLikeOffice2k(CCmdUI* pCmdUI);
	afx_msg void OnViewLikeOfficeXp();
	afx_msg void OnUpdateViewLikeOfficeXp(CCmdUI* pCmdUI);
	afx_msg void OnViewLikeOffice2003();
	afx_msg void OnUpdateViewLikeOffice2003(CCmdUI* pCmdUI);
	afx_msg void OnViewLikeVS2005();
	afx_msg void OnUpdateViewLikeVS2005(CCmdUI* pCmdUI);
	afx_msg void OnUpdateControlBarMenu(CCmdUI* pCmdUI);
	afx_msg BOOL OnBarCheck(UINT nID);
	afx_msg void OnClose();
	afx_msg LRESULT SetView(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnLoadSettings(WPARAM wParam, LPARAM lParam);
	afx_msg void OnFileOpen();
	afx_msg void OnFileSaveConfig();
	afx_msg void OnUserlistAddadmin32773();
	afx_msg void OnUserlistRemoveadmin32774();
	afx_msg void OnUserlistVoice();
	afx_msg void OnUserlistDevoice();
	afx_msg void OnUserlistAddagent();
	afx_msg void OnUserlistRemoveagent();
	afx_msg void OnUserlistCopyusername32781();
	afx_msg void OnUserlistCopyip();
	afx_msg void OnUserlistHide();
	afx_msg void OnUserlistShow();
	afx_msg void OnUserlistStats();
	afx_msg void OnUserlistModes();
	afx_msg void OnRoomnameBox();
	afx_msg void OnUpdateRoomnameBox(CCmdUI* pCmdUI);
	afx_msg void OnRoomStart();
	afx_msg void OnRoomStop();
	afx_msg void OnUpdateRoomStop(CCmdUI* pCmdUI);
	afx_msg void OnHelpCommands();
	afx_msg void OnHelpHomepage();
	afx_msg void OnSwitchLang();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnUserlistRemoveghost();
	afx_msg void OnUserlistKick();
	afx_msg void OnUserlistKickban();
	afx_msg void OnUserlistBan();
	afx_msg void OnFileCopychannelname();
	afx_msg void OnFileCopyloopbackname();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	LRESULT OnCreateSubchannel(WPARAM wParam, LPARAM lPAram);
	void LoadBans(CString strFile);
	afx_msg LRESULT OnCloseSubChannel(WPARAM wParam, LPARAM lParam);
	
	virtual void SetTopic(CString strTopic);
	virtual void SetRoom(const CString strName);

	void DetectExternalIP(CString &rIP);
	BOOL ExecuteChannelCommand(const CString strUser, const CString strMsg, const UINT uMode);
	virtual BOOL SendRename(CClientSocket* pClient, const CString strOldName, DWORD dwOldIP, WORD wOldPort, const CString strNewName, DWORD dwNewIP, WORD wNewPort, WORD wLine, DWORD dwFiles, WORD wUserLevel);
	virtual void SendPart(const CString strUser, DWORD dwIP, WORD wPort, BOOL bRealPart);
	virtual void SendJoin(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel, DWORD dwRealIP);
	static UINT Loginthread(LPVOID pParam);
	void Stop(BOOL bRedirect = FALSE, CString strTarget = "");
	void UpdateModeUserlist(CString strUser, UINT uMode);
	static CString BuiltModeString(UINT uMode);
	void List(BOOL bStart);

};


