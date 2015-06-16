// RoomLister.cpp: implementation of the CRoomLister class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "roboserv.h"
#include "RoomLister.h"
#include "MXSock.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CString g_sCache[PRIMARIES][256]={"                                "};

CRoomLister* CRoomLister::m_pThis = NULL;

CRoomLister::CRoomLister()
{

	m_nMaxPrimaries = PRIMARIES;
	m_bShutdown     = TRUE;
	m_nMaxListed	= 6;

	for(UINT i = 0; i < m_nMaxPrimaries; i++){

		m_bConnected[i]   = FALSE;
	}

	CRoomLister::m_pThis = this;
}


CRoomLister::~CRoomLister()
{

}

int CRoomLister::Recieve(SOCKET s, char *buffer, int nLen, int nFlags)
{

	int nRecv  = 1;
	int nTotal = 0;

	while(nRecv > 0 && nTotal < nLen && !m_bShutdown){

		
		nRecv = recv(s, buffer+nTotal, nLen-nTotal, nFlags);
		
		if(nRecv > 0)
			nTotal+= nRecv;
	}
	
	return nTotal;
}

BOOL CRoomLister::GetPrimaryNode(PNODEINFO pNodeInfo)
{


	char Buff[512] = {'\0'};
	
	BYTE *pNodeBuff = (BYTE *)Buff + sizeof(Buff) / 2;
	int i = 0, j = 0, nFree = 0;
	
	TCHAR szHostName[256] = {'\0'};

	WORD wHostIndex = rand()%9;
	WORD wPortIndex = 0;
	
	NODEINFO FreeNode;
	
	SOCKET sSocket = socket(PF_INET, SOCK_STREAM, 0);
	
	wsprintf(szHostName, "c331%d.z1303.winmx.com", wHostIndex);
		
	SOCKADDR_IN SockAddr;
	SockAddr.sin_family = AF_INET;
	SockAddr.sin_port = htons(7950);
	DWORD dwIP = inet_addr(szHostName);
	
	if(dwIP	== INADDR_NONE){
			
		struct hostent *pHost = gethostbyname(szHostName);
			
		if(!pHost){		
		
			return FALSE;
		
		}
		
		dwIP = *(DWORD *)pHost->h_addr_list[0];
	
	}

	SockAddr.sin_addr.S_un.S_addr = dwIP;
	if(connect(sSocket, (PSOCKADDR)&SockAddr, sizeof(sockaddr))){

		return FALSE;
	}
		
	// Recv 0x38
	if(Recieve(sSocket, Buff, 1, 0) != 1){
		
		return FALSE;
	}

	if(Buff[0] != 0x38){
		
		return FALSE;
	}

	// Recv DW Key Block
	if(Recieve(sSocket,Buff, 16, 0) != 16){
		
		return FALSE;
	}
	
	if(GetCryptKeyID((BYTE *)Buff) != 0x54){
		
		return FALSE;
	}

	// Recv Node Information
	if(Recieve(sSocket,Buff, 132, 0) != 132){
		
		return FALSE;
	}

	closesocket(sSocket);

	// Decrypt Node Information
	DecryptFrontCode((BYTE *)Buff, pNodeBuff);

	memcpy(pNodeInfo, pNodeBuff, 120);
	
	// Sort by Free Secondary Num
	for(j = 0 ; j < 9 ; j++){

		nFree = j;
		
		for(i = j + 1 ; i < 10 ; i++){

			if(pNodeInfo[i].bFreePri > pNodeInfo[nFree].bFreePri || 
				(pNodeInfo[i].bFreePri == pNodeInfo[nFree].bFreePri && 
				  pNodeInfo[i].bFreeSec < pNodeInfo[nFree].bFreeSec)){
				
				nFree = i;
			}
		}

		FreeNode = pNodeInfo[j];
		pNodeInfo[j] = pNodeInfo[nFree];
		pNodeInfo[nFree] = FreeNode;
	}

	// Retry if No Free Parent Node
	if(pNodeInfo[0].bFreePri < 3){

		return FALSE;
	}

	return TRUE;
}

