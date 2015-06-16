// ClientManager.cpp: implementation of the CClientManager class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "roboServ.h"
#include "ClientManager.h"
#include ".\tokenizer.h"
#include ".\settings.h"
#include "Util.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern CSettings g_sSettings;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
UINT UWM_INCOMMING = ::RegisterWindowMessage("UWM_INCOMMING-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");
UINT UWM_CLNNOTIFY = ::RegisterWindowMessage("UWM_CLNNOTIFY-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");

CClientManager::CClientManager(HWND& hWnd) :

	m_hWndTarget(hWnd)
{

	m_bHosted = FALSE;
	m_eDone.SetEvent();
	m_eHosted.ResetEvent();
	m_eNotifyDone.SetEvent();
	m_strRoomBase = "";
	m_strRoomFull = "";
	m_dwIP        = 0;
	m_wPort       = 0;
	m_dwLimit     = 80;
	m_dwUsers     = 0;
	m_strTopic    = "";
	m_strMotd     = "";
	m_uMode		  = CM_NORMAL;
	m_bShutdown	  = FALSE;
	m_pNotifyThread = NULL;
	m_nTotalMessages = 0;
	m_nTotalJoins    = 0;
	m_nFailedJoins	 = 0;
	m_aClients.RemoveAll();
	
	InitializeCriticalSection(&m_csLock);
	InitializeCriticalSection(&m_csWriteLock);
}

CClientManager::~CClientManager()
{

	DeleteCriticalSection(&m_csLock);
	DeleteCriticalSection(&m_csWriteLock);
}

void CClientManager::Stop(BOOL bRedirect, CString strTarget)
{

	m_bHosted = FALSE;

	TRACE("Waiting for Worker threads to exit\n");	
	
	DWORD n = WaitForSingleObject(m_eNotifyDone, 1000);
	if(n == WAIT_TIMEOUT || n == WAIT_FAILED){

		TerminateThread(m_pNotifyThread->m_hThread, 0);
	}

	TRACE("Cleaning up...\n");
	
	// if redirect: 
	for(int i = 0; i < m_aClients.GetSize() && bRedirect; i++){
 
		m_aClients[i]->SendRedirect(strTarget);
		Sleep(50);
	}

	if(bRedirect){

		DWORD dwWait = 0;
		while(m_aClients.GetSize() && (dwWait < 5000)){

			Sleep(50);
			dwWait+=50;
		}
	}
	while(!bRedirect && (m_aClients.GetSize() > 0)){

		CClientSocket* pTmp = m_aClients[m_aClients.GetSize()-1];
		m_aClients.RemoveAt(m_aClients.GetSize()-1);
		pTmp->LogOut();
		delete pTmp;
		pTmp = NULL;
	}
	
	ClientNotify* pN = NULL;
	while(!bRedirect && (!m_qNotifies.empty())){

		pN = m_qNotifies.front();
		m_qNotifies.pop();
		delete pN;
		pN = NULL;
	}

	ASSERT(m_qNotifies.empty());

	m_pNotifyThread = NULL;
}

DWORD CClientManager::TranslateIP(CString strIP)
{
	CString strTmp;

	CTokenizer ipToken(strIP, ".");
	ipToken.Next(strTmp);
	WORD wA = (WORD)atoi(strTmp);
	ipToken.Next(strTmp);
	WORD wB = (WORD)atoi(strTmp);
	ipToken.Next(strTmp);
	WORD wC = (WORD)atoi(strTmp);
	ipToken.Next(strTmp);
	WORD wD = (WORD)atoi(strTmp);
	strTmp.Format("%02X%02X%02X%02X", wD, wC, wB, wA);
	
	DWORD dwIP = Util::axtoi((LPTSTR)(LPCTSTR)strTmp, 8);

	return dwIP;
}

void CClientManager::CheckClients(void)
{

	for(int i = 0; i < m_aClients.GetSize(); i++){

		if(!m_aClients[i]->m_bListen){

			CClientSocket *pTmp = m_aClients[i];
			m_aClients.RemoveAt(i);
			pTmp->LogOut(); // just to be on the save side....
			SendPart(pTmp->m_strName, pTmp->m_dwIP, pTmp->m_wPort);
			if(pTmp->m_bBlocked){

				CString strMsg;
				strMsg.Format(IDS_FLOODBLOCK, pTmp->m_strName);
				ServerAction(g_sSettings.GetGodName(), strMsg);
			}
			delete pTmp;
			pTmp = NULL;
			i--;
		}
	}
}

void CClientManager::SetTopic(const CString strTopic)
{

	if(strTopic.GetLength() > 115){
		
		m_strTopic = strTopic.Left(115);
	}
	else{

		m_strTopic = strTopic;
	}
}

void CClientManager::SetMotd(const CString strMotd)
{

	m_strMotd = strMotd;
}

// Broadcast Send
void CClientManager::SendTopic()
{

	CString strTopic, strSend;
	strTopic.LoadString(IDS_TOPIC2);
	EchoChat(strTopic, m_strTopic);

	for(int i = 0; i < m_aClients.GetSize(); i++){

		strSend = m_strTopic;
		Util::ReplaceVars(strSend);
		strSend.Replace("%USERS%", Util::Formatint(m_aClients.GetSize()));
		strSend.Replace("%NAME%", m_aClients[i]->m_strName);
		strSend.Replace("%LINE%", Util::FormatLine(m_aClients[i]->m_wLineType));
		strSend.Replace("%FILES%", Util::Formatint(m_aClients[i]->m_dwFiles));
		strSend.Replace("%IP%", Util::FormatIP(m_aClients[i]->m_dwSrcIP));
		strSend.Replace("%HOSTNAME%", m_aClients[i]->m_strSrcHost);
		strSend.Replace("%CLIENT%", m_aClients[i]->m_strClientString);
		strSend.Replace("%CLIENTVER%", m_aClients[i]->m_strClientVer);
		strSend.Replace("%ROOMNAME%", m_strRoomBase);
		if(strSend.GetLength() > 125){
			
			strSend = strSend.Left(125);
		}
		m_aClients[i]->SendTopic(strSend);
	}
}

void CClientManager::SendMotd()
{

	EchoChat("Motd", m_strMotd);
	
	CString strSend;
	
	for(int i = 0; i < m_aClients.GetSize(); i++){

		strSend = m_strMotd;
		strSend.Replace("\\n", "\n");
		Util::ReplaceVars(strSend);
		strSend.Replace("%USERS%", Util::Formatint(m_aClients.GetSize()));
		strSend.Replace("%NAME%", m_aClients[i]->m_strName);
		strSend.Replace("%LINE%", Util::FormatLine(m_aClients[i]->m_wLineType));
		strSend.Replace("%FILES%", Util::Formatint(m_aClients[i]->m_dwFiles));
		strSend.Replace("%IP%", Util::FormatIP(m_aClients[i]->m_dwSrcIP));
		strSend.Replace("%HOSTNAME%", m_aClients[i]->m_strSrcHost);
		strSend.Replace("%CLIENT%", m_aClients[i]->m_strClientString);
		strSend.Replace("%CLIENTVER%", m_aClients[i]->m_strClientVer);
		strSend.Replace("%ROOMNAME%", m_strRoomBase);
		m_aClients[i]->SendMotd(strSend);
	}
}

void CClientManager::SendMsg(CString strUser, CString strMsg)
{

	if(CheckCommands(strUser, strMsg)){

		// do not relay the message when it was a /mode command
		return;
	}

	FixString(strMsg);
	EchoChat(strUser, strMsg);

	if((m_uMode & CM_NOID) == CM_NOID){

		Util::BaseName(strUser);
	}

	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendMsg(strUser, strMsg);
	}
}

void CClientManager::SendChannelRename(void)
{
    
	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendChannelRename(m_strRoomBase);
	}
}

