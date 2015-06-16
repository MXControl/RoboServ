// MainFrm.cpp : implementation of the CMainFrame class
//

#include "stdafx.h"
#include "roboServ.h"

#include "MainFrm.h"

#include ".\clientsocket.h"
#include ".\tokenizer.h"
#include ".\settings.h"
#include "Util.h"
#include ".\mainfrm.h"
#include ".\ini.h"
//#include ".\wpnlist.h"
#include ".\clipboard.h"
#include "roboServDoc.h"
#include "roboServView.h"
#include "SubChannel.h"
#include "OpenConfigDlg.h"
#include "SimpleSplashWnd.h"
#include "update.h"
#include "RoomLister.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define TIMER_CHECK_TRAY_ICON 100000
#define TIMER_UPDATE_STATUS   100001

extern UINT UWM_INCOMMING; // = ::RegisterWindowMessage("UWM_INCOMMING-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");
extern UINT UWM_CLNNOTIFY; // = ::RegisterWindowMessage("UWM_CLNNOTIFY-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");
extern UINT UWM_WRITETEXT;

UINT UWM_SETVIEW   = ::RegisterWindowMessage("UWM_SETVIEW-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");
UINT UWM_SETTINGS  = ::RegisterWindowMessage("UWM_SETTINGS-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");
UINT UWM_CREATESUB = ::RegisterWindowMessage("UWM_CREATESUB-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");

#define WM_TRAY_ICON_NOTIFY_MESSAGE (WM_USER + 1)

CRoomLister g_rLister;

extern UINT UWM_INPUT;
extern UINT UWM_CLOSEME;

extern CSettings g_sSettings;

/*extern CString ChannelName;
extern CString ChannelName2;
extern WORD    PORT;
extern CString Topic;
extern DWORD   Users;
extern DWORD   Limit;
extern volatile BOOL ShutItDown;
extern volatile UINT nNumPrimaries;*/
// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_REGISTERED_MESSAGE(UWM_WRITETEXT, OnWriteTextSub)
	ON_COMMAND(ID_SYSTRAY_RESTORE, OnSystrayRestore)
	ON_WM_SYSCOMMAND()
	ON_MESSAGE(WM_TRAY_ICON_NOTIFY_MESSAGE,OnTrayNotify)
	ON_REGISTERED_MESSAGE(UWM_CLNNOTIFY, ClientCallback)
	ON_REGISTERED_MESSAGE(UWM_INPUT, OnInput)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(ID_VIEW_LIKE_OFFICE_2K, OnViewLikeOffice2k)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIKE_OFFICE_2K, OnUpdateViewLikeOffice2k)
	ON_COMMAND(ID_VIEW_LIKE_OFFICE_XP, OnViewLikeOfficeXp)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIKE_OFFICE_XP, OnUpdateViewLikeOfficeXp)
	ON_COMMAND(ID_VIEW_LIKE_OFFICE_2003, OnViewLikeOffice2003)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIKE_OFFICE_2003, OnUpdateViewLikeOffice2003)
	ON_COMMAND(ID_VIEW_LIKE_STUDIO_2005, OnViewLikeVS2005)
	ON_UPDATE_COMMAND_UI(ID_VIEW_LIKE_STUDIO_2005, OnUpdateViewLikeVS2005)
	ON_UPDATE_COMMAND_UI(ID_VIEW_MENUBAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_MENUBAR, OnBarCheck)
	ON_WM_CLOSE()
	ON_REGISTERED_MESSAGE(UWM_SETVIEW, SetView)
	ON_REGISTERED_MESSAGE(UWM_SETTINGS, OnLoadSettings)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	ON_COMMAND(ID_FILE_SAVECFG, OnFileSaveConfig)
	ON_COMMAND(ID_USERLIST_ADDADMIN, OnUserlistAddadmin32773)
	ON_COMMAND(ID_USERLIST_REMOVEADMIN, OnUserlistRemoveadmin32774)
	ON_COMMAND(ID_USERLIST_GIVEVOICE, OnUserlistVoice)
	ON_COMMAND(ID_USERLIST_REMOVEVOICE, OnUserlistDevoice)
	ON_COMMAND(ID_USERLIST_ADDAGENT, OnUserlistAddagent)
	ON_COMMAND(ID_USERLIST_REMOVEAGENT, OnUserlistRemoveagent)
	ON_COMMAND(ID_USERLIST_COPYUSERNAME, OnUserlistCopyusername32781)
	ON_COMMAND(ID_USERLIST_COPYIP, OnUserlistCopyip)
	ON_COMMAND(ID_USERLIST_HIDE, OnUserlistHide)
	ON_COMMAND(ID_USERLIST_SHOW, OnUserlistShow)
	ON_COMMAND(ID_USERLIST_STATISTICS, OnUserlistStats)
	ON_COMMAND(ID_USERLIST_USERMODES, OnUserlistModes)
	ON_COMMAND(ID_ROOMNAMEBOX, OnRoomnameBox)
	ON_UPDATE_COMMAND_UI(ID_ROOMIP, OnUpdateRoomnameBox)
	ON_COMMAND(ID_HOSTCHANNEL, OnRoomStart)
	ON_COMMAND(ID_STOPCHANNEL, OnRoomStop)
	ON_UPDATE_COMMAND_UI(ID_STOPCHANNEL, OnUpdateRoomStop)
	ON_COMMAND(ID_HELP_COMMANDHELP, OnHelpCommands)
	ON_COMMAND(ID_HELP_VISITHOMEPAGE, OnHelpHomepage)
	ON_CBN_SELCHANGE(ID_LANGSWITCHER, OnSwitchLang)
	ON_WM_TIMER()
	ON_COMMAND(ID_USERLIST_REMOVEGHOST, OnUserlistRemoveghost)
	ON_COMMAND(ID_USERLIST_KICK, OnUserlistKick)
	ON_COMMAND(ID_USERLIST_KICKBAN, OnUserlistKickban)
	ON_COMMAND(ID_USERLIST_BAN, OnUserlistBan)
	ON_COMMAND(ID_FILE_COPYCHANNELNAME, OnFileCopychannelname)
	ON_REGISTERED_MESSAGE(UWM_CREATESUB, OnCreateSubchannel)
	ON_REGISTERED_MESSAGE(UWM_CLOSEME, OnCloseSubChannel)
	ON_COMMAND_EX(IDR_TOOLBAR2, OnBarCheck)
	ON_UPDATE_COMMAND_UI(IDR_TOOLBAR2, OnUpdateControlBarMenu)
	ON_COMMAND_EX(IDR_HOSTBAR, OnBarCheck)
	ON_UPDATE_COMMAND_UI(IDR_HOSTBAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX(IDR_LANGBAR, OnBarCheck)
	ON_UPDATE_COMMAND_UI(IDR_LANGBAR, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_RESIZABLE_BAR_0, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESIZABLE_BAR_0, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_RESIZABLE_BAR_1, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESIZABLE_BAR_1, OnUpdateControlBarMenu)
	ON_COMMAND_EX(ID_VIEW_RESIZABLE_BAR_2, OnBarCheck)
	ON_UPDATE_COMMAND_UI(ID_VIEW_RESIZABLE_BAR_2, OnUpdateControlBarMenu)
	ON_COMMAND(ID_ROOMIP, OnRoomnameBox)
	ON_UPDATE_COMMAND_UI(ID_ROOMNAMEBOX, OnUpdateRoomnameBox)
	ON_COMMAND(ID_ROOMPORT, OnRoomnameBox)
	ON_UPDATE_COMMAND_UI(ID_ROOMPORT, OnUpdateRoomnameBox)
	ON_UPDATE_COMMAND_UI(ID_HOSTCHANNEL, OnUpdateRoomnameBox)
	ON_COMMAND(ID_LANGSWITCHER, OnRoomnameBox)
	ON_UPDATE_COMMAND_UI(ID_LANGSWITCHER, OnUpdateRoomnameBox)
	ON_COMMAND(ID_FILE_COPYLOOPBACKNAME, OnFileCopyloopbackname)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_SEPARATOR,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL,
};



static UINT g_statBasicCommands[] =
{
	ID_APP_ABOUT,
	ID_APP_EXIT,
	ID_VIEW_MENUBAR,
	ID_VIEW_TOOLBAR,
	IDR_TOOLBAR2,
	IDR_HOSTBAR,
	ID_VIEW_RESIZABLE_BAR_0,
	ID_VIEW_RESIZABLE_BAR_1,
	ID_VIEW_RESIZABLE_BAR_2,
	IDR_LANGBAR,
	ID_HELP_VISITHOMEPAGE,
	ID_HELP_COMMANDHELP,
	ID_USERLIST_ADDADMIN,
	ID_USERLIST_REMOVEADMIN,
	ID_USERLIST_COPYUSERNAME,
	ID_USERLIST_COPYIP,
	0, // end of list
};

CMainFrame::CMainFrame() :
	CClientManager(m_hWnd)
{

	//{{AFX_DATA_INIT(CMainFrame)
	//}}AFX_DATA_INIT
	::memset(&m_dataFrameWP, 0, sizeof(WINDOWPLACEMENT));
	m_dataFrameWP.length = sizeof(WINDOWPLACEMENT);
	m_dataFrameWP.showCmd = SW_HIDE;

	m_pServerThread = NULL;
	m_pView			 = NULL;
	m_nStyle		 = WIN2K5;
	m_uMode			 = CM_NORMAL;
	CExtControlBar::g_eResizablePanelDockingType =
		CExtControlBar::__RESIZABLE_DOCKING_TYPE_STUDIO_2005;
}

CMainFrame::~CMainFrame()
{

}

void CMainFrame::OnUpdateControlBarMenu(CCmdUI* pCmdUI)
{
	CExtControlBar::DoFrameBarCheckUpdate(this, pCmdUI, true);
}

BOOL CMainFrame::OnBarCheck(UINT nID)
{
	return CExtControlBar::DoFrameBarCheckCmd(this, nID, true);
}

