// SubChannel.cpp: implementation of the CSubChannel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "roboServ.h"
#include "SubChannel.h"
#include "Settings.h"
#include "RoomLister.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
UINT UWM_WRITETEXT = ::RegisterWindowMessage("UWM_WRITETEXT-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");
UINT UWM_CLOSEME   = ::RegisterWindowMessage("UWM_CLOSEME-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");

extern UINT UWM_CLNNOTIFY;
extern CSettings g_sSettings;
extern  CRoomLister g_rLister;

BEGIN_MESSAGE_MAP(CSubChannel, CWnd)
ON_REGISTERED_MESSAGE(UWM_CLNNOTIFY, ClientCallback)
END_MESSAGE_MAP()

CSubChannel::CSubChannel(HWND& hWnd) :
  CClientManager(m_hWnd),
  m_hWndWrite(hWnd)
{

	  m_uMode = CM_NORMAL;
}

CSubChannel::~CSubChannel()
{

}

UINT CSubChannel::AddClient(CClientSocket* pClient)
{


	//pClient->m_hMsgTarget = m_hWnd;
	return CClientManager::AddClient(pClient);
}

LRESULT CSubChannel::ClientCallback(WPARAM wParam, LPARAM lParam)
{

	EnterCriticalSection(&m_csLock);
	
	ClientNotify* pNotify = (ClientNotify*)lParam;
	
	ASSERT(pNotify);

	m_qNotifies.push(pNotify);

	LeaveCriticalSection(&m_csLock);
	return 1;
}

void CSubChannel::EchoChat(CString strName, CString strMsg)
{

	if(!g_sSettings.GetDisplaySubText()) return;
	if(!::IsWindow(m_hWndWrite)) return;
	if(m_bShutdown) return;

	WriteText(strName + ": " + strMsg, RGB(0, 120, 0), FALSE, TRUE);
}

void CSubChannel::WriteText(CString strText, COLORREF crText, BOOL bBold, BOOL bAppendNewLine)
{

	if(!g_sSettings.GetDisplaySubText()) return;
	if(!::IsWindow(m_hWndWrite)) return;
	if(m_bShutdown) return;

	WRITE_STRUCT tWrite;
	tWrite.strRoom = m_strRoomBase;
	tWrite.strText = strText;
	tWrite.crColor = crText;
	tWrite.bBold   = bBold;
	tWrite.bNewLine = bAppendNewLine;

	::SendMessage(m_hWndWrite, UWM_WRITETEXT, 0, (LPARAM)&tWrite);
}


void CSubChannel::WriteText(COLORREF crText, UINT nID, ...)
{

    TCHAR szBuffer[8192];
    TCHAR szFormat[8192];
	::LoadString(AfxGetApp()->m_hInstance, nID, szFormat, 8192);
	va_list ap;

	ZeroMemory(szBuffer,8192);
    va_start(ap, nID);
	
    _vsntprintf(szBuffer, sizeof(szBuffer), szFormat, ap);
	va_end(ap);

	WriteText(szBuffer, crText);
}

void CSubChannel::SendPart(const CString strUser, DWORD dwIP, WORD wPort, BOOL bRealPart)
{

	CClientManager::SendPart(strUser, dwIP, wPort, bRealPart);

	if(bRealPart && m_aClients.GetSize() == 0){

		if(g_sSettings.GetAutoCloseSub()){

			::PostMessage(m_hWndWrite, UWM_CLOSEME, 0, (LPARAM)this);
		}
	}

}


BOOL CSubChannel::ExecuteChannelCommand(const CString strUser, const CString strMsg, const UINT uMode)
{


	if((strMsg == "/part") || (strMsg == "/join main")){


		for(int i = 0; i < m_aClients.GetSize(); i++)
		{

			if(m_aClients[i]->m_strName.Find(strUser, 0) == 0){

				CString strTarget;
				if(m_aClients[i]->m_strJoinSuffix.IsEmpty()){

					strTarget = m_strRoomFull;
					strTarget.Replace(m_strRoomBase, m_strParentRoom);
				}
				else{
		
					strTarget = m_strParentRoom + m_aClients[i]->m_strJoinSuffix;
				}
				m_aClients[i]->SendRedirect(strTarget);
				return TRUE;
			}
		}
	}
	
	return CClientManager::ExecuteChannelCommand(strUser, strMsg, uMode);
}

void CSubChannel::SetTopic(const CString strTopic)
{

	CClientManager::SetTopic(strTopic);
	if(g_sSettings.GetSubListed()){
	
		g_rLister.ChangeTopic(m_strRoomBase, m_strTopic);
	}
}

void CSubChannel::SetLimit(DWORD dwLimit)
{

	CClientManager::SetLimit(dwLimit);
	if(g_sSettings.GetSubListed()){
	
		g_rLister.ChangeLimit(m_strRoomBase, m_dwLimit);
	}
}

void CSubChannel::SetUsers(DWORD dwUsers)
{

	CClientManager::SetUsers(dwUsers);
	if(g_sSettings.GetSubListed()){
	
		g_rLister.ChangeUsers(m_strRoomBase, m_dwUsers);
	}
}

void CSubChannel::SetRoom(const CString strName)
{

	if(g_sSettings.GetSubListed()){

		g_rLister.ChangeName(m_strRoomBase, strName);
	}
	CClientManager::SetRoom(strName);
}
