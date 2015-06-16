// SubChannelCfg.cpp : implementation file
//

#include "stdafx.h"
#include "roboServ.h"
#include "SubChannelCfg.h"
#include "ClientSocket.h"
#include "ClientManager.h"
#include "Settings.h"
#include "Util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CSettings g_sSettings;

/////////////////////////////////////////////////////////////////////////////
// CSubChannelCfg dialog
extern UINT UWM_SETTINGS;

IMPLEMENT_DYNCREATE(CSubChannelCfg, CExtResizablePropertyPage)

CSubChannelCfg::CSubChannelCfg()
	: CExtResizablePropertyPage(CSubChannelCfg::IDD)
{
	//{{AFX_DATA_INIT(CSubChannelCfg)
	m_bEnable = FALSE;
	m_bLimit = FALSE;
	m_uMax = 0;
	m_nSubLevel = -1;
	m_bAutoClose = FALSE;
	m_bDisplaySubs = FALSE;
	m_bFirstIsSuper = FALSE;
	m_bSubList = FALSE;
	//}}AFX_DATA_INIT
}


void CSubChannelCfg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizablePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSubChannelCfg)
	DDX_Control(pDX, IDC_SUBLIST, m_btSubList);
	DDX_Control(pDX, IDC_STATIC_A, m_wndA);
	DDX_Control(pDX, IDC_SUB_FIRSTISSUPER, m_wndFirstIsSuper);
	DDX_Control(pDX, IDC_DISPLAY_MESSAGES, m_btDisplaySubs);
	DDX_Control(pDX, IDC_SUB_CREATE_LEVEL, m_cbSubLevel);
	DDX_Control(pDX, IDC_MAX_SUBS, m_eMaxSubs);
	DDX_Control(pDX, IDC_LIMIT_NUMBER_SUBS, m_btLimitSubs);
	DDX_Control(pDX, IDC_ENABLE_SUBS, m_btEnableSubs);
	DDX_Control(pDX, IDC_AUTOCLOSE, m_btAutoClose);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Check(pDX, IDC_ENABLE_SUBS, m_bEnable);
	DDX_Check(pDX, IDC_LIMIT_NUMBER_SUBS, m_bLimit);
	DDX_Text(pDX, IDC_MAX_SUBS, m_uMax);
	DDV_MinMaxUInt(pDX, m_uMax, 0, 20);
	DDX_CBIndex(pDX, IDC_SUB_CREATE_LEVEL, m_nSubLevel);
	DDX_Check(pDX, IDC_AUTOCLOSE, m_bAutoClose);
	DDX_Check(pDX, IDC_DISPLAY_MESSAGES, m_bDisplaySubs);
	DDX_Check(pDX, IDC_SUB_FIRSTISSUPER, m_bFirstIsSuper);
	DDX_Check(pDX, IDC_SUBLIST, m_bSubList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSubChannelCfg, CExtResizablePropertyPage)
	//{{AFX_MSG_MAP(CSubChannelCfg)
	ON_BN_CLICKED(IDC_ENABLE_SUBS, OnEnableSubs)
	ON_REGISTERED_MESSAGE(UWM_SETTINGS, OnLoad)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSubChannelCfg message handlers

void CSubChannelCfg::OnEnableSubs() 
{

	UpdateData(TRUE);
	GetDlgItem(IDC_SUB_CREATE_LEVEL)->EnableWindow(m_bEnable);
	GetDlgItem(IDC_AUTOCLOSE)->EnableWindow(m_bEnable);
	GetDlgItem(IDC_LIMIT_NUMBER_SUBS)->EnableWindow(m_bEnable);
	GetDlgItem(IDC_MAX_SUBS)->EnableWindow(m_bEnable);
	GetDlgItem(IDC_DISPLAY_MESSAGES)->EnableWindow(m_bEnable);
	GetDlgItem(IDC_SUB_FIRSTISSUPER)->EnableWindow(m_bEnable);
	GetDlgItem(IDC_STATIC_A)->EnableWindow(m_bEnable);
	GetDlgItem(IDC_SUBLIST)->EnableWindow(m_bEnable);
}	

void CSubChannelCfg::OnCancel() 
{

	Load();
}

