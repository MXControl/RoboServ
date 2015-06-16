/*
** As there has been some irritation on the origin of this class:
** The original implementation of this class has been taken from
** 'NetzBingo' a programing tuturial of the University of Hannover.
** The code has been made available to the public domain without
** any licensing restrictions or copyright restrictions.
**
** Adaption, Modification, Extension & Redesign for RoboMX
** by (c) Thees Schwab
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
#include "MySocket.h"
#include ".\mysocket.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

// default constructor for TCP server/client socket
CMySocket::CMySocket()
{

	m_sSocket	  = INVALID_SOCKET;
	m_dwLastError = SOCK_NOERROR;
}

// Constructor for incoming TCP clients
CMySocket::CMySocket(SOCKET sSocket) :
  m_sSocket(sSocket),
  m_dwLastError(SOCK_NOERROR)
{

}

// Constructor for UDP datagram socket
CMySocket::CMySocket(int nProtocol, u_short uPort)
{

	m_sSocket	  = INVALID_SOCKET;
	m_dwLastError = SOCK_NOERROR;

	sockaddr_in local;
	
    // Binding local address with receiving socket
	local.sin_family=AF_INET;
    local.sin_port=htons(uPort);
	local.sin_addr.s_addr=htonl(INADDR_ANY);
	
	m_sSocket = socket(AF_INET, SOCK_DGRAM, 0);

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = WSAGetLastError();
		Close();
	}
	if(bind(m_sSocket, (SOCKADDR*)&local, sizeof(local)) != 0){

		m_dwLastError = WSAGetLastError();
		Close();
	}

	m_dwLastError = SOCK_NOERROR;
}

 CMySocket::~CMySocket()
{

	Close();
}


void CMySocket::SetSocket(SOCKET sSocket)
{

	if(m_sSocket != INVALID_SOCKET){

		Close();
	}

	m_sSocket = sSocket;
}

int CMySocket::Listen(u_short uPort)
{
	sockaddr_in local;

	local.sin_family	  = AF_INET;
	local.sin_addr.s_addr = INADDR_ANY;
	local.sin_port		  = htons(uPort);

	m_sSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = WSAGetLastError();
		Close();
		return 0;
	}

	if(bind(m_sSocket, (sockaddr*)&local, sizeof(local)) != 0){

		m_dwLastError = WSAGetLastError();
		Close();
		return 0;
	}

	if(listen(m_sSocket, SOMAXCONN) != 0){

		m_dwLastError = WSAGetLastError();
		Close();
	}
	return 1;
}

BOOL CMySocket::Connect(LPCTSTR lpszIP, WORD wPort, int nWait, int nProtocol)
{

	DWORD dwIP = inet_addr(lpszIP);

	if(dwIP == INADDR_NONE){

		struct hostent *pHost = gethostbyname(lpszIP);
		if(!pHost){		

			m_dwLastError = WSAGetLastError();
			return FALSE;
		}
		else{
			
			return Connect(*((DWORD *)pHost->h_addr_list[0]), wPort, nWait, nProtocol);
		}
	}
	else{
		
		return Connect(dwIP, wPort);
	}
}

BOOL CMySocket::Connect(DWORD dwIP, WORD wPort, int nWait, int nProtocol)
{

	// already connected?
	if(m_sSocket != INVALID_SOCKET){

		m_dwLastError = WSAGetLastError();
		return FALSE;
	}

	m_sSocket = socket(PF_INET, SOCK_STREAM, nProtocol);
	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = WSAGetLastError();
		return FALSE;		
	}

	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(wPort);
	SockAddr.sin_addr.S_un.S_addr = dwIP;

	if(!nWait){

		if(connect(m_sSocket, (PSOCKADDR)&SockAddr, sizeof(sockaddr)) == SOCKET_ERROR){

			m_dwLastError = WSAGetLastError();
			Close();
			return FALSE;		
		}
	
		m_dwLastError = SOCK_NOERROR;
		return TRUE;
	}

	unsigned long nArg = 1;
	fd_set FdSet;
	struct timeval TimeVal;
	TimeVal.tv_sec = nWait;
	TimeVal.tv_usec = 0;
	FD_ZERO(&FdSet);
	FD_SET(m_sSocket, &FdSet);

	if(ioctlsocket(m_sSocket, FIONBIO, &nArg) == SOCKET_ERROR){

		m_dwLastError = WSAGetLastError();
		Close();
		return FALSE;		
	}

	if(connect(m_sSocket, (PSOCKADDR)&SockAddr, sizeof(sockaddr)) != SOCKET_ERROR){

		m_dwLastError = WSAGetLastError();
		Close();
		return FALSE;		
	}
	if(WSAGetLastError() != WSAEWOULDBLOCK){

		m_dwLastError = WSAGetLastError();
		Close();
		return FALSE;		
	}

	if(select(32, NULL, &FdSet, NULL, &TimeVal) == SOCKET_ERROR){

		m_dwLastError = WSAGetLastError();
		Close();
		return FALSE;		
	}

	if(!FD_ISSET(m_sSocket, &FdSet)){

		m_dwLastError = SOCK_TIMEOUT;
		Close();
		return FALSE;
	}

	nArg = 0;
	if(ioctlsocket(m_sSocket, FIONBIO, &nArg) == SOCKET_ERROR){

		m_dwLastError = WSAGetLastError();
		Close();
		return FALSE;		
	}

	m_dwLastError = SOCK_NOERROR;
	return TRUE;
}

SOCKET CMySocket::Accept(sockaddr_in* from)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = WSAGetLastError();
		return INVALID_SOCKET;
	}

	int fromlen = (from != NULL ? sizeof(from) : 0);

	SOCKET nSocket = accept(m_sSocket, 0, 0);
	if(nSocket == INVALID_SOCKET){

		Close();
		m_dwLastError = WSAGetLastError();
		return INVALID_SOCKET;
	}
	
	return nSocket;
}

int CMySocket::Recv(char *pBuff, int nLen, int nWait)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = WSAGetLastError();
		return SOCKET_ERROR;
	}

	int nRef = 0, nRecv = 0;

	do{

		nRef = RecvOnce(pBuff + nRecv, nLen - nRecv, nWait);
		if(nRef == SOCKET_ERROR) return SOCKET_ERROR;
		nRecv += nRef;
	}
	while(nRecv < nLen);

	return nRecv;
}

int CMySocket::RecvOnce(char *pBuff, int nMaxLen, int nWait)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = WSAGetLastError();
		return SOCKET_ERROR;
	}
	
	// Set Socket-Timeout
	int nValue  = 0, nSize = sizeof(int);

	if(getsockopt(m_sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nValue, &nSize)){

		m_dwLastError = WSAGetLastError();
		Close();
		return SOCKET_ERROR;
	}

	if(nValue != (nWait * 1000)){

		nValue = nWait * 1000;
		if(setsockopt(m_sSocket, SOL_SOCKET, SO_RCVTIMEO, (char *)&nValue, sizeof(int))){

			m_dwLastError = WSAGetLastError();
			Close();
			return SOCKET_ERROR;
		}
	}
	
	// Receive data :-)
	int nRef = recv(m_sSocket, pBuff, nMaxLen, 0);
	if(nRef == SOCKET_ERROR || !nRef){

		DWORD dwLastError = (WSAGetLastError() == WSAETIMEDOUT) ? SOCK_TIMEOUT : WSAGetLastError();
		if(dwLastError != SOCK_TIMEOUT){

			Close();
		}
		m_dwLastError = dwLastError;
		return SOCKET_ERROR;
	}
		
	return nRef;
}


int CMySocket::Send(char *pBuff, int nLen, int nWait)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = WSAGetLastError();
		return SOCKET_ERROR;
	}

	int nRef = 0, nSend = 0;

	do{

		nRef = SendOnce(pBuff + nSend, nLen - nSend, nWait);
		if(nRef == SOCKET_ERROR){

			return SOCKET_ERROR;
		}
		nSend += nRef;
	}
	while(nSend < nLen);

	return nSend;
}

int CMySocket::SendOnce(char *pBuff, int nMaxLen, int nWait)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = WSAGetLastError();
		return SOCKET_ERROR;
	}
	
	// Set timeout
	int nValue  = 0, nSize = sizeof(int);
	if(getsockopt(m_sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&nValue, &nSize)){

		m_dwLastError = WSAGetLastError();
		Close();
		return SOCKET_ERROR;
	}
	if(nValue != (nWait * 1000)){

		nValue = nWait * 1000;
		if(setsockopt(m_sSocket, SOL_SOCKET, SO_SNDTIMEO, (char *)&nValue, sizeof(int))){

			m_dwLastError = WSAGetLastError();
			Close();
			return SOCKET_ERROR;
		}
	}
	
	// send...
	int nRef = send(m_sSocket, pBuff, nMaxLen, 0);
	if(nRef == SOCKET_ERROR || !nRef){

		DWORD dwLastError = (WSAGetLastError() == WSAETIMEDOUT) ? SOCK_TIMEOUT : WSAGetLastError();
		if(dwLastError != SOCK_TIMEOUT){

			Close();
		}
		m_dwLastError = dwLastError;
		return SOCKET_ERROR;
	}
		
	return nRef;
}


BOOL CMySocket::Close(void)
{

	if(m_sSocket != INVALID_SOCKET){

		shutdown(m_sSocket, SD_BOTH);

		closesocket(m_sSocket);
		m_sSocket = INVALID_SOCKET;
	}
	else{
		
		m_dwLastError = WSAGetLastError();
	}

	return (m_dwLastError == SOCK_NOERROR) ? TRUE : FALSE;
}

DWORD CMySocket::GetLastError(void)
{

	return m_dwLastError;
}

CString CMySocket::GetLastErrorStr(void)
{

	CString strError;
	switch(GetLastError()){

	case SOCK_NOERROR:
		strError.LoadString(IDS_ERROR_KICKED);
		break;
	case SOCK_TIMEOUT:
		strError.LoadString(IDS_ERROR_TIMEOUT);
		break;
	case SOCK_SOCKINVALID:
		strError.LoadString(IDS_ERROR_INVALID_SOCKET);
		break;
	default:
		strError = GetErrorString(GetLastError());
	}
	return strError;
}


BOOL CMySocket::GetLocalAddr(DWORD *pIP, WORD *pPort)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = SOCK_SOCKINVALID;
		return FALSE;
	}

	struct sockaddr_in LocalAddr;
	int AddrLen = sizeof(LocalAddr);
	
	if(getsockname(m_sSocket, (struct sockaddr *)&LocalAddr, &AddrLen) == SOCKET_ERROR){

		m_dwLastError = WSAGetLastError();
		Close();
		return FALSE;
	}

	*pIP = LocalAddr.sin_addr.S_un.S_addr;
	*pPort = ntohs(LocalAddr.sin_port);

	return TRUE;
}

BOOL CMySocket::GetPeerAddr(DWORD *pIP, WORD *pPort)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = SOCK_SOCKINVALID;
		return FALSE;
	}

	struct sockaddr_in PeerAddr;
	int AddrLen = sizeof(PeerAddr);
	
	if(getpeername(m_sSocket, (struct sockaddr *)&PeerAddr, &AddrLen) == SOCKET_ERROR){

		m_dwLastError = WSAGetLastError();
		Close();
		return FALSE;
	}

	*pIP = PeerAddr.sin_addr.S_un.S_addr;
	*pPort = ntohs(PeerAddr.sin_port);

	return TRUE;
}

CString CMySocket::GetErrorString(DWORD dwError){

     CString strError;
     LPTSTR szTemp;
    
	 if(::FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
            FORMAT_MESSAGE_FROM_SYSTEM,
            NULL,
            dwError,
            0,
            (LPTSTR)&szTemp,
            0,
            NULL) == 0){

		// Unknown error code %08x (%d)
		 strError.Format(IDS_ERROR_UNKNOWN_ID, dwError, LOWORD(dwError));
    }
	else{

		LPTSTR p = _tcschr(szTemp, _T('\r'));

		if(p != NULL){

			*p = _T('\0');
		}

		strError = szTemp;
		::LocalFree(szTemp);
	}
	return strError;
}


int CMySocket::RecvFrom(char *pBuff, int nLen, int nWait)
{

	if(m_sSocket == INVALID_SOCKET){

		m_dwLastError = SOCK_SOCKINVALID;
		return FALSE;
	}

	timeval tv;
	tv.tv_sec  = 0;
	tv.tv_usec = 0;

	SOCKADDR_IN sender;
	fd_set      fdread;

	int nSenderSize = 0;
	int nRecv = 0;

	FD_ZERO(&fdread);
	FD_SET(m_sSocket, &fdread);
	
	if(select(0, &fdread, NULL, NULL, &tv) > 0){

		nSenderSize = sizeof(sender);

		nRecv = recvfrom(m_sSocket, pBuff, nLen, 0, (SOCKADDR*)&sender, &nSenderSize);

		if(nRecv == SOCKET_ERROR){
			
			m_dwLastError = WSAGetLastError();
			Close();
			return SOCKET_ERROR;
		} 
	}


	return nRecv;
}

CString CMySocket::GetHostName(CString strIP)
{

	if(strIP.IsEmpty() || strIP == "0.0.0.0") return strIP;

	CString strHost;
	HOSTENT *host = NULL;
	if(inet_addr(strIP) == INADDR_NONE){

		return strIP;
	}
	else{

		unsigned long addr = inet_addr(strIP);
		host = gethostbyaddr((char*)&addr, sizeof(addr), AF_INET);
	}
	if(host == NULL){

		strHost = strIP;
	}
	else{

		strHost = host->h_name;
	}

	return strHost;
}

DWORD CMySocket::GetIP(CString strHostname)
{

	//Convert from dotted notation if required
	unsigned long	addr = inet_addr(strHostname);
	if(addr == INADDR_NONE){

		//Not a dotted address, then do a lookup of the name
		hostent* hp = gethostbyname(strHostname);
		if(hp){

			memcpy(&addr, hp->h_addr, hp->h_length);
		}
		else{

			TRACE(_T("Could not resolve the host name %s\n"), strHostname);
			return 0;
		}
	}
	return (DWORD)addr;
}