#define _BETA
//#define _RELEASE_TEST

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{

#ifdef _BETA
#ifndef _DEBUG
	AfxMessageBox("This is a beta version of roboServe.\nUse at own risk. This copy will stop working when the beta test is finished.", MB_ICONINFORMATION);
#endif
#endif

	CWaitCursor wc;

	CSimpleSplashWnd splash( this, IDB_ROBOSERV );

	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	TRACE("%s\n", __DATE__);


#ifndef _DEBUG
#ifndef _RELEASE_TEST
	splash.SetStatusText(IDS_CHECK_UPDATE);
	
	CString strTempFile = g_sSettings.GetWorkingDir()+ "\\update.ini";

	if(Util::LoadFromServer("http://mxcontrol.sf.net/roboServ/update.ini", 	strTempFile)){

		splash.DestroyWindow();

		CString strMyStamp;
		strMyStamp.Format("%s", __DATE__);
		CIni ini;
		ini.SetIniFileName(strTempFile);
		int nBounceOut = IDCANCEL;
		if(ini.GetValue("roboServ", "TimeStamp", "") != strMyStamp){

			CUpdate update;
			update.m_strUpdateURL = ini.GetValue("roboServ", "UpdateURL", "http://mxcontrol.sf.net/roboServ.exe");
			update.m_nSize		  = ini.GetValue("roboServ", "UpdateSize", 0);
			nBounceOut = update.DoModal();
		}
		CFile::Remove(strTempFile);

		if(nBounceOut == IDOK){
			
			CString strCmd;
			strCmd.Format("\"%s\\update.bat\"", g_sSettings.GetWorkingDir());
			ShellExecute(0, "open", strCmd, 0, 0, SW_HIDE);
			exit(0); // well this is unsafe but i am too lazy :P
			return -1;
		}
	}
#endif
#endif

	splash.SetStatusText(IDS_INIT_1);
	
	CWinApp * pApp = ::AfxGetApp();
	ASSERT(pApp != NULL);
	ASSERT(pApp->m_pszRegistryKey != NULL);
	ASSERT(pApp->m_pszRegistryKey[0] != _T('\0'));
	ASSERT(pApp->m_pszProfileName != NULL);
	ASSERT(pApp->m_pszProfileName[0] != _T('\0'));
	ASSERT(pApp->m_pszProfileName != NULL);

	HICON hIcon = pApp->LoadIcon(IDR_MAINFRAME);
	ASSERT(hIcon != NULL);
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);
	
	m_cmSystray.LoadMenu(IDR_SYSTRAY);
	LoadToTray(
		m_hWnd,
		WM_TRAY_ICON_NOTIFY_MESSAGE, 
		pApp->m_pszAppName, 
		"",
		pApp->m_pszAppName, 
		1, 
		hIcon,
		0
	); 

	g_CmdManager->ProfileSetup(
		pApp->m_pszProfileName,
		GetSafeHwnd()
		);

	VERIFY(
		g_CmdManager->UpdateFromMenu(
			pApp->m_pszProfileName,
			IDR_MAINFRAME
			)
		);

	splash.SetStatusText(IDS_INIT_2);

	if(!m_wndMenuBar.Create(
			NULL, // _T("Menu Bar"),
			this,
			ID_VIEW_MENUBAR
			)
		)
    {
        TRACE0("Failed to create menubar\n");
        return -1;      // failed to create
    }

	if(!m_wndToolBar.Create(
			Util::LoadString(IDS_TOOLBAR),
			this,
			AFX_IDW_TOOLBAR
			)
		||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME)
		)
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if(!m_wndToolBar2.Create(
		Util::LoadString(IDS_VIS),
			this,
			IDR_TOOLBAR2
			)
		||
		!m_wndToolBar2.LoadToolBar(IDR_TOOLBAR2)
		)
	{
		TRACE0("Failed to create toolbar2\n");
		return -1;      // fail to create
	}

	if(!m_wndHostBar.Create(
		Util::LoadString(IDS_HOSTBAR),
			this,
			IDR_HOSTBAR
			)
		||
		!m_wndHostBar.LoadToolBar(IDR_HOSTBAR)
		)
	{
		TRACE0("Failed to create toolbar2\n");
		return -1;      // fail to create
	}


	if(!m_wndNameBox.Create(
            WS_CHILD|WS_VISIBLE|CBS_HASSTRINGS|CBS_DROPDOWN|CBS_SORT,
            CRect(0,0,180,200),
            &m_wndHostBar,
            ID_ROOMNAMEBOX))
    {
        TRACE0(_T("Failed to create Room COMBO\n"));
        return -1;      // fail to create
    }

    m_wndHostBar.SetButtonCtrl(
        m_wndHostBar.CommandToIndex(ID_ROOMNAMEBOX),
        &m_wndNameBox
	);

    m_wndNameBox.SetFont( 
        CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));
    m_wndNameBox.SetItemHeight(
        -1, m_wndNameBox.GetItemHeight(-1) - 1);
    g_CmdManager->CmdGetPtr(pApp->m_pszProfileName, 
		m_wndNameBox.GetDlgCtrlID())->m_sMenuText = Util::LoadString(IDS_ROOMNAME); 

	m_wndNameBox.AddString(g_sSettings.GetRoomname());
	m_wndNameBox.SetCurSel(0);

	if(!m_wndIP.Create(
            WS_CHILD|WS_VISIBLE|CBS_HASSTRINGS|CBS_DROPDOWN|CBS_SORT,
            CRect(0,0,180,200),
            &m_wndHostBar,
            ID_ROOMIP))
    {
        TRACE0(_T("Failed to create IP COMBO\n"));
        return -1;      // fail to create
    }

    m_wndHostBar.SetButtonCtrl(
        m_wndHostBar.CommandToIndex(ID_ROOMIP),
        &m_wndIP
   );

    m_wndIP.SetFont(
		CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));

    m_wndIP.SetItemHeight(-1, m_wndIP.GetItemHeight(-1) - 1);

    g_CmdManager->CmdGetPtr(pApp->m_pszProfileName, 
		m_wndIP.GetDlgCtrlID())->m_sMenuText = Util::LoadString(IDS_ROOMIP);


	splash.SetStatusText(IDS_INIT_3);

	m_wndIP.AddString("127.0.0.1");
	m_wndIP.AddString("127.0.0.2");

	CString strIP;
	DetectExternalIP(strIP);
	if(strIP.GetLength()){
		
		m_wndIP.AddString(strIP);
		int nSel = m_wndIP.FindStringExact(-1, strIP);
		if(nSel >= 0) m_wndIP.SetCurSel(nSel);
	}
	else{
		
		m_wndIP.SetCurSel(0);
	}
	
	if(!m_wndPort.Create(
            WS_CHILD|WS_VISIBLE|CBS_HASSTRINGS|CBS_DROPDOWN|CBS_SORT,
            CRect(0,0,180,200),
            &m_wndHostBar,
            ID_ROOMPORT))
    {
        TRACE0(_T("Failed to create  Port COMBO\n"));
        return -1;      // fail to create
    }

	splash.SetStatusText(IDS_INIT_4);

    m_wndHostBar.SetButtonCtrl(
        m_wndHostBar.CommandToIndex(ID_ROOMPORT),
        &m_wndPort
   );

    m_wndPort.SetFont(
		CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));

    m_wndPort.SetItemHeight(-1, m_wndPort.GetItemHeight(-1) - 1);

    g_CmdManager->CmdGetPtr(pApp->m_pszProfileName, 
		m_wndPort.GetDlgCtrlID())->m_sMenuText = Util::LoadString(IDS_ROOMPORT); 
   

	m_wndPort.AddString("6698");
	m_wndPort.AddString("16699");
	m_wndPort.AddString("26699");
	m_wndPort.SetCurSel(0);


	if(!m_wndLangBar.Create(
			_T("Lanugagebar"),
			this,
			IDR_LANGBAR
			)
		||
		!m_wndLangBar.LoadToolBar(IDR_LANGBAR)
		)
	{
		TRACE0("Failed to create langbar\n");
		return -1;      // fail to create
	}


	if(!m_wndLang.Create(
            WS_CHILD|WS_VISIBLE|CBS_HASSTRINGS|CBS_DROPDOWNLIST|CBS_SORT,
            CRect(0,0,180,200),
            &m_wndLangBar,
            ID_LANGSWITCHER))
    {
        TRACE0(_T("Failed to create Lang COMBO\n"));
        return -1;      // fail to create
    }

	m_wndLangBar.SetButtonCtrl(
        m_wndLangBar.CommandToIndex(ID_LANGSWITCHER),
        &m_wndLang
	);

    m_wndLang.SetFont( 
        CFont::FromHandle((HFONT)::GetStockObject(DEFAULT_GUI_FONT)));
    m_wndLang.SetItemHeight(
        -1, m_wndLang.GetItemHeight(-1) - 1);
    g_CmdManager->CmdGetPtr(pApp->m_pszProfileName, 
		m_wndLang.GetDlgCtrlID())->m_sMenuText = Util::LoadString(IDS_LANGUAGE); 

	m_wndLang.AddString("English");
	
	CFileFind finder;
	BOOL bFound = finder.FindFile(g_sSettings.GetWorkingDir() + "\\lang_*.dll");
	CString strTmp;
	while(bFound){

		bFound = finder.FindNextFile();
		strTmp = finder.GetFileTitle();
		strTmp = strTmp.Mid(5);
		m_wndLang.AddString(strTmp);
	}

	CIni ini;
	ini.SetIniFileName(g_sSettings.GetWorkingDir() + "\\roboServ.ini");
	strTmp = ini.GetValue("Language", "Active", "English");
	int n = m_wndLang.FindStringExact(-1, strTmp);
	if(n < 0){

		n = 0;
	}
	m_wndLang.SetCurSel(n);

	splash.SetStatusText(IDS_INIT_5);
	
	CExtCmdItem * pCmdItem;

	pCmdItem =
		g_CmdManager->CmdGetPtr(
			pApp->m_pszProfileName,
			ID_VIEW_LIKE_OFFICE_2K
			);
	ASSERT(pCmdItem != NULL);
	pCmdItem->m_sToolbarText = _T("Win2K Look");

	pCmdItem =
		g_CmdManager->CmdGetPtr(
			pApp->m_pszProfileName,
			ID_VIEW_LIKE_OFFICE_XP
			);
	ASSERT(pCmdItem != NULL);
	pCmdItem->m_sToolbarText = _T("XP Look");

	pCmdItem =
		g_CmdManager->CmdGetPtr(
			pApp->m_pszProfileName,
			ID_VIEW_LIKE_OFFICE_2003
			);
	ASSERT(pCmdItem != NULL);
	pCmdItem->m_sToolbarText = _T("2003 Look");

	pCmdItem =
		g_CmdManager->CmdGetPtr(
			pApp->m_pszProfileName,
			ID_VIEW_LIKE_STUDIO_2005
			);
	ASSERT(pCmdItem != NULL);
	pCmdItem->m_sToolbarText = _T("VS 2005 Look");

	if(	!m_wndChatBar.Create(
			Util::LoadString(IDS_COMMANDBAR),
			this,
			ID_VIEW_RESIZABLE_BAR_0
			)
		)
	{
		TRACE0("Failed to create m_wndChatBar\n");
		return -1;		// fail to create
	}
	if(!m_wndInBarEdit.Create(
			WS_CHILD|WS_VISIBLE|WS_VSCROLL|WS_HSCROLL|ES_LEFT,
			CRect(0,0,0,0),
			&m_wndChatBar,
			m_wndChatBar.GetDlgCtrlID()
			)
		)
	{
		TRACE0("Failed to create m_wndInBarEdit\n");
		return -1;		// fail to create
	}
	m_wndInBarEdit.SetFont(
		CFont::FromHandle(
			(HFONT)::GetStockObject(DEFAULT_GUI_FONT)
			)
		);

	splash.SetStatusText(IDS_INIT_6);

	m_aCommands.Add("/kick <user> [reason]");
	m_aCommands.Add("/kickban <user> [reason]");
	m_aCommands.Add("/ban <user>");
	m_aCommands.Add("/unban <user>");
	m_aCommands.Add("/exile  <user> [reason]");
	m_aCommands.Add("/exileto  <user> [target] [reason]");
	m_aCommands.Add("/listbans");
	m_aCommands.Add("/listhidden");
	m_aCommands.Add("/stat");
	m_aCommands.Add("/private <user>");
	m_aCommands.Add("/channelname");
	m_aCommands.Add("/channelmodes");
	m_aCommands.Add("/usermodes <user>");
	m_aCommands.Add("/hide <user>");
	m_aCommands.Add("/show <user>");
	m_aCommands.Add("/topic <topic>");
	m_aCommands.Add("/setmotd <motd>");
	m_aCommands.Add("/limit <limit>");
	m_aCommands.Add("/setpassword <password>");
	m_aCommands.Add("/rempassword <password>");
	m_aCommands.Add("/setmoderated");
	m_aCommands.Add("/remmoderated");
	m_aCommands.Add("/enableexile <targetroom>");
	m_aCommands.Add("/disableexile");
	m_aCommands.Add("/remagent <user>");
	m_aCommands.Add("/agent <user>");
	m_aCommands.Add("/remadmin <user>");
	m_aCommands.Add("/admin <user>");
	m_aCommands.Add("/muzzle <user>");
	m_aCommands.Add("/voice <user>");
	m_aCommands.Add("channelname <new roomname>");
	m_aCommands.Add("redirect <Targetroom>");
	m_aCommands.Add("stop");
	m_aCommands.Add("host <IP> <port> <roomname>");
	
	m_wndInBarEdit.SetCommands(&m_aCommands);

	if(	!m_wndUserBar.Create(
			Util::LoadString(IDS_USERLIST),
			this,
			ID_VIEW_RESIZABLE_BAR_1
			)
		)
	{
		TRACE0("Failed to create m_wndUserBar\n");
		return -1;		// fail to create
	}

	if(!m_wndInBarListBox.Create(
			WS_CHILD|WS_VISIBLE|WS_HSCROLL|WS_VSCROLL
				|LVS_REPORT|LVS_SHOWSELALWAYS|LVS_SINGLESEL|LVS_SORTASCENDING|LVS_NOSORTHEADER
				,
			CRect(0,0,200,200),
			&m_wndUserBar,
			m_wndUserBar.GetDlgCtrlID()
			)
		)
	{
		TRACE0("Failed to create m_wndInBarListBox\n");
		return -1;		// fail to create
	}
	m_wndInBarListBox.SetFont(
		CFont::FromHandle(
			(HFONT)::GetStockObject(DEFAULT_GUI_FONT)
			)
		);


	if(	!m_wndSettingsBar.Create(
			_T("Quick Config"),
			this,
			ID_VIEW_RESIZABLE_BAR_2
			)
		)
	{
		TRACE0("Failed to create m_wndSettingsBar\n");
		return -1;		// fail to create
	}

	m_wndDockedPS.m_bInitTabSingleLine		= true;
	m_wndDockedPS.m_bInitTabButtons			= true;
	m_wndDockedPS.m_bInitTabButtonsFlat		= true;
	m_wndDockedPS.m_bInitTabSeparatorsFlat	= true;
	m_wndDockedPS.AddPage(&m_wndSettings);
	m_wndDockedPS.AddPage(&m_wndSubSettings);
	m_wndDockedPS.AddPage(&m_wndLogin);
	m_wndDockedPS.AddPage(&m_wndMisc);

	if( !m_wndDockedPS.Create(&m_wndSettingsBar) )
	{
		TRACE0("Failed to create m_wndDockedPS window\n");
		return -1;
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}


	m_wndStatusBar.SetPaneInfo(1, ID_SEPARATOR, SBPS_NORMAL, 100);
	m_wndStatusBar.SetPaneInfo(2, ID_SEPARATOR, SBPS_NORMAL, 100);
	m_wndStatusBar.SetPaneInfo(3, ID_SEPARATOR, SBPS_NORMAL, 100);

    m_wndMenuBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndToolBar2.EnableDocking(CBRS_ALIGN_ANY);
	m_wndLangBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndHostBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndChatBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndUserBar.EnableDocking(CBRS_ALIGN_ANY);
	m_wndSettingsBar.EnableDocking(CBRS_ALIGN_ANY);
	
	if(!CExtControlBar::FrameEnableDocking(this))
	{
		ASSERT(FALSE);
		return -1;
	}
	
	g_CmdManager->SerializeState(
		pApp->m_pszProfileName,
		pApp->m_pszRegistryKey,
		pApp->m_pszProfileName,
		false
		);


	CSize size(320, 350);
	m_wndSettingsBar.SetInitDesiredSizeVertical(size);
	size.cx = 200;
	size.cx = 200;
	m_wndUserBar.SetInitDesiredSizeVertical(size);
	size.cy = 400;
	size.cy = 50;
	m_wndChatBar.SetInitDesiredSizeHorizontal(size);
	if(!CExtControlBar::ProfileBarStateLoad(
			this,
			pApp->m_pszRegistryKey,
			pApp->m_pszProfileName,
			pApp->m_pszProfileName,
			&m_dataFrameWP
			)
		)
	{
		DockControlBar(&m_wndMenuBar);
		DockControlBar(&m_wndToolBar);
		RecalcLayout();
		CRect wrAlredyDockedBar;
		m_wndToolBar.GetWindowRect(&wrAlredyDockedBar);
		wrAlredyDockedBar.OffsetRect(1, 0);
		DockControlBar(&m_wndToolBar2,AFX_IDW_DOCKBAR_TOP,&wrAlredyDockedBar);
		wrAlredyDockedBar.OffsetRect(1, 0);
		DockControlBar(&m_wndLangBar,AFX_IDW_DOCKBAR_TOP,&wrAlredyDockedBar);
		DockControlBar(&m_wndHostBar);
	
		m_wndSettingsBar.DockControlBar(AFX_IDW_DOCKBAR_LEFT, 1, this, false);
		m_wndUserBar.DockControlBar(AFX_IDW_DOCKBAR_TOP, 2, this, false);
		m_wndChatBar.DockControlBar(AFX_IDW_DOCKBAR_BOTTOM, 2, this, false);
		RecalcLayout();
	}


	m_wndInBarListBox.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_FLATSB);

	m_wndInBarListBox.InsertColumn(0, Util::LoadString(IDS_USERNAME), LVCFMT_LEFT, 200);
	m_wndInBarListBox.InsertColumn(1, Util::LoadString(IDS_USERMODE), LVCFMT_LEFT, 100);
	m_wndInBarListBox.InsertColumn(2, Util::LoadString(IDS_USERIP), LVCFMT_LEFT, 100);
	m_wndInBarListBox.InsertColumn(3, Util::LoadString(IDS_USERHOSTNAME), LVCFMT_LEFT, 100);
	m_wndInBarListBox.InsertColumn(4, Util::LoadString(IDS_USERCLIENT), LVCFMT_LEFT, 100);
	m_wndInBarListBox.InsertColumn(5, Util::LoadString(IDS_USERFILES), LVCFMT_LEFT, 50);
	m_wndInBarListBox.InsertColumn(6, Util::LoadString(IDS_USERLINE), LVCFMT_LEFT, 50);
	
	CString strUsers;
	strUsers.Format(IDS_STATUS_USERS, 0, m_dwLimit);
	m_wndStatusBar.SetPaneText(1, strUsers);
	strUsers.Format(IDS_STATUS_SUBROOMS, m_aRooms.GetSize(), g_sSettings.GetLimitSub());
	m_wndStatusBar.SetPaneText(2, strUsers);
	strUsers.Format(IDS_STATUS_PRIMARIES, 0, g_rLister.m_nMaxPrimaries);
	m_wndStatusBar.SetPaneText(3, strUsers);

	m_strRoomBase = g_sSettings.GetRoomname();
	m_strTopic		= g_sSettings.GetTopic();
	m_strMotd		= g_sSettings.GetMotd();
	m_strMotd.Replace("\n", "\\n");
	m_strExile	    = g_sSettings.GetExile();
	m_uMode			= g_sSettings.GetChannelMode();
	//if(g_sSettings.GetEnableSubs() && ((m_uMode&CM_SUBCHANNELS) != CM_SUBCHANNELS))
	//	m_uMode|=CM_SUBCHANNELS;

	m_strGodName	= g_sSettings.GetGodName();
	m_dwLimit		= g_sSettings.GetLimit();

	LoadBans(g_sSettings.GetWorkingDir() + "\\roboServ.ini");

	splash.SetStatusText(IDS_INIT_7);

	m_nStyle = g_sSettings.GetStyle();
	switch(m_nStyle){

	case WIN2K:
		g_PaintManager.InstallPaintManager(new CExtPaintManager);
		break;
	case WINXP:
		g_PaintManager.InstallPaintManager(new CExtPaintManagerXP);
		break;
	case WIN2K3:
		g_PaintManager.InstallPaintManager(new CExtPaintManagerOffice2003);
		break;
	default:
		g_PaintManager.InstallPaintManager(new CExtPaintManagerStudio2005);
		break;
	}
	m_hWndTarget = m_hWnd;

	SetTimer(TIMER_CHECK_TRAY_ICON, 10000, NULL);
	SetTimer(TIMER_UPDATE_STATUS, 2000, NULL);

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if(!CFrameWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
	cs.lpszClass = AfxRegisterWndClass(0);

	return TRUE;
}


