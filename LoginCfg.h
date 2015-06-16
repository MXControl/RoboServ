#if !defined(AFX_LOGINCFG_H__C6240FDD_230D_47F4_8874_FBE91336F266__INCLUDED_)
#define AFX_LOGINCFG_H__C6240FDD_230D_47F4_8874_FBE91336F266__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// LoginCfg.h : header file
//

#include "SkinCheckBox.h"
/////////////////////////////////////////////////////////////////////////////
// CLoginCfg dialog

class CLoginCfg : public CExtResizablePropertyPage
{

	DECLARE_DYNCREATE(CLoginCfg)
// Construction
public:
	void Load();
	CLoginCfg();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CLoginCfg)
	enum { IDD = IDD_LOGIN_CFG };
	CExtGroupBox	m_wndB;
	CExtGroupBox	m_wndA;
	CSkinCheckBox	m_wndDefVoice;
	CSkinCheckBox	m_wndDefAgent;
	CSkinCheckBox	m_wndDefAdmin;
	CSkinCheckBox m_btSub;
	CSkinCheckBox m_btMain;
	CExtEdit	m_ePassword;
	CListCtrl	m_lcLogin;
	CSkinCheckBox	m_btVoice;
	CSkinCheckBox	m_btAgent;
	CSkinCheckBox	m_btAdmin;
	CExtButton	m_btEdit;
	CExtButton	m_btDelete;
	CExtButton	m_btOK;
	CExtButton	m_btCancel;
	CExtButton	m_btAdd;
	CString	m_strPassword;
	BOOL	m_bAdmin;
	BOOL	m_bAgent;
	BOOL	m_bVoice;
	BOOL	m_bMain;
	BOOL	m_bSub;
	BOOL	m_bDefAdmin;
	BOOL	m_bDefAgent;
	BOOL	m_bDefVoice;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CLoginCfg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CLoginCfg)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnLoad(WPARAM wParam, LPARAM lParam);
	afx_msg void OnAdd();
	afx_msg void OnEdit();
	afx_msg void OnDelete();
	virtual void OnCancel();
	afx_msg void OnAdminDefault();
	afx_msg void OnAgentDefault();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LOGINCFG_H__C6240FDD_230D_47F4_8874_FBE91336F266__INCLUDED_)
