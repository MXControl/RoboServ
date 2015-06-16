// OpenConfigDlg.cpp : implementation file
//

#include "stdafx.h"
#include "roboServ.h"
#include "OpenConfigDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COpenConfigDlg dialog


COpenConfigDlg::COpenConfigDlg(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(COpenConfigDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COpenConfigDlg)
	m_strURL = _T("");
	//}}AFX_DATA_INIT
}


void COpenConfigDlg::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COpenConfigDlg)
	DDX_Control(pDX, IDC_STATIC_1, m_wndText);
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_BROWSE, m_btBrowse);
	DDX_Control(pDX, IDC_URL, m_eURL);
	DDX_Text(pDX, IDC_URL, m_strURL);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COpenConfigDlg, CExtResizableDialog)
	//{{AFX_MSG_MAP(COpenConfigDlg)
	ON_BN_CLICKED(IDC_BROWSE, OnBrowse)
	ON_EN_CHANGE(IDC_URL, OnChangeUrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COpenConfigDlg message handlers

void COpenConfigDlg::OnOK() 
{
	// TODO: Add extra validation here
	
	CExtResizableDialog::OnOK();
}

void COpenConfigDlg::OnBrowse() 
{

	CString strFilter;
	strFilter.LoadString(IDS_SERVER_FILTER);

	CFileDialog dlg(TRUE, ".rsi", NULL, OFN_FILEMUSTEXIST, strFilter, this);

	if(dlg.DoModal() == IDOK){

		m_strURL = dlg.GetPathName();
		UpdateData(FALSE);
		OnChangeUrl();
	}
	
}

void COpenConfigDlg::OnChangeUrl() 
{

	UpdateData(TRUE);
	GetDlgItem(IDOK)->EnableWindow(m_strURL.GetLength());
}