void CRoomLister::SendRoom(const DWORD dwIP, const int nPort, LPCTSTR pzData, LPCTSTR pzName, LPCTSTR pzTopic, const DWORD dwUsers, const DWORD dwLimit, const int nTCPPort)
{

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(0);
	addr.sin_addr.s_addr = INADDR_ANY;
	
	SOCKET udpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bind(udpsock, (struct sockaddr *) &addr, sizeof(addr));
	char Response[1024];

	addr.sin_family = AF_INET;
	addr.sin_port = htons(nPort);
	addr.sin_addr.s_addr = dwIP;
	
	*(WORD*)(Response + 17)	= nTCPPort;
	*(WORD*)(Response + 15)	= 0x0000;
	*(WORD*)(Response + 13)	= (WORD)dwLimit;
	*(WORD*)(Response + 11)	= 0;
	
	*(WORD*)(Response + 9)	= (WORD)dwUsers;
	
	memcpy(Response + 5, pzData, 4);
	
	*(BYTE*)(Response + 4)	= 0x47;
	
	*(DWORD*)(Response)		= rand() % 0xFFFFFFFF;
	
	strcpy(Response + 19, pzName);
	
	strcpy(Response + 20 + strlen(pzName), pzTopic);
	
	int nLen = strlen(pzName) + strlen(pzTopic) + 21;

	EncryptMXUDP((BYTE*)Response, nLen);
	
	sendto(udpsock, Response, nLen, 0, (struct sockaddr *)&addr, sizeof(addr));
	
	sprintf(Response, "%d", nTCPPort);
	
	closesocket(udpsock);
}

