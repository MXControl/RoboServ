#if !defined(AFX_MISCCFG_H__0D788471_442B_4749_820B_C28AD12654BA__INCLUDED_)
#define AFX_MISCCFG_H__0D788471_442B_4749_820B_C28AD12654BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MiscCfg.h : header file
//
#include "SkinCheckBox.h"
/////////////////////////////////////////////////////////////////////////////
// CMiscCfg dialog

class CMiscCfg : public CExtResizablePropertyPage
{
	DECLARE_DYNCREATE(CMiscCfg)

// Construction
public:
	void Load();
	CMiscCfg();
	~CMiscCfg();

// Dialog Data
	//{{AFX_DATA(CMiscCfg)
	enum { IDD = IDD_MISC };
	CExtLabel	 m_wndA;
	CExtLabel	 m_wndB;
	CExtGroupBox m_wndC;
	CExtGroupBox m_wndD;
	CExtGroupBox m_wndE;
	CExtLabel	 m_wndF;
	CExtLabel	 m_wndG;
	CExtLabel	 m_wndH;
	CExtLabel	 m_wndI;
	CExtLabel	 m_wndJ;
	CSkinCheckBox	m_wndModerated;
	CExtButton	m_btnOK;
	CExtButton	m_btnCancel;
	CExtComboBox	m_wndVoiceWatch;
	CExtComboBox	m_wndAdminWatch;
	CSkinCheckBox	m_btMultiIPOk;
	CSkinCheckBox	m_btLocalIsOp;
	CSkinCheckBox	m_btBlockNushi;
	CSkinCheckBox	m_btAdvertiseRose;
	CSkinRadioButton	m_btNoNormal;
	CSkinRadioButton	m_btPlusNormal;
	CSkinRadioButton	m_btAtNormal;
	CSkinRadioButton	m_btNoVoiced;
	CSkinRadioButton	m_btPlusVoiced;
	CSkinRadioButton	m_btAtVoiced;
	CSkinRadioButton	m_btNoAdmin;
	CSkinRadioButton	m_btPlusAdmin;
	CSkinRadioButton	m_btAtAdmin;
	CSkinRadioButton	m_btNoAgent;
	CSkinRadioButton	m_btPlusAgent;
	CSkinRadioButton	m_btAtAgent;
	
	int		m_nAdminWatch;
	int		m_nVoiceWatch;
	BOOL	m_bAdvertiseRose;
	BOOL    m_bMultiIPOk;
	BOOL    m_bLocalIsOp;
	BOOL	m_bBlockNushi;
	BOOL	m_bModerated;
	//}}AFX_DATA

	int  m_nNormalIcon;
	int  m_nVoiceIcon;
	int  m_nAdminIcon;
	int  m_nAgentIcon;

// Overrides
	// ClassWizard generate virtual function overrides
	//{{AFX_VIRTUAL(CMiscCfg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	// Generated message map functions
	//{{AFX_MSG(CMiscCfg)
	virtual BOOL OnInitDialog();
	afx_msg void OnLoad(WPARAM wParam, LPARAM lParam);
	afx_msg void OnAdvertise();
	virtual void OnOK();
	afx_msg void OnNoNormal();
	afx_msg void OnPlusNormal();
	afx_msg void OnAtNormal();
	afx_msg void OnNoVoice();
	afx_msg void OnPlusVoice();
	afx_msg void OnAtVoice();
	afx_msg void OnNoAdmin();
	afx_msg void OnPlusAdmin();
	afx_msg void OnAtAdmin();
	afx_msg void OnNoAgent();
	afx_msg void OnPlusAgent();
	afx_msg void OnAtAgent();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MISCCFG_H__0D788471_442B_4749_820B_C28AD12654BA__INCLUDED_)