void CMainFrame::OnSysCommand(UINT nID, LPARAM lParam) 
{

	CFrameWnd::OnSysCommand(nID, lParam);

 	if((nID & 0xFFF0) == SC_MINIMIZE)
	{

		this->ShowWindow(SW_HIDE);
	}
}

LRESULT CMainFrame::OnTrayNotify(WPARAM wParam, LPARAM lParam)
{

    UINT uID; 
    UINT uMsg; 
 
    uID = (UINT) wParam; 
    uMsg = (UINT) lParam; 
 
	if (uID != 120132)
		return 0;
	
	CPoint pt;	

    switch (uMsg) 
	{ 
	case WM_LBUTTONDBLCLK:
		ShowWindow(SW_RESTORE);
		//SetFocus();
		//SetForegroundWindow();
		break;
	
	case WM_RBUTTONDOWN:
	case WM_CONTEXTMENU:
		GetCursorPos(&pt);
		m_cmSystray.GetSubMenu(0)->TrackPopupMenu(TPM_BOTTOMALIGN|TPM_LEFTBUTTON|TPM_RIGHTBUTTON,pt.x,pt.y,this);
		m_cmSystray.GetSubMenu(0)->SetDefaultItem(0,TRUE);
		break;
    } 
	return 1;
}

void CMainFrame::OnSystrayRestore()
{

	ShowWindow(SW_RESTORE);
}

// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG


