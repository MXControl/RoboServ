/*
** Copyright (C) 2004 Thees Schwab
**  
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
** 
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
** 
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software 
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#if !defined(AFX_MYEDIT_H__C0CB48EF_BEB1_4FBC_843F_14DAC1C31FEA__INCLUDED_)
#define AFX_MYEDIT_H__C0CB48EF_BEB1_4FBC_843F_14DAC1C31FEA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MyEdit.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMyEdit window

class CMyEdit : public CEdit
{
// Construction
public:
	CMyEdit();
	static int SearchItem(CString strString);
	int SearchRCMSItem(CString strString);
	int SearchHistory(CString strString);
	CStringArray m_aHistory;
// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMyEdit)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void SetExtended();
	BOOL m_bEx;
	void SetBkColor(COLORREF cr);
	virtual ~CMyEdit();


	// Generated message map functions
protected:
	COLORREF m_crBg;
	COLORREF m_crBgFocus;
	COLORREF m_crDraw;
	CBrush m_brBkgnd;
	CStringArray *m_pCommands;
	//{{AFX_MSG(CMyEdit)
	afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
public:
	void SetCommands(CStringArray* pCmd);
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MYEDIT_H__C0CB48EF_BEB1_4FBC_843F_14DAC1C31FEA__INCLUDED_)
