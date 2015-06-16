// LoginCfg.cpp : implementation file
//

#include "stdafx.h"
#include "roboServ.h"
#include "LoginCfg.h"
#include "Settings.h"
#include "ClientSocket.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSettings g_sSettings;
extern UINT UWM_SETTINGS;

/////////////////////////////////////////////////////////////////////////////
// CLoginCfg dialog

IMPLEMENT_DYNCREATE(CLoginCfg, CExtResizablePropertyPage)

CLoginCfg::CLoginCfg()
	: CExtResizablePropertyPage(CLoginCfg::IDD)
{
	//{{AFX_DATA_INIT(CLoginCfg)
	m_strPassword = _T("");
	m_bAdmin = FALSE;
	m_bAgent = FALSE;
	m_bVoice = FALSE;
	m_bMain = FALSE;
	m_bSub = FALSE;
	m_bDefAdmin = FALSE;
	m_bDefAgent = FALSE;
	m_bDefVoice = FALSE;
	//}}AFX_DATA_INIT
}


void CLoginCfg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizablePropertyPage::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CLoginCfg)
	DDX_Control(pDX, IDC_STATIC_B, m_wndB);
	DDX_Control(pDX, IDC_STATIC_A, m_wndA);
	DDX_Control(pDX, IDC_VOICE_DEFAULT, m_wndDefVoice);
	DDX_Control(pDX, IDC_AGENT_DEFAULT, m_wndDefAgent);
	DDX_Control(pDX, IDC_ADMIN_DEFAULT, m_wndDefAdmin);
	DDX_Control(pDX, IDC_VALID_SUB, m_btSub);
	DDX_Control(pDX, IDC_VALID_MAIN, m_btMain);
	DDX_Control(pDX, IDC_PASSWORD, m_ePassword);
	DDX_Control(pDX, IDC_LOGINS, m_lcLogin);
	DDX_Control(pDX, IDC_VOICE, m_btVoice);
	DDX_Control(pDX, IDC_AGENT, m_btAgent);
	DDX_Control(pDX, IDC_ADMIN, m_btAdmin);
	DDX_Control(pDX, IDC_EDIT, m_btEdit);
	DDX_Control(pDX, IDC_DELETE, m_btDelete);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_ADD, m_btAdd);
	DDX_Text(pDX, IDC_PASSWORD, m_strPassword);
	DDX_Check(pDX, IDC_ADMIN, m_bAdmin);
	DDX_Check(pDX, IDC_AGENT, m_bAgent);
	DDX_Check(pDX, IDC_VOICE, m_bVoice);
	DDX_Check(pDX, IDC_VALID_MAIN, m_bMain);
	DDX_Check(pDX, IDC_VALID_SUB, m_bSub);
	DDX_Check(pDX, IDC_ADMIN_DEFAULT, m_bDefAdmin);
	DDX_Check(pDX, IDC_AGENT_DEFAULT, m_bDefAgent);
	DDX_Check(pDX, IDC_VOICE_DEFAULT, m_bDefVoice);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CLoginCfg, CExtResizablePropertyPage)
	//{{AFX_MSG_MAP(CLoginCfg)
	ON_REGISTERED_MESSAGE(UWM_SETTINGS, OnLoad)
	ON_BN_CLICKED(IDC_ADD, OnAdd)
	ON_BN_CLICKED(IDC_EDIT, OnEdit)
	ON_BN_CLICKED(IDC_DELETE, OnDelete)
	ON_BN_CLICKED(IDC_ADMIN_DEFAULT, OnAdminDefault)
	ON_BN_CLICKED(IDC_AGENT_DEFAULT, OnAgentDefault)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CLoginCfg message handlers