UINT CRoomLister::PrimaryThread(LPVOID pParam)
{

	ASSERT(m_pThis);

	int nConNumber = (int)pParam;
	TRACE("Entering Primary Thread %d\n", nConNumber);
	
	
	m_pThis->m_bConnected[nConNumber] = FALSE;
	
	BYTE btPos = 0;
	srand(GetTickCount());
  
	SOCKET s = socket(PF_INET, SOCK_STREAM, 0);

	DWORD dwUPKey = 0;	// Encrypt Key for Send
	DWORD dwDWKey = 0;	// Decrypt Key for Recv
	char  cBuffer[1024] = {0};	// Recv Buffer
	
	while (!m_pThis->m_bShutdown){


		while((m_pThis->m_aRooms.GetSize() == 0) && !m_pThis->m_bShutdown){

			Sleep(100);	
		}

		if(m_pThis->m_bShutdown) return 0;

		NODEINFO ndNode;
		
		BOOL bOK = FALSE;


		while(!bOK && !m_pThis->m_bShutdown){

			// Get ParentNode
			m_pThis->m_bConnected[nConNumber] = FALSE;
			
			s = socket(PF_INET, SOCK_STREAM, 0);
		
			Sleep(100);

			ndNode.wTCPPort = 6699;
			ndNode.dwNodeIP = inet_addr("192.168.0.251");
		
			while (!m_pThis->GetPrimaryNode(&ndNode) && !m_pThis->m_bShutdown){

				Sleep(1000);
			}

			if(m_pThis->m_bShutdown) return 0;

			SOCKADDR_IN SockAddr11;
			SockAddr11.sin_family = AF_INET;
			SockAddr11.sin_port = htons(ndNode.wTCPPort);
			SockAddr11.sin_addr.S_un.S_addr = ndNode.dwNodeIP;
			if(connect(s, (PSOCKADDR)&SockAddr11, sizeof(sockaddr)) != 0){

				Sleep(1000);
				closesocket(s);
				bOK = FALSE;
			}
			else{

				bOK = TRUE;
			}
		}

		int iValue = 5000;
		setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&iValue, sizeof(int));

		// Connection to p1 node was successfull
		// login

		// Recv 0x31
		if(recv(s, cBuffer, 1, 0) != 1){			// Recv Timeout

			closesocket(s);
			continue;
		}

		if(cBuffer[0] != 0x31){	// Recv Except 0x31 

			closesocket(s);
			continue;
		}

		// Make Key Block
		CreateCryptKeyID(0x0050, (BYTE*)cBuffer);	// Chat Client Key ID is 0x0057

		// Send Key Block
		if(send(s, cBuffer, 16, 0) != 16){	// Send Key Block Failed

			closesocket(s);
			continue;
		}

		// Recv Key Bloack
		if(recv(s, cBuffer, 16, 0) != 16){

			closesocket(s);
			continue;
		}

		// Analyzes Key Blockz
		if(GetCryptKey((BYTE*)cBuffer, &dwUPKey, &dwDWKey) != 0x0051){

			closesocket(s);
			continue;
		}		

		ZeroMemory(&cBuffer, 1024);
		
		char cBuffer2[1024]={0,};

		m_pThis->m_bConnected[nConNumber] = TRUE;

		// encryption established, we have a go :P
		
		while((m_pThis->Recieve(s, cBuffer, 17, 0) == 17) && m_pThis->m_aRooms.GetSize() && !m_pThis->m_bShutdown){

			int nRecv = 0;
			
			dwDWKey		= DecryptMXTCP((BYTE*)cBuffer, 17, dwDWKey);
		
			WORD wLen   = *(BYTE *)(cBuffer + 15);
			
			if(wLen <= 0) continue;

			DWORD dwIP		= *(DWORD*)(cBuffer);
			WORD  wUdpPort	= *(WORD*)(cBuffer + 4);

			nRecv = m_pThis->Recieve(s, cBuffer2, wLen, 0);
			
			if(nRecv != wLen){

				closesocket(s);
				m_pThis->m_bConnected[nConNumber] = FALSE;
				break;
			}

			dwDWKey = DecryptMXTCP((BYTE *)cBuffer2, wLen, dwDWKey);
			
			if(cBuffer2[0] != 0x4E){

				// this data is not intersting for us :P
				continue;
			}

			CSingleLock intLock( &m_pThis->m_csLock );
			intLock.Lock();

			BOOL bSend = TRUE;
			
			char temp[1024]={0};
			
			sprintf(temp, "%02X%02X%02X%02X",(BYTE)cBuffer[8],(BYTE)cBuffer[9],(BYTE)cBuffer[10],(BYTE)cBuffer[11]);
			
			for(UINT z = 0; (z < m_pThis->m_nMaxPrimaries) && !m_pThis->m_bShutdown; z++){
				
				for(int y=0; y<256;y++){

					if(g_sCache[z][y] == temp){

						bSend = FALSE;
					} 
				}
			}

			g_sCache[nConNumber][btPos] = temp;
			intLock.Unlock();
			btPos++;
			
			if(!bSend){

				continue;
			}

			if(m_pThis->m_bShutdown){

				m_pThis->m_bConnected[nConNumber] = FALSE;
				bSend   = FALSE;
				closesocket(s);
				m_pThis->m_eEvents[nConNumber].SetEvent();
				TRACE("Leaving Primary Thread %d (At Point 1)\n", nConNumber);
				return 0;
			}

			for(int nRoom = 0; nRoom < m_pThis->m_aRooms.GetSize(); nRoom++){

				ROOMDATA room = m_pThis->m_aRooms[nRoom];
				m_pThis->SendRoom(dwIP, wUdpPort, (cBuffer+8),
								room.strName, room.strTopic, room.dwUsers, room.dwLimit, room.wPort);
					
			} 
		}
	}
	closesocket(s);
	m_pThis->m_eEvents[nConNumber].SetEvent();
	TRACE("Leaving Primary Thread %d (Normal Exit)\n", nConNumber);
	return 0;
}

