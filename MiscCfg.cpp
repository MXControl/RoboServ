// MiscCfg.cpp : implementation file
//

#include "stdafx.h"
#include "roboServ.h"
#include "MiscCfg.h"
#include "Util.h"
#include "Settings.h"
#include "ClientSocket.h"
#include "ClientManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMiscCfg property page
extern CSettings g_sSettings;
extern UINT UWM_SETTINGS;

IMPLEMENT_DYNCREATE(CMiscCfg, CExtResizablePropertyPage)

CMiscCfg::CMiscCfg() : CExtResizablePropertyPage(CMiscCfg::IDD)
{
	//{{AFX_DATA_INIT(CMiscCfg)
	m_nAdminWatch = -1;
	m_nVoiceWatch = -1;
	m_bAdvertiseRose = TRUE;
	m_bMultiIPOk	 = TRUE;
	m_bLocalIsOp	 = TRUE;
	m_bBlockNushi	 = FALSE;
	m_bModerated	 = FALSE;
	//}}AFX_DATA_INIT
	m_nNormalIcon = 0;
	m_nVoiceIcon  = 2;
	m_nAdminIcon  = 1;
	m_nAgentIcon  = 1;
}

CMiscCfg::~CMiscCfg()
{
}

void CMiscCfg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizablePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CMiscCfg)
	DDX_Control(pDX, IDC_STATIC_I, m_wndI);
	DDX_Control(pDX, IDC_STATIC_H, m_wndH);
	DDX_Control(pDX, IDC_STATIC_G, m_wndG);
	DDX_Control(pDX, IDC_MODERATED, m_wndModerated);
	DDX_Control(pDX, IDC_STATIC_F, m_wndF);
	DDX_Control(pDX, IDC_STATIC_E, m_wndE);
	DDX_Control(pDX, IDC_STATIC_D, m_wndD);
	DDX_Control(pDX, IDC_STATIC_C, m_wndC);
	DDX_Control(pDX, IDOK, m_btnOK);
	DDX_Control(pDX, IDCANCEL, m_btnCancel);
	DDX_Control(pDX, IDC_VOICE_WATCH, m_wndVoiceWatch);
	DDX_Control(pDX, IDC_STATIC_B, m_wndB);
	DDX_Control(pDX, IDC_STATIC_A, m_wndA);
	DDX_Control(pDX, IDC_ADMIN_WATCH, m_wndAdminWatch);
	DDX_Control(pDX, IDC_SINGLEUSER_PER_IP, m_btMultiIPOk);
	DDX_Control(pDX, IDC_AUTO_OP_LOCALHOST, m_btLocalIsOp);
	DDX_Control(pDX, IDC_BLOCK_MX331, m_btBlockNushi);
	DDX_Control(pDX, IDC_ADVERTISE, m_btAdvertiseRose);
	DDX_Control(pDX, IDC_STATIC_J, m_wndJ);
	DDX_CBIndex(pDX, IDC_ADMIN_WATCH, m_nAdminWatch);
	DDX_CBIndex(pDX, IDC_VOICE_WATCH, m_nVoiceWatch);
	DDX_Control(pDX, IDC_NO_NORMAL, m_btNoNormal);
	DDX_Control(pDX, IDC_PLUS_NORMAL, m_btPlusNormal);
	DDX_Control(pDX, IDC_AT_NORMAL, m_btAtNormal);
	DDX_Control(pDX, IDC_NO_VOICE, m_btNoVoiced);
	DDX_Control(pDX, IDC_PLUS_VOICE, m_btPlusVoiced);
	DDX_Control(pDX, IDC_AT_VOICE, m_btAtVoiced);
	DDX_Control(pDX, IDC_NO_ADMIN, m_btNoAdmin);
	DDX_Control(pDX, IDC_PLUS_ADMIN, m_btPlusAdmin);
	DDX_Control(pDX, IDC_AT_ADMIN, m_btAtAdmin);
	DDX_Control(pDX, IDC_NO_AGENT, m_btNoAgent);
	DDX_Control(pDX, IDC_PLUS_AGENT, m_btPlusAgent);
	DDX_Control(pDX, IDC_AT_AGENT, m_btAtAgent);
	DDX_Check(pDX, IDC_ADVERTISE, m_bAdvertiseRose);
	DDX_Check(pDX, IDC_SINGLEUSER_PER_IP, m_bMultiIPOk);
	DDX_Check(pDX, IDC_AUTO_OP_LOCALHOST, m_bLocalIsOp);
	DDX_Check(pDX, IDC_BLOCK_MX331, m_bBlockNushi);
	DDX_Check(pDX, IDC_MODERATED, m_bModerated);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CMiscCfg, CExtResizablePropertyPage)
	//{{AFX_MSG_MAP(CMiscCfg)
	ON_REGISTERED_MESSAGE(UWM_SETTINGS, OnLoad)
	ON_BN_CLICKED(IDC_ADVERTISE, OnAdvertise)
	ON_BN_CLICKED(IDC_NO_NORMAL, OnNoNormal)
	ON_BN_CLICKED(IDC_PLUS_NORMAL, OnPlusNormal)
	ON_BN_CLICKED(IDC_AT_NORMAL, OnAtNormal)
	ON_BN_CLICKED(IDC_NO_VOICE, OnNoVoice)
	ON_BN_CLICKED(IDC_PLUS_VOICE, OnPlusVoice)
	ON_BN_CLICKED(IDC_AT_VOICE, OnAtVoice)
	ON_BN_CLICKED(IDC_NO_ADMIN, OnNoAdmin)
	ON_BN_CLICKED(IDC_PLUS_ADMIN, OnPlusAdmin)
	ON_BN_CLICKED(IDC_AT_ADMIN, OnAtAdmin)
	ON_BN_CLICKED(IDC_NO_AGENT, OnNoAgent)
	ON_BN_CLICKED(IDC_PLUS_AGENT, OnPlusAgent)
	ON_BN_CLICKED(IDC_AT_AGENT, OnAtAgent)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMiscCfg message handlers

