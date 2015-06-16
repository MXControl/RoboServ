// roboServ.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "roboServ.h"
#include "MainFrm.h"

#include <exdlgbase.h>
#include "roboServDoc.h"
#include "roboServView.h"
#include "settings.h"
#include "ini.h"
#include <extcontrolbar.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

CSettings g_sSettings;
// CroboServApp

BEGIN_MESSAGE_MAP(CroboServApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// CroboServApp construction

CroboServApp::CroboServApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
	m_hInstLang = NULL;
}


// The one and only CroboServApp object

CroboServApp theApp;

// CroboServApp initialization

BOOL CroboServApp::InitInstance()
{

	CWinApp::InitInstance();

	CIni ini;
	ini.SetIniFileName(g_sSettings.GetWorkingDir(FALSE) + "\\roboServ.ini");
	CString strLang;
	strLang.Format("lang_%s.dll",ini.GetValue("Language", "Active", "English"));

	if(strLang != "lang_English.dll"){

		m_hInstLang = LoadLibrary(strLang);
		if(m_hInstLang != NULL){

			AfxSetResourceHandle(m_hInstLang);
		}
		else{

			ini.SetValue("Language", "Active", "English");
			AfxMessageBox(strLang + " was not found. Defaulting to English", MB_ICONSTOP);
		}
	}
	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}
	AfxEnableControlContainer();

	// InitCommonControls() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	InitCommonControls();

	WSADATA wsaData;

	m_nWSA = WSAStartup(MAKEWORD(1, 1), &wsaData);
	
	if(m_nWSA){

		AfxMessageBox(IDS_ERROR_WINSOCK, MB_OK+MB_ICONSTOP);
		return FALSE;
	}

	if(	LOBYTE(wsaData.wVersion) != 1 || HIBYTE(wsaData.wVersion) != 1){
		
		AfxMessageBox(IDS_ERROR_WINSOCK, MB_OK+MB_ICONSTOP);
		return FALSE;
	}

	g_sSettings.Load();

	CExtPopupMenuWnd::g_bMenuWithShadows = true;
	CExtPopupMenuWnd::g_bMenuShowCoolTips = true;
	CExtPopupMenuWnd::g_bMenuExpanding = false;
	CExtPopupMenuWnd::g_bMenuHighlightRarely = true;
	CExtPopupMenuWnd::g_bMenuExpandAnimation = true;
	CExtPopupMenuWnd::g_bUseDesktopWorkArea = true;
	CExtPopupMenuWnd::g_DefAnimationType =
		CExtPopupMenuWnd::__AT_FADE;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("roboServ v1.1"));
	LoadStdProfileSettings(0);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CroboServDoc),
		RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		RUNTIME_CLASS(CroboServView));
	if (!pDocTemplate)
		return FALSE;
	AddDocTemplate(pDocTemplate);
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The one and only window has been initialized, so show and update it
	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}


// CAboutDlg dialog used for App About

class CAboutDlg : public CExtDlgBase
{
	bool m_bFirstShow;
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	CExtLabel m_wnd7;
	CExtLabel m_wnd6;
	CExtLabel m_wnd5;
	CExtLabel m_wnd4;
	CExtLabel m_wnd3;
	CExtLabel m_wnd2;
	CExtLabel m_wnd1;
	CExtHyperLinkButton	m_btProfui;
	CExtButton	m_BtnOK;
	CExtHyperLinkButton	m_wndMXControl;
	CExtHyperLinkButton	m_wndMXControlGreece;
	CExtHyperLinkButton	m_wndKM;
	CExtHyperLinkButton	m_wndNushi;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedOk();
};

CAboutDlg::CAboutDlg() : CExtDlgBase(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
	m_bFirstShow = true;
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtDlgBase::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	DDX_Control(pDX, IDC_STATIC_7, m_wnd7);
	DDX_Control(pDX, IDC_STATIC_6, m_wnd6);
	DDX_Control(pDX, IDC_STATIC_5, m_wnd5);
	DDX_Control(pDX, IDC_STATIC_4, m_wnd4);
	DDX_Control(pDX, IDC_STATIC_3, m_wnd3);
	DDX_Control(pDX, IDC_STATIC_2, m_wnd2);
	DDX_Control(pDX, IDC_STATIC_1, m_wnd1);
	DDX_Control(pDX, IDC_PROFS_UI, m_btProfui);
	DDX_Control(pDX, IDOK, m_BtnOK);
	DDX_Control(pDX, IDC_MXCONTROL_URL, m_wndMXControl);
	DDX_Control(pDX, IDC_ZHEN_URL, m_wndMXControlGreece);
	DDX_Control(pDX, IDC_KM_URL, m_wndKM);
	DDX_Control(pDX, IDC_NUSHI_URL, m_wndNushi);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CExtDlgBase)
	//{{AFX_MSG_MAP(CAboutDlg)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CExtDlgBase::OnInitDialog();

	EnableResizing( false );
	EnableSaveRestore( _T("ExtResizableDialog"), _T("AboutDlg") );
	
	m_wndMXControl.m_strURL = _T("http://mxcontrol.sourceforge.net/");
	m_wndMXControl.SetTooltipText( m_wndMXControl.m_strURL );

	m_wndMXControlGreece.m_strURL = _T("http://mxcontrolgreece.sourceforge.net/");
	m_wndMXControlGreece.SetTooltipText( m_wndMXControlGreece.m_strURL );

	m_wndKM.m_strURL = _T("http://www.kingmacro.net/");
	m_wndKM.SetTooltipText( m_wndKM.m_strURL );

	m_wndNushi.m_strURL = _T("http://2sen.dip.jp/");
	m_wndNushi.SetTooltipText( m_wndNushi.m_strURL );

	m_btProfui.m_strURL = _T("http://www.prof-uis.com");
	m_btProfui.SetTooltipText(m_btProfui.m_strURL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CAboutDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CExtDlgBase::OnShowWindow(bShow, nStatus);
	
	if( !bShow )
		return;
	if( !m_bFirstShow )
		return;
	m_bFirstShow = false;
}

void CAboutDlg::OnOK() 
{
	CExtDlgBase::OnOK();
}

void CAboutDlg::OnCancel() 
{
	CExtDlgBase::OnCancel();
}


void CroboServApp::OnAppAbout()
{

	CAboutDlg dlg;
	dlg.DoModal();
}