void CMainFrame::OnClose()
{


	if(m_bHosted){


		if(AfxMessageBox(IDS_EXITQUESTION, MB_ICONQUESTION+MB_YESNO) == IDNO){

			return;
		}
	}

	m_pView->GetDocument()->SetModifiedFlag(FALSE);
	if(m_bHosted){	 // last chance to stop

		m_bShutdown = TRUE;
		for(int i = 0; i < m_aClients.GetSize(); i++){

			m_aClients[i]->m_hMsgTarget = NULL;
		}
		Stop();
	}

	KillTimer(TIMER_CHECK_TRAY_ICON);
	KillTimer(TIMER_UPDATE_STATUS);

	if(m_nIconData.hWnd && m_nIconData.uID>0)
	{
		Shell_NotifyIcon(NIM_DELETE,&m_nIconData);
	}


	SaveBans(g_sSettings.GetWorkingDir() + "\\roboServ.ini");
	m_strMotd.Replace("\n", "\\n");
	g_sSettings.SetTopic(m_strTopic);
	g_sSettings.SetMotd(m_strMotd);
	g_sSettings.SetExile(m_strExile);
	g_sSettings.SetRoomname(m_strRoomBase);
	g_sSettings.SetChannelMode(m_uMode);
	g_sSettings.SetLimit(m_dwLimit);
	g_sSettings.SetStyle(m_nStyle);
	g_sSettings.Save();

	CFrameWnd::OnClose();
}

void CMainFrame::LoadToTray(HWND hWnd, UINT uCallbackMessage, CString strInfoTitle, CString strInfo, CString strTip, int uTimeout, HICON icon, DWORD dwIcon)
{

	//NOTIFYICONDATA contains information that the system needs to process taskbar status area messages

	ZeroMemory(&m_nIconData, sizeof(NOTIFYICONDATA));

	m_nIconData.cbSize		      = sizeof(NOTIFYICONDATA);
	m_nIconData.hWnd			  = hWnd;
	m_nIconData.uID			      = 120132;
	
	if(strInfo.IsEmpty()){

		m_nIconData.uFlags		      = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	}
	else{

		m_nIconData.uFlags		      = NIF_MESSAGE | NIF_ICON | NIF_TIP;
	}

	m_nIconData.uCallbackMessage = uCallbackMessage;  
	m_nIconData.hIcon		   	 = icon;
	strcpy(m_nIconData.szTip,       strTip      );

	Shell_NotifyIcon(NIM_ADD, &m_nIconData); // add to the taskbar's status area
}

void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	pOldWnd;
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) 
{
	if(m_wndMenuBar.TranslateMainFrameMessage(pMsg))
		return TRUE;
	
	return CFrameWnd::PreTranslateMessage(pMsg);
}

BOOL CMainFrame::DestroyWindow() 
{
CWinApp * pApp = ::AfxGetApp();
	ASSERT(pApp != NULL);
	ASSERT(pApp->m_pszRegistryKey != NULL);
	ASSERT(pApp->m_pszRegistryKey[0] != _T('\0'));
	ASSERT(pApp->m_pszProfileName != NULL);
	ASSERT(pApp->m_pszProfileName[0] != _T('\0'));

	VERIFY(
		CExtControlBar::ProfileBarStateSave(
			this,
			pApp->m_pszRegistryKey,
			pApp->m_pszProfileName,
			pApp->m_pszProfileName
			)
		);
	VERIFY(
		g_CmdManager->SerializeState(
			pApp->m_pszProfileName,
			pApp->m_pszRegistryKey,
			pApp->m_pszProfileName,
			true
			)
		);
	g_CmdManager->ProfileWndRemove(GetSafeHwnd());
	
	return CFrameWnd::DestroyWindow();
}

void CMainFrame::OnViewLikeOffice2k() 
{

	if(m_nStyle == WIN2K) return;

	VERIFY(g_PaintManager.InstallPaintManager(new CExtPaintManager));

	RecalcLayout();
	RedrawWindow(
		NULL,
		NULL,
		RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE
			| RDW_FRAME | RDW_ALLCHILDREN
		);
	CExtControlBar::stat_RedrawFloatingFrames(this);
	CExtControlBar::stat_RecalcBarMetrics(this);
	m_nStyle = WIN2K;
}

void CMainFrame::OnUpdateViewLikeOffice2k(CCmdUI* pCmdUI) 
{

	pCmdUI->Enable();
	pCmdUI->SetRadio(m_nStyle == WIN2K);
}

void CMainFrame::OnViewLikeOfficeXp() 
{

	if(m_nStyle == WINXP) return;

	VERIFY(g_PaintManager.InstallPaintManager(new CExtPaintManagerXP));

	RecalcLayout();
	RedrawWindow(
		NULL,
		NULL,
		RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE
			| RDW_FRAME | RDW_ALLCHILDREN
		);
	CExtControlBar::stat_RedrawFloatingFrames(this);
	CExtControlBar::stat_RecalcBarMetrics(this);
	m_nStyle = WINXP;
}



void CMainFrame::OnUpdateViewLikeOfficeXp(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable();
	pCmdUI->SetRadio(m_nStyle == WINXP);
}

void CMainFrame::OnViewLikeOffice2003() 
{

	if(m_nStyle == WIN2K3) return;


	VERIFY(g_PaintManager.InstallPaintManager(new CExtPaintManagerOffice2003));

	RecalcLayout();
	RedrawWindow(
		NULL,
		NULL,
		RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE
			| RDW_FRAME | RDW_ALLCHILDREN
		);
	CExtControlBar::stat_RedrawFloatingFrames( this );
	CExtControlBar::stat_RecalcBarMetrics( this );
	m_nStyle = WIN2K3;
}

void CMainFrame::OnUpdateViewLikeOffice2003(CCmdUI* pCmdUI) 
{

	pCmdUI->Enable();
	pCmdUI->SetRadio(m_nStyle == WIN2K3);
}

void CMainFrame::OnViewLikeVS2005() 
{

	if(m_nStyle == WIN2K5) return;
	VERIFY(g_PaintManager.InstallPaintManager(new CExtPaintManagerStudio2005));

	RecalcLayout();
	RedrawWindow(
		NULL,
		NULL,
		RDW_INVALIDATE | RDW_UPDATENOW | RDW_ERASE
			| RDW_FRAME | RDW_ALLCHILDREN
		);
	CExtControlBar::stat_RedrawFloatingFrames( this );
	CExtControlBar::stat_RecalcBarMetrics( this );
	m_nStyle = WIN2K5;
}

void CMainFrame::OnUpdateViewLikeVS2005(CCmdUI* pCmdUI) 
{

	pCmdUI->SetRadio(m_nStyle == WIN2K5);
}

void CMainFrame::ActivateFrame(int nCmdShow) 
{
	// window placement persistence
	if(m_dataFrameWP.showCmd != SW_HIDE)
	{
		SetWindowPlacement(&m_dataFrameWP);
		CFrameWnd::ActivateFrame(m_dataFrameWP.showCmd);
		m_dataFrameWP.showCmd = SW_HIDE;
		return;
	}
	
	CFrameWnd::ActivateFrame(nCmdShow);
}


void CMainFrame::EchoChat(CString strName, CString strMsg)
{

	if(m_bShutdown) return;

	WriteText(strName + ": ", RGB(0, 128, 0), FALSE, FALSE);
	WriteText(strMsg, RGB(0, 0, 0), FALSE, TRUE);

}

void CMainFrame::WriteText(CString strText, COLORREF crText, BOOL bBold, BOOL bAppendNewLine)
{

	if(m_bShutdown) return;
	if(m_pView == NULL) return;

	strText.Remove('\r');
	strText.Replace("\n", "\r\n");
	if(bAppendNewLine) strText += "\r\n";
	m_pView->WriteText(strText, crText, bBold);
}

void CMainFrame::WriteText(COLORREF crText, UINT nID, ...)
{

    TCHAR szBuffer[8192];
    TCHAR szFormat[8192];
	::LoadString(AfxGetApp()->m_hInstance, nID, szFormat, 8192);
	va_list ap;

	ZeroMemory(szBuffer,8192);
    va_start(ap, nID);
	
    _vsntprintf(szBuffer, sizeof(szBuffer), szFormat, ap);
	va_end(ap);

	WriteText(szBuffer, crText);
}

