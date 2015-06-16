#if !defined(AFX_SUBCHANNELCFG_H__ABB3E397_C75F_48E4_840D_5BA0B48A0964__INCLUDED_)
#define AFX_SUBCHANNELCFG_H__ABB3E397_C75F_48E4_840D_5BA0B48A0964__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SubChannelCfg.h : header file
//
#include "SkinCheckBox.h"

/////////////////////////////////////////////////////////////////////////////
// CSubChannelCfg dialog

class CSubChannelCfg : public CExtResizablePropertyPage
{

	DECLARE_DYNCREATE(CSubChannelCfg)
// Construction
public:
	void Load();
	CSubChannelCfg();   // standard constructor

// Dialog Data
	//{{AFX_DATA(CSubChannelCfg)
	enum { IDD = IDD_SUBCHANNELS_CFG };
	CSkinCheckBox	m_btSubList;
	CExtLabel	m_wndA;
	CSkinCheckBox	m_wndFirstIsSuper;
	CSkinCheckBox   m_btDisplaySubs;
	CExtComboBox	m_cbSubLevel;
	CExtEdit		m_eMaxSubs;
	CSkinCheckBox	m_btLimitSubs;
	CSkinCheckBox	m_btEnableSubs;
	CSkinCheckBox	m_btAutoClose;
	CExtButton		m_btOK;
	CExtButton		m_btCancel;
	BOOL	m_bEnable;
	BOOL	m_bLimit;
	UINT	m_uMax;
	int		m_nSubLevel;
	BOOL	m_bAutoClose;
	BOOL	m_bDisplaySubs;
	BOOL	m_bFirstIsSuper;
	BOOL	m_bSubList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSubChannelCfg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSubChannelCfg)
	afx_msg void OnEnableSubs();
	afx_msg void OnLoad(WPARAM wParam, LPARAM lParam);
	virtual void OnCancel();
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SUBCHANNELCFG_H__ABB3E397_C75F_48E4_840D_5BA0B48A0964__INCLUDED_)