void CClientManager::SendOpMsg(CString strUser, CString strMsg, UINT uColor, UINT uRecpMode)
{

	FixString(strMsg);
	BOOL bEcho = TRUE;

	EchoChat(strUser, strMsg);

	if(((m_uMode & CM_NOID) == CM_NOID) && (uColor < 4000)){

		Util::BaseName(strUser);
	}


	if(uColor == 4000){

		CString strName = " ";
		CTokenizer token(strMsg, " ");
		if(token.Next(strName)){

			strMsg = token.Tail();
		}
		for(int i = 0; i < m_aClients.GetSize(); i++){

		
			if(m_aClients[i]->m_strName.Find(strUser, 0) == 0){

				m_aClients[i]->SendAction(strName, strMsg);
			}
		}
	}
	else if(uColor > 4000){

		for(int i = 0; i < m_aClients.GetSize(); i++){

		
			if(m_aClients[i]->m_strName.Find(strUser, 0) == 0){

				m_aClients[i]->SendOperator("", strMsg, TRUE, uColor-4000);
			}
		}
	}
	else{

		if(!strMsg.IsEmpty() && !strUser.IsEmpty()){

			bEcho = FALSE;
		}
		
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName != strUser){

				switch(uRecpMode){

					case UM_NORMAL: // everyone may see this
						break;
					case UM_VOICED: // voice or higher may see this
						if(((m_aClients[i]->m_uMode & UM_VOICED) != UM_VOICED) &&
							((m_aClients[i]->m_uMode & UM_ADMIN) != UM_ADMIN)){

							continue;
						}
						break;
					default: // only operators may see this
						if((m_aClients[i]->m_uMode & UM_ADMIN) != UM_ADMIN){

							continue;
						}
						break;
				}
			}

			m_aClients[i]->SendOperator(strUser, strMsg, bEcho, uColor);
		}
	}
}

void CClientManager::SendAction(CString strUser, CString strMsg)
{

	FixString(strMsg);
	EchoChat(strUser, strMsg);
	if((m_uMode & CM_NOID) == CM_NOID){

		Util::BaseName(strUser);
	}
	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendAction(strUser, strMsg);
	}
}

void CClientManager::SendJoin(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel, DWORD dwRealIP)
{

	EchoChat("+ ", strUser);
	
	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendJoin(strUser, dwIP, wPort, wLine, dwFiles, wUserLevel, dwRealIP);
	}
	SetUsers(m_aClients.GetSize());
}

void CClientManager::SendPart(const CString strUser, DWORD dwIP, WORD wPort, BOOL bRealPart)
{

	EchoChat("- ", strUser);

	for(int i = 0; i < m_aClients.GetSize(); i++){

		m_aClients[i]->SendPart(strUser, dwIP, wPort);
	}
	SetUsers(m_aClients.GetSize());

}


void CClientManager::HandleVisible(CClientSocket* pClient, WORD wMode, BOOL bSetVisible)
{

	if(bSetVisible){

		SendJoin(pClient->m_strName, pClient->m_dwIP, pClient->m_wPort, pClient->m_wLineType, pClient->m_dwFiles, wMode, pClient->m_dwSrcIP);
	}
	else{

		SendPart(pClient->m_strName, pClient->m_dwIP, pClient->m_wPort, FALSE); // prevent user from beeing cleared off the list :P
	}
}


BOOL CClientManager::SendRename(CClientSocket* pClient, const CString strOldName, DWORD dwOldIP, WORD wOldPort, const CString strNewName, DWORD dwNewIP, WORD wNewPort, WORD wLine, DWORD dwFiles, WORD wUserLevel)
{


	if(pClient <= 0) return FALSE;

	if(!CheckUserName(strNewName, pClient->m_dwSrcIP, pClient->m_wSrcPort)){

		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i] == pClient){

				m_aClients.RemoveAt(i);
				break;
			}
		}

		pClient->LogOut();
		SendPart(pClient->m_strName, dwOldIP, wOldPort); 

		CString strWarn;
		strWarn.Format(IDS_WARN_USERNAMEHIJACK, strOldName, Util::FormatIP(pClient->m_dwSrcIP));
		SendOpMsg("", strWarn);

		delete pClient;
		return FALSE;
	}


	CString strOut;
	strOut.Format(IDS_RENAMEMSG, strOldName, strNewName);
	WriteText(strOut, RGB(0, 128, 255), TRUE);

	BOOL bSend = ((pClient->m_uMode & UM_HIDDEN) != UM_HIDDEN);

	for(int i = 0; i < m_aClients.GetSize() && bSend; i++){

		m_aClients[i]->SendRename(strOldName, dwOldIP, wOldPort, strNewName, dwNewIP, wNewPort, wLine, dwFiles, wUserLevel);
	}
	return TRUE;
}

void CClientManager::SendMode(const CString strSender, const CString strMode)
{

	ServerAction(strSender, strMode);
}

void CClientManager::ServerAction(const CString strSender, const CString strMsg, UINT uColor, UINT uMode)
{

	ClientOpMsg* pN = new ClientOpMsg();
	pN->dwIP = 0;
	pN->wPort = 0;
	pN->uMode = UM_ADMIN;
	pN->strName = strSender;
	pN->strText = strMsg;
	pN->uParam  = uColor;
	pN->uRecipientMode = uMode;
	::SendMessage(m_hWndTarget, UWM_CLNNOTIFY, (WPARAM)pN->wType, (LPARAM)pN);
}



void CClientManager::SendWelcome(CString strMsg)
{


	if((m_uMode & CM_MAINCHANNEL) != CM_MAINCHANNEL) return;
	int nMsgID = CC_MSG;

	if(strMsg.Find("/me ") == 0){

		nMsgID = CC_ACTION;
		strMsg = strMsg.Mid(4);
	}
	else if(strMsg.Find("/action ") == 0){

		nMsgID = CC_ACTION;
		strMsg = strMsg.Mid(8);
	}
	else if(strMsg.Find("/emote ") == 0){

		nMsgID = CC_ACTION;
		strMsg = strMsg.Mid(7);
	}
	else if(strMsg.Find("/opmsg ") == 0){

		nMsgID = CC_OPMSG;
		strMsg = strMsg.Mid(7);
	}
	if(nMsgID == CC_ACTION){

		ClientAction* cm = new ClientAction();
		cm->dwIP	= 0;
		cm->wPort   = 0;
		cm->uMode	= UM_ADMIN;
		cm->strName = g_sSettings.GetGodName();
		cm->strText = strMsg;
		::SendMessage(m_hWndTarget, UWM_CLNNOTIFY, (WPARAM)cm->wType, (LPARAM)cm);
	}
	else if(nMsgID == CC_OPMSG){

		ClientOpMsg* cm = new ClientOpMsg();
		cm->dwIP	= 0;
		cm->wPort   = 0;
		cm->uMode	= UM_ADMIN;
		cm->strName = g_sSettings.GetGodName();
		cm->strText = strMsg;
		::SendMessage(m_hWndTarget, UWM_CLNNOTIFY, (WPARAM)cm->wType, (LPARAM)cm);
	}
	else{

		ClientMessage* cm = new ClientMessage();
		cm->dwIP	= 0;
		cm->wPort   = 0;
		cm->uMode	= UM_ADMIN;
		cm->strName = g_sSettings.GetGodName();
		cm->strText = strMsg;
		::SendMessage(m_hWndTarget, UWM_CLNNOTIFY, (WPARAM)cm->wType, (LPARAM)cm);
	}
}