void CRoomLister::StopList()
{

	m_bShutdown = TRUE;
	for(UINT n = 0; n < m_nMaxPrimaries; n++){

		m_bConnected[n]  = FALSE;
		
		CWaitCursor wc;
		DWORD nRes = WaitForSingleObject(m_eEvents[n], 1000);

		if(nRes == WAIT_TIMEOUT || nRes == WAIT_FAILED){

			TRACE("Warning: Wait timeout for thread %d\n", n);
			TerminateThread(m_pThreads[n]->m_hThread, 0);
		}
		if(m_pThreads[n] > 0){

			TRACE("Warning: CWinThread object %d seems not to have selfdeleted!\n", n);
		}
	}
}


void CRoomLister::StartList()
{

	m_bShutdown = FALSE;
	for (UINT n = 0; n < m_nMaxPrimaries && !m_bShutdown; n++){

		m_eEvents[n].ResetEvent();
		m_pThreads[n] = AfxBeginThread(PrimaryThread, (LPVOID)n, THREAD_PRIORITY_NORMAL);
		m_pThreads[n]->m_bAutoDelete = TRUE;
		Sleep(100);
	}
}

void CRoomLister::AddRoom(CString strName, CString strTopic, DWORD dwUsers, DWORD dwLimit, WORD wPort)
{

	CSingleLock cLock(&m_csChangeLock);
	cLock.Lock();

	if(m_aRooms.GetSize() < (int)m_nMaxListed){

		ROOMDATA room;
		room.strName  = strName.Left(40);
		room.strTopic = strTopic.Left(125);
		room.dwUsers  = dwUsers;
		room.dwLimit  = dwLimit;
		room.wPort    = wPort;

		m_aRooms.Add(room);
	}
	cLock.Unlock();
}

void CRoomLister::RemoveRoom(CString strRoom)
{

	CSingleLock cLock(&m_csChangeLock);
	cLock.Lock();
	for(int i = 0; i < m_aRooms.GetSize(); i++){

		if(m_aRooms[i].strName == strRoom){

			m_aRooms.RemoveAt(i);
			return;
		}
	}
	cLock.Unlock();
}

void CRoomLister::ChangeName(CString strOldName, CString strNewName)
{

	CSingleLock cLock(&m_csChangeLock);
	cLock.Lock();

	for(int i = 0; i < m_aRooms.GetSize(); i++){

		if(m_aRooms[i].strName == strOldName){

			m_aRooms[i].strName = strNewName;
			return;
		}
	}
	cLock.Unlock();
}

void CRoomLister::ChangeTopic(CString strRoom, CString strTopic)
{

	CSingleLock cLock(&m_csChangeLock);
	cLock.Lock();

	for(int i = 0; i < m_aRooms.GetSize(); i++){

		if(m_aRooms[i].strName == strRoom){

			m_aRooms[i].strTopic = strTopic;
			return;
		}
	}
	cLock.Unlock();
}

void CRoomLister::ChangeUsers(CString strRoom, DWORD dwUser)
{

	CSingleLock cLock(&m_csChangeLock);
	cLock.Lock();
	for(int i = 0; i < m_aRooms.GetSize(); i++){

		if(m_aRooms[i].strName == strRoom){

			m_aRooms[i].dwUsers = dwUser;
			return;
		}
	}
	cLock.Unlock();
}

void CRoomLister::ChangeLimit(CString strRoom, DWORD dwLimit)
{

	CSingleLock cLock(&m_csChangeLock);
	cLock.Lock();

	for(int i = 0; i < m_aRooms.GetSize(); i++){

		if(m_aRooms[i].strName == strRoom){

			m_aRooms[i].dwLimit = dwLimit;
			return;
		}
	}
	cLock.Unlock();
}

int CRoomLister::GetPrimaryNum()
{

	int nNum = 0;
	for(UINT i = 0; i < m_nMaxPrimaries; i++){

		nNum+=m_bConnected[i];
	}
	return nNum;
}