void CMainFrame::HandleCommand(CString strCmd)
{

	strCmd.TrimRight();

	if(strCmd.CompareNoCase("help") == 0){

		PrintHelp();
	}
	else if(strCmd.Find("host") == 0){

		if(m_bHosted){

			WriteText(RGB(128, 0, 0), IDS_ERROR_SERVER_ONEROOMPERCONSOLE);
			Beep(1000, 200);
		}
		else{

			strCmd.Replace("host ", "");
			strCmd.TrimLeft();

			CString strTmp;
			DWORD  dwIndex = 0;
			
			dwIndex = strCmd.Find(" ", 0);
			if(dwIndex > 0){


				strTmp = strCmd.Left(dwIndex);
				m_dwIP = TranslateIP(strTmp);
            }

			strCmd.Replace(strTmp, "");
			strCmd.TrimLeft();

			dwIndex = strCmd.Find(" ", 0);
			if(dwIndex > 0){

				strTmp = strCmd.Left(dwIndex);
				m_wPort = (WORD)atoi((LPTSTR)(LPCTSTR)strTmp);
			}

			strCmd.Replace(strTmp + " ", "");
			m_strRoomBase = strCmd;
			
			if(m_dwIP == 0 || m_wPort < 1024 || m_strRoomBase.IsEmpty()){

				WriteText(RGB(128, 0, 0), IDS_ERROR_SERVER_SYNTAXERROR);
				Beep(1000, 200);
				return;
			}
			if(m_strRoomBase.GetLength() > 40){

				WriteText(RGB(128,0,0), IDS_ERROR_ROOMLENGTH);
				Beep(1000, 200);
				return;
			}
			if(m_wPort == 6699){

				WriteText(RGB(128,0,0), IDS_ERROR_PORT);
				Beep(1000, 200);
				return;
			}
			m_strRoomFull.Format("%s_%08X%04X", m_strRoomBase, m_dwIP, m_wPort);

			SetTopic(g_sSettings.GetTopic());
			SetMotd(g_sSettings.GetMotd());

			m_uMode = g_sSettings.GetChannelMode();
			if((m_uMode&CM_MAINCHANNEL) != CM_MAINCHANNEL){

				m_uMode |= CM_MAINCHANNEL;
			}
			/*if(g_sSettings.GetModerated() && ((m_uMode&CM_MODERATED) != CM_MODERATED)){

				m_uMode	   |= CM_MODERATED;
			}
			if(g_sSettings.GetEnableSubs() && ((m_uMode&CM_SUBCHANNELS) != CM_SUBCHANNELS))
				m_uMode|=CM_SUBCHANNELS;*/
			m_dwLimit = g_sSettings.GetLimit();
			
			m_eHosted.ResetEvent();
			m_bHosted = FALSE;
			m_pServerThread = AfxBeginThread(CMainFrame::ServerThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);
			WaitForSingleObject(m_eHosted, INFINITE);
			
			if(m_bHosted){

				m_pNotifyThread = AfxBeginThread(CMainFrame::NotifyThread, (LPVOID)(CClientManager*)this, THREAD_PRIORITY_NORMAL);
			}

			if(m_bHosted){

				CString strMsg;
				strMsg.Format(IDS_SERVER_SUCCESS, m_strRoomFull);
				WriteText(strMsg);
				m_pView->GetDocument()->SetTitle(m_strRoomFull + " [hosted]");
				m_strGodName = g_sSettings.GetGodName();
				SetRoomName(m_strRoomBase, Util::FormatIP(m_dwIP), Util::Formatint(m_wPort));
				if(g_sSettings.GetListed()){

					List(TRUE);
				}
			}
			else{

				CString strMsg;
				strMsg.Format(IDS_ERROR_SERVER_HOST, m_strRoomFull);
				WriteText(strMsg, RGB(128, 0, 0));
			}
		}
	}
	else if((strCmd.Find("channelname") == 0) && m_bHosted){

		CString strOldName, strNewName;
		CTokenizer token(strCmd, " ");
		token.Next(strNewName);
		strNewName = token.Tail();
		if(strNewName.IsEmpty()){

			WriteText(RGB(128, 0, 0), IDS_ERROR_SERVER_SYNTAXERROR);
			Beep(1000, 200);
			return;
		}
		if(strNewName.GetLength() > 40){

			strNewName = strNewName.Left(40);
		}
		strOldName = m_strRoomBase;

		SetRoom(strNewName);

		CString strMsg;
		strMsg.Format(IDS_SERVER_ROOMRENAME, strOldName, strNewName, m_strRoomFull);
		WriteText(strMsg);
		SendChannelRename();
		m_pView->GetDocument()->SetTitle(m_strRoomFull + " [hosted]");
		SetRoomName(m_strRoomBase, Util::FormatIP(m_dwIP), Util::Formatint(m_wPort));
		m_strGodName = g_sSettings.GetGodName();
		if(g_sSettings.GetListed()){

			List(FALSE);
			List(TRUE);
		}
		for(int i = 0; i < m_aRooms.GetSize(); i++){

			((CSubChannel*)m_aRooms[i])->m_strParentRoom = m_strRoomBase;
		}
	}
	else if((strCmd.Find("/say ", 0) == 0) && m_bHosted){

		CString strMsg = strCmd.Mid(5);
		if(strMsg.Find("/me ", 0) == 0){

			strMsg = strMsg.Mid(4);
			SendAction(g_sSettings.GetGodName(), strMsg);
		}
		else{

			SendMsg(g_sSettings.GetGodName(), strMsg);
		}                            		

	}
	else if((strCmd.Find("/impose ", 0) == 0) && m_bHosted){

		CTokenizer token(strCmd, " ");
		CString strName, strMsg, strTmp;
		token.Next(strTmp);

		if(!token.Next(strName)){

			WriteText(RGB(128, 0, 0), IDS_ERROR_SERVER_SYNTAXERROR);
			Beep(1000, 200);
			return;
		}
		strMsg = token.Tail();
		if(strMsg.Find("/me ", 0) == 0){

			strMsg = strMsg.Mid(4);
			SendAction(strName, strMsg);
		}
		else{

			SendMsg(strName, strMsg);
		}                            		
	}
	else if((strCmd.Find("/", 0) == 0) && m_bHosted){


		if(ExecuteChannelCommand(g_sSettings.GetGodName(), strCmd, UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, strCmd);
		}
		else{

			WriteText(RGB(128, 0, 0), IDS_COMMAND_NOTUNDERSTOOD, strCmd);
			Beep(1000, 200);
		}
	}
	else if(strCmd.Find("redirect") == 0){

		CString strTarget = strCmd.Mid(9);
		// Syntax: /redirect Target
		// redirects entire room to 'Target'
		if(m_bHosted){

			Stop(TRUE, strTarget);
			CString strMsg;
			strMsg.Format(IDS_SERVER_TERMINATED, m_strRoomFull);
			WriteText(strMsg);
			m_pView->GetDocument()->SetTitle(m_strRoomFull + " [stopped]");
			if(g_sSettings.GetListed()){

				List(FALSE);
			}
		}
		else{

			WriteText(RGB(128, 0, 0), IDS_ERROR_SERVER_NOROOM);
			Beep(1000, 200);
		}
	}
	else if(strCmd.CompareNoCase("stop") == 0){

		if(m_bHosted){

			Stop();
			CString strMsg;
			strMsg.Format(IDS_SERVER_TERMINATED, m_strRoomFull);
			WriteText(strMsg);
			m_pView->GetDocument()->SetTitle(m_strRoomFull + " [stopped]");
			if(g_sSettings.GetListed()){

				List(FALSE);
			}
		}
		else{

			WriteText(RGB(128, 0, 0), IDS_ERROR_SERVER_NOROOM);
			Beep(1000, 200);
		}
	}
	else{

		PrintCmdNotUnderstood(strCmd);
	}
}

void CMainFrame::PrintCmdNotUnderstood(const CString strCmd)
{

	if(strCmd.IsEmpty()){

		CString strError;
		strError.LoadString(IDS_ERROR_CMDEMPTY);
		WriteText(strError, RGB(128, 0, 0));
	}
	else{

		CString strError;
		strError.Format(IDS_ERROR_NOTUNDERSTOOD, strCmd);
		WriteText(strError, RGB(128, 0, 0));
	}
}

void CMainFrame::PrintHelp(void)
{

	CString strHelp;
	strHelp.LoadString(IDS_SERVER_HELP);
	WriteText(strHelp, RGB(128, 128, 128), FALSE);
}

UINT CMainFrame::ServerThread(LPVOID pParam)
{

	CMainFrame* pServer = (CMainFrame*)pParam;
	ASSERT(pServer);

	
	{

		CString strIP;
		pServer->DetectExternalIP(strIP);
		if(strIP.GetLength()){

			pServer->m_strRoomSuffix.Format("_%8X%4X", CMySocket::GetIP(strIP), pServer->m_wPort);
		}
	}

	if(pServer->m_mServer.Listen(pServer->m_wPort) == 0){

		TRACE("Error creating socket\n");
		pServer->m_eHosted.SetEvent();
		pServer->m_bHosted = FALSE;
		return 0;
	}

	pServer->m_bHosted = TRUE;
	pServer->m_eHosted.SetEvent();
	pServer->m_eDone.ResetEvent();

	pServer->m_sIn = INVALID_SOCKET;
	
	pServer->m_nTotalMessages = 0;
	pServer->m_nTotalJoins    = 0;
	pServer->m_strStartDate  = Util::GetMyDate();
	pServer->m_strStartTime  = Util::GetMyLocalTime();
	
	{
		SYSTEMTIME time;
		GetLocalTime(&time);
		pServer->m_cStartTime = time;
	}

	while(pServer->m_bHosted){


		pServer->m_sIn = pServer->m_mServer.Accept();
		if(pServer->m_aClients.GetSize() >= (int)pServer->m_dwLimit){

			pServer->m_nFailedJoins++;
			continue;
		}
		if(pServer->m_sIn != INVALID_SOCKET){

			AfxBeginThread(Loginthread, (LPVOID)pServer, THREAD_PRIORITY_NORMAL);
		}
		else
			break;
	}

	pServer->m_bHosted = FALSE;
	pServer->m_eDone.SetEvent();
	TRACE("Leaving ServerThread\n");
	return 0;
}

LRESULT CMainFrame::ClientCallback(WPARAM wParam, LPARAM lParam)
{

	EnterCriticalSection(&m_csLock);
	
	ClientNotify* pNotify = (ClientNotify*)lParam;
	ASSERT(pNotify);

	m_qNotifies.push(pNotify);

	LeaveCriticalSection(&m_csLock);
	return 1;
}



LRESULT CMainFrame::SetView(WPARAM wParam, LPARAM lParam)
{

	m_pView = (CroboServView*)lParam;
	ASSERT(m_pView);

	m_pView->GetRichEditCtrl().SetReadOnly();
	CString strOut;
	strOut.Format(IDS_WELCOME1, STR_VERSION_DLG);
	WriteText(strOut, RGB(0, 0, 200));
	m_wndInBarEdit.SetSel(0, -1);
	strOut.LoadString(IDS_WELCOME2);
	m_wndInBarEdit.ReplaceSel(strOut);
	m_wndChatBar.SetFocus();
	m_wndInBarEdit.SetFocus();
	m_wndInBarEdit.SetSel(0, -1);
	return 1;
}

LRESULT CMainFrame::OnInput(WPARAM wParam, LPARAM lParam)
{

	CString strText;

	m_wndInBarEdit.GetWindowText(strText);
	if(strText.GetLength()){

		WriteText(strText);
		HandleCommand(strText);
		m_wndInBarEdit.SetSel(0, -1);
		m_wndInBarEdit.ReplaceSel("");
	}

	return 1;
}

LRESULT CMainFrame::OnLoadSettings(WPARAM wParam, LPARAM lParam)
{

	if(g_sSettings.GetTopic() != m_strTopic){

		SetTopic(g_sSettings.GetTopic());
		if(m_bHosted){

			SendTopic();
		}
	}
	if(g_sSettings.GetMotd() != m_strMotd){

		SetMotd(g_sSettings.GetMotd());
		
		if(m_bHosted){

			SendMotd();
		}
	}
	if(g_sSettings.GetGodVisible()){

		for(int i = 0; i < m_aClients.GetSize(); i++){

			m_aClients[i]->SendRename(m_strGodName, 0, 0, 
				g_sSettings.GetGodName(), 0, 0, 
				g_sSettings.GetGodLine(), g_sSettings.GetGodFiles(), 1);
		}
		m_strGodName = g_sSettings.GetGodName();
	}

	/*if(g_sSettings.GetModerated() && ((m_uMode & CM_MODERATED) != CM_MODERATED)){

		m_uMode |= CM_MODERATED;
	}
	else if(!g_sSettings.GetModerated() && ((m_uMode & CM_MODERATED) == CM_MODERATED)){

		m_uMode &= ~CM_MODERATED;
	}

	if(g_sSettings.GetEnableSubs() && ((m_uMode & CM_SUBCHANNELS) != CM_SUBCHANNELS)){

		m_uMode |= CM_SUBCHANNELS;
		TRACE("added subchannelflag\n"); 
	}
	else if(!g_sSettings.GetEnableSubs() && ((m_uMode & CM_SUBCHANNELS) == CM_SUBCHANNELS)){

		m_uMode &= ~CM_SUBCHANNELS;
		TRACE("removed subchannelflag\n");
	}*/
	m_uMode = g_sSettings.GetChannelMode();
	m_strKeyword = g_sSettings.GetKeyword();
	if(!m_strKeyword.IsEmpty()){

		m_uMode |= CM_KEYWORD;
	}
	else{

		m_uMode &= ~CM_KEYWORD;
	}
	if(m_bHosted)	List(g_sSettings.GetListed());

	return 0;
}

void CMainFrame::LoadBans(CString strFile)
{

	CIni ini;
	ini.SetIniFileName(strFile);
	CString strTmp;
	int nCount = ini.GetValue("Banned Users", "Count", 0);

	m_aBans.RemoveAll();

	for(int i  = 0; i < nCount; i++){

		BAN ban;
		strTmp.Format("Banned User %03d", i);
		
		ban.strName = ini.GetValue(strTmp, "Name", "");
		ban.strIP   = ini.GetValue(strTmp, "IP", "");
		ban.strHost = ini.GetValue(strTmp, "Host", "");

		m_aBans.Add(ban);
	}
}