void CClientManager::SendCmdNotUnderstood(const CString strUser, const CString strCmd)
{
	
	CString strMsg;
	strMsg.Format(IDS_ERROR_NOTUNDERSTOOD_SEND, strCmd);

	ClientOpMsg* pN = new ClientOpMsg();
	pN->dwIP = 0;
	pN->wPort = 0;
	pN->uMode = UM_ADMIN;
	pN->strName = strUser;
	pN->strText = strMsg;
	pN->uParam  = 0;
	::SendMessage(m_hWndTarget, UWM_CLNNOTIFY, (WPARAM)pN->wType, (LPARAM)pN);
}

void CClientManager::SendPermissionDenied(const CString strUser, const CString strCmd)
{
	
	CString strMsg;
	strMsg.Format(IDS_ERROR_PERMISSIONDENIED_SEND, strCmd);

	ClientOpMsg* pN = new ClientOpMsg();
	pN->dwIP = 0;
	pN->wPort = 0;
	pN->uMode = UM_ADMIN;
	pN->strName = strUser;
	pN->strText = strMsg;
	pN->uParam  = 0;
	pN->uRecipientMode = UM_VOICED;
	::SendMessage(m_hWndTarget, UWM_CLNNOTIFY, (WPARAM)pN->wType, (LPARAM)pN);
}

UINT CClientManager::NotifyThread(LPVOID pParam)
{

	CClientManager* pServer = (CClientManager*)pParam;
	ASSERT(pServer);

	pServer->m_eNotifyDone.ResetEvent();

	int n = -1;

	while(pServer->m_bHosted){

		if(pServer->m_qNotifies.empty()){

            Sleep(50);
			continue;
		}

		ClientNotify* pN = pServer->m_qNotifies.front();
		pServer->m_qNotifies.pop();

		switch(pN->wType){
		
			case CC_MSG:
				CClientManager::HandleColorPermission(pN->uMode, ((ClientMessage*)pN)->strName, ((ClientMessage*)pN)->strText);
				if(pServer->HasSpeakPermission(pN->uMode))
					pServer->SendMsg(((ClientMessage*)pN)->strName, ((ClientMessage*)pN)->strText);
				else
					pServer->SendNoVoice(pN->guid);
				pServer->m_nTotalMessages++;
				break;
			case CC_ACTION:
				CClientManager::HandleColorPermission(pN->uMode, ((ClientAction*)pN)->strName, ((ClientAction*)pN)->strText);
				if(pServer->HasSpeakPermission(pN->uMode))
					pServer->SendAction(((ClientAction*)pN)->strName, ((ClientAction*)pN)->strText);
				else
					pServer->SendNoVoice(pN->guid);
				pServer->m_nTotalMessages++;
				break;
			case CC_OPMSG:
				CClientManager::HandleColorPermission(pN->uMode, ((ClientOpMsg*)pN)->strName, ((ClientOpMsg*)pN)->strText);
				if(pServer->HasSpeakPermission(pN->uMode))
					pServer->SendOpMsg(((ClientOpMsg*)pN)->strName, ((ClientOpMsg*)pN)->strText, ((ClientOpMsg*)pN)->uParam, ((ClientOpMsg*)pN)->uRecipientMode);
				else
					pServer->SendNoVoice(pN->guid);
				pServer->m_nTotalMessages++;
				break;
			case CC_RENAME:
				if(((ClientRename*)pN)->nVisible != 2){

					pServer->HandleVisible(((ClientRename*)pN)->pClient, ((ClientRename*)pN)->wUserLevel, ((ClientRename*)pN)->nVisible);
				}
				else{

					pServer->SendRename(((ClientRename*)pN)->pClient, ((ClientRename*)pN)->strOldName, ((ClientRename*)pN)->dwOldIP, ((ClientRename*)pN)->wOldPort,
										((ClientRename*)pN)->strNewName, ((ClientRename*)pN)->dwNewIP, ((ClientRename*)pN)->wNewPort,
										((ClientRename*)pN)->wNewLine, ((ClientRename*)pN)->dwNewFiles, ((ClientRename*)pN)->wUserLevel
										);
				}
				break;
			case CC_ERROR:

				n = pServer->GetByID(((ClientError*)pN)->guID);
				if((n < pServer->m_aClients.GetSize()) && (n != -1)){

					CClientSocket* pTmp = pServer->m_aClients[n];
					pServer->m_aClients.RemoveAt(n, 1);
					pServer->SendPart(pTmp->m_strName, pTmp->m_dwIP, pTmp->m_wPort);
					delete pTmp;
					pTmp = NULL;
				}
				break;
			default:
				ASSERT(FALSE);
		}


		// end
		//TRACE("Deleting packet\n");
		delete pN;
		pN = NULL;

		pServer->CheckClients();
	}

	TRACE("Leaving Notify thread\n");
	pServer->m_eNotifyDone.SetEvent();
	return 0;
}

BOOL CClientManager::CheckUserName(const CString strName, CString strHost, DWORD dwIP)
{

	if(strName.GetLength() < 11) return FALSE; // username too short
	if(strName.Find(" ") >= 0)	return FALSE; // username may not contain a space
	if(strName.Find("\n") >= 0) return FALSE;
	if(strName.Find("\r") >= 0) return FALSE;
	if(strName.Find("\t") >= 0) return FALSE;
    if(strName == g_sSettings.GetGodName()) return FALSE;

	// check bans
	for(int i = 0; i < m_aBans.GetSize(); i++){

		//if(m_aBans[i].strName == strName){
		if(Util::WildcardMatch(strName, m_aBans[i].strName, FALSE, TRUE)){

			return FALSE;
		}
		//if(m_aBans[i].strHost == strHost){
		if(Util::WildcardMatch(strHost, 
						m_aBans[i].strHost.IsEmpty() ? m_aBans[i].strName : m_aBans[i].strHost, 
						FALSE, TRUE)){
		
			return FALSE;
		}
		//if(m_aBans[i].strIP == Util::FormatIP(dwIP)){
		if(Util::WildcardMatch(Util::FormatIP(dwIP), 
						m_aBans[i].strIP.IsEmpty() ? m_aBans[i].strName : m_aBans[i].strIP, 
						FALSE, TRUE)){

			return FALSE;
		}
	}

	int nEqualIP = 0;
	for(i = 0; i < m_aClients.GetSize(); i++){

		if(m_aClients[i]->m_strName == strName){

			return FALSE;
		}
		if(!g_sSettings.GetMultiIPOk() && (m_aClients[i]->m_strSrcHost == strHost)){

			return FALSE;
		}
		if(!g_sSettings.GetMultiIPOk() && (m_aClients[i]->m_dwSrcIP == dwIP)){

			return FALSE;
		}
		if(m_aClients[i]->m_dwSrcIP == dwIP){

			nEqualIP++;
		}
		if(nEqualIP > 5){

			return FALSE;
		}
	}

	return TRUE;
}

BOOL CClientManager::CheckUserName(const CString strName, DWORD dwIP, WORD wPort)
{
	
	if(strName.GetLength() < 11) return FALSE; // username too short
	if(strName.Find(" ") >= 0)	return FALSE; // username may not contain a space
	if(strName.Find("\n") >= 0) return FALSE;
	if(strName.Find("\r") >= 0) return FALSE;
	if(strName.Find("\t") >= 0) return FALSE;
    if(strName == g_sSettings.GetGodName()) return FALSE;

	// check if user already exists.
	if(dwIP == 0){

		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName == strName){

				return FALSE;
			}
		}
	}
	else{

		for(int i = 0; i < m_aClients.GetSize(); i++){

			if((m_aClients[i]->m_strName == strName) &&	(m_aClients[i]->m_dwSrcIP != dwIP) && (m_aClients[i]->m_wSrcPort != wPort))
			{
                		
				return FALSE;
			}
		}
	}
	return TRUE;
}

