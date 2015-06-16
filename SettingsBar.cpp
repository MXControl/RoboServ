// SettingsBar.cpp : implementation file
//

#include "stdafx.h"
#include "roboServ.h"
#include "SettingsBar.h"
#include ".\settingsbar.h"
#include "Settings.h"
#include "util.h"
#include "ClientManager.h"

extern CSettings g_sSettings;
extern UINT UWM_SETTINGS;
// CSettingsBar dialog

IMPLEMENT_DYNCREATE(CSettingsBar, CExtResizablePropertyPage)

CSettingsBar::CSettingsBar()
	: CExtResizablePropertyPage(CSettingsBar::IDD)
{
	//{{AFX_DATA_INIT(CSettingsBar)
	m_strGodName	= "";
	m_dwGodFiles	= 0;
	m_nLine			= 0;
	m_bRoomListed	= 0;
	m_nColor		= 1;
	m_bGodVisible	= FALSE;
}	//}}AFX_DATA_INIT


CSettingsBar::~CSettingsBar()
{
}

void CSettingsBar::DoDataExchange(CDataExchange* pDX)
{
	CExtResizablePropertyPage::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(CSettingsBar)
	DDX_Text(pDX, IDC_WELCOME, m_strWelcome);
	DDX_Text(pDX, IDC_TOPIC, m_strTopic);
	DDX_Text(pDX, IDC_MOTD, m_strMotd);
	DDX_Text(pDX, IDC_KEYMSG, m_strKeyMsg);
	DDX_Text(pDX, IDC_PASSWORD, m_strKeyWord);
	DDX_Text(pDX, IDC_GOD_NAME, m_strGodName);
	DDX_Check(pDX, IDC_VISIBLE, m_bGodVisible);
	DDX_Check(pDX, IDC_LISTED, m_bRoomListed);
	DDX_Text(pDX, IDC_GOD_FILES, m_dwGodFiles);
	DDX_CBIndex(pDX, IDC_GOD_LINE, m_nLine);
	DDX_CBIndex(pDX, IDC_COLOR_PERMISSION, m_nColor);
	DDV_MaxChars(pDX, m_strGodName, 44);
	DDV_MinMaxDWord(pDX, m_dwGodFiles, 0, 65535);
	
	DDX_Control(pDX, IDC_COLOR_PERMISSION, m_cbColor);
	DDX_Control(pDX, IDC_GOD_NAME, m_eGodName);
	DDX_Control(pDX, IDC_VISIBLE, m_btGodVisible);
	DDX_Control(pDX, IDC_GOD_FILES, m_eGodFiles);
	DDX_Control(pDX, IDC_GOD_LINE, m_cbLine);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_LISTED, m_btRoomListed);
	DDX_Control(pDX, IDC_WELCOME, m_eWelcome);
	DDX_Control(pDX, IDC_TOPIC, m_eTopic);
	DDX_Control(pDX, IDC_MOTD, m_eMotd);
	DDX_Control(pDX, IDC_KEYMSG, m_eKeyMsg);
	DDX_Control(pDX, IDC_PASSWORD, m_eKeyWord);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSettingsBar, CExtResizablePropertyPage)
	//{{AFX_MSG_MAP(CSettingsBar)
	ON_BN_CLICKED(IDOK, OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, OnBnClickedCancel)
	ON_REGISTERED_MESSAGE(UWM_SETTINGS, OnLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


// CSettingsBar message handlers

BOOL CSettingsBar::OnInitDialog(void)
{

	CExtResizablePropertyPage::OnInitDialog();

	m_btRoomListed.m_nBitmapResource = IDB_BOX_IMAGES_XP_BLUE;
	m_btGodVisible.m_nBitmapResource = IDB_BOX_IMAGES_XP_BLUE;
	
	m_cbColor.AddString(Util::LoadString(IDS_COLORS_DISABLE));
	m_cbColor.AddString(Util::LoadString(IDS_COLORS_EVERYONE));
	m_cbColor.AddString(Util::LoadString(IDS_COLORS_VOICED));
	m_cbColor.AddString(Util::LoadString(IDS_COLORS_ADMIN));
	m_cbColor.AddString(Util::LoadString(IDS_COLORS_AGENTS));

	m_cbLine.AddString("Unknown");
	m_cbLine.AddString("14.4K");
	m_cbLine.AddString("28.8K");
	m_cbLine.AddString("33.6K");
	m_cbLine.AddString("56K");
	m_cbLine.AddString("64K ISDN");
	m_cbLine.AddString("128K ISDN");
	m_cbLine.AddString("Cable");
	m_cbLine.AddString("DSL");
	m_cbLine.AddString("T1");
	m_cbLine.AddString("T3");

	Load();

	return TRUE;
}

void CSettingsBar::OnBnClickedOk()
{

	UpdateData(TRUE);

	m_strKeyMsg.Replace("\r\n", "\\n");
	m_strMotd.Replace("\r\n", "\\n");
	g_sSettings.SetGodName(m_strGodName);
	g_sSettings.SetGodVisible(m_bGodVisible);
	g_sSettings.SetGodFiles(m_dwGodFiles);
	g_sSettings.SetGodLine(m_nLine);
	g_sSettings.SetListed(m_bRoomListed);
	g_sSettings.SetTopic(m_strTopic);
	g_sSettings.SetMotd(m_strMotd);
	g_sSettings.SetKeyMsg(m_strKeyMsg);
	g_sSettings.SetWelcome(m_strWelcome);
	g_sSettings.SetColor(m_nColor);
	g_sSettings.SetKeyword(m_strKeyWord);
	
	UINT uMode = g_sSettings.GetChannelMode();
	if(m_strKeyWord.GetLength() && (uMode&CM_KEYWORD != CM_KEYWORD)){

		uMode |= CM_KEYWORD;
	}
	else if(m_strKeyWord.IsEmpty() && (uMode&CM_KEYWORD == CM_KEYWORD)){

		uMode &= ~CM_KEYWORD;
	}
	g_sSettings.SetChannelMode(uMode);
	g_sSettings.Save();

	CWnd* pWnd = AfxGetApp()->m_pMainWnd;
	if(pWnd){

		pWnd->SendMessage(UWM_SETTINGS, 0, 0);
	}
}

void CSettingsBar::OnBnClickedCancel()
{

	UpdateData(FALSE);
}

void CSettingsBar::Load()
{

	m_strGodName	= g_sSettings.GetGodName();
	m_bGodVisible	= g_sSettings.GetGodVisible();
	m_dwGodFiles	= g_sSettings.GetGodFiles();
	m_nLine			= g_sSettings.GetGodLine();
	m_bRoomListed	= g_sSettings.GetListed();
	m_strTopic		= g_sSettings.GetTopic();
	m_strMotd		= g_sSettings.GetMotd();
	m_strKeyMsg		= g_sSettings.GetKeyMsg();
	m_strWelcome	= g_sSettings.GetWelcome();
	m_nColor		= g_sSettings.GetColor();
	m_strKeyWord	= g_sSettings.GetKeyword();
	m_strKeyMsg.Replace("\\n", "\r\n");
	m_strMotd.Replace("\\n", "\r\n");
	UpdateData(FALSE);
}

void CSettingsBar::OnLoad(WPARAM wParam, LPARAM lParam)
{

	if(lParam == 1){

		OnOK();
	}
	else{

		Load();
	}
}