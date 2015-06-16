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


#include "stdafx.h"
#include "roboServ.h"
#include "MyEdit.h"
#include "Settings.h"
#include "util.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


extern CSettings g_sSettings;

/////////////////////////////////////////////////////////////////////////////
// CMyEdit

UINT UWM_INPUT = ::RegisterWindowMessage("UWM_INPUT-7A14F66B-ABAB-4525-AC01-841C82EC48B6");

#define m_aCommandsNUM (DWORD)m_pCommands->GetSize()
#define m_aHistoryNUM (DWORD)m_aHistory.GetSize()

CMyEdit::CMyEdit()
{

	m_crBg = ::GetSysColor(COLOR_WINDOW); // Initializing the Background Color to the system face color.
	m_crBgFocus = m_crBg;
	m_brBkgnd.CreateSolidBrush(m_crBg); // Create the Brush Color for the Background.
	m_crDraw	= m_crBg;
	m_bEx		= FALSE;
	m_pCommands = NULL;
}

CMyEdit::~CMyEdit()
{
}


BEGIN_MESSAGE_MAP(CMyEdit, CEdit)
	//{{AFX_MSG_MAP(CMyEdit)
	ON_WM_CTLCOLOR_REFLECT()
	ON_WM_KILLFOCUS()
	ON_WM_SETFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyEdit message handlers

/////////////////////////////////
// Search string in autocommand
/////////////////////////////////

int CMyEdit::SearchHistory(CString strString)
{

	CString strTmp;

	for(int nIndex = 0; nIndex < m_aHistory.GetSize(); nIndex++){

		strTmp = m_aHistory[nIndex];
		if(strTmp == strString) break;
	}

	if((nIndex >= m_aHistory.GetSize())  || m_aHistory.GetSize() == 0) return 0;

	return nIndex + 1;
}

int CMyEdit::SearchRCMSItem(CString strString)
{

	CString strTmp;

	for(int nIndex = 0; nIndex < m_pCommands->GetSize(); nIndex++){

		strTmp = m_pCommands->GetAt(nIndex);
		if(strTmp == strString) break;
	}

	if((nIndex >= m_pCommands->GetSize())  || m_pCommands->GetSize() == 0) return 0;

	return nIndex + 1;
}