void CMainFrame::OnFileOpen()
{


	COpenConfigDlg dlg;

	if(dlg.DoModal() == IDOK){ 
	
		CString strFile;
		if(dlg.m_strURL.Find("http://", 0) == 0){

			g_sSettings.LoadFromServer(dlg.m_strURL);
			strFile = g_sSettings.GetWorkingDir() + "\\temp.dat";
		}
		else{
			
			g_sSettings.Load(dlg.m_strURL);
			strFile = dlg.m_strURL;
		}
		


		SetTopic(g_sSettings.GetTopic());
		SetMotd(g_sSettings.GetMotd());

		m_uMode		  = g_sSettings.GetChannelMode();
		
		/*if(g_sSettings.GetEnableSubs() && ((m_uMode&CM_SUBCHANNELS) != CM_SUBCHANNELS))
			m_uMode|=CM_SUBCHANNELS;
	*/
		if((m_uMode&CM_MAINCHANNEL) != CM_MAINCHANNEL){

			m_uMode |= CM_MAINCHANNEL;
		}

		/*if(g_sSettings.GetModerated() && ((m_uMode&CM_MODERATED)!=CM_MODERATED)){

			m_uMode |= CM_MODERATED;
		}
*/
		m_dwLimit	  = g_sSettings.GetLimit();
		m_dwIP		  = g_sSettings.GetIP();
		m_wPort		  = g_sSettings.GetPort();
		m_strExile    = g_sSettings.GetExile();
		m_strKeyword  = g_sSettings.GetKeyword();
		SetRoom(g_sSettings.GetRoomname());
 		m_strRoomFull.Format("%s_%08X%04X", m_strRoomBase, m_dwIP, m_wPort);


		LoadBans(strFile);

		::SendMessage(m_wndDockedPS.m_hWnd, UWM_SETTINGS, 0, 0);

		if(!m_bHosted){

			m_eHosted.ResetEvent();
			m_bHosted = FALSE;
			m_pServerThread = AfxBeginThread(CMainFrame::ServerThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);
			WaitForSingleObject(m_eHosted, INFINITE);
			m_pNotifyThread = AfxBeginThread(CMainFrame::NotifyThread, (LPVOID)(CClientManager*)this, THREAD_PRIORITY_NORMAL);
			
			if(m_bHosted){

				CString strMsg;
				strMsg.Format(IDS_SERVER_SUCCESS, m_strRoomFull);
				WriteText(strMsg);
				m_pView->GetDocument()->SetTitle(m_strRoomFull + " [hosted]");
				m_strGodName = g_sSettings.GetGodName();
				SetRoomName(m_strRoomBase, Util::FormatIP(m_dwIP), Util::Formatint(m_wPort));
				if(g_sSettings.GetListed()){

					List(TRUE);
				}
			}
			else{

				CString strMsg;
				strMsg.Format(IDS_ERROR_SERVER_HOST, m_strRoomFull);
				WriteText(strMsg);
				m_pView->GetDocument()->SetTitle(m_strRoomFull + " [error]");
			}
			WriteText(RGB(0,0,128), IDS_SERVER_CFG_LOADED);
		}

	}
}


void CMainFrame::SaveBans(CString strFile)
{

	CIni ini;
	ini.SetIniFileName(strFile);
	CString strTmp;
	ini.SetValue("Banned Users", "Count", m_aBans.GetSize());

	for(int i  = 0; i < m_aBans.GetSize(); i++){

		strTmp.Format("Banned User %03d", i);
		ini.SetValue(strTmp, "Name", m_aBans[i].strName);
		ini.SetValue(strTmp, "IP", m_aBans[i].strIP);
		ini.SetValue(strTmp, "Host", m_aBans[i].strHost);
	}
}

void CMainFrame::OnFileSaveConfig()
{

	CString strFilter;
	strFilter.LoadString(IDS_SERVER_FILTER);

	CFileDialog dlg(FALSE, ".rsi", NULL, OFN_NOREADONLYRETURN|OFN_PATHMUSTEXIST, strFilter, this);

	if(dlg.DoModal() == IDOK){ 
	
		CStdioFile file;
		TRY{

			if(file.Open(dlg.GetPathName(), CFile::modeCreate|CFile::modeNoTruncate|CFile::typeText))
				file.Close();

		}CATCH(CFileException, e){}END_CATCH;



		g_sSettings.SetRoomname(m_strRoomBase);
		g_sSettings.SetTopic(m_strTopic);
		CString strMotd = m_strMotd;
		strMotd.Replace("\n", "\\n");
		strMotd.Replace("\r", "\\r");
		g_sSettings.SetMotd(strMotd);
		g_sSettings.SetChannelMode(m_uMode);
		g_sSettings.SetLimit(m_dwLimit);
		g_sSettings.SetIP(m_dwIP);
		g_sSettings.SetPort(m_wPort);
		g_sSettings.SetExile(m_strExile);
		g_sSettings.Save(dlg.GetPathName());

		SaveBans(dlg.GetPathName());

		WriteText(RGB(0,0,128), IDS_SERVER_CFG_SAVED);
	}
}

void CMainFrame::List(BOOL bStart)
{

	if(bStart && g_rLister.m_bShutdown){

		g_rLister.AddRoom(m_strRoomBase, m_strTopic, m_aClients.GetSize(), m_dwLimit, m_wPort);
		if(g_sSettings.GetSubListed()){

			for(int i = 0; i < m_aRooms.GetSize(); i++){

				g_rLister.AddRoom(m_aRooms[i]->m_strRoomBase, m_aRooms[i]->m_strTopic, m_aRooms[i]->m_dwUsers, m_aRooms[i]->m_dwLimit, m_aRooms[i]->m_wPort);
			}
		}
		g_rLister.StartList();
	}
	else if(!bStart && !g_rLister.m_bShutdown){

		g_rLister.RemoveRoom(m_strRoomBase);
		if(g_sSettings.GetSubListed()){

			for(int i = 0; i < m_aRooms.GetSize(); i++){

				g_rLister.RemoveRoom(m_aRooms[i]->m_strRoomBase);
			}
		}
		g_rLister.StopList();
	}
}

void CMainFrame::OnUserlistAddadmin32773()
{


	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/admin " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/admin");
		}
	}
}

void CMainFrame::OnUserlistRemoveadmin32774()
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/remadmin " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/remadmin");
		}
	}
}

void CMainFrame::OnUserlistKick() 
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/kick " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/kic");
		}
	}
}

void CMainFrame::OnUserlistKickban() 
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/kickban " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/kickban");
		}
	}
}

void CMainFrame::OnUserlistBan() 
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/ban " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/ban");
		}
	}
}

void CMainFrame::OnUserlistVoice()
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/voice " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/voice");
		}
	}
}


void CMainFrame::OnUserlistDevoice()
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/muzzle " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/muzzle");
		}
	}
}

void CMainFrame::OnUserlistAddagent()
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/agent " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/agent");
		}
	}
}

void CMainFrame::OnUserlistStats()
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/stat " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/stat");
		}
	}
}

void CMainFrame::OnUserlistModes()
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/usermodes " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/usermodes");
		}
	}
}


void CMainFrame::OnUserlistRemoveagent()
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/remagent " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/remagent");
		}
	}
}

void CMainFrame::OnUserlistCopyusername32781()
{
	
	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		CClipboard::SetText((LPTSTR)(LPCTSTR)m_wndInBarListBox.GetItemText(n, 0));
	}
}

void CMainFrame::OnUserlistCopyip()
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		CClipboard::SetText((LPTSTR)(LPCTSTR)m_wndInBarListBox.GetItemText(n, 2));
	}
}


void CMainFrame::OnUserlistHide()
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/hide " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/admin");
		}
	}
}

void CMainFrame::OnUserlistShow()
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		if(ExecuteChannelCommand(g_sSettings.GetGodName(), "/show " + m_wndInBarListBox.GetItemText(n, 0), UM_SUPER)){

			WriteText(RGB(0, 0, 128), IDS_COMMAND_EXECUTED, "/admin");
		}
	}
}

void CMainFrame::OnUserlistRemoveghost() 
{

	int n = m_wndInBarListBox.GetNextItem(-1, LVNI_SELECTED);
	if(n >= 0){

		CString strName = m_wndInBarListBox.GetItemText(n, 0);
		
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName == strName){

				CString strWarning;
				strWarning.Format(IDS_NOGHOST, strName);
				if(AfxMessageBox(strWarning, MB_ICONSTOP+MB_YESNO) == IDNO) return;
			}
		}

		m_wndInBarListBox.DeleteItem(n);
	}
}

CString CMainFrame::BuiltModeString(UINT uMode)
{

	CString strModes;
	if((uMode & UM_ADMIN) == UM_ADMIN) strModes += "admin ";
	if((uMode & UM_VOICED) == UM_VOICED)  strModes += "voiced ";
	if((uMode & UM_BANNED) == UM_BANNED)  strModes += "banned ";
	if((uMode & UM_AGENT) == UM_AGENT)  strModes += "agent ";
	if((uMode & UM_HIDDEN) == UM_HIDDEN)  strModes += "hidden ";
	return strModes;
}

void CMainFrame::UpdateModeUserlist(CString strUser, UINT uMode)
{

	LVFINDINFO find;
	find.flags = LVFI_PARTIAL|LVFI_STRING|LVFI_WRAP;
	find.psz   = strUser;

	int nIndex = m_wndInBarListBox.FindItem(&find, -1);
	if(nIndex >= 0){

		m_wndInBarListBox.SetItemText(nIndex, 1, BuiltModeString(uMode));
	}
}

void CMainFrame::OnRoomnameBox()
{
//dummy :P
}

void CMainFrame::OnUpdateRoomnameBox(CCmdUI* pCmdUI)
{

	pCmdUI->Enable(!m_bHosted);
}

void CMainFrame::OnRoomStart()
{


	// save settings !!
	::SendMessage(m_wndDockedPS.m_hWnd, UWM_SETTINGS, 0, 1);

	CString strOld = CClipboard::GetText();

	m_wndNameBox.SetFocus();
	m_wndNameBox.SetEditSel(0, -1);
	m_wndNameBox.Copy();
	m_wndNameBox.SetEditSel(-1, 0);
	CString strRoom = CClipboard::GetText(m_hWnd);

	if(strRoom.IsEmpty()) return;

	if(!strRoom.IsEmpty() && (m_wndNameBox.FindStringExact(-1, strRoom) < 0)){

		m_wndNameBox.AddString(strRoom);
	}

	m_wndPort.SetFocus();
	m_wndPort.SetEditSel(0, -1);
	m_wndPort.Copy();
	m_wndPort.SetEditSel(-1, 0);
	CString strPort = CClipboard::GetText(m_hWnd);
	if(!strPort.IsEmpty() && (m_wndPort.FindStringExact(-1, strPort) < 0)){

		m_wndPort.AddString(strPort);
	}

	m_wndIP.SetFocus();
	m_wndIP.SetEditSel(0, -1);
	m_wndIP.Copy();
	m_wndIP.SetEditSel(-1, 0);
	CString strIP = CClipboard::GetText(m_hWnd);
	if(!strIP.IsEmpty() && (m_wndIP.FindStringExact(-1, strIP) < 0)){

		m_wndIP.AddString(strIP);
	}

	CClipboard::SetText((LPTSTR)(LPCTSTR)strOld);

	strOld.Format("host %s %s %s", strIP, strPort, strRoom);

	WriteText(strOld);
	HandleCommand(strOld);
}