BOOL CClientManager::CheckCommands(const CString strUser, const CString strMsg)
{

	if(strMsg.IsEmpty()) return FALSE;
	if(strMsg.Find("/", 0) != 0)  return FALSE;

	UINT uMode = UM_NORMAL;
	for(int i = 0; i < m_aClients.GetSize(); i++){

		if(m_aClients[i]->m_strName == strUser){

			uMode = m_aClients[i]->m_uMode;
			break;
		} 
	} 

	if(i >= m_aClients.GetSize()) return FALSE;


	return ExecuteChannelCommand(strUser, strMsg, uMode);
}


#ifdef _DEBUG
	#define ASSERT_VALID_STRING(str) ASSERT(!IsBadStringPtr(str, 0xfffff))
#else
	#define ASSERT_VALID_STRING(str)((void)0)
#endif	//	_DEBUG

bool IsNumber(LPCTSTR pszText)
{

	ASSERT_VALID_STRING(pszText);

	BOOL bDecDel = FALSE;

	for(int i = 0; i < lstrlen(pszText); i++){

		if(!_istdigit(pszText[i])){

			if((pszText[i] == '.') && !bDecDel){

				bDecDel = TRUE;
			}
			else{

				return false;
			}
		}
	}

	return true;
}


BOOL CClientManager::ExecuteChannelCommand(const CString strUser, const CString strMsg, const UINT uMode)
{


	if(strMsg == "/channelmodes"){

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}
		// if it is exactly /mode print all the channelmodes
		CString strModes;
		if(m_uMode & CM_EXILEENABLE) strModes += "Exile [" + m_strExile + "] ";
		if(m_uMode & CM_KEYWORD) strModes += "Password [" + m_strKeyword + "] ";
		if(m_uMode & CM_MODERATED) strModes += "Moderated [on]";

		CString strTmp;
		strTmp.LoadString(IDS_MODES);
		ServerAction(strTmp, strModes, 0x01, UM_ADMIN);
		ServerAction(strUser, strMsg, 0, UM_ADMIN);
		return TRUE;
	}

	if(strMsg == "/listbans"){

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}

		CString strMsg;
		strMsg.LoadString(IDS_BANLISTINGSTART);
		ServerAction("", strMsg, 0x04);

		for(int i = 0; i < m_aBans.GetSize(); i++){

			strMsg.Format(IDS_BANLISTING, m_aBans[i].strName, m_aBans[i].strIP, m_aBans[i].strHost);
			ServerAction("", strMsg);
		}

		strMsg.LoadString(IDS_BANLISTINGEND);
		ServerAction("", strMsg, 0x05);
		ServerAction(strUser, "/listbans");
		return TRUE;
	}

	if(strMsg == "/listhidden"){

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}

		CString strMsg;
		strMsg.LoadString(IDS_HIDDENLISTINGSTART);
		ServerAction("", strMsg, 0x04);

		for(int i = 0; i < m_aClients.GetSize(); i++){

			if((m_aClients[i]->m_uMode & UM_HIDDEN) != UM_HIDDEN) continue;

			strMsg.Format(IDS_HIDDENLISTING, m_aClients[i]->m_strName);
			ServerAction("", strMsg);
		}

		strMsg.LoadString(IDS_HIDDENLISTINGEND);
		ServerAction("", strMsg, 0x05);
		ServerAction(strUser, "/listhidden");
		return TRUE;
	}

	CTokenizer token(strMsg, " ");
	CString strCmd, strParam;

	if(!token.Next(strCmd)) return FALSE; // first ist /mode.
	//token.Next(strParam); //) return FALSE; // note this is <param> for topic motd, kick, muzzle
	strParam = token.Tail();

	if(strCmd == "/private"){

		CTokenizer pr(strParam, " ");
		CString strName;
		if(!pr.Next(strName)) return FALSE;
		
		CString strPrivate = pr.Tail();
		if(strPrivate.IsEmpty()) return FALSE;

		CString strText;
		if(g_sSettings.GetColor() == 0){
			
			strText = strUser + " [Private] " + strPrivate;
		}
		else{
			
			strText = strUser + " #c3#[Private] #c1#" + strPrivate;
		}
		
		
		ServerAction(strUser, strText, 4000, UM_NORMAL);
		if(strUser.Find(strName, 0) != 0){

			ServerAction(strName, strText, 4000, UM_NORMAL);
		}
		return TRUE;
	}
	else if(strCmd == "/channelname"){
	
		ServerAction(strUser, m_strRoomFull, 4000+0x008, UM_NORMAL);
		ServerAction(strUser, strCmd, 4000+0x008, UM_NORMAL);
		return TRUE;
	}
	else if(strCmd == "/login"){


		int uMode = g_sSettings.GetLogin(strParam);

		if(((m_uMode & CM_MAINCHANNEL) != 0) && ((uMode & LOGIN_MAIN) != LOGIN_MAIN)){

			SendPermissionDenied(strUser, "/login ***hidden***");
		}
		else if(((m_uMode & CM_MAINCHANNEL) == 0) && ((uMode & LOGIN_SUB) != LOGIN_SUB)){
			
			SendPermissionDenied(strUser, "/login ***hidden***");
		}
		else if(uMode == 0){

			SendPermissionDenied(strUser, "/login ***hidden***");
		}
		else{
			
			for(int i = 0; i < m_aClients.GetSize(); i++){

				if(m_aClients[i]->m_strName == strUser){

					if((uMode & UM_VOICED) == UM_VOICED)
						m_aClients[i]->AddMode(UM_VOICED);
					if((uMode & UM_AGENT) == UM_AGENT)
						m_aClients[i]->AddMode(UM_AGENT);
					if((uMode & UM_ADMIN) == UM_ADMIN)
						m_aClients[i]->AddMode(UM_ADMIN);

					SendMode(strUser, "/login ***hidden***");
					UpdateModeUserlist(m_aClients[i]->m_strName, m_aClients[i]->m_uMode);
					SendIPClient(m_aClients[i], TRUE);
					break;
				}
			}
		}

		return TRUE;
	}

	if(strCmd == "/topic"){

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strCmd);
			return TRUE;
		}

		SetTopic(strParam);
		SendTopic();
		ServerAction(strUser, strMsg, 0, UM_ADMIN);
		return TRUE;
	}
	else if(strCmd == "/setmotd"){

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strCmd);
			return TRUE;
		}

		SetMotd(strParam);
		SendMotd();
		ServerAction(strUser, strMsg, 0, UM_ADMIN);
		return TRUE;
	}
	else if(strCmd == "/stat"){

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strCmd);
			return TRUE;
		}
		if(strParam.IsEmpty()){

			ServerAction(strUser, strCmd);
			SendChannelStats();
			return TRUE;
		}

		CString strStats;
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName == strParam){

				strParam = m_aClients[i]->m_strName;
				strStats = m_aClients[i]->GetUserStats();
				break;
			}
		}
		if(!strStats.IsEmpty()){

			ServerAction("", strStats, 0x02);
			ServerAction(strUser, strMsg, 0, UM_ADMIN);
		}
		else{

			SendCmdNotUnderstood(strUser, strMsg);
		}
		return TRUE;
	}
	else if(strCmd == "/kick"){

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strCmd);
			return TRUE;
		}
		// Syntax: /kick Username [Reason]
	
		CTokenizer token(strParam, " ");
		CString strName, strReason;
		token.Next(strName);
		strReason = token.Tail();
		
		if(strName == strUser){
			
			SendPermissionDenied(strUser, strCmd + " " + strParam);
			return TRUE; // user may not kick himself
		}
		

		CClientSocket *pKick = NULL;
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strName, 0) == 0){

				if(((uMode & UM_AGENT) != UM_AGENT) && ((m_aClients[i]->m_uMode & UM_AGENT) == UM_AGENT)){

					SendPermissionDenied(strUser, strCmd + " " + strParam);
					return TRUE; // user operators may not kick agents
				}
				strName = m_aClients[i]->m_strName;
				pKick = m_aClients[i];
				m_aClients.RemoveAt(i);
				break;
			}
		}
		if(pKick > NULL){

			CString strKick;
			strKick.Format(IDS_SERVER_KICKED, strUser, strReason);
			ServerAction(strName, strKick, 0x05);
			SendPart(pKick->m_strName, pKick->m_dwIP, pKick->m_wPort);
			pKick->LogOut();
			delete pKick;
		}
		else{

			SendCmdNotUnderstood(strUser, strMsg);
		}
		return TRUE;
	}
	else if(strCmd == "/kickban"){

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strCmd);
			return TRUE;
		}
		// Syntax: /kickban Username [Reason]
		CTokenizer token(strParam, " ");
		CString strName, strReason;
		token.Next(strName);
		strReason = token.Tail();
		
		if(strName == strUser){
			
			SendPermissionDenied(strUser, strCmd + " " + strParam);
			return TRUE; // user may not kick himself
		}
		
		CClientSocket *pKick = NULL;
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strName, 0) == 0){

				if(((uMode & UM_AGENT) != UM_AGENT) && ((m_aClients[i]->m_uMode & UM_AGENT) == UM_AGENT)){

					SendPermissionDenied(strUser, strCmd + " " + strParam);
					return TRUE; // user operators may not kick agents
				}
				strName = m_aClients[i]->m_strName;

				BAN ban;
				ban.strName = m_aClients[i]->m_strName;
				ban.strIP   = Util::FormatIP(m_aClients[i]->m_dwSrcIP);
				ban.strHost = m_aClients[i]->m_strSrcHost;
    			m_aBans.Add(ban);
				m_aClients[i]->AddMode(UM_BANNED);
				
				pKick = m_aClients[i];
				m_aClients.RemoveAt(i);

				break;
			}
		}
		if(pKick > NULL){

			CString strKick;
			strKick.Format(IDS_SERVER_KICKEDBANNED, strUser, strReason);
			ServerAction(strParam, strKick, 0x05);
			SendPart(pKick->m_strName, pKick->m_dwIP, pKick->m_wPort);
			pKick->LogOut();
			delete pKick;
		}
		else{

			SendCmdNotUnderstood(strUser, strMsg);
		}
		return TRUE;
	}
	else if(strCmd == "/exile"){

		if(!CheckPermission(uMode, UM_ADMIN) || ((m_uMode & CM_EXILEENABLE) != CM_EXILEENABLE)){

			SendPermissionDenied(strUser, strCmd);
			return TRUE;
		}
		// Syntax: /exile Username Target [Reason]
		
		CTokenizer token(strParam, " ");
		CString strName, strReason;
		token.Next(strName);
		strReason = token.Tail();
		
		CClientSocket *pExile = NULL;
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strName, 0) == 0){

				if(((uMode & UM_AGENT) != UM_AGENT) && ((m_aClients[i]->m_uMode & UM_AGENT) == UM_AGENT)){

					SendPermissionDenied(strUser, strCmd + " " + strParam);
					return TRUE; // user operators may not kick agents
				}
				strName = m_aClients[i]->m_strName;
				pExile = m_aClients[i];
				m_aClients.RemoveAt(i);
				break;
			}
		}
		if(pExile > NULL){
			
			pExile->SendRedirect(m_strExile);
			CString strExile;
			strExile.Format(IDS_SERVER_EXILED, m_strExile, strUser, strReason);
			ServerAction(strParam, strExile, 0x05);
			SendPart(pExile->m_strName, pExile->m_dwIP, pExile->m_wPort);
		}
		else{

			SendCmdNotUnderstood(strUser, strMsg);
		}
		return TRUE;
	}
	else if((strCmd == "/exileto") && ((m_uMode & CM_EXILEENABLE) == CM_EXILEENABLE)){

		if(!CheckPermission(uMode, UM_ADMIN) || ((m_uMode & CM_EXILEENABLE) != CM_EXILEENABLE)){

			SendPermissionDenied(strUser, strCmd);
			return TRUE;
		}
		// Syntax: /exile Username Target [Reason]
        CTokenizer token(strParam, " ");
		CString strKick, strTarget, strReason;
	
		token.Next(strKick);
		token.Next(strTarget);
		strReason = token.Tail();

		CClientSocket *pExile = NULL;
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strKick, 0) == 0){

				strKick = m_aClients[i]->m_strName;
				pExile = m_aClients[i];
				m_aClients.RemoveAt(i);
				break;
			}
		}
		if(pExile > NULL){
			
			pExile->SendRedirect(strTarget);
			CString strExile;
			strExile.Format(IDS_SERVER_EXILED, strTarget, strUser, strReason);
			ServerAction(strKick, strParam, 0x05);
			SendPart(pExile->m_strName, pExile->m_dwIP, pExile->m_wPort);
		}
		else{

			SendCmdNotUnderstood(strUser, strMsg);
		}
		return TRUE;
	}

	else if((strCmd == "/usermodes") || (strCmd == "/usermode")){

		// Display user modes
		if(!CheckPermission(uMode, UM_VOICED) && !CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strCmd);
			return TRUE;
		}

		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strParam, 0) == 0){

				strParam = m_aClients[i]->m_strName;
				CString strModes, strOut;
				UINT uMode = m_aClients[i]->m_uMode;
				if(uMode & UM_BANNED) strModes += "banned ";
				if(uMode & UM_AGENT) strModes += "agent ";
				if(uMode & UM_HIDDEN) strModes += "hidden ";
				if(uMode & UM_ADMIN) strModes += "admin ";
				if(uMode & UM_VOICED) strModes += "voiced ";

				strOut.Format(IDS_USERMODES, m_aClients[i]->m_strName, strModes);
				ServerAction(strUser, strOut, 0, UM_VOICED);
				return TRUE;
			}

		}
		return FALSE;
	}

	strParam.TrimRight();

	// add channel mode
	if(strCmd == "/setpassword"){ // keyword

		if(!CheckPermission(uMode, UM_AGENT)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		if(m_uMode & CM_KEYWORD){

			// Keyword already set
			CString strMsg;
			strMsg.LoadString(IDS_ERROR_SERVER_KEYEXISTS);
			ServerAction("", strMsg);
		}
		else{

			m_uMode |= CM_KEYWORD;
			m_strKeyword = strParam;
			SendMode(strUser, "/setpassword " + strParam);
		}

	}
	else if(strCmd == "/setmoderated"){ // moderated

		if(!CheckPermission(uMode, UM_AGENT)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		m_uMode |= CM_MODERATED;
		SendMode(strUser, "/setmoderated");
	}
	else if(strCmd == "/hideid"){ // no userid

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		m_uMode |= CM_NOID;
		SendMode(strUser, "/hideid");
	}
	else if(strCmd == "/enableexile"){


		if(!CheckPermission(uMode, UM_AGENT)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}
		if(!strParam.IsEmpty()){

			m_uMode |= CM_EXILEENABLE;
			m_strExile = strParam;
			SendMode(strUser, "/enableexile " + strParam);
		}
		else{

			SendCmdNotUnderstood(strUser, strMsg);
		}
	}
	// end channel modes

	// start user modes
	else if(strCmd == "/voice"){ // speak permission

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strParam, 0) == 0){

				strParam = m_aClients[i]->m_strName;
				m_aClients[i]->AddMode(UM_VOICED);
				SendMode(strUser, "/voice " + strParam);
				UpdateModeUserlist(m_aClients[i]->m_strName, m_aClients[i]->m_uMode);
				break;
			}
		}
		if(i >= m_aClients.GetSize()){

			SendCmdNotUnderstood(strUser, strMsg);
		}
	}
	else if(strCmd == "/admin"){ // operator status

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strParam, 0) == 0){

				strParam = m_aClients[i]->m_strName;
				m_aClients[i]->AddMode(UM_ADMIN);
				SendMode(strUser, "/admin " + strParam);
				UpdateModeUserlist(m_aClients[i]->m_strName, m_aClients[i]->m_uMode);
				SendIPClient(m_aClients[i], TRUE);
				break;
			}
		}
		if(i >= m_aClients.GetSize()){

			SendCmdNotUnderstood(strUser, strMsg);
		}
	}
	else if(strCmd == "/limit"){ // limit

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		if(!strParam.IsEmpty() || !IsNumber(strParam)){

			SetLimit(atoi((LPTSTR)(LPCTSTR)strParam));
			SendMode(strUser, "/limit " + strParam);
			
		}
		else{

			SendCmdNotUnderstood(strUser, strMsg);
		}
	}
	else if(strCmd == "/ban"){ // ban

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strParam, 0) == 0){

				strParam = m_aClients[i]->m_strName;
				BAN ban;
				ban.strName = m_aClients[i]->m_strName;
				ban.strIP   = Util::FormatIP(m_aClients[i]->m_dwSrcIP);
				ban.strHost = m_aClients[i]->m_strSrcHost;
    			m_aBans.Add(ban);

				m_aClients[i]->AddMode(UM_BANNED);
				SendMode(strUser, "/ban " + strParam + " (" + ban.strHost + ")");
				UpdateModeUserlist(m_aClients[i]->m_strName, m_aClients[i]->m_uMode);
				break;
			}
		}
		if(i >= m_aClients.GetSize()){

			//SendCmdNotUnderstood(strUser, strMsg);
			BAN ban;
			ban.strName = strParam;
			m_aBans.Add(ban);
			SendMode(strUser, "/ban " + strParam);
		}
	}
	else if(strCmd == "/agent"){ // impose, may use fake user name command /impose

		if(!CheckPermission(uMode, UM_AGENT)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strParam, 0) == 0){

				strParam = m_aClients[i]->m_strName;
				m_aClients[i]->AddMode(UM_AGENT);
				SendMode(strUser, "/agent " + strParam);
				UpdateModeUserlist(m_aClients[i]->m_strName, m_aClients[i]->m_uMode);
				break;
			}
		}
		if(i >= m_aClients.GetSize()){

			SendCmdNotUnderstood(strUser, strMsg);
		}
	}
	else if(strCmd == "/hide"){ // hide user

		if(!CheckPermission(uMode, UM_HIDDEN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strParam, 0) == 0){

				strParam = m_aClients[i]->m_strName;
				SendMode(strUser, "/hide " + strParam);
				m_aClients[i]->AddMode(UM_HIDDEN);
				UpdateModeUserlist(m_aClients[i]->m_strName, m_aClients[i]->m_uMode);
				break;
			}
		}
		if(i >= m_aClients.GetSize()){

			SendCmdNotUnderstood(strUser, strMsg);
		}
	}

	else if(strCmd == "/rempassword"){ // keyword

		if(!CheckPermission(uMode, UM_AGENT)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		if(m_uMode | CM_KEYWORD){

			if(m_strKeyword.Compare(strParam) == 0){

				m_uMode &= ~CM_KEYWORD;
				m_strKeyword.Empty();
				SendMode(strUser, "/rempassword ");

			}
			else{

				CString strMsg;
				strMsg.LoadString(IDS_ERROR_SERVER_WRONGKEY);
				ServerAction("", strMsg);
			}
		}
		else{

			CString strMsg;
			strMsg.LoadString(IDS_ERROR_SERVER_NOKEY);
			ServerAction("", strMsg);
		}

	}
	else if(strCmd == "/remmoderated"){ // moderated

		if(!CheckPermission(uMode, UM_AGENT)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		m_uMode &= ~CM_MODERATED;

		SendMode(strUser, "/remmoderated");
	}
	else if(strCmd == "/showid"){ // no userid

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		m_uMode &= ~CM_NOID;
		SendMode(strUser, "/showid");
	}
	else if(strCmd == "/disableexile"){

		if(!CheckPermission(uMode, UM_AGENT)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}
		m_uMode &= ~CM_EXILEENABLE;
		m_strExile.Empty();
		SendMode(strUser, "-e");
	}
	// end channel modes
	else if(strCmd == "/muzzle"){ // speak permission

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strParam, 0) == 0){

				strParam = m_aClients[i]->m_strName;
				m_aClients[i]->RemoveMode(UM_VOICED);
				TRACE("%X", m_aClients[i]->GetUserMode());
				SendMode(strUser, "/muzzle " + strParam);
				UpdateModeUserlist(m_aClients[i]->m_strName, m_aClients[i]->m_uMode);
				break;
			}
		}
		if(i >= m_aClients.GetSize()){

			SendCmdNotUnderstood(strUser, strMsg);
		}
	}
	else if(strCmd == "/remadmin"){ // operator status

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strParam, 0) == 0){

				strParam = m_aClients[i]->m_strName;
				m_aClients[i]->RemoveMode(UM_ADMIN);
				SendMode(strUser, "/remadmin " + strParam);
				UpdateModeUserlist(m_aClients[i]->m_strName, m_aClients[i]->m_uMode);
				SendIPClient(m_aClients[i], FALSE);
				break;
			}
		}
		if(i >= m_aClients.GetSize()){

			SendCmdNotUnderstood(strUser, strMsg);
		}
	}
	else if(strCmd == "/unban"){ // ban

		if(!CheckPermission(uMode, UM_ADMIN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}	
		CString strMsg;
		INT_PTR nBans = m_aBans.GetSize();	   
		for(int i = 0; i < nBans; i++){

			if((m_aBans[i].strName.Find(strParam, 0) == 0) || (m_aBans[i].strHost == strParam) || (m_aBans[i].strIP == strParam)){

				strParam = m_aClients[i]->m_strName;
				strMsg.Format(IDS_SERVER_UNBAN, m_aBans[i].strName, m_aBans[i].strIP, m_aBans[i].strHost);
				m_aBans.RemoveAt(i, 1);
                ServerAction(strUser, strMsg);
				break;
			}

		}
		if(i >= nBans){

			strMsg.Format(IDS_NOSUCHBAN, strParam);
			ServerAction("", strMsg);
			return TRUE;
		}
		for(i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strParam, 0) == 0){

				UpdateModeUserlist(m_aBans[i].strName, m_aClients[i]->m_uMode);
				m_aClients[i]->RemoveMode(UM_BANNED);
				break;
			}
		}
	}
	else if(strCmd == "/show"){ // show hidden user

		if(!CheckPermission(uMode, UM_HIDDEN)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strParam, 0) == 0){

				strParam = m_aClients[i]->m_strName;
				SendMode(strUser, strMsg);
				m_aClients[i]->RemoveMode(UM_HIDDEN);
				UpdateModeUserlist(m_aClients[i]->m_strName, m_aClients[i]->m_uMode);
				break;
			}
		}
		if(i >= m_aClients.GetSize()){

			SendCmdNotUnderstood(strUser, strMsg);
		}
	}
	else if(strCmd == "/remagent"){ // impose, may use fake user name command /impose

		if(!CheckPermission(uMode, UM_AGENT)){

			SendPermissionDenied(strUser, strMsg);
			return TRUE;
		}
		for(int i = 0; i < m_aClients.GetSize(); i++){

			if(m_aClients[i]->m_strName.Find(strParam, 0) == 0){

				strParam = m_aClients[i]->m_strName;
				SendMode(strUser, "/remagent " + strParam);
				m_aClients[i]->RemoveMode(UM_AGENT);
				UpdateModeUserlist(m_aClients[i]->m_strName, m_aClients[i]->m_uMode);
				break;
			}
		}
		if(i >= m_aClients.GetSize()){

			SendCmdNotUnderstood(strUser, strMsg);
		}
	}
	else if(strCmd != "/me" && strCmd != "/action" && strCmd != "/emote"){

		SendCmdNotUnderstood(strUser, strMsg);
		return TRUE;
	}
	
	return TRUE;
}