BOOL CMiscCfg::OnInitDialog() 
{
	CExtResizablePropertyPage::OnInitDialog();
	
	m_btMultiIPOk.m_nBitmapResource		= IDB_BOX_IMAGES_XP_BLUE;
	m_btLocalIsOp.m_nBitmapResource		= IDB_BOX_IMAGES_XP_BLUE;
	m_btBlockNushi.m_nBitmapResource	= IDB_BOX_IMAGES_XP_BLUE;
	m_btAdvertiseRose.m_nBitmapResource = IDB_BOX_IMAGES_XP_BLUE;
	m_wndModerated.m_nBitmapResource	= IDB_BOX_IMAGES_XP_BLUE;
	m_btNoNormal.m_nBitmapResource		= IDB_RADIOBOX_IMAGES_XP_BLUE;
	m_btPlusNormal.m_nBitmapResource	= IDB_RADIOBOX_IMAGES_XP_BLUE;
	m_btAtNormal.m_nBitmapResource		= IDB_RADIOBOX_IMAGES_XP_BLUE;
	m_btNoVoiced.m_nBitmapResource		= IDB_RADIOBOX_IMAGES_XP_BLUE;
	m_btPlusVoiced.m_nBitmapResource	= IDB_RADIOBOX_IMAGES_XP_BLUE;
	m_btAtVoiced.m_nBitmapResource		= IDB_RADIOBOX_IMAGES_XP_BLUE;
	m_btNoAdmin.m_nBitmapResource		= IDB_RADIOBOX_IMAGES_XP_BLUE;
	m_btPlusAdmin.m_nBitmapResource		= IDB_RADIOBOX_IMAGES_XP_BLUE;
	m_btAtAdmin.m_nBitmapResource		= IDB_RADIOBOX_IMAGES_XP_BLUE;
	m_btNoAgent.m_nBitmapResource		= IDB_RADIOBOX_IMAGES_XP_BLUE;
	m_btPlusAgent.m_nBitmapResource		= IDB_RADIOBOX_IMAGES_XP_BLUE;
	m_btAtAgent.m_nBitmapResource		= IDB_RADIOBOX_IMAGES_XP_BLUE;

	m_wndAdminWatch.AddString(Util::LoadString(IDS_SUB_EVERYONE));
	m_wndAdminWatch.AddString(Util::LoadString(IDS_SUB_VOICED));
	m_wndAdminWatch.AddString(Util::LoadString(IDS_SUB_ADMIN));
	m_wndAdminWatch.AddString(Util::LoadString(IDS_SUB_AGENTS));
	m_wndAdminWatch.AddString(Util::LoadString(IDS_SUB_HOST));

	m_wndVoiceWatch.AddString(Util::LoadString(IDS_SUB_EVERYONE));
	m_wndVoiceWatch.AddString(Util::LoadString(IDS_SUB_VOICED));
	m_wndVoiceWatch.AddString(Util::LoadString(IDS_SUB_ADMIN));
	m_wndVoiceWatch.AddString(Util::LoadString(IDS_SUB_AGENTS));
	m_wndVoiceWatch.AddString(Util::LoadString(IDS_SUB_HOST));
		
	Load();
	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CMiscCfg::OnLoad(WPARAM wParam, LPARAM lParam)
{

	if(lParam == 1){

		OnOK();
	}
	else{

		Load();
	}
}

void CMiscCfg::Load()
{

	UINT uMode = g_sSettings.GetAdminWatch();

	if(uMode == UM_NORMAL){

		m_nAdminWatch = 0;
	}
	else if(((uMode & UM_ADMIN) == UM_ADMIN) && ((uMode & UM_AGENT) == UM_AGENT) && ((uMode & UM_VOICED) == UM_VOICED)){

		m_nAdminWatch = 1;
	}
	else if(((uMode & UM_ADMIN) == UM_ADMIN) && ((uMode & UM_AGENT) == UM_AGENT)){

		m_nAdminWatch = 2;
	}
	else if((uMode & UM_ADMIN) == UM_ADMIN){

		m_nAdminWatch = 3;
	}
	else{

		m_nAdminWatch = 4;
	}

	uMode = g_sSettings.GetVoiceWatch();

	if(uMode == UM_NORMAL){

		m_nVoiceWatch = 0;
	}
	else if(((uMode & UM_ADMIN) == UM_ADMIN) && ((uMode & UM_AGENT) == UM_AGENT) && ((uMode & UM_VOICED) == UM_VOICED)){

		m_nVoiceWatch = 1;
	}
	else if(((uMode & UM_ADMIN) == UM_ADMIN) && ((uMode & UM_AGENT) == UM_AGENT)){

		m_nVoiceWatch = 2;
	}
	else if((uMode & UM_ADMIN) == UM_ADMIN){

		m_nVoiceWatch = 3;
	}
	else{

		m_nVoiceWatch = 4;
	}


	m_nNormalIcon = g_sSettings.GetNormalIcon();
	m_btNoNormal.SetCheck(m_nNormalIcon == 0);
	m_btPlusNormal.SetCheck(m_nNormalIcon == 2);
	m_btAtNormal.SetCheck(m_nNormalIcon == 1);

	m_nVoiceIcon = g_sSettings.GetVoiceIcon();
	m_btNoVoiced.SetCheck(m_nVoiceIcon == 0);
	m_btPlusVoiced.SetCheck(m_nVoiceIcon == 2);
	m_btAtVoiced.SetCheck(m_nVoiceIcon == 1);

	m_nAdminIcon = g_sSettings.GetAdminIcon();
	m_btNoAdmin.SetCheck(m_nAdminIcon == 0);
	m_btPlusAdmin.SetCheck(m_nAdminIcon == 2);
	m_btAtAdmin.SetCheck(m_nAdminIcon == 1);

	m_nAgentIcon = g_sSettings.GetAgentIcon();
	m_btNoAgent.SetCheck(m_nAgentIcon == 0);
	m_btPlusAgent.SetCheck(m_nAgentIcon == 2);
	m_btAtAgent.SetCheck(m_nAgentIcon == 1);

	m_bModerated    = (g_sSettings.GetChannelMode() & CM_MODERATED) != 0;
	m_bMultiIPOk	= g_sSettings.GetMultiIPOk();
	m_bLocalIsOp	= g_sSettings.GetLocalIsOp();
	m_bBlockNushi	= g_sSettings.GetBlockNushi();
	m_bAdvertiseRose    = g_sSettings.GetAdvertiseRose();
	UpdateData(FALSE);
}

void CMiscCfg::OnOK() 
{

	UpdateData(TRUE);

	UINT uMode = 0;
	switch(m_nAdminWatch){

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
	g_sSettings.SetAdminWatch(uMode);
	
	uMode = 0;
	switch(m_nVoiceWatch){

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
	g_sSettings.SetVoiceWatch(uMode);

	g_sSettings.SetMultiIPOk(m_bMultiIPOk);
	g_sSettings.SetLocalIsOp(m_bLocalIsOp);
	g_sSettings.SetBlockNushi(m_bBlockNushi);
	g_sSettings.SetAdvertiseRose(m_bAdvertiseRose);

	g_sSettings.SetNormalIcon(m_nNormalIcon);
	g_sSettings.SetVoiceIcon(m_nVoiceIcon);
	g_sSettings.SetAdminIcon(m_nAdminIcon);
	g_sSettings.SetAgentIcon(m_nAgentIcon);

	uMode = g_sSettings.GetChannelMode();
	if(m_bModerated){

		uMode |= CM_MODERATED;
	}
	else{

		uMode &= ~CM_MODERATED;
	}
	g_sSettings.SetChannelMode(uMode);

	g_sSettings.Save();

	CWnd* pWnd = AfxGetApp()->m_pMainWnd;
	if(pWnd){

		pWnd->SendMessage(UWM_SETTINGS, 0, 0);
	}
}

void CMiscCfg::OnAdvertise()
{

	UpdateData(TRUE);
	if(!m_bAdvertiseRose){

		if(AfxMessageBox(IDS_ADVERTISE_ROSE, MB_ICONQUESTION+MB_YESNO) == IDNO){

			m_bAdvertiseRose = TRUE;
			UpdateData(FALSE);
		}
	}
}

void CMiscCfg::OnNoNormal() 
{

	m_nNormalIcon = 0;
	m_btPlusNormal.SetCheck(0);
	m_btAtNormal.SetCheck(0);
}

void CMiscCfg::OnPlusNormal() 
{

	m_nNormalIcon = 2;
	m_btNoNormal.SetCheck(0);
	m_btAtNormal.SetCheck(0);
}

void CMiscCfg::OnAtNormal() 
{

	m_nNormalIcon = 1;
	m_btPlusNormal.SetCheck(0);
	m_btNoNormal.SetCheck(0);
}

void CMiscCfg::OnNoVoice() 
{

	m_nNormalIcon = 0;
	m_btPlusNormal.SetCheck(0);
	m_btAtNormal.SetCheck(0);
}

void CMiscCfg::OnPlusVoice() 
{

	m_nVoiceIcon = 2;
	m_btNoVoiced.SetCheck(0);
	m_btAtVoiced.SetCheck(0);
}

void CMiscCfg::OnAtVoice() 
{

	m_nVoiceIcon = 1;
	m_btPlusVoiced.SetCheck(0);
	m_btNoVoiced.SetCheck(0);
}

void CMiscCfg::OnNoAdmin() 
{

	m_nAdminIcon = 0;
	m_btPlusAdmin.SetCheck(0);
	m_btAtAdmin.SetCheck(0);
}

void CMiscCfg::OnPlusAdmin() 
{

	m_nAdminIcon = 2;
	m_btNoAdmin.SetCheck(0);
	m_btAtAdmin.SetCheck(0);
}

void CMiscCfg::OnAtAdmin() 
{

	m_nAdminIcon = 1;
	m_btPlusAdmin.SetCheck(0);
	m_btNoAdmin.SetCheck(0);
}

void CMiscCfg::OnNoAgent() 
{

	m_nAgentIcon = 0;
	m_btPlusAgent.SetCheck(0);
	m_btAtAgent.SetCheck(0);
}

void CMiscCfg::OnPlusAgent() 
{

	m_nAgentIcon = 2;
	m_btNoAgent.SetCheck(0);
	m_btAtAgent.SetCheck(0);
}

void CMiscCfg::OnAtAgent() 
{

	m_nAgentIcon = 1;
	m_btPlusAgent.SetCheck(0);
	m_btNoAgent.SetCheck(0);
}