void CMainFrame::OnRoomStop()
{

	if(AfxMessageBox(IDS_REALLYSTOP, MB_ICONQUESTION+MB_YESNO) == IDYES){

		HandleCommand("stop");
	}
}

void CMainFrame::OnUpdateRoomStop(CCmdUI* pCmdUI)
{

	pCmdUI->Enable(m_bHosted);
}

void CMainFrame::OnHelpCommands()
{

	ShellExecute(0, "open", g_sSettings.GetWorkingDir() + "\\commands.txt", 0, 0, SW_SHOW);
}

void CMainFrame::OnHelpHomepage()
{

	ShellExecute(0, "open", "http://mxcontrol.sf.net", 0, 0, SW_SHOW);
}

void CMainFrame::OnSwitchLang()
{


	CIni ini;
	ini.SetIniFileName(g_sSettings.GetWorkingDir() + "\\roboServ.ini");
	CString strLang = ini.GetValue("Language", "Active", "English");
	CString strSel;
	
	int n = m_wndLang.GetCurSel();
	if(n >= 0){

		m_wndLang.GetLBText(n, strSel);
		if(strLang != strSel){

			ini.SetValue("Language", "Active", strSel);
			AfxMessageBox(IDS_RESTART, MB_ICONINFORMATION);
		}
	}
}

void CMainFrame::SetRoomName(CString strName, CString strIP, CString strPort)
{

	CString strOld = CClipboard::GetText();

	int n = -1;
	if(((n = m_wndNameBox.FindStringExact(-1, strName)) < 0)){

		n = m_wndNameBox.AddString(strName);
		
	}
	if(n >= 0) m_wndNameBox.SetCurSel(n);

	m_wndPort.SetEditSel(0, -1);
	m_wndPort.Copy();
	CString strPortA = CClipboard::GetText();
	if(!strPortA.IsEmpty() && (m_wndPort.FindStringExact(-1, strPortA) < 0)){

		m_wndPort.AddString(strPortA);
	}
	CClipboard::SetText((LPTSTR)(LPCTSTR)strPort);
	m_wndPort.Paste();
	m_wndPort.SetEditSel(-1, -1);

	m_wndIP.SetEditSel(0, -1);
	m_wndIP.Copy();
	CString strIPA = CClipboard::GetText();
	if(!strIPA.IsEmpty() && (m_wndIP.FindStringExact(-1, strIPA) < 0)){

		m_wndIP.AddString(strIPA);
	}
	CClipboard::SetText((LPTSTR)(LPCTSTR)strIP);
	m_wndIP.Paste();
	m_wndIP.SetEditSel(-1, -1);

	CClipboard::SetText((LPTSTR)(LPCTSTR)strOld);
}


void CMainFrame::Stop(BOOL bRedirect, CString strTarget)
{

	for(int i = m_aRooms.GetSize()-1; i >= 0 ; i--){

		CSubChannel* pSub = (CSubChannel*)m_aRooms[i];
		m_aRooms.RemoveAt(i);
		if(pSub <= 0) continue;
		pSub->Stop();
		delete pSub;
		pSub = 0;
	}

	m_mServer.Close();
	m_wndInBarListBox.DeleteAllItems();

	DWORD n = WaitForSingleObject(m_eDone, 1000);
	if(n == WAIT_TIMEOUT || n == WAIT_FAILED){

		TerminateThread(m_pServerThread->m_hThread, 0);
	}

	CClientManager::Stop(bRedirect, strTarget);
	m_pServerThread = NULL;
}

UINT CMainFrame::Loginthread(LPVOID pParam)
{
	
	CMainFrame* pServer = (CMainFrame*)pParam;
	ASSERT(pServer);

	// first check the clients
	pServer->CheckClients();

	CClientSocket* mClient = new CClientSocket(pServer->m_sIn);

	CString strRoom;

	if(!mClient->HandShake(strRoom)){

		// Login failed
		pServer->m_nFailedJoins++;
		delete mClient;
		mClient = 0;
		return 0;
	}

	BOOL bLoggedIn = FALSE;

	{

		int n = strRoom.ReverseFind('_');
		if(n > 0){

			strRoom = strRoom.Left(n);
		}
	}
	
	if(strRoom == pServer->m_strRoomBase) // This is the main room
	{

		if(pServer->AddClient(mClient) == 0) return 0;
		bLoggedIn = TRUE;

		int nIndex = pServer->m_wndInBarListBox.InsertItem(0, mClient->m_strName, 0);
		pServer->m_wndInBarListBox.SetItemText(nIndex, 1, BuiltModeString(mClient->m_uMode)); // modes
		pServer->m_wndInBarListBox.SetItemText(nIndex, 2, Util::FormatIP(mClient->m_dwSrcIP));
		pServer->m_wndInBarListBox.SetItemText(nIndex, 3, mClient->m_strSrcHost);
		pServer->m_wndInBarListBox.SetItemText(nIndex, 4, mClient->m_strClientString+ " " +mClient->m_strClientVer); // client
		pServer->m_wndInBarListBox.SetItemText(nIndex, 5, Util::Formatint(mClient->m_dwFiles));
		pServer->m_wndInBarListBox.SetItemText(nIndex, 6, Util::FormatLine(mClient->m_wLineType));
	}
	else{

		for(int i = 0; i < pServer->m_aRooms.GetSize(); i++){

			if(pServer->m_aRooms[i]->m_strRoomBase == strRoom){

				if(pServer->m_aRooms[i]->AddClient(mClient) == 0) return 0;
				bLoggedIn = TRUE;
				break; // bounce out
			}
		}
	}
	
	if(!bLoggedIn){

		TRACE("Login failed (%s wanted to join %s)\n", mClient->m_strName, strRoom);
		// Client failed to join an existing room
		pServer->m_nFailedJoins++;
		mClient->LogOut();
		delete mClient;
		mClient = 0;
	}
	
	TRACE("Login Complete\n");
	return 0;
}

void CMainFrame::SendJoin(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel, DWORD dwRealIP)
{

	CClientManager::SendJoin(strUser, dwIP, wPort, wLine, dwFiles, wUserLevel, dwRealIP);
	if(g_sSettings.GetListed()){

		//Users = m_aClients.GetSize();
		g_rLister.ChangeUsers(m_strRoomBase, m_aClients.GetSize());
	}
}

void CMainFrame::SendPart(const CString strUser, DWORD dwIP, WORD wPort, BOOL bRealPart)
{

	if(bRealPart){

		if(g_sSettings.GetListed()){

			g_rLister.ChangeUsers(m_strRoomBase, m_aClients.GetSize()-1);
		}

		LVFINDINFO find;
		find.flags = LVFI_PARTIAL|LVFI_STRING|LVFI_WRAP;
		find.psz   = strUser;

		int nIndex = m_wndInBarListBox.FindItem(&find, -1);
		if(nIndex >= 0){

			m_wndInBarListBox.DeleteItem(nIndex);
		}
#ifdef _DEBUG
		else{

			WriteText("Error removing user from list :-( " + strUser, RGB(128,0,0));
		}
#endif
	}

	CClientManager::SendPart(strUser, dwIP, wPort, bRealPart);
}

BOOL CMainFrame::SendRename(CClientSocket* pClient, const CString strOldName, DWORD dwOldIP, WORD wOldPort, const CString strNewName, DWORD dwNewIP, WORD wNewPort, WORD wLine, DWORD dwFiles, WORD wUserLevel)
{

	if(pClient <= 0) return FALSE;

	if(CheckUserName(pClient->m_strName, pClient->m_dwSrcIP, pClient->m_wSrcPort)){


		LVFINDINFO find;
		find.flags = LVFI_PARTIAL|LVFI_STRING|LVFI_WRAP;
		find.psz   = strOldName;

		int nIndex = m_wndInBarListBox.FindItem(&find, -1);
		if(nIndex >= 0){

			m_wndInBarListBox.SetItemText(nIndex, 0, strNewName);
			m_wndInBarListBox.SetItemText(nIndex, 2, Util::FormatIP(pClient->m_dwSrcIP));
			m_wndInBarListBox.SetItemText(nIndex, 3, pClient->m_strSrcHost);
			m_wndInBarListBox.SetItemText(nIndex, 4, pClient->m_strClientString + " " + pClient->m_strClientVer);
			m_wndInBarListBox.SetItemText(nIndex, 5, Util::Formatint(dwFiles));
			m_wndInBarListBox.SetItemText(nIndex, 6, Util::FormatLine(wLine));
		}
	}	
	return CClientManager::SendRename(pClient, strOldName, dwOldIP, wOldPort, strNewName, dwNewIP, wNewPort, wLine, dwFiles, wUserLevel);
}