void CSubChannelCfg::OnOK() 
{

	UpdateData(TRUE);

	g_sSettings.SetSubFirstIsSupoer(m_bFirstIsSuper);
	g_sSettings.SetAutoCloseSub(m_bAutoClose);
	g_sSettings.SetLimitSub(m_uMax);


	UINT uMode = 0;


	uMode = g_sSettings.GetChannelMode();
	if(m_bEnable){

		uMode |= CM_SUBCHANNELS;
	}
	else{

		uMode &= ~CM_SUBCHANNELS;
	}
	g_sSettings.SetChannelMode(uMode);

	switch(m_nSubLevel){

	case 0:  // everyone
		uMode = UM_NORMAL;
		break;
	case 1:	// voice admin agent
		uMode = UM_ADMIN|UM_AGENT|UM_VOICED;
		break;
	case 2: // admin agent
		uMode = UM_ADMIN|UM_AGENT;
		break;
	case 3:// admin
		uMode = UM_ADMIN;
		break;
	default: // host
		uMode = UM_HOST;
		break;
	}
	g_sSettings.SetSubPermission(uMode);
	g_sSettings.SetDisplaySubText(m_bDisplaySubs);
	g_sSettings.SetSubListed(m_bSubList);

	g_sSettings.Save();
	CWnd* pWnd = AfxGetApp()->m_pMainWnd;
	if(pWnd){

		pWnd->SendMessage(UWM_SETTINGS, 0, 0);
	}
}


BOOL CSubChannelCfg::OnInitDialog() 
{

	CExtResizablePropertyPage::OnInitDialog();
	
	m_btSubList.m_nBitmapResource		= IDB_BOX_IMAGES_XP_BLUE;
	m_btLimitSubs.m_nBitmapResource		= IDB_BOX_IMAGES_XP_BLUE;
	m_btEnableSubs.m_nBitmapResource	= IDB_BOX_IMAGES_XP_BLUE;
	m_btAutoClose.m_nBitmapResource		= IDB_BOX_IMAGES_XP_BLUE;
	m_btDisplaySubs.m_nBitmapResource   = IDB_BOX_IMAGES_XP_BLUE;
	m_wndFirstIsSuper.m_nBitmapResource	= IDB_BOX_IMAGES_XP_BLUE;
	
	m_cbSubLevel.AddString(Util::LoadString(IDS_SUB_EVERYONE));
	m_cbSubLevel.AddString(Util::LoadString(IDS_SUB_VOICED));
	m_cbSubLevel.AddString(Util::LoadString(IDS_SUB_ADMIN));
	m_cbSubLevel.AddString(Util::LoadString(IDS_SUB_AGENTS));
	m_cbSubLevel.AddString(Util::LoadString(IDS_SUB_HOST));

	Load();
	
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CSubChannelCfg::Load()
{

	m_bEnable		= (g_sSettings.GetChannelMode() & CM_SUBCHANNELS) == CM_SUBCHANNELS;
	m_bAutoClose	= g_sSettings.GetAutoCloseSub();
	m_uMax			= g_sSettings.GetLimitSub();
	m_bLimit		= m_uMax > 0;
	m_bDisplaySubs	= g_sSettings.GetDisplaySubText();
	m_bFirstIsSuper = g_sSettings.GetSubFirstIsSuper();
	m_bSubList		= g_sSettings.GetSubListed();

	UINT uMode = g_sSettings.GetSubPermission();

	if(uMode == UM_NORMAL){

		m_nSubLevel = 0;
	}
	else if(((uMode & UM_ADMIN) == UM_ADMIN) && ((uMode & UM_AGENT) == UM_AGENT) && ((uMode & UM_VOICED) == UM_VOICED)){

		m_nSubLevel = 1;
	}
	else if(((uMode & UM_ADMIN) == UM_ADMIN) && ((uMode & UM_AGENT) == UM_AGENT)){

		m_nSubLevel = 2;
	}
	else if((uMode & UM_ADMIN) == UM_ADMIN){

		m_nSubLevel = 3;
	}
	else{

		m_nSubLevel = 4;
	}

	UpdateData(FALSE);

	OnEnableSubs();
}

void CSubChannelCfg::OnLoad(WPARAM wParam, LPARAM lParam)
{

	if(lParam == 1){

		OnOK();
	}
	else{

		Load();
	}
}