void CClientManager::FixString(CString& strFix)
{
	strFix.Replace("\n", " ");
	strFix.Replace("\r", " ");
	strFix.Replace("\t", " ");
	strFix.Replace("{\rtf", "####");
	if(strFix.GetLength() > 400){

		strFix = strFix.Left(400);
	}
}

int CClientManager::GetByID(GUID guid)
{

	for(int i = 0; i < m_aClients.GetSize(); i++){

		if(m_aClients[i]->m_guID == guid){

			return i;
		}
	}
	
	return -1;
}


BOOL CClientManager::HasSpeakPermission(UINT uMode)
{

	if((m_uMode & CM_MODERATED) == CM_MODERATED){

		return ((uMode & UM_VOICED) == UM_VOICED) || ((uMode & UM_ADMIN) == UM_ADMIN);
	}
	else{

		return TRUE;
	}
}

void CClientManager::SendNoVoice(GUID guid)
{

	int n = GetByID(guid);
	if(n >= 0){

		CString strMsg;
		strMsg.LoadString(IDS_NOVOICE);
		m_aClients[n]->SendMsg(g_sSettings.GetGodName(), strMsg);
	}
}

BOOL CClientManager::CheckPermission(UINT uMode, UINT uRequired)
{

	TRACE("Permission: (%03X & %03X) == %3X\n", uMode, uRequired, uMode & uRequired);
	return (uMode & uRequired) != 0;
}

