#pragma once

#include "SkinCheckBox.h"
// CSettingsBar dialog

class CSettingsBar : public CExtResizablePropertyPage
{

	DECLARE_DYNCREATE(CSettingsBar)

public:
	CSettingsBar();   // standard constructor
	virtual ~CSettingsBar();

// Dialog Data
// Dialog Data
	//{{AFX_DATA(CSettingsBar)
	enum { IDD = IDD_IN_BAR_DLG };
	CString m_strGodName;
	CString	m_strTopic;
	CString m_strMotd;
	CString m_strKeyMsg;
	CString m_strKeyWord;
	CString m_strWelcome;
	int		m_nColor;
	int		m_nLine;
	BOOL	m_bRoomListed;
	BOOL	m_bGodVisible;
	DWORD	m_dwGodFiles;

	CSkinCheckBox m_btRoomListed;
	CSkinCheckBox m_btGodVisible;
	CExtButton	  m_btOK;
	CExtButton	  m_btCancel;
	CExtComboBox  m_cbLine;
	CExtComboBox  m_cbColor;
	CExtEdit      m_eGodFiles;
	CExtEdit      m_eGodName;
	CExtEdit      m_eTopic;
	CExtEdit      m_eMotd;
	CExtEdit      m_eWelcome;
	CExtEdit      m_eKeyMsg;
	CExtEdit	  m_eKeyWord;
	//}}AFX_DATA

protected:
// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CSettingsBar)
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CSettingsBar)
	virtual BOOL OnInitDialog(void);
	afx_msg void OnLoad(WPARAM wParam, LPARAM lParam);
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	void Load();
};