BOOL CMainFrame::ExecuteChannelCommand(const CString strUser, const CString strMsg, const UINT uMode)
{

	
	if((m_uMode & CM_SUBCHANNELS) == CM_SUBCHANNELS){


		if(strMsg.Find("/createchannel ", 0) == 0){

			// open up a new subchannel
			if(!CheckPermission(uMode, g_sSettings.GetSubPermission())){

				SendPermissionDenied(strUser, strMsg);
				TRACE("1\n");
				return TRUE;
			}
			
			CString strRoom = strMsg.Mid(15);
			strRoom.TrimLeft();
			strRoom.TrimRight();
			if(strRoom == m_strRoomBase){

				ServerAction(strUser, Util::LoadString(IDS_SUBEQUALMAIN), 4000+0x008, UM_NORMAL);
				ServerAction(strUser, strMsg, 0, UM_ADMIN);
			}
			else if(!strRoom.IsEmpty()){

				for(int i = 0; i < m_aRooms.GetSize(); i++){

					if(m_aRooms[i]->m_strRoomBase == strRoom){

						break;
					}
				}

				if(i >= m_aRooms.GetSize()){

					// The subchannel dummy window needs to be
					// created in the correct thread context so
					// we have to send a message to CMainFrame
					::SendMessage(m_hWnd, UWM_CREATESUB, (WPARAM)(LPCTSTR)strUser, (LPARAM)(LPCTSTR)strRoom);
				}
			}
			else{

				SendCmdNotUnderstood(strUser, strMsg);
				
			}
			return TRUE;			
		}
		else if(strMsg.Find("/join ", 0) == 0){

			CString strRoom = strMsg.Mid(6);
			strRoom.TrimLeft();
			strRoom.TrimRight();
			for(int i = 0; i < m_aRooms.GetSize(); i++){

				if(m_aRooms[i]->m_strRoomFull.Find(strRoom, 0) == 0){

					break;
				}
			}
			if(i >= m_aRooms.GetSize()){

				CString strError;
				strError.Format(IDS_JOINNOROOM, strRoom);
				ServerAction(strUser, strError, 4000+0x008, UM_NORMAL);
				ServerAction(strUser, strMsg, 0, UM_ADMIN);
			}
			else{

				CString strRed;
				strRed.Format(IDS_SUBJOIN, strRoom);
				ServerAction(strUser, strRed, 4000+0x008, UM_NORMAL);
				ServerAction(strUser, strMsg, 0, UM_ADMIN);

				for(int j = 0; j < m_aClients.GetSize(); j++){

					if(m_aClients[i]->m_strName == strUser){

						m_aClients[i]->SendRedirect(m_aRooms[i]->m_strRoomBase + m_aClients[i]->m_strJoinSuffix);
					}
				}
			}
			return TRUE;
		}
		else if(strMsg == "/listchannels"){

			CString strOut;
			strOut.Format(IDS_CHANNEL_START, m_strRoomBase);
			ServerAction(strUser, strOut, 4000+0x04, UM_NORMAL);

			for(int i = 0; i < m_aRooms.GetSize(); i++){

				strOut.Format("+ %s(%02d/%02d) [%s]", m_aRooms[i]->m_strRoomFull, m_aRooms[i]->m_aClients.GetSize(), m_aRooms[i]->m_dwLimit, m_aRooms[i]->m_strKeyword.IsEmpty() ? Util::LoadString(IDS_PUBLIC) : Util::LoadString(IDS_PRIVATE));
				ServerAction(strUser, strOut, 4000+0x01, UM_NORMAL);
			}

			strOut.LoadString(IDS_CHANNEL_STOP);
			ServerAction(strUser, strOut, 4000+0x005, UM_NORMAL);
			ServerAction(strUser, strMsg, 0x0, UM_ADMIN);
			
			return TRUE;
		}
		else if(strMsg.Find("/closechannel ", 0) == 0){

			if(!CheckPermission(uMode, UM_AGENT)){

				SendPermissionDenied(strUser, strMsg);
				return TRUE;
			}

			CString strRoom = strMsg.Mid(14);
			strRoom.TrimLeft();
			strRoom.TrimRight();
			for(int i = 0; i < m_aRooms.GetSize(); i++){

				if(m_aRooms[i]->m_strRoomFull.Find(strRoom, 0) == 0){

					CSubChannel* sub = (CSubChannel*)m_aRooms[i];
					m_aRooms.RemoveAt(i);
					
					strRoom = sub->m_strRoomBase;
					sub->Stop();
					delete sub;
					sub = 0;

					CString strOut;
					strOut.Format(IDS_SUBCHANNEL_CLOSED, strRoom, strUser);
					ServerAction("", strOut, 0, UM_ADMIN);
					return TRUE;;
				}
			}

			CString strOut;
			strOut.Format(IDS_SUBCHANNEL_NOTFOUND, strRoom);
			ServerAction("", strOut, 0, UM_ADMIN);
			ServerAction(strUser, strMsg, 0x000, UM_ADMIN);


			return TRUE;
		}

	}

	BOOL bReturn = CClientManager::ExecuteChannelCommand(strUser, strMsg, uMode);
	
	if(bReturn){
		
		if(g_sSettings.GetListed()){

			SetTopic(m_strTopic);
		}

		if(g_sSettings.GetListed()){

			g_rLister.ChangeLimit(m_strRoomBase, m_dwLimit);
		}
	}
	return bReturn;
}

LRESULT CMainFrame::OnWriteTextSub(WPARAM w, LPARAM l)
{

	//EnterCriticalSection(&m_csWriteLock);

	WRITE_STRUCT *ws = (WRITE_STRUCT*)l;
	
	ASSERT(ws != NULL);

	if(ws->strRoom.GetLength() > 20){

		ws->strRoom = ws->strRoom.Left(20);
	}

	WriteText("[" + ws->strRoom + "] ", RGB(100, 0, 0), FALSE, FALSE);
	WriteText(ws->strText, ws->crColor, ws->bBold, ws->bNewLine);
	//LeaveCriticalSection(&m_csWriteLock);
	return 0;
}

void CMainFrame::DetectExternalIP(CString &rIP)
{

	CInternetSession	is(AfxGetApp()->m_pszAppName,
							1, INTERNET_OPEN_TYPE_PRECONFIG,
							NULL, NULL, INTERNET_FLAG_DONT_CACHE);

	try{

		CHttpFile* pFile = (CHttpFile*) is.OpenURL("http://mxcontrol.sourceforge.net/detect-ip.php");

		if(pFile == NULL) return;

		pFile->ReadString(rIP);
		TRACE(rIP);

		pFile->Close();
		delete pFile;
		is.Close();
	}
	catch(CInternetException* pEx){
		
		TCHAR   szCause[255];
		CString strFormatted;

		pEx->GetErrorMessage(szCause, 255);
		strFormatted.Format("Error during Update Query: %s\n", szCause);
		TRACE(strFormatted);
		return;
	}
}

void CMainFrame::OnTimer(UINT nIDEvent) 
{

	if(nIDEvent == TIMER_UPDATE_STATUS){

		CString strUsers;
		strUsers.Format(IDS_STATUS_USERS, m_aClients.GetSize(), m_dwLimit);
		m_wndStatusBar.SetPaneText(1, strUsers);
		strUsers.Format(IDS_STATUS_SUBROOMS, m_aRooms.GetSize(), g_sSettings.GetLimitSub());
		m_wndStatusBar.SetPaneText(2, strUsers);
		strUsers.Format(IDS_STATUS_PRIMARIES, g_rLister.GetPrimaryNum(), g_rLister.m_nMaxPrimaries);
		m_wndStatusBar.SetPaneText(3, strUsers);
	}
	else if(nIDEvent == TIMER_CHECK_TRAY_ICON){
		
		if(Shell_NotifyIcon(NIM_MODIFY, &m_nIconData) == NULL){

			CWinApp* pApp = AfxGetApp();
			ASSERT(pApp);

			HICON hIcon = pApp->LoadIcon(IDR_MAINFRAME);
			ASSERT(hIcon != NULL);

			LoadToTray(m_hWnd, WM_TRAY_ICON_NOTIFY_MESSAGE, pApp->m_pszAppName, "", pApp->m_pszAppName, 1, hIcon, 0); 
		}
	}

	CFrameWnd::OnTimer(nIDEvent);
}


void CMainFrame::SetTopic(CString strTopic)
{

	m_strTopic = strTopic;
	if(g_sSettings.GetListed()){

		Util::ReplaceVars(strTopic);
		if(g_sSettings.GetAdvertiseRose()){

			g_rLister.ChangeTopic(m_strRoomBase, "[rose Beta] " + strTopic);
		}
		else{

			g_rLister.ChangeTopic(m_strRoomBase, strTopic);
		}
	}
}

LRESULT CMainFrame::OnCloseSubChannel(WPARAM wParam, LPARAM lParam)
{

	CSubChannel *sub = (CSubChannel*)lParam;

	ASSERT(sub);

	for(int i = 0; i < m_aRooms.GetSize(); i++){

		if(m_aRooms[i] == sub){

			g_rLister.RemoveRoom(sub->m_strRoomBase);
			m_aRooms.RemoveAt(i);
			sub->Stop();
			delete sub;
			sub = 0;
		}
	}
	return 0;
}



LRESULT CMainFrame::OnCreateSubchannel(WPARAM wParam, LPARAM lParam)
{

	CString strUser = (LPCTSTR)wParam;
	CString strRoom = (LPCTSTR)lParam;
	
	// room doesnt exist yet
	CSubChannel* sub   = new CSubChannel(m_hWnd);

	RECT rc;
	GetClientRect(&rc);

	sub->m_strRoomBase = strRoom;
	sub->m_strRoomFull = strRoom + m_strRoomSuffix;
	sub->m_strGodName  = g_sSettings.GetGodName();
	sub->m_dwIP        = m_dwIP;
	sub->m_wPort	   = m_wPort;
	sub->m_dwLimit     = 40;
	sub->m_strTopic    = "";
	sub->m_strMotd     = "";
	sub->m_bHosted = TRUE;
	sub->m_nTotalMessages = 0;
	sub->m_nTotalJoins    = 0;
	sub->m_strStartDate  = Util::GetMyDate();
	sub->m_strStartTime  = Util::GetMyLocalTime();
	sub->m_strParentRoom = m_strRoomBase;
	sub->m_uMode		 = m_uMode;
	sub->m_uMode		 = m_uMode &~ CM_KEYWORD;
	sub->m_uMode         = sub->m_uMode &~ CM_MAINCHANNEL;
	sub->m_uMode         = sub->m_uMode &~ CM_SUBCHANNELS;
	SYSTEMTIME time;
	GetLocalTime(&time);
	sub->m_cStartTime = time;
	
	m_aRooms.Add(sub);
	if(sub->CreateEx(0, AfxRegisterWndClass(0), strRoom, WS_POPUP, rc, this, 0, NULL) == 0){

		DWORD dwError = GetLastError();
		TRACE(">> %d\n", dwError);
		WriteText("Internal error. Can not create subchannel\n", RGB(200,0,0));
		return TRUE;
	}

	CString strOut;
	strOut.Format(IDS_SUBCREATED, sub->m_strRoomFull);
	ServerAction(m_strGodName, strOut, 0x04, g_sSettings.GetSubPermission());
	if(strUser != m_strGodName){

		strOut.Format(IDS_SUBJOIN, strRoom);
		ServerAction(strUser, strOut, 4000+0x008, UM_NORMAL);
	}
	ServerAction(strUser, "/createchannel " + strRoom, 0x00, UM_ADMIN);
	
	m_pNotifyThread = AfxBeginThread(CSubChannel::NotifyThread, (LPVOID)(CClientManager*)sub, THREAD_PRIORITY_NORMAL);

	if(g_sSettings.GetSubListed()){

		g_rLister.AddRoom(sub->m_strRoomBase, sub->m_strTopic, sub->m_dwUsers, sub->m_dwLimit, sub->m_wPort);
	}

	for(int j = 0; j < m_aClients.GetSize(); j++){

		if(m_aClients[j]->m_strName.Find(strUser, 0) == 0){

			CString strTarget;
			if(m_aClients[j]->m_strJoinSuffix.IsEmpty()){

				strTarget = sub->m_strRoomFull;
			}
			else{

				strTarget = sub->m_strRoomBase + m_aClients[j]->m_strJoinSuffix;
			}
			m_aClients[j]->SendRedirect(strTarget);
		}
	}
	return TRUE;
}

void CMainFrame::OnFileCopychannelname() 
{

	CClipboard::SetText((LPTSTR)(LPCTSTR)m_strRoomFull);
}

void CMainFrame::OnFileCopyloopbackname() 
{

	CString strLoopback;
	strLoopback.Format("%s_0100007F%4X", m_strRoomBase, m_wPort);
	CClipboard::SetText((LPTSTR)(LPCTSTR)strLoopback);
}

void CMainFrame::SetRoom(const CString strName)
{

	if(g_sSettings.GetListed()){

		g_rLister.ChangeName(m_strRoomBase, strName);
	}
	CClientManager::SetRoom(strName);
}