void CClientManager::SendChannelStats(void)
{

	if(m_nTotalMessages < 0) m_nTotalMessages = 0;

	DWORD dwWinMX331 = 0; float f331Perc = 0;
	DWORD dwWinMX353 = 0; float f353Perc = 0;
	DWORD dwIPClient = 0; float fIPPerc  = 0;
	DWORD dwRoboMX   = 0; float fRMXPerc = 0;
	DWORD dwRabbit   = 0; float fRabPerc = 0;
	DWORD dwUnknown  = 0; float fUnkPerc = 0;

	for(int i = 0; i < m_aClients.GetSize(); i++){

		if(m_aClients[i]->m_wClientType == CLIENT_WINMX331) dwWinMX331++;
		else if(m_aClients[i]->m_wClientType == CLIENT_WINMX353) dwWinMX353++;
		else if(m_aClients[i]->m_wClientType == CLIENT_IPCLIENT){
		
			if(m_aClients[i]->m_strClientString == "Rabbit") dwRabbit++;
			else if(m_aClients[i]->m_strClientString == "Rabbit 1.x or RoboMX 1.01") dwIPClient++;
			else dwRoboMX++; // RoboMX sends random string :D
		}
		else{

			dwUnknown++;
		}
	}
	
	float fCnt = (float)m_aClients.GetSize();
	f331Perc = ((float)dwWinMX331)/fCnt*100.f;
	f353Perc  = ((float)dwWinMX353)/fCnt*100.f;
	fIPPerc  = ((float)dwIPClient)/fCnt*100.f;
	fRabPerc = ((float)dwRabbit)/fCnt*100.f;
	fRMXPerc = ((float)dwRoboMX)/fCnt*100.f;
	fUnkPerc = ((float)dwUnknown)/fCnt*100.f;

	CString strTmp;

	ServerAction("", Util::LoadString(IDS_SERVER_STATS), 0x04, UM_NORMAL);

	strTmp.Format(IDS_SERVER_UPTIME, m_strStartDate, m_strStartTime);
	ServerAction("", strTmp, 0x01, UM_NORMAL);

	strTmp.Format(IDS_SERVER_CLIENTS, m_aClients.GetSize());
	ServerAction("", strTmp, 0x01, UM_NORMAL);
	
	if(dwWinMX331){
	
		strTmp.Format(IDS_SERVER_MX331, dwWinMX331, f331Perc);
		ServerAction("", strTmp, 0x01, UM_NORMAL);
	}
	if(dwWinMX353){
	
		strTmp.Format(IDS_SERVER_MX353, dwWinMX353, f353Perc);
		ServerAction("", strTmp, 0x01, UM_NORMAL);
	}
	if(dwIPClient){
	
		strTmp.Format(IDS_SERVER_IPCLIENT, dwIPClient, fIPPerc);
		ServerAction("", strTmp, 0x01, UM_NORMAL);
	}
	if(dwRoboMX){
	
		strTmp.Format(IDS_SERVER_RMX, dwRoboMX, fRMXPerc);
		ServerAction("", strTmp, 0x01, UM_NORMAL);
	}
	if(dwRabbit){
	
		strTmp.Format(IDS_SERVER_RABBIT, dwRabbit, fRabPerc);
		ServerAction("", strTmp, 0x01, UM_NORMAL);
	}
	if(dwUnknown){
	
		strTmp.Format(IDS_SERVER_OTHER, dwUnknown, fUnkPerc);
		ServerAction("", strTmp, 0x01, UM_NORMAL);
	}

	strTmp.Format(IDS_SERVER_JOINS, m_nTotalJoins);
	ServerAction("", strTmp, 0x01, UM_NORMAL);
	strTmp.Format(IDS_SERVER_JOINSFAILED, m_nFailedJoins);
	ServerAction("", strTmp, 0x01, UM_NORMAL);
	strTmp.Format(IDS_SERVER_MSGS, m_nTotalMessages);
	ServerAction("", strTmp, 0x01, UM_NORMAL);

	CString strLurk;

	time_t t;
	time(&t);
	double elapsed = difftime(t, m_cStartTime.GetTime());
	if(elapsed == 0) elapsed = 0.00001f;

	double mps = (float)(m_nTotalMessages)/elapsed;

	if(mps > 0)	strLurk.Format(IDS_LURKMETER1, log(1/mps)-1);   //mps is of course > 0 ;)
	else strLurk.LoadString(IDS_LURKMETER2);

	strTmp.Format(IDS_LURKMETER, strLurk);
	ServerAction("", strTmp, 0x01, UM_NORMAL);	


	ServerAction("", Util::LoadString(IDS_SERVER_STATSEND), 0x05, UM_NORMAL);

}