BOOL CLoginCfg::OnInitDialog() 
{
	CExtResizablePropertyPage::OnInitDialog();
	
	m_btVoice.m_nBitmapResource		 = IDB_BOX_IMAGES_XP_BLUE;
	m_btAgent.m_nBitmapResource		 = IDB_BOX_IMAGES_XP_BLUE;
	m_btAdmin.m_nBitmapResource		 = IDB_BOX_IMAGES_XP_BLUE;
	m_btSub.m_nBitmapResource		 = IDB_BOX_IMAGES_XP_BLUE;
	m_btMain.m_nBitmapResource		 = IDB_BOX_IMAGES_XP_BLUE;
	m_wndDefVoice.m_nBitmapResource	 = IDB_BOX_IMAGES_XP_BLUE;
	m_wndDefAgent.m_nBitmapResource	 = IDB_BOX_IMAGES_XP_BLUE;
	m_wndDefAdmin.m_nBitmapResource	 = IDB_BOX_IMAGES_XP_BLUE;
	
	m_lcLogin.SetExtendedStyle(LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_FLATSB);

	m_lcLogin.InsertColumn(0, "Login", LVCFMT_LEFT, 80);
	m_lcLogin.InsertColumn(1, "Main", LVCFMT_CENTER, 50);
	m_lcLogin.InsertColumn(2, "Sub", LVCFMT_CENTER, 50);
	m_lcLogin.InsertColumn(3, "+", LVCFMT_CENTER, 30);
	m_lcLogin.InsertColumn(4, "#", LVCFMT_CENTER, 30);
	m_lcLogin.InsertColumn(5, "@", LVCFMT_CENTER, 130);

	m_lcLogin.ModifyStyleEx(WS_EX_CLIENTEDGE, WS_EX_STATICEDGE, 0);
	m_lcLogin.Invalidate();
	
	m_lcLogin.SetColumnWidth(5, 30);
	Load();	

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CLoginCfg::OnAdd() 
{

	if(!UpdateData(TRUE)) return;

	if(m_strPassword.IsEmpty()){
		
		AfxMessageBox(IDS_PASSWORDEMPTY, MB_ICONINFORMATION+MB_OK);
		return;
	}

	if(!m_bMain && !m_bSub){

		AfxMessageBox(IDS_NOCHANNELLOGIN, MB_ICONINFORMATION+MB_OK);
		return;
	}

	if(!m_bVoice && !m_bAgent && !m_bAdmin){

		AfxMessageBox(IDS_NOUSERMODE, MB_ICONINFORMATION+MB_OK);
		return;
	}

	for(int i = 0; i < m_lcLogin.GetItemCount(); i++){

		if(m_lcLogin.GetItemText(i, 0) == m_strPassword){

			AfxMessageBox(IDS_LOGINEXISTS, MB_ICONINFORMATION+MB_OK);
			return;
		}
	}

	int nPos = m_lcLogin.InsertItem(0, m_strPassword);
	if(nPos < 0) return;

	m_lcLogin.SetItemText(nPos, 1, m_bMain  ? "x" : "");
	m_lcLogin.SetItemText(nPos, 2, m_bSub   ? "x" : "");
	m_lcLogin.SetItemText(nPos, 3, m_bVoice ? "x" : "");
	m_lcLogin.SetItemText(nPos, 4, m_bAgent ? "x" : "");
	m_lcLogin.SetItemText(nPos, 5, m_bAdmin ? "x" : "");
	
	UINT uMode = 0;
	if(m_bMain) uMode |= LOGIN_MAIN;
	if(m_bSub) uMode |= LOGIN_SUB;
	if(m_bVoice) uMode |= UM_VOICED;
	if(m_bAgent) uMode |= UM_AGENT;
	if(m_bAdmin) uMode |= UM_ADMIN;

	g_sSettings.AddLogin(m_strPassword, uMode);

	m_strPassword.Empty();
	m_bMain = FALSE;
	m_bSub  = FALSE;
	m_bVoice = FALSE;
	m_bAgent = FALSE;
	m_bAdmin = FALSE;

	UpdateData(FALSE);

}

void CLoginCfg::OnEdit() 
{

	int nPos = m_lcLogin.GetNextItem(-1, LVNI_SELECTED);
	if(nPos < 0){

		AfxMessageBox(IDS_NOSELECTION, MB_ICONINFORMATION+MB_OK);
		return;
	}

	m_strPassword = m_lcLogin.GetItemText(nPos, 0);
	m_bMain       = m_lcLogin.GetItemText(nPos, 1) == "x";
	m_bSub        = m_lcLogin.GetItemText(nPos, 2) == "x";
	m_bVoice      = m_lcLogin.GetItemText(nPos, 3) == "x";
	m_bAgent      = m_lcLogin.GetItemText(nPos, 4) == "x";
	m_bAdmin      = m_lcLogin.GetItemText(nPos, 5) == "x";

	m_lcLogin.DeleteItem(nPos);
	UpdateData(FALSE);
}

void CLoginCfg::OnDelete() 
{

	int nPos = m_lcLogin.GetNextItem(-1, LVNI_SELECTED);
	if(nPos < 0){

		AfxMessageBox(IDS_NOSELECTION, MB_ICONINFORMATION+MB_OK);
		return;
	}

	if(AfxMessageBox(IDS_DELETELOGIN, MB_ICONQUESTION+MB_YESNO) == IDYES){

		g_sSettings.DeleteLogin(m_lcLogin.GetItemText(nPos, 0));
		m_lcLogin.DeleteItem(nPos);
	}
}

void CLoginCfg::OnCancel() 
{

}

void CLoginCfg::OnOK() 
{

	if(!UpdateData(TRUE)) return;

	UINT uMode = UM_NORMAL;
	if(m_bDefVoice) uMode |= UM_VOICED;
	if(m_bDefAdmin) uMode |= UM_ADMIN;
	if(m_bDefAgent) uMode |= UM_AGENT;
	g_sSettings.SetDefaultUserMode(uMode);

	g_sSettings.Save();

	CWnd* pWnd = AfxGetApp()->m_pMainWnd;
	if(pWnd){

		pWnd->SendMessage(UWM_SETTINGS, 0, 0);
	}
}


void CLoginCfg::Load()
{

	m_lcLogin.DeleteAllItems();
	POSITION pos = g_sSettings.GetLoginIterator();

	while(pos){

		CString strPass;
		UINT uMode;

		g_sSettings.IterateLogins(pos, strPass, uMode);

		if(!strPass.IsEmpty() && uMode != 0){

			int nPos = m_lcLogin.InsertItem(0, strPass);
			if(nPos < 0) continue; //error

			m_lcLogin.SetItemText(nPos, 1, ((uMode & LOGIN_MAIN) == LOGIN_MAIN) ? "x" : "");
			m_lcLogin.SetItemText(nPos, 2, ((uMode & LOGIN_SUB) == LOGIN_SUB) ? "x" : "");
			m_lcLogin.SetItemText(nPos, 3, ((uMode & UM_VOICED) == UM_VOICED) ? "x" : "");
			m_lcLogin.SetItemText(nPos, 4, ((uMode & UM_AGENT) == UM_AGENT) ? "x" : "");
			m_lcLogin.SetItemText(nPos, 5, ((uMode & UM_ADMIN) == UM_ADMIN) ? "x" : "");
		}
	}

	UINT uMode = g_sSettings.GetDefaultUserMode();

	if((uMode & UM_VOICED) == UM_VOICED) m_bDefVoice = TRUE;
	if((uMode & UM_ADMIN) == UM_ADMIN) m_bDefAdmin = TRUE;
	if((uMode & UM_AGENT) == UM_AGENT) m_bDefAgent = TRUE;

	UpdateData(FALSE);

}

void CLoginCfg::OnLoad(WPARAM wParam, LPARAM lParam)
{

	if(lParam == 1){

		OnOK();
	}
	else{

		Load();
	}
}

void CLoginCfg::OnAdminDefault() 
{

	UpdateData(TRUE);
	if(m_bDefAdmin){

		if(AfxMessageBox(IDS_USERLEVELWARNING, MB_ICONQUESTION+MB_YESNO) == IDNO){

			m_bDefAdmin = FALSE;
			UpdateData(FALSE);
		}
	}
}

void CLoginCfg::OnAgentDefault() 
{

	UpdateData(TRUE);
	if(m_bDefAgent){

		if(AfxMessageBox(IDS_USERLEVELWARNING, MB_ICONQUESTION+MB_YESNO) == IDNO){

			m_bDefAgent = FALSE;
			UpdateData(FALSE);
		}
	}
}
