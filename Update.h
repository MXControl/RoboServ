#if !defined(AFX_UPDATE_H__3832FCA8_B846_48C2_9EC7_6B55B72E05FC__INCLUDED_)
#define AFX_UPDATE_H__3832FCA8_B846_48C2_9EC7_6B55B72E05FC__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// Update.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CUpdate dialog

class CUpdate : public CExtResizableDialog
{
// Construction
public:
	UINT m_nRead;
	UINT m_nSize;
	void DownloadUpdate();
	static UINT DownloadThread(LPVOID pParam);
	CString m_strUpdateURL;
	CUpdate(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CUpdate)
	enum { IDD = IDD_UPDATE };
	CExtButton	m_btOK;
	CExtButton	m_btCancel;
	CProgressCtrl	m_wndProgress;
	CExtEdit	m_wndDetails;
	CExtLabel	 m_wnd4;
	CExtLabel	 m_wnd3;
	CExtGroupBox m_wnd2;
	CExtGroupBox m_wnd1;
	CString	m_strDetails;
	//}}AFX_DATA
	CInternetSession	m_is;
	

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CUpdate)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CUpdate)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UPDATE_H__3832FCA8_B846_48C2_9EC7_6B55B72E05FC__INCLUDED_)