void CClientManager::SendIPClient(CClientSocket *pAdmin, BOOL bAdd)
{

	if(pAdmin == NULL) return;
	if(pAdmin->m_wClientType != CLIENT_IPCLIENT) return;

	WORD wMode = 0;
	for(int i = 0; i < m_aClients.GetSize(); i++){

		if((m_aClients[i]->m_uMode & UM_HIDDEN) == UM_HIDDEN) continue;
		wMode = 0;
		if((m_aClients[i]->m_uMode & UM_VOICED) == UM_VOICED) wMode = 2;
		if((m_aClients[i]->m_uMode & UM_ADMIN) == UM_ADMIN)wMode = 1;
		pAdmin->SendIPClient(m_aClients[i]->m_strName,
							 m_aClients[i]->m_dwIP,
							 m_aClients[i]->m_wPort,
							 m_aClients[i]->m_wLineType,
							 m_aClients[i]->m_dwFiles,
							 wMode,
							 bAdd ? m_aClients[i]->m_dwSrcIP : 0,
							 bAdd ? m_aClients[i]->m_strSrcHost : "");
	}
}


UINT CClientManager::AddClient(CClientSocket *pClient)
{

	if(g_sSettings.GetBlockNushi() && (pClient->m_wClientType < CLIENT_WINMX353)){
	
		m_nFailedJoins++;
		pClient->LogOut();
		delete pClient;
		pClient = 0;
		return 0;
	}

	if(!CheckUserName(pClient->m_strName, pClient->m_strSrcHost, pClient->m_dwSrcIP)){

		// Client has illegal name
		TRACE("Username check failed (%s)\n", pClient->m_strName);
		m_nFailedJoins++;
		pClient->LogOut();
		delete pClient;
		pClient = 0;
		return 0;
	}

	pClient->SetUserMode(UM_NORMAL);

	if(!pClient->HighlevelHandshake(m_strKeyword)){

		// Login failed
		m_nFailedJoins++;
		WriteText(RGB(128, 0, 0), IDS_ERROR_SERVER_HANDSHAKE, pClient->m_strName, Util::FormatIP(pClient->m_dwSrcIP), pClient->m_strSrcHost);
		delete pClient;
		pClient = 0;
		return 0;
	}
	

	pClient->StartUp();

	pClient->SetUserMode(UM_NORMAL|g_sSettings.GetDefaultUserMode());

	if((m_uMode & CM_MAINCHANNEL) == 0){

		if((m_aClients.GetSize() == 0) && g_sSettings.GetSubFirstIsSuper()){

			pClient->SetUserMode(UM_NORMAL|UM_VOICED|UM_ADMIN|UM_AGENT);
		}
	}

	if(g_sSettings.GetLocalIsOp() && (pClient->m_dwSrcIP == 16777343) && ((pClient->m_uMode&UM_ADMIN) != UM_ADMIN)){

		//pClient->AddMode(UM_ADMIN);
		pClient->m_uMode|=UM_ADMIN;
	}

	CString strSend = m_strTopic;
	Util::ReplaceVars(strSend);
	strSend.Replace("%ROOMNAME%", m_strRoomBase);
	strSend.Replace("%USERS%", Util::Formatint(m_aClients.GetSize()));
	strSend.Replace("%NAME%", pClient->m_strName);
	strSend.Replace("%IP%", Util::FormatIP(pClient->m_dwSrcIP));
	strSend.Replace("%HOSTNAME%", pClient->m_strSrcHost);
	strSend.Replace("%CLIENT%", pClient->m_strClientString);
	strSend.Replace("%CLIENTVER%", pClient->m_strClientVer);
 	strSend.Replace("%LINE%", Util::FormatLine(pClient->m_wLineType));
	strSend.Replace("%FILES%", Util::Formatint(pClient->m_dwFiles));

	pClient->SendTopic(strSend);

	CheckClients();

	if(g_sSettings.GetGodVisible()){

		pClient->SendUserlist(m_strGodName, 0, 0, 
							  g_sSettings.GetGodLine(),
							  g_sSettings.GetGodFiles(), 1);
	}

	WORD wMode = 0;

	for(int i = 0; i < m_aClients.GetSize(); i++){

		if((m_aClients[i]->m_uMode & UM_HIDDEN) == UM_HIDDEN) continue;
		wMode = 0;
		if((m_aClients[i]->m_uMode & UM_VOICED) == UM_VOICED) wMode = 2;
		if((m_aClients[i]->m_uMode & UM_ADMIN) == UM_ADMIN)wMode = 1;
		pClient->SendUserlist(m_aClients[i]->m_strName, 
							  m_aClients[i]->m_dwIP, 
							  m_aClients[i]->m_wPort, 
			                  m_aClients[i]->m_wLineType,
							  m_aClients[i]->m_dwFiles,
							  wMode);

	}
	
	strSend = m_strMotd;
	Util::ReplaceVars(strSend);
	strSend.Replace("%ROOMNAME%", m_strRoomBase);
	strSend.Replace("%USERS%", Util::Formatint(m_aClients.GetSize()));
	strSend.Replace("%NAME%", pClient->m_strName);
	strSend.Replace("%IP%", Util::FormatIP(pClient->m_dwSrcIP));
	strSend.Replace("%HOSTNAME%", pClient->m_strSrcHost);
	strSend.Replace("%CLIENT%", pClient->m_strClientString);
	strSend.Replace("%CLIENTVER%", pClient->m_strClientVer);
	strSend.Replace("%LINE%", Util::FormatLine(pClient->m_wLineType));
	strSend.Replace("%FILES%", Util::Formatint(pClient->m_dwFiles));
	pClient->SendMotd(strSend);

	INT_PTR nPos = m_aClients.Add(pClient);
	

	CoCreateGuid(&pClient->m_guID);
	
	if(pClient->m_hMsgTarget == NULL)
		pClient->m_hMsgTarget = m_hWndTarget;

	wMode = 0;
	if((pClient->m_uMode & UM_AGENT) == UM_AGENT) wMode = g_sSettings.GetAgentIcon();
	else if((pClient->m_uMode & UM_ADMIN) == UM_ADMIN) wMode = g_sSettings.GetAdminIcon();
	else if((pClient->m_uMode & UM_VOICED) == UM_VOICED) wMode = g_sSettings.GetVoiceIcon();
	else wMode = g_sSettings.GetNormalIcon();
	SendJoin(pClient->m_strName, pClient->m_dwIP, pClient->m_wPort, pClient->m_wLineType, pClient->m_dwFiles, wMode, pClient->m_dwSrcIP);
	
	m_nTotalJoins++;

	strSend = g_sSettings.GetWelcome();
	if(strSend.GetLength()){

		Util::ReplaceVars(strSend);
		strSend.Replace("%ROOMNAME%", m_strRoomBase);
		strSend.Replace("%USERS%", Util::Formatint(m_aClients.GetSize()));
		strSend.Replace("%NAME%", pClient->m_strName);
		strSend.Replace("%IP%", Util::FormatIP(pClient->m_dwSrcIP));
		strSend.Replace("%HOSTNAME%", pClient->m_strSrcHost);
		strSend.Replace("%CLIENT%", pClient->m_strClientString);
		strSend.Replace("%CLIENTVER%", pClient->m_strClientVer);
		strSend.Replace("%LINE%", Util::FormatLine(pClient->m_wLineType));
		strSend.Replace("%FILES%", Util::Formatint(pClient->m_dwFiles));
		SendWelcome(strSend);
	}

	SetUsers(m_aClients.GetSize());
	return 1;
}

