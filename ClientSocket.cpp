/*
** Copyright (C) 2004 Thees Schwab
**  
4** This program is free software; you can redistribute it and/or modify
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


#include ".\clientsocket.h"
#include "util.h"
#include "settings.h"
#include "resource.h"
#include "Tokenizer.h"
#include "ClientManager.h"

extern CSettings g_sSettings;

extern UINT UWM_CLNNOTIFY;


const BYTE color_map[] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58,
					 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74, 75,
					 76, 80, 81, 82, 83, 84, 221, 228, 237, 244, 252, 253, 254};

const BYTE num_color = sizeof(color_map)/sizeof(char);

#ifdef _DEBUG
// tracing winmx binary packets to the debug console
void Hex_Trace(PVOID pBuffer, int nLen)
{

	char* pOut = (char*)pBuffer;

	TRACE("> ");

	for(int i = 0; i < nLen; i++){

		TRACE("%02X ", pOut[i]);
	}

	TRACE("\n");
	
}
#else
#define Hex_Trace
#endif

CClientSocket::CClientSocket(SOCKET sSocket) :
	CMySocket(sSocket),
	m_strName(""),
	m_strSrcHost(""),
	m_dwSrcIP(0),
	m_wSrcPort(0),
	m_dwIP(0),
	m_wPort(0),
	m_wLineType(0),
	m_dwFiles(0),
	m_uMode(UM_PENDING),
	m_bListen(FALSE),
	m_hMsgTarget(NULL),
	m_wClientType(CLIENT_WINMX331),
	m_pThread(NULL),
	m_strClientString("WinMX"),
	m_strClientVer("3.3x or compatible"),
	m_nMessages(0),
	m_dwUpKey(0),
	m_dwDWKey(0),
	m_dwLast(0),
	m_wFloodCounter(0),
	m_dwLastRen(0),
	m_wRenCounter(0),
	m_bBlocked(FALSE)

{

	ZeroMemory(m_cBuffer, MAX_BUFFER_SIZE);
	m_eDone.SetEvent();
}

CClientSocket::~CClientSocket(void)
{
}

BOOL CClientSocket::HandShake(CString& strRoom)
{

	WORD wType = 0;
	WORD wLen  = 0;

	*(WORD*)m_cBuffer = 0x31;

	if(Send(m_cBuffer, 1, 10) != 1){

		Close();
		m_eDone.SetEvent();
		return FALSE;
	}

	BYTE btKey[16];
	CreateCryptKeyID(0x0058, (BYTE*)btKey);
	GetCryptKey((BYTE*)btKey, &m_dwUpKey, &m_dwDWKey);

	// Recieve Client Keyblock
	if(Recv(m_cBuffer, 16, 5) != 16){

		Close();
		m_eDone.SetEvent();
		return FALSE;
	}

	if(GetCryptKeyID((BYTE*)m_cBuffer) != 0x57){

		TRACE("Invalid CryptKey ID\n");
		Close();
		m_eDone.SetEvent();
		return FALSE;
	}

	ZeroMemory(m_cBuffer, MAX_BUFFER_SIZE);

	// Send up key block
	if(Send((char*)btKey, 16, 5) != 16){

		m_eDone.SetEvent();
		Close();
		return FALSE;
	}

	ZeroMemory(m_cBuffer, MAX_BUFFER_SIZE);

	// Recv Login info
	if(Recv(m_cBuffer, 4, 5) != 4){

		TRACE("Login recv error\n");
		m_eDone.SetEvent();
		Close();
		return FALSE;
	}

	m_dwUpKey = DecryptMXTCP((BYTE*)m_cBuffer, 4, m_dwUpKey);

	wType = *(WORD*)m_cBuffer;
	wLen  = *(WORD*)(m_cBuffer+2);
	
	while(wType == 0x13ED || wType == 0x13EE){ 


		if(wType == 0x13ED){

			m_wClientType  = CLIENT_WINMX353;
			m_strClientString = "WinMX";
			m_strClientVer	  = "3.5x or compatible";
		}
		else if(wType == 0x13EE){ // incoming redirect

			TRACE("Incoming redirected client\n");
		}

		if(Recv(m_cBuffer, wLen, 5) != wLen){	  // recieve rest of data

			TRACE("Login recv error\n");
			m_eDone.SetEvent();
			Close();
			return FALSE;
		}
		m_dwUpKey = DecryptMXTCP((BYTE*)m_cBuffer, wLen, m_dwUpKey);		
		
		// now recieve start of next package ;)
 		if(Recv(m_cBuffer, 4, 5) != 4){

			TRACE("Login recv error\n");
			m_eDone.SetEvent();
			Close();
			return FALSE;
		}
		
		m_dwUpKey = DecryptMXTCP((BYTE*)m_cBuffer, 4, m_dwUpKey);
		wType = *(WORD*)m_cBuffer;
		wLen  = *(WORD*)(m_cBuffer+2);
	}

	if(Recv(m_cBuffer, wLen, 5) != wLen){

		TRACE("Login recv error #2\n");
		m_eDone.SetEvent();
		Close();
		return FALSE;
	}
	
	m_dwUpKey = DecryptMXTCP((BYTE*)m_cBuffer, wLen, m_dwUpKey);

	LPSTR lpszRoom = 0;
	LPSTR lpszName = 0;
	if(Util::ScanMessage(m_cBuffer, MAX_BUFFER_SIZE, "SWDWDS", &lpszRoom, &m_wLineType, &m_dwIP, &m_wPort, &m_dwFiles, &lpszName) != 6){

		TRACE("Invalid argument count in login\n");
		m_eDone.SetEvent();
		Close();
		return FALSE;
	}


	if(lpszRoom != 0){

		strRoom = lpszRoom;
		int n = strRoom.ReverseFind('_');
		if(n > 0){

			m_strJoinSuffix = strRoom.Mid(n);
		}
	}

	if(lpszName != 0){

		m_strName = lpszName;
	}

	ZeroMemory(m_cBuffer, 512);
	*(WORD*)m_cBuffer = 0x66;
	*(WORD*)(m_cBuffer+2) = 1;

	if(SendCrypted(m_cBuffer, 5, 5) != 5){

		TRACE("Error sending login grant\n");
		m_eDone.SetEvent();
		Close();
		return FALSE;
	}

	if(m_wClientType >= CLIENT_WINMX353){


		// Send new client/server ID packet
		WORD wLenA = Util::FormatMXMessage(0x9904, (char*)&m_cBuffer, "S", "#c%d#");

		if(SendCrypted(m_cBuffer, wLenA, 5) != wLenA){

			TRACE("Error sending client/server ID\n");
			m_eDone.SetEvent();
			Close();
			return FALSE;
		}

		*(WORD*)m_cBuffer = 0x0068;
		*(WORD*)(m_cBuffer+2) = 1;

		if(SendCrypted(m_cBuffer, 5, 5) != 5){

			TRACE("Error sending login grant 2\n");
			m_eDone.SetEvent();
			Close();
			return FALSE;
		}

	}   // end if client == WINMX_353

	GetPeerAddr(&m_dwSrcIP, &m_wSrcPort);
	m_strSrcHost = CClientSocket::GetHostName(Util::FormatIP(m_dwSrcIP));

	return TRUE;
}

BOOL CClientSocket::HighlevelHandshake(CString strKey)
{

	WORD wLen = 0;

	if(!strKey.IsEmpty()){

		CString strKeyMsg = g_sSettings.GetKeyMsg();
		strKeyMsg.Replace("\\n", "\n");
		strKeyMsg.Replace("%NAME%", m_strName);
		Util::ReplaceVars(strKeyMsg);
		if(ReplaceColors(strKeyMsg)){
	
			CString strSend;
			strSend.Format("%c%c%s%c%c %c%c%s", 3,3, " ", 3, 2, 3,3, strKeyMsg);
			FixColor(strSend);
			wLen = Util::FormatMXMessage(0x000D2, (char*)&m_cBuffer, "S", (LPCSTR)strSend);
		}
		else{

			wLen = Util::FormatMXMessage(0x00078, (char*)&m_cBuffer, "S", strKeyMsg);
		}

		if(SendCrypted(m_cBuffer, wLen, 5) != wLen){

			TRACE("Error performing highlevel Handshake\n");
			Close();
			m_eDone.SetEvent();
			return FALSE;
		}
		
		if(Recv(m_cBuffer, 4, 60) != 4){

			TRACE("Error performing highlevel Handshake\n");
			Close();
			m_eDone.SetEvent();
			return FALSE;
		}

		m_dwUpKey = DecryptMXTCP((BYTE*)(m_cBuffer), 4, m_dwUpKey);

		WORD wType = *(WORD*)m_cBuffer;
		wLen = *(WORD*)(m_cBuffer+2);
		if((wType != 0x00C8) && (wType != 0x1450)){

			TRACE("Error performing highlevel Handshake\n");
			Close();
			m_eDone.SetEvent();
			return FALSE;
		}

		ZeroMemory(&m_cBuffer, MAX_BUFFER_SIZE);
		if(Recv(m_cBuffer, wLen, 60) != wLen){

			TRACE("Error performing highlevel Handshake\n");
			Close();
			m_eDone.SetEvent();
			return FALSE;
		}

		m_dwUpKey = DecryptMXTCP((BYTE*)(m_cBuffer), wLen, m_dwUpKey);

		LPSTR lpString = 0;

		if(Util::ScanMessage(m_cBuffer, MAX_BUFFER_SIZE, "S", &lpString) == 1){

			
			if(strKey.Compare(lpString) != 0){

				TRACE("Wrong keyword\n");
				Close();
				m_eDone.SetEvent();
				return FALSE;
			}
		}

	} // end key

	if(m_wClientType >= CLIENT_WINMX353){

		// Send IPCLient test
		*(WORD*)m_cBuffer = 0x9900;
		*(WORD*)(m_cBuffer+2) = 1;
		wLen = Util::FormatMXMessage(0x9900, (char*)&m_cBuffer, "B", 0x00);

		if(SendCrypted(m_cBuffer, wLen, 5) != wLen){

			TRACE("Error sending login grant 3\n");
			m_eDone.SetEvent();
			Close();
			return FALSE;
		}

		// Send new client/server ID packet
		WORD wLen = Util::FormatMXMessage(0x9905, (char*)&m_cBuffer, "SS", ROBOMX_STRING, VERSION_STRING);

		if(SendCrypted(m_cBuffer, wLen, 5) != wLen){

			TRACE("Error sending client/server ID\n");
			m_eDone.SetEvent();
			Close();
			return FALSE;
		}
	}

	m_strEnterDate = Util::GetMyDate();
	m_strEnterTime = Util::GetMyLocalTime();
	m_strEnterName = m_strName;
	return TRUE;
}

void CClientSocket::StartUp(void)
{

	m_bListen = TRUE;
	m_eDone.ResetEvent();
	m_pThread = AfxBeginThread(CClientSocket::RecvProc, (LPVOID)this, THREAD_PRIORITY_NORMAL);
	Sleep(100);
}

UINT CClientSocket::RecvProc(LPVOID pParam)
{
	
	CClientSocket* pClient = (CClientSocket*)pParam;
	ASSERT(pClient);

	char buffer[MAX_BUFFER_SIZE];
	int nRecv = 0;
	WORD wType = 0;
	WORD wLen  = 0;

	while(pClient->m_bListen){

		ZeroMemory(buffer, MAX_BUFFER_SIZE);
		nRecv = pClient->Recv((char*)&buffer, 4, 5);
		if(nRecv == SOCKET_ERROR){

			if((pClient->GetLastError() != SOCK_TIMEOUT) && pClient->m_bListen){

				TRACE("Socket Error\n");
				pClient->m_bListen  = FALSE;
				ClientError* pE = new ClientError();
				pE->guID = pClient->m_guID;
				if(pClient->GetLastError() !=  SOCK_NOERROR){

					pE->strCause	= pClient->GetLastErrorStr();
				}
				else{

					pE->strCause.LoadString(IDS_CLIENTDISC);
				}
				pClient->SendNotify(CC_ERROR, (LPARAM)pE);
				break;
			}
			else if((pClient->GetLastError() == SOCK_TIMEOUT) && pClient->m_bListen){

				Sleep(20);
				continue;
			}
		}
	
		if(nRecv != 4){
			
			Sleep(20);
			CString strError;
			strError.Format("Stream Error? %s\n", buffer);
			TRACE(strError);
			continue;
		}
		
		pClient->m_dwUpKey = DecryptMXTCP((BYTE*)(buffer), 4, pClient->m_dwUpKey);

		wType = *(WORD*)buffer;
		wLen = *(WORD*)(buffer+2);

		if(wType == 0xFDE8){ //ping...

			TRACE("Ping\n");
			pClient->SendPong(); // ...pong :-P
			if(wLen){

				nRecv = pClient->Recv(buffer, wLen, 5);
				pClient->m_dwUpKey = DecryptMXTCP((BYTE*)(buffer), wLen, pClient->m_dwUpKey);
			}
			continue;
		}

		
		if(wLen == 0) continue;
		if(!pClient->m_bListen) break;
		
		ZeroMemory(buffer, 5);
		nRecv = pClient->Recv(buffer, wLen, 0);

		if(nRecv == SOCKET_ERROR){

			if(pClient->GetLastError() != SOCK_TIMEOUT){

				CString strError;
				strError.Format("Robo-Panic [b]: %s :'(\n", pClient->GetLastErrorStr());
				TRACE(strError);
				pClient->m_bListen  = FALSE;
				ClientError* pE = new ClientError();
				pE->guID = pClient->m_guID;
				if(pClient->GetLastError() !=  SOCK_NOERROR){

					pE->strCause	= pClient->GetLastErrorStr();
				}
				else{

					pE->strCause.LoadString(IDS_CLIENTDISC);
				}
				pClient->SendNotify(CC_ERROR, (LPARAM)pE);
				break;
			}
		}
		if(nRecv != wLen) continue;

		pClient->m_dwUpKey = DecryptMXTCP((BYTE*)(buffer), wLen, pClient->m_dwUpKey);
		
		pClient->HandlePacket(wType, wLen, buffer);
		
		Sleep(20);
	}

	TRACE("Leaving Client %s\n", pClient->m_strName);
	pClient->m_eDone.SetEvent();
	return 0;
}

void CClientSocket::AddMode(UINT uMode)
{ 

	if((m_uMode & uMode) == uMode) return; // mode is already set

	m_uMode |= uMode;

	ClientRename* ur = new ClientRename();
	ur->strOldName  = m_strName;
	ur->dwOldIP	    = m_dwIP;
	ur->wOldPort    = m_wPort;
	ur->dwIP		= m_dwSrcIP;
	ur->wPort		= m_wSrcPort;
	ur->pClient		= this;
	ur->dwNewFiles  = m_dwFiles;
	ur->dwNewIP     = m_dwIP;
	ur->wNewLine    = m_wLineType;
	ur->wNewPort    = m_wPort;
	ur->uModeInternal = m_uMode;
	ur->strNewName  = m_strName;
	ur->guid		= m_guID;
	

	if((uMode & UM_AGENT) == UM_AGENT)
		ur->wUserLevel = g_sSettings.GetAgentIcon();
	else if((m_uMode & UM_ADMIN) == UM_ADMIN)
		ur->wUserLevel =  g_sSettings.GetAdminIcon();
	else if((m_uMode & UM_VOICED) == UM_VOICED)
		ur->wUserLevel  = g_sSettings.GetVoiceIcon();
	else
		ur->wUserLevel = g_sSettings.GetNormalIcon();

	if((uMode & UM_HIDDEN) == UM_HIDDEN){

		ur->nVisible  = 0; // make me invisible
	}

	if(uMode == UM_BANNED) return;

	SendNotify(CC_RENAME, (LPARAM)ur);
}

void CClientSocket::RemoveMode(UINT uMode)
{ 

	if((m_uMode & uMode) != uMode) return; // mode is not set
	
	m_uMode &= ~uMode; 

	ClientRename* ur = new ClientRename();
	ur->strOldName  = m_strName;
	ur->dwOldIP		= m_dwIP;
	ur->wPort		= m_wPort;
	ur->dwIP		= m_dwSrcIP;
	ur->wPort		= m_wSrcPort;
	ur->pClient		= this;
	ur->dwNewFiles  = m_dwFiles;
	ur->dwNewIP     = m_dwIP;
	ur->wNewLine    = m_wLineType;
	ur->wNewPort    = m_wPort;
 	ur->uModeInternal = m_uMode;
	ur->strNewName  = m_strName;
	ur->guid		= m_guID;

	if((uMode & UM_AGENT) == UM_AGENT)
		ur->wUserLevel = g_sSettings.GetAgentIcon();
	else if((m_uMode & UM_ADMIN) == UM_ADMIN)
		ur->wUserLevel = g_sSettings.GetAdminIcon();
	else if((m_uMode & UM_VOICED) == UM_VOICED)
		ur->wUserLevel = g_sSettings.GetVoiceIcon();
	else
		ur->wUserLevel = g_sSettings.GetNormalIcon();

	if((uMode & UM_HIDDEN) == UM_HIDDEN){

		ur->nVisible = 1; // make me visible again
	}

	if(uMode == UM_BANNED) return;

	SendNotify(CC_RENAME, (LPARAM)ur);
}

void CClientSocket::HandlePacket(WORD wType, WORD wLen, char* buffer)
{

	if(m_bBlocked) return;

	if(wType == 0x1450){

		// this is the new WinMX 3.5x send method, we have to handle the /me part too -sigh-
		int nMsgID = CC_MSG;
		CString strText = buffer;
		CString strName = m_strName;

		strText.Replace("{\\rtf", "#####");

		if((strText.Find("/impose ", 0) == 0) && ((m_uMode & UM_AGENT) == UM_AGENT)){

			strText = strText.Mid(8);
			int n = strText.Find(" ");
			if(n >= 0){

				strName = strText.Left(n);
				strText = strText.Mid(n+1);
			}
		}
		if(strText.Find("/me ", 0) == 0){

			strText = strText.Mid(4);
			nMsgID = CC_ACTION;
		}
		else if(strText.Find("/action ", 0) == 0){

			strText = strText.Mid(8);
			nMsgID = CC_ACTION;
		}
		else if(strText.Find("/emote ", 0) == 0){

			strText = strText.Mid(7);
			nMsgID = CC_ACTION;
		}
		else if(strText.Find("/opmsg ", 0) == 0){

			strText = strText.Mid(7);
			nMsgID = CC_OPMSG;
		}
		else if(strText.Find("/disablecolors", 0) == 0){

			m_uMode |= UM_NOCOLORS;	// disable colors for this client
			SendOperator("", strText, TRUE, 0);
			return;
		}
		else if(strText.Find("/enablecolors", 0) == 0){

			m_uMode &= ~UM_NOCOLORS; // enable colors for this client
			SendOperator("", strText, TRUE, 0);
			return;
		}
		else if(strText.Find("/commands", 0) == 0){

			SendCommandHelp();
			return;
		}

		if(strText.IsEmpty()) return; // do not handle that

		if(IsFlooding()){

			m_bBlocked = TRUE;
			Close();
			return;
		}

		if(nMsgID == CC_ACTION){

			ClientAction* cm = new ClientAction();
			cm->guid	= m_guID; 
			cm->dwIP	= m_dwSrcIP;
			cm->wPort   = m_wPort;
			cm->uMode	= m_uMode;
			cm->strName = strName;
			cm->strText = strText;
			SendNotify(CC_ACTION, (LPARAM)cm);
		}
		else if(nMsgID == CC_OPMSG){

			ClientOpMsg* cm = new ClientOpMsg();
			cm->guid	= m_guID; 
			cm->dwIP	= m_dwSrcIP;
			cm->wPort   = m_wPort;
			cm->uMode	= m_uMode;
			cm->strName = strName;
			cm->strText = strText;
			SendNotify(CC_OPMSG, (LPARAM)cm);
		}
		else{

			if((strText.Find("/setmotd", 0) < 0 ) && (strText.Find("\\n", 0) >= 0) && ((m_uMode&UM_AGENT)==UM_AGENT)){
			
				strText.Replace("#\\r\\n#", "\n");
				strText.Replace("\\n", "\n");
				CTokenizer token(strText, "\n");
				CString strTmp;
				while(token.Next(strTmp)){

					ClientAction* cm = new ClientAction();
					cm->guid	= m_guID; 
					cm->dwIP	= m_dwSrcIP;
					cm->wPort   = m_wPort;
					cm->uMode	= m_uMode;
					cm->strName = "";
					cm->strText = strTmp;
					SendNotify(CC_ACTION, (LPARAM)cm);
				}
			}
			else{

				ClientMessage* cm = new ClientMessage();
				cm->guid	= m_guID; 
				cm->dwIP	= m_dwSrcIP;
				cm->wPort   = m_wPort;
				cm->uMode	= m_uMode;
				cm->strName = strName;
				cm->strText = strText;
				SendNotify(CC_MSG, (LPARAM)cm);
			}
		}
		m_nMessages++;

	}
	else if(wType == 0x00C8){ // message

		
		CString strMsg = buffer;
		if(strMsg.IsEmpty()) return; // do not handle that
		
		if(IsFlooding()){

			ClientOpMsg* cm = new ClientOpMsg();
			cm->guid	= m_guID; 
			cm->dwIP	= m_dwSrcIP;
			cm->wPort   = m_wPort;
			cm->uMode	= m_uMode;
			cm->strName = m_strName;
			cm->strText = "Messageflood blocked.";
			SendNotify(CC_OPMSG, (LPARAM)cm);
			m_bBlocked = TRUE;
			Close();
			return;
		}
		
		if(strMsg.GetLength() > 4){
		
			if(strMsg[0] == '#' && strMsg[1] != 'c' && strMsg[1] != '\\' && strMsg[3] != '#'){

				strMsg.SetAt(0, '/');
			}
		}
		CString strName = m_strName;

		if((strMsg.Find("/impose ", 0) == 0) && ((m_uMode & UM_AGENT) == UM_AGENT)){

			strMsg = strMsg.Mid(8);
			int n = strMsg.Find(" ");
			if(n >= 0){

				strName = strMsg.Left(n);
				strMsg = strMsg.Mid(n+1);
			}
		}
		else if(strMsg.Find("/opmsg ", 0) == 0){

			strMsg = strMsg.Mid(7);
			ClientOpMsg* cm = new ClientOpMsg();
			cm->guid	= m_guID; 
			cm->dwIP	= m_dwSrcIP;
			cm->wPort   = m_wPort;
			cm->uMode	= m_uMode;
			cm->strName = strName;
			cm->strText = strMsg;
			cm->strText.Replace("{\\rtf", "#####");
			SendNotify(CC_OPMSG, (LPARAM)cm);
			return;
		}
		else if(strMsg.Find("/commands", 0) == 0){

			SendCommandHelp();
			return;
		}

		if((strMsg.Find("/setmotd", 0) < 0 ) && (strMsg.Find("\\n", 0) >= 0) && ((m_uMode&UM_AGENT)==UM_AGENT)){
		
			strMsg.Replace("#\\r\\n#", "\n");
			strMsg.Replace("\\n", "\n");
			CTokenizer token(strMsg, "\n");
			CString strTmp;
			while(token.Next(strTmp)){

				ClientAction* cm = new ClientAction();
				cm->guid	= m_guID; 
				cm->dwIP	= m_dwSrcIP;
				cm->wPort   = m_wPort;
				cm->uMode	= m_uMode;
				cm->strName = "";
				cm->strText = strTmp;
				SendNotify(CC_ACTION, (LPARAM)cm);
			}
		}
		else{
			ClientMessage* cm = new ClientMessage();
			cm->guid	= m_guID; 
			cm->dwIP = m_dwSrcIP;
			cm->wPort = m_wPort;
			cm->uMode	= m_uMode;
			cm->strName = strName;
			cm->strText = strMsg;
			cm->strText.Replace("{\\rtf", "#####");
			SendNotify(CC_MSG, (LPARAM)cm);
		}
		m_nMessages++;
	}
	else if(wType == 0x00CA){ // action

		CString strMsg = buffer;
		if(strMsg.IsEmpty()) return; // do not handle that

		if(IsFlooding()){

			ClientOpMsg* cm = new ClientOpMsg();
			cm->guid	= m_guID; 
			cm->dwIP	= m_dwSrcIP;
			cm->wPort   = m_wPort;
			cm->uMode	= m_uMode;
			cm->strName = m_strName;
			cm->strText = "Messageflood blocked.";
			SendNotify(CC_OPMSG, (LPARAM)cm);
			m_bBlocked = TRUE;
			Close();
			return;
		}
		ClientAction* cm = new ClientAction();
		cm->guid	= m_guID; 
		cm->dwIP    = m_dwSrcIP;
		cm->wPort   = m_wPort;
		cm->uMode	= m_uMode;
		cm->strName = m_strName;
		cm->strText = strMsg;
		cm->strText.Replace("{\\rtf", "#####");
		SendNotify(CC_ACTION, (LPARAM)cm);
		m_nMessages++;
	}
	else if(wType == 0x0065){ // rename


		ClientRename* ur = new ClientRename();
		
		LPSTR lpName = 0;
		if(Util::ScanMessage(buffer, MAX_BUFFER_SIZE, "WDWDS", &ur->wNewLine, &ur->dwNewIP, &ur->wNewPort, &ur->dwNewFiles, &lpName) == 5){

			if((m_uMode & UM_AGENT) == UM_AGENT)
				ur->wUserLevel = g_sSettings.GetAgentIcon();
			else if((m_uMode & UM_ADMIN) == UM_ADMIN)
				ur->wUserLevel = g_sSettings.GetAdminIcon();
			else if((m_uMode & UM_VOICED) == UM_VOICED)
				ur->wUserLevel = g_sSettings.GetVoiceIcon();
			else
				ur->wUserLevel = g_sSettings.GetNormalIcon();
			
			ur->strNewName  = lpName;
			ur->uModeInternal = m_uMode;

			ur->guid		= m_guID; 
			ur->strOldName  = m_strName;
			ur->dwOldIP		= m_dwIP;
			ur->wOldPort    = m_wPort;
			ur->dwIP		= m_dwSrcIP;
			ur->wPort		= m_wSrcPort;
			ur->uMode       = m_uMode;
			ur->pClient		= this;
			m_dwIP			= ur->dwNewIP;
			m_wPort			= ur->wNewPort;
			m_strName       = ur->strNewName;

			if(Util::GetBaseName(ur->strOldName, TRUE) != Util::GetBaseName(ur->strNewName, TRUE)){

				if(GetTickCount() - m_dwLastRen < 3000){
				
					m_wRenCounter++;
				}
				else{

					m_wRenCounter = 0;
				}
				m_dwLastRen = GetTickCount();
				if(m_wRenCounter > 5){

					m_bBlocked = TRUE;
					Close();
					delete ur;
					return;
				}
			}

			SendNotify(CC_RENAME, (LPARAM)ur);
			Sleep(1000);
		}
	}
	else if(wType == 0x9901){ // IPClient is supported

        TRACE("IPClient\n");
		m_wClientType	  = CLIENT_IPCLIENT;
		if(wLen == 0){

			m_strClientString = "RoboMX";
			m_strClientVer    = "1.01";
		}
		else if((wLen == 1) && (*(BYTE*)buffer) == 0x11){

			m_strClientString = "RoboMX";
			m_strClientVer    = "1.1x";
		}
		else{

			m_strClientString = "IPClient comp. (Rabbit/RoboMX)";
			m_strClientVer    = "1.x";
		}
	}
	else if(wType == 0x9905){	// Client string

		LPSTR lpClient = 0, lpVer = 0;
		if(Util::ScanMessage(buffer, wLen, "SS", &lpClient, &lpVer) == 2){

			m_strClientString = lpClient;
			m_strClientVer    = lpVer;
			if((m_strClientString.Find("Metis", 0)>=0) && ((m_uMode & UM_NOCOLORS) != UM_NOCOLORS)){

				// automatically disable colors for Metis so the bot wont have any problems
				m_uMode |= UM_NOCOLORS;			
			}
		}
	}
	else if(wType == 0xAA || wType == 0xBB || wType == 0xCC){

		// Poisonblock
		m_bListen = FALSE;
		Close();
	}
}

BOOL CClientSocket::SendTopic(CString strTopic)
{

	char buffer[MAX_SEND_SIZE];

	WORD wLen = Util::FormatMXMessage(0x012C, (char*)buffer, "S", (LPCSTR)strTopic);

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendMotd(CString strMotd)
{

	if(strMotd.IsEmpty()) return TRUE;

	strMotd.Replace("\\n", "\n");
	char buffer[MAX_SEND_SIZE];
	WORD wLen = 0;
	if(ReplaceColors(strMotd)){

		CTokenizer token(strMotd, "\n");
		CString strSend, strColor;
		while(token.Next(strSend)){
			
			strColor.Format("%c%c%s%c%c %c%c%s", 3,3, " ", 3, 2, 3,3, strSend);
			FixColor(strColor);
			wLen = Util::FormatMXMessage(0x000D2, (char*)buffer, "S", (LPCSTR)strColor);

			if(SendCrypted(buffer, wLen, 5) != wLen) return FALSE;
		}
		return TRUE;
	}
	else if(m_wClientType == CLIENT_WINMX331){


		CTokenizer token(strMotd, "\n");
		CString strSend;
		while(token.Next(strSend)){

			wLen = Util::FormatMXMessage(0x00078, (char*)buffer, "S", (LPCSTR)strMotd);
			
			if(SendCrypted(buffer, wLen, 5) != wLen) return FALSE;
		}
		return TRUE;
	}
	else{ // no colors and winmx 3.5x and compatible

		wLen = Util::FormatMXMessage(0x00078, (char*)buffer, "S", (LPCSTR)strMotd);
		return SendCrypted(buffer, wLen, 5) == wLen;
	}
	return FALSE;
}


BOOL CClientSocket::SendMsg(CString strUser, CString strMsg)
{

	char buffer[MAX_SEND_SIZE];
	
	WORD wLen = 0;
	
	BOOL bColor = FALSE;
	if(ReplaceColors(strUser)) bColor = TRUE;
	if(ReplaceColors(strMsg)) bColor = TRUE;

	if(bColor){
		
		CString strSend;
		strSend.Format("%c%c<%s%c%c> %c%c%s", 3,2, strUser, 3, 2, 3,1, strMsg);
		FixColor(strSend);
		wLen = Util::FormatMXMessage(0x00D2, (char*)buffer, "S", (LPCSTR)strSend);
	}
	else{

		wLen = Util::FormatMXMessage(0x00C9, (char*)buffer, "SS", (LPCSTR)strUser, (LPCSTR)strMsg);
	}

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendOperator(CString strUser, CString strMsg, BOOL bEcho, UINT uColor)
{

	if(m_wClientType >= CLIENT_WINMX353){

		char buffer[MAX_SEND_SIZE];

		CString strSend;
		if(bEcho && uColor){

			if(strUser.IsEmpty()) strUser = " ";
			strSend.Format("%s%c%c%s", strUser, 3, uColor, strMsg);
		}
		else if(bEcho && !uColor){

			strSend.Format("%s", strUser.IsEmpty() ? strMsg : strUser);
		}
		else{

			strUser.Insert(0, "<");
			strUser+= "> ";
			strSend.Format("%s%c%c %s", strUser, 3, '', strMsg);
		}

		//FixColor(strSend);

		WORD wLen = 0;
		if(bEcho){
			
			wLen = Util::FormatMXMessage(0x00D3, (char*)buffer, "S", (LPCSTR)strSend);
		}
		else{
			
			wLen = Util::FormatMXMessage(0x00D2, (char*)buffer, "S", (LPCSTR)strSend);
		}
		
		return SendCrypted(buffer, wLen, 5) == wLen;
	}
	else{

		if(!strUser.IsEmpty()){
			strUser.Insert(0, "<");
			strUser+= "> ";
		}
		return SendAction(strUser, strMsg);
	}
}


BOOL CClientSocket::SendAction(CString strUser, CString strMsg)
{

	char buffer[MAX_SEND_SIZE];
	WORD wLen = 0;

	BOOL bColor = FALSE;
	if(ReplaceColors(strUser)) bColor = TRUE;
	if(ReplaceColors(strMsg)) bColor = TRUE;

	if(bColor){

		CString strSend;
		strSend.Format("%c%c%s%c%c %c%c%s", 3,3, strUser, 3, 2, 3,3, strMsg);
		FixColor(strSend);
		wLen = Util::FormatMXMessage(0x00D2, (char*)buffer, "S", (LPCSTR)strSend);
	}
	else{

		wLen = Util::FormatMXMessage(0x00CB, (char*)buffer, "SS", (LPCSTR)strUser, (LPCSTR)strMsg);
	}

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendUserlist(CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel)
{

	if((wUserLevel == 2) && ((g_sSettings.GetVoiceWatch() & m_uMode) == 0)){

		wUserLevel = 0;
	}
	if((wUserLevel == 1) && ((g_sSettings.GetAdminWatch() & m_uMode) == 0)){

		wUserLevel = 0;
	}

	char buffer[MAX_SEND_SIZE];

	WORD wLen = 0;

	if(m_wClientType >= CLIENT_WINMX353){

		wLen = Util::FormatMXMessage(0x0072, (char*)buffer, "SDWWDW", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles, wUserLevel);
	}
	else{

		wLen = Util::FormatMXMessage(0x006F, (char*)buffer, "SDWWD", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles);
	}

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendIPClient(CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel, DWORD dwSrcIP, CString strHost)
{

	char buffer[MAX_SEND_SIZE];

	WORD wLen = 0;

	if(m_wClientType == CLIENT_IPCLIENT){

		wLen = Util::FormatMXMessage(0x9902, (char*)buffer, "SDWWWBSS", (LPCSTR)strUser, dwIP, wPort, wLine, (WORD)dwFiles, (BYTE)wUserLevel, (LPCTSTR)Util::FormatIP(dwSrcIP), strHost);
	}

	return SendCrypted(buffer, wLen, 5) == wLen;
}


BOOL CClientSocket::SendJoin(CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel, DWORD dwRealIP)
{

	char buffer[MAX_SEND_SIZE];
	WORD wLen = 0;

	if((wUserLevel == 2) && ((g_sSettings.GetVoiceWatch() & m_uMode) == 0)){

		wUserLevel = 0;
	}
	if((wUserLevel == 1) && ((g_sSettings.GetAdminWatch() & m_uMode) == 0)){

		wUserLevel = 0;
	}

	if((m_wClientType >= CLIENT_WINMX353) && (m_uMode & UM_ADMIN)){

		// user is admin so send IP too
		wLen = Util::FormatMXMessage(0x0075, (char*)buffer, "SDWWDBD", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles, wUserLevel, dwRealIP);
	}
	else if(m_wClientType >= CLIENT_WINMX353){

		// this user is no admin so dont send IP :P
		wLen = Util::FormatMXMessage(0x0071, (char*)buffer, "SDWWDB", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles, wUserLevel);
	}
	else{

		wLen = Util::FormatMXMessage(0x006E, (char*)buffer, "SDWWD", (LPCSTR)strUser, dwIP, wPort, wLine, dwFiles);
	}

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendPong()
{

	char buffer[10];
	WORD wLen = Util::FormatMXMessage(0xFDE8, (char*)buffer, "B", 0);
	
	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendPart(CString strUser, DWORD dwIP, WORD wPort)
{

	char buffer[MAX_SEND_SIZE];
	WORD wLen = Util::FormatMXMessage(0x0073, (char*)buffer, "SDW", (LPCSTR)strUser, dwIP, wPort);

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendChannelRename(CString strRoom)
{

	char buffer[MAX_SEND_SIZE];

	WORD wLen = 0;

	if(m_wClientType >= CLIENT_WINMX353){

		wLen = Util::FormatMXMessage(0x012D, (char*)buffer, "S", (LPCSTR)strRoom);		
	}
	else{

		wLen = Util::FormatMXMessage(0x00CB, (char*)buffer, "SS", (LPCSTR)"", (LPCSTR)strRoom);
	}

    return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendRedirect(CString strTarget)
{

	char buffer[MAX_SEND_SIZE];

	WORD wLen = Util::FormatMXMessage(0x0190, (char*)buffer, "S", (LPCSTR)strTarget);

	return SendCrypted(buffer, wLen, 5) == wLen;
}

BOOL CClientSocket::SendRename(CString strOldName, DWORD dwOldIP, WORD wOldPort, CString strNewName, DWORD dwNewIP, WORD wNewPort, WORD wLine, DWORD dwFiles, WORD wUserLevel)
{

	char buffer[MAX_SEND_SIZE];
	WORD wLen = 0;
	
	if((wUserLevel == 2) && ((g_sSettings.GetVoiceWatch() & m_uMode) == 0)){

		wUserLevel = 0;
	}
	if((wUserLevel == 1) && ((g_sSettings.GetAdminWatch() & m_uMode) == 0)){

		wUserLevel = 0;
	}

	if(m_wClientType >= CLIENT_WINMX353){

		wLen = Util::FormatMXMessage(0x0074, (char*)buffer, "SDWSDWWDW", 
										(LPCTSTR)strOldName,
										dwOldIP,
										wOldPort,
										(LPCTSTR)strNewName,
										dwNewIP,
										wNewPort,
										wLine,
										dwFiles,
										wUserLevel
									);
	}
	else{

		wLen = Util::FormatMXMessage(0x0070, (char*)buffer, "SDWSDWWD", 
										(LPCTSTR)strOldName,
										dwOldIP,
										wOldPort,
										(LPCTSTR)strNewName,
										dwNewIP,
										wNewPort,
										wLine,
										dwFiles							
									);
	}
	return SendCrypted(buffer, wLen, 5) == wLen;

}

void CClientSocket::LogOut(void)
{

	m_bListen = FALSE;
	Close();
	TRACE("CClientSocket: Waiting for Listen thread to exit\n");
	DWORD n = WaitForSingleObject(m_eDone, 5000);
	if(n == WAIT_TIMEOUT || n == WAIT_FAILED){

		TerminateThread(m_pThread->m_hThread, 0);
	}
	m_pThread = NULL;
	TRACE("LogOut Complete :-)\n");
}

BOOL CClientSocket::SendNotify(WPARAM wParam, LPARAM lParam)
{

	if(m_hMsgTarget == NULL) return FALSE;
	if(!::IsWindow(m_hMsgTarget)) return FALSE;
	if(m_bBlocked) return FALSE;

	::SendMessage(m_hMsgTarget, UWM_CLNNOTIFY, wParam, lParam);
	return TRUE;
}

int CClientSocket::SendCrypted(char *pBuff, int nLen, int nWait)
{

	m_dwDWKey = EncryptMXTCP((BYTE*)pBuff, nLen, m_dwDWKey);

	int nSend = Send(pBuff, nLen, nWait);

	if(nSend != nLen){

		ClientError* ce = new ClientError();
		ce->dwIP		= m_dwSrcIP;
		ce->wPort		= m_wSrcPort;
		ce->guID		= m_guID;
		if(GetLastError() !=  SOCK_NOERROR){

			ce->strCause	= GetLastErrorStr();
		}
		else{

			ce->strCause.LoadString(IDS_CLIENTDISC);
		}
		SendNotify(CC_ERROR, (LPARAM)ce);
	}

	return nSend;
}


BOOL CClientSocket::ReplaceColors(CString& rString)
{

	if(!g_sSettings.GetColor()) return FALSE;

	BOOL bReturn = FALSE;
	
	if((m_wClientType >= CLIENT_WINMX353) && ((m_uMode & UM_NOCOLORS) != UM_NOCOLORS)){

		if(rString.Find("#") == -1) return FALSE;

		CString strColor, strRep;

		// handle random color statement
		int nStart = -1;
		while((nStart = rString.Find("#cx#")) >= 0){

			strColor.Format("%d", (1+rand())%40);
			rString.SetAt(nStart+2, strColor[0]);
			if(strColor.GetLength() == 2){

				rString.Insert(nStart+3, strColor[1]);
			}
		}
		while((nStart = rString.Find("#c?#")) >= 0){

			strColor.Format("%d", (1+rand())%40);
			rString.SetAt(nStart+2, strColor[0]);
			if(strColor.GetLength() == 2){

				rString.Insert(nStart+3, strColor[1]);
			}
		}
		
		for(int c = 0; c < num_color; c++){

			strRep.Format("#c%d#", c+1);
			strColor.Format("%c%c", 3, color_map[c]);
			if(rString.Replace(strRep, strColor) > 0) bReturn = TRUE;
		}
	}
	else{ // winmx 3.3x or colors disabled


		if(rString.Find("#") == -1) return FALSE;
		
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

	return bReturn;
}

void CClientSocket::FixColor(CString &strText)
{

	for (int c = 0; c < num_color; c++){

		char cFind[] = {3, color_map[c], 3, 0};
		char CRepl[] = {3, 0};
		
		strText.Replace(cFind, CRepl);
	}
}


CString CClientSocket::GetUserStats(void)
{

	CString strStats;
	strStats.Format(IDS_USERSTATS, 
		m_strName, m_strClientString, m_strClientVer, m_strSrcHost, m_strEnterDate, m_strEnterTime, m_strEnterName, m_nMessages);
	return strStats;
}

BOOL CClientSocket::IsFlooding(void)
{

	if((m_uMode & UM_ADMIN) == UM_ADMIN){

		return FALSE; // Operators never flood
	}
	if((GetTickCount() - m_dwLast) < 1000){

		m_wFloodCounter++;
	}
	else{

		m_wFloodCounter = 0;
	}
	if(m_wFloodCounter > 4){

		return TRUE;
	}

	m_dwLast = GetTickCount();

	return FALSE;
}

void CClientSocket::SendCommandHelp()
{

	SendOperator("", Util::LoadString(IDS_CMD_START), TRUE, 0x04);
	SendOperator("", Util::LoadString(IDS_CMD_LOGIN), TRUE, 0x01);
	SendOperator("", Util::LoadString(IDS_CMD_PRIVATE), TRUE, 0x01);
	SendOperator("", Util::LoadString(IDS_CMD_CHANNELNAME), TRUE, 0x01);
	//if(g_sSettings.GetEnableSubs()){
	if((g_sSettings.GetChannelMode() & CM_SUBCHANNELS) == CM_SUBCHANNELS){

		SendOperator("", Util::LoadString(IDS_CMD_LISTCHANNELS), TRUE, 0x01);
		if((m_uMode & g_sSettings.GetSubPermission()) != 0){

			SendOperator("", Util::LoadString(IDS_CMD_CREATECHANNEL), TRUE, 0x01);
		}
		SendOperator("", Util::LoadString(IDS_CMD_JOIN), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_SUBPART), TRUE, 0x01);
	}
	SendOperator("", Util::LoadString(IDS_CMD_NOCLR), TRUE, 0x01);
	SendOperator("", Util::LoadString(IDS_CMD_ENABLECLR), TRUE, 0x01);
	if((m_uMode & UM_ADMIN) == UM_ADMIN){

		SendOperator("", Util::LoadString(IDS_CMD_OP), TRUE, 0x04);
		SendOperator("", Util::LoadString(IDS_CMD_KICK), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_KICKBAN), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_BAN), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_UNBAN), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_LISTBANS), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_EXILE), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_TOPIC), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_MOTD), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_LIMIT), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_MODES), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_STATA), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_STATB), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_IDS_CMD_ADMIN), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_REMADMIN), TRUE, 0x01);

	}
	if(m_uMode & UM_AGENT == UM_AGENT){

		SendOperator("", Util::LoadString(IDS_CMD_AGENT), TRUE, 0x04);
		SendOperator("", Util::LoadString(IDS_CMD_IMPOSE), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_ADDAGENT), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_REMAGENT), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_ENABLEEXILE), TRUE, 0x01);
		SendOperator("", Util::LoadString(IDS_CMD_DISABLEEXILE), TRUE, 0x01);

	}
	SendOperator("", Util::LoadString(IDS_CMD_END), TRUE, 0x05);

}