BOOL CMyEdit::PreTranslateMessage(MSG* pMsg) 
{

	if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)
			&& (GetKeyState(VK_SHIFT) >= 0)){

		CString strText;
		GetWindowText(strText);
		if(!strText.IsEmpty()){

			m_aHistory.Add(strText);
		}
		::SendMessage(AfxGetApp()->m_pMainWnd->m_hWnd, UWM_INPUT, 0, 0);
	}
	else if((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_RETURN)
				&& (GetKeyState(VK_SHIFT) < 0) && (GetStyle() & ES_MULTILINE)){

		CString strText;
		GetWindowText(strText);
		strText+="\r\n";
		SetWindowText(strText);
		SetSel(0, -1, FALSE);
		SetSel(-1, 0, FALSE);
		return TRUE;
	}
	else if((pMsg->message == WM_MOUSEWHEEL || pMsg->message == WM_KEYDOWN) && !m_bEx){

		ASSERT(m_pCommands);
		///////////////////////////////////////////////////////
		// Scroll through RCMS commands UP (PAGE UP)
		///////////////////////////////////////////////////////
		if((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_PRIOR) || 
			(pMsg->message == WM_MOUSEWHEEL && ((short)HIWORD(pMsg->wParam)) > 0)){
		
			if(!m_pCommands) return TRUE;
			if(m_pCommands->GetSize() == 0) return TRUE;
			SetSel(0,-1);
			TCHAR szTempStr[1024];
			if(SendMessage(WM_GETTEXT, 1024, (LPARAM)szTempStr)){
		
				//DWORD dwIndex = atol(szTempStr);
				DWORD dwIndex = SearchRCMSItem(szTempStr);
				if(dwIndex == 0){

					lstrcpy(szTempStr, m_pCommands->GetAt(m_aCommandsNUM - 1));
					ReplaceSel(szTempStr, TRUE);
					SendMessage(WM_KEYDOWN, VK_END, 0);
					CString strTemp = m_pCommands->GetAt(m_aCommandsNUM - 1);
					int nStart = strTemp.Find(" ", 0);
					if(nStart > 0) SetSel(nStart+1, -1, FALSE);
					return TRUE;
				}
				else if((dwIndex == 1) || (dwIndex > m_aCommandsNUM)){

					ReplaceSel("", TRUE);
				}
				else{

					lstrcpy(szTempStr, m_pCommands->GetAt(dwIndex - 2));
					ReplaceSel(szTempStr, TRUE);
					CString strTemp = m_pCommands->GetAt(dwIndex - 2);
					int nStart = strTemp.Find(" ", 0);
					SendMessage(WM_KEYDOWN, VK_END, 0);
					if(nStart > 0) SetSel(nStart+1, -1, FALSE);
				}
				return TRUE;
			}
			else{

				lstrcpy(szTempStr, m_pCommands->GetAt(m_aCommandsNUM - 1));
				ReplaceSel(szTempStr, TRUE);
				SendMessage(WM_KEYDOWN, VK_END, 0);
				CString strTemp = m_pCommands->GetAt(m_aCommandsNUM - 1);
				int nStart = strTemp.Find(" ", 0);
				if(nStart > 0) SetSel(nStart+1, -1, FALSE);
				return TRUE;
			}
		}
		///////////////////////////////////////////////////////
		// RCMS Down (PAGE DOWN)
		///////////////////////////////////////////////////////
		else if(((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_NEXT) || 
			(pMsg->message == WM_MOUSEWHEEL && ((short)HIWORD(pMsg->wParam)) <= 0)) && !m_bEx){
		
			if(!m_pCommands) return TRUE;
			if(m_pCommands->GetSize() == 0) return TRUE;
			SetSel(0,-1);
			TCHAR szTempStr[1024];
			if(SendMessage(WM_GETTEXT, 1024, (LPARAM)szTempStr)){

				//DWORD dwIndex = atol(szTempStr);
				DWORD dwIndex = SearchRCMSItem(szTempStr);
				if(dwIndex >= m_aCommandsNUM){

					ReplaceSel("", TRUE);
					//Beep(1000, 50);
				}
				else{

					lstrcpy(szTempStr, m_pCommands->GetAt(dwIndex));
					ReplaceSel(szTempStr, TRUE);
					SendMessage(WM_KEYDOWN, VK_END, 0);
					CString strTemp = m_pCommands->GetAt(dwIndex);
					int nStart = strTemp.Find(" ", 0);
					if(nStart > 0) SetSel(nStart+1, -1, FALSE);
				}
				return TRUE;
			}
			else{

				lstrcpy(szTempStr, m_pCommands->GetAt(0));
				ReplaceSel(szTempStr, TRUE);
				SendMessage(WM_KEYDOWN, VK_END, 0);
				CString strTemp = m_pCommands->GetAt(0);
				int nStart = strTemp.Find(" ", 0);
				if(nStart > 0) SetSel(nStart+1, -1, FALSE);
				return TRUE;
			}
		}
		///////////////////////////////////////////////////////
		// Commands UP UPARROW or MOUSEHWEL UP
		///////////////////////////////////////////////////////
		else if(((pMsg->message == WM_KEYDOWN) && (pMsg->wParam == VK_UP)) && (m_aHistoryNUM != 0) && !m_bEx){
		
			SetSel(0,-1);
			TCHAR szTempStr[1024];
			if(SendMessage(WM_GETTEXT, 1024, (LPARAM)szTempStr)){
		
				DWORD dwIndex = SearchHistory(szTempStr);
				if(dwIndex == 0){ // Item was not found

					CString strOut = m_aHistory[m_aHistory.GetSize() - 1];
					//Util::ReplaceVars(strOut);
					lstrcpy(szTempStr, strOut);
					ReplaceSel(szTempStr, TRUE);
					SendMessage(WM_KEYDOWN, VK_END, 0);
					return TRUE;
				}
				else if(dwIndex == 1){ // Item found but we are at no1

					SetWindowText("");
					//Beep(1000, 50);
				}
				else{ // item found 

					CString strOut = m_aHistory[dwIndex - 2];
					lstrcpy(szTempStr, strOut);
					ReplaceSel(szTempStr, TRUE);
					SendMessage(WM_KEYDOWN, VK_END, 0);
				}
				return TRUE;
			}
			else{

				CString strOut = m_aHistory[m_aHistory.GetSize() - 1];
				lstrcpy(szTempStr, strOut);
				ReplaceSel(szTempStr, TRUE);
				SendMessage(WM_KEYDOWN, VK_END, 0);
				return TRUE;
			}
		}
		///////////////////////////////////////////////////////
		// HISTORY down DOWNARROW or+MOUSEWHEEL DOWN
		///////////////////////////////////////////////////////
		else if((pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DOWN) && (m_aHistoryNUM != 0) && !m_bEx){
		
			TRACE("HistoryUP %d\n",GetKeyState(VK_LCONTROL));
			SetSel(0,-1);
			TCHAR szTempStr[1024];
			if(SendMessage(WM_GETTEXT, 1024, (LPARAM)szTempStr)){
				DWORD dwIndex = SearchHistory(szTempStr);
				if(dwIndex >= (DWORD)m_aHistory.GetSize()){

					SetWindowText("");
					//Beep(1000, 50);
				}
				else{

					CString strOut = m_aHistory[dwIndex];

					lstrcpy(szTempStr, strOut);

					SetWindowText(szTempStr);
					SendMessage(WM_KEYDOWN, VK_END, 0);
				}
				return TRUE;
			}
			else{

				CString strOut = m_aHistory[0];

				lstrcpy(szTempStr, strOut);
				SetWindowText(szTempStr);
				SendMessage(WM_KEYDOWN, VK_END, 0);
				return TRUE;
			}
		}
	}
	return CEdit::PreTranslateMessage(pMsg);
}