void CClientManager::HandleColorPermission(UINT uMode, CString &strName, CString &strText)
{

	switch(g_sSettings.GetColor()){

	case 2: // voiced, admin, agent
		if(((uMode & UM_VOICED) != UM_VOICED) && ((uMode & UM_ADMIN) != UM_ADMIN) && ((uMode & UM_AGENT) != UM_AGENT)){

			CClientManager::RemoveColor(strName);
			CClientManager::RemoveColor(strText);
		}
		break;
	case 3: // admin, agent
		if(((uMode & UM_ADMIN) != UM_ADMIN) && ((uMode & UM_AGENT) != UM_AGENT)){

			CClientManager::RemoveColor(strName);
			CClientManager::RemoveColor(strText);
		}
		break;
	case 4: // agent
		if((uMode & UM_AGENT) != UM_AGENT){

			CClientManager::RemoveColor(strName);
			CClientManager::RemoveColor(strText);
		}
		break;
	default: // everyone / disabled
		break;
	}
}

void CClientManager::RemoveColor(CString &rString)
{

	if(rString.Find("#") == -1) return;
	
	rString.Replace("#cx#", "");
	CString strRep;
	for(int i = 1; i < 10; i++){

		strRep.Format("#c%d#", i);
		rString.Replace(strRep, "");
	}
	for(i = 10; i <= 40; i++){

		strRep.Format("#c%d#", i);
		rString.Replace(strRep, "");
	}
}


void CClientManager::SetUsers(DWORD dwUsers)
{

	m_dwUsers = dwUsers;
}

void CClientManager::SetLimit(DWORD dwLimit)
{

	if(dwLimit > 1000) m_dwLimit = 1000;
	else m_dwLimit = dwLimit;
}

void CClientManager::SetRoom(const CString strName)
{

	m_strRoomFull.Replace(m_strRoomBase, strName.Left(40));
	m_strRoomBase = strName.Left(40);
}

