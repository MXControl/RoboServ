#if !defined(AFX_OPENCONFIGDLG_H__43812DF4_EC1B_4FE0_9665_1DDC59D8AF77__INCLUDED_)
#define AFX_OPENCONFIGDLG_H__43812DF4_EC1B_4FE0_9665_1DDC59D8AF77__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OpenConfigDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COpenConfigDlg dialog

class COpenConfigDlg : public CExtResizableDialog
{
// Construction
public:
	COpenConfigDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COpenConfigDlg)
	enum { IDD = IDD_OPENCONFIG };
	CExtLabel	m_wndText;
	CExtButton	m_btOK;
	CExtButton	m_btCancel;
	CExtButton	m_btBrowse;
	CExtEdit	m_eURL;
	CString	m_strURL;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COpenConfigDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COpenConfigDlg)
	virtual void OnOK();
	afx_msg void OnBrowse();
	afx_msg void OnChangeUrl();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPENCONFIGDLG_H__43812DF4_EC1B_4FE0_9665_1DDC59D8AF77__INCLUDED_)