HBRUSH CMyEdit::CtlColor(CDC* pDC, UINT nCtlColor) 
{

	
	HBRUSH hbr;
	hbr = (HBRUSH)m_brBkgnd;
	pDC->SetBkColor(m_crDraw);
	pDC->SetTextColor(::GetSysColor(COLOR_WINDOWTEXT));
	return hbr;
	
}

void CMyEdit::OnKillFocus(CWnd* pNewWnd) 
{

	CEdit::OnKillFocus(pNewWnd);

	m_crDraw = m_crBg;
	m_brBkgnd.DeleteObject(); 
	m_brBkgnd.CreateSolidBrush(m_crDraw);
	RedrawWindow();
}

void CMyEdit::OnSetFocus(CWnd* pOldWnd) 
{

	CEdit::OnSetFocus(pOldWnd);

	m_crDraw = (!m_bEx ?  m_crBgFocus : m_crBg);
	m_brBkgnd.DeleteObject();
	m_brBkgnd.CreateSolidBrush(m_crDraw);
	RedrawWindow();
}

void CMyEdit::SetBkColor(COLORREF cr)
{

	m_crBg = cr;
	m_crBgFocus = m_crBg;
	m_crDraw = cr;
	RedrawWindow();
}

void CMyEdit::SetExtended()
{

	m_bEx = TRUE;
}

void CMyEdit::SetCommands(CStringArray* pCmd)
{

	ASSERT(pCmd);
	
	m_pCommands = pCmd;
}
