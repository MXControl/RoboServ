// Update.cpp : implementation file
//

#include "stdafx.h"
#include "roboServ.h"
#include "Update.h"
#include "Settings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

extern CSettings g_sSettings;

/////////////////////////////////////////////////////////////////////////////
// CUpdate dialog


CUpdate::CUpdate(CWnd* pParent /*=NULL*/)
	: CExtResizableDialog(CUpdate::IDD, pParent)
{
	//{{AFX_DATA_INIT(CUpdate)
	m_strDetails = _T("");
	//}}AFX_DATA_INIT
	m_nSize = 0;
	m_nRead = 0;
}


void CUpdate::DoDataExchange(CDataExchange* pDX)
{
	CExtResizableDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CUpdate)
	DDX_Control(pDX, IDOK, m_btOK);
	DDX_Control(pDX, IDCANCEL, m_btCancel);
	DDX_Control(pDX, IDC_UPDATE_PROGRESS, m_wndProgress);
	DDX_Control(pDX, IDC_UPDATE_DETAILS, m_wndDetails);
	DDX_Control(pDX, IDC_STATIC_4, m_wnd4);
	DDX_Control(pDX, IDC_STATIC_3, m_wnd3);
	DDX_Control(pDX, IDC_STATIC_2, m_wnd2);
	DDX_Control(pDX, IDC_STATIC_1, m_wnd1);
	DDX_Text(pDX, IDC_UPDATE_DETAILS, m_strDetails);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CUpdate, CExtResizableDialog)
	//{{AFX_MSG_MAP(CUpdate)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CUpdate message handlers

BOOL CUpdate::OnInitDialog() 
{

	CExtResizableDialog::OnInitDialog();
	
	CStdioFile			localTmp;
	CString strTmp, strURL = "http://mxcontrol.sf.net/roboServ/update.info";
	
	try{

		CHttpFile* pFile = (CHttpFile*) m_is.OpenURL(strURL);
		
		if(pFile == NULL) return FALSE;

		while(pFile->ReadString(strTmp)){

			m_strDetails += (strTmp + "\r\n");
		}

		pFile->Close();
		delete pFile;
	}
	catch(CException* pEx){
		
		TCHAR   szCause[255];
		CString strFormatted;

		pEx->GetErrorMessage(szCause, 255);
		strFormatted.Format("Error during Update Query: %s\n", szCause);
		TRACE(strFormatted);
		return FALSE;
	}
	
	UpdateData(FALSE);
	AfxBeginThread(DownloadThread, (LPVOID)this, THREAD_PRIORITY_NORMAL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CUpdate::OnOK() 
{
	
	m_is.Close();
	CExtResizableDialog::OnOK();
}


UINT CUpdate::DownloadThread(LPVOID pParam)
{

	CUpdate *pDlg = (CUpdate*)pParam;
	ASSERT(pDlg);

	pDlg->DownloadUpdate();

	return 0;
}

void CUpdate::DownloadUpdate()
{

	m_wndDetails.SetSel(-1, 0);
	try{

		CHttpFile* pFile = (CHttpFile*) m_is.OpenURL(m_strUpdateURL, 1, INTERNET_FLAG_TRANSFER_BINARY|INTERNET_FLAG_DONT_CACHE, 0, 0);
		
		if(pFile == NULL){
		
			m_btCancel.EnableWindow(TRUE);
			return;
		}

		CStdioFile localFile;
		if(!localFile.Open(g_sSettings.GetWorkingDir() + "\\roboServ.tmp", CFile::modeWrite|CFile::typeBinary|CFile::modeCreate)){
			
			m_btCancel.EnableWindow(TRUE);
			return;
		}

		m_wndProgress.SetRange32(0, m_nSize);

		char buffer[64] = {0,};

		UINT nRead = 0;
		
		while((nRead = pFile->Read(&buffer, 64)) == 64){

			localFile.Write(buffer, nRead);
			m_nRead+=nRead;
			m_wndProgress.SetPos(m_nRead);
		}

		m_wndProgress.SetPos(m_nSize);
		localFile.Close();
		pFile->Close();
		delete pFile;

		if(!localFile.Open(g_sSettings.GetWorkingDir() + "\\update.bat", CFile::modeCreate|CFile::modeWrite|CFile::typeText)){
			
			m_btCancel.EnableWindow(TRUE);
			return;
		}

		
		CString strCommands;
		strCommands.Format(":Repeat\r\n"
						   "del \"%s\\roboServ.exe\"\r\n"
						   "if exist \"%s\\roboServ.exe\" goto Repeat\r\n"
						   "rename \"%s\\roboServ.tmp\" roboServ.exe\r\n"
						   "start /b %s\\roboServ.exe\r\n"
						   "del \"%s\\update.bat\"\r\n",
						   g_sSettings.GetWorkingDir(), g_sSettings.GetWorkingDir(), 
						   g_sSettings.GetWorkingDir(), g_sSettings.GetWorkingDir(), 
						   g_sSettings.GetWorkingDir());

		localFile.WriteString(strCommands);
		localFile.Close();

	}
	catch(CException* pEx){
		
		TCHAR   szCause[255];
		CString strFormatted;

		pEx->GetErrorMessage(szCause, 255);
		strFormatted.Format("Error during Update Query: %s\n", szCause);
		TRACE(strFormatted);
		m_btCancel.EnableWindow(TRUE);
		m_strDetails = strFormatted;
		UpdateData(FALSE);
		return;
	}

	m_btOK.EnableWindow(TRUE);
	MessageBeep(MB_OK);
}

