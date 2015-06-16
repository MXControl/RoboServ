
// source taken from MCS
// (c) by King Marco


#include "MXSock.h"
#include "MXTables.h"

#define CONNECTIONS 8

CString ChannelName = "";   
WORD    PORT		 = 16699;  // port of the channel :P
CString Topic		 = "";	   // topic of da channel

DWORD   Users = 0;
DWORD   Limit = 40;
volatile BOOL ShutItDown    = TRUE;
volatile UINT nNumPrimaries = 0;

typedef struct TAG_PARENTNODEINFO {
	DWORD dwNodeIP;		// Primary Node IP Addresses
	WORD wUDPPort;		// Primary Node UDP Port
	WORD wTCPPort;		// Primary Node TCP Port
	BYTE bFreePri;		// Free Primary Connections
	BYTE bFreeSec;		// Free Secondary Connections
	WORD wDummy;		// Padding
	} PARENTNODEINFO, *PPARENTNODEINFO;

int recvit(SOCKET s, char *buff, int len, int flags)
{
	
	int ret=1;
	int done=0;
	while (ret > 0 && done < len && !ShutItDown){

		ret = recv(s,buff+done,len-done,flags);
		if(ret > 0)
			done = done + ret;
	}
	return done;
}


BOOL GetParentNode(PARENTNODEINFO *pNodeInfo)
{

	char Buff[512] = {'\0'};
	BYTE *pNodeBuff = (BYTE *)Buff + sizeof(Buff) / 2;
	int i = 0, j = 0, iFree = 0;
	TCHAR szHostName[256] = {'\0'};
	srand(GetTickCount());
	WORD wHostIndex = rand()%9;
	WORD wPortIndex = 0;
	PARENTNODEINFO FreeNode;
	SOCKET m_PeerCacheSock= socket(PF_INET, SOCK_STREAM, 0);
	
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
	if(connect(m_PeerCacheSock, (PSOCKADDR)&SockAddr, sizeof(sockaddr))){

		return FALSE;
	}
		
	// Recv 0x38
	if(recvit(m_PeerCacheSock, Buff, 1, 0) != 1) return FALSE;
	if(Buff[0] != 0x38)	return FALSE;

	// Recv DW Key Block
	if(recvit(m_PeerCacheSock,Buff, 16, 0) != 16) return FALSE;
	
	if(GetCryptKeyID((BYTE *)Buff) != 0x54)	return FALSE;

	// Recv Node Information
	if(recvit(m_PeerCacheSock,Buff, 132, 0) != 132)	return FALSE;

	closesocket(m_PeerCacheSock);

	// Decrypt Node Information
	DecryptFrontCode((BYTE *)Buff, pNodeBuff);
	memcpy(pNodeInfo, pNodeBuff, 120);
	
	// Sort by Free Secondary Num
	for(j = 0 ; j < 9 ; j++){

		iFree = j;
		
		for(i = j + 1 ; i < 10 ; i++){

			if(pNodeInfo[i].bFreePri > pNodeInfo[iFree].bFreePri || 
				(pNodeInfo[i].bFreePri == pNodeInfo[iFree].bFreePri && 
				  pNodeInfo[i].bFreeSec < pNodeInfo[iFree].bFreeSec)){
				
				iFree = i;
			}
		}

		FreeNode = pNodeInfo[j];
		pNodeInfo[j] = pNodeInfo[iFree];
		pNodeInfo[iFree] = FreeNode;
	}

	// Retry if No Free Parent Node
	if(pNodeInfo[0].bFreePri < 3){

		return FALSE;
	}

	return TRUE;
}

//	SOCKET udpsock;
void SendRoom(const DWORD IP, const int Port, const char* Data, const char* name, const char* topic, const int users, const unsigned long limit, const int TCPPort)
{
	

	SOCKADDR_IN addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(0);
	addr.sin_addr.s_addr = INADDR_ANY;
	
	SOCKET udpsock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	bind(udpsock, (struct sockaddr *) &addr, sizeof(addr));
	char Response[1024];

	addr.sin_family = AF_INET;
	addr.sin_port = htons(Port);
	addr.sin_addr.s_addr = IP;
	*(WORD*)(Response+17)=TCPPort;
	*(WORD*)(Response+15)=0x0000;
	*(WORD*)(Response+13)=limit;
	*(WORD*)(Response+11)=0;
	*(WORD*)(Response+9)=users;
	memcpy(Response+5,Data, 4);
	*(BYTE*)(Response+4)=0x47;
	*(DWORD*)(Response)=rand()%0xFFFFFFFF;
	strcpy(Response+19, name);
	strcpy(Response+20+strlen(name), topic);
	int ret = 21+strlen(name)+strlen(topic);
	EncryptMXUDP((BYTE*) Response, ret);
	sendto(udpsock, Response, ret, 0, (struct sockaddr *) &addr, sizeof(addr));
	sprintf(Response, "%d", TCPPort);
	closesocket(udpsock);
	return;
}

CString Cache[CONNECTIONS][256]={"                                "};

CCriticalSection m_pInternalData; // = new CCriticalSection;
BOOL Connected[CONNECTIONS]={FALSE};

void Connector(void* params)
{

	int connumber = (int)params;
	
	Connected[connumber] = FALSE;
	
	BYTE pos=0;
	srand(GetTickCount());
  
	SOCKET s = socket(PF_INET, SOCK_STREAM, 0);

	DWORD dwUPKey = 0;	// Encrypt Key for Send
	DWORD dwDWKey = 0;	// Decrypt Key for Recv
	char Buff[1024] = {0};	// Recv Buffer
	
	while (!ShutItDown){

		Connected[connumber] = FALSE;

		while((strlen(ChannelName) < 2) && !ShutItDown){

			Sleep(100);	
		}
		
		if(ShutItDown) return;

		PARENTNODEINFO parent;
		BOOL again = TRUE;


		while (again && !ShutItDown){

			// Get ParentNode
			Connected[connumber] = FALSE;
			closesocket(s);
			s = socket(PF_INET, SOCK_STREAM, 0);
			Sleep(100);
			parent.wTCPPort=6699;
			parent.dwNodeIP=inet_addr("192.168.0.251");
			while (!GetParentNode(&parent) && !ShutItDown){

				Sleep(1000);
			}
			if(ShutItDown) return;

			SOCKADDR_IN SockAddr11;
			SockAddr11.sin_family = AF_INET;
			SockAddr11.sin_port = htons(parent.wTCPPort);
			SockAddr11.sin_addr.S_un.S_addr = parent.dwNodeIP;
			if(connect(s, (PSOCKADDR)&SockAddr11, sizeof(sockaddr)) != 0){

				Sleep(1000);
				closesocket(s);
				again = TRUE;
			}
			else{

				again = FALSE;
			}
		}

		int iValue = 5000;
		setsockopt(s, SOL_SOCKET, SO_RCVTIMEO, (char *)&iValue, sizeof(int));

		// Connection to p1 node was successfull
		// login

		// Recv 0x31
		if(recv(s,Buff, 1, 0) != 1){			// Recv Timeout

			closesocket(s);
			continue;
		}

		if(Buff[0] != 0x31){	// Recv Except 0x31 

			closesocket(s);
			continue;
		}

		// Make Key Block
		CreateCryptKeyID(0x0050, (BYTE *)Buff);	// Chat Client Key ID is 0x0057

		// Send Key Block
		if(send(s, Buff, 16, 0) != 16){	// Send Key Block Failed

			closesocket(s);
			continue;
		}

		// Recv Key Bloack
		if(recv(s, Buff, 16, 0) != 16){

			closesocket(s);
			continue;
		}

		// Analyzes Key Blockz
		if(GetCryptKey((BYTE *)Buff, &dwUPKey, &dwDWKey) != 0x0051){

			closesocket(s);
			continue;
		}		

		memset(Buff,0,1024);
		char Buff1[1024]={0};
		Connected[connumber] = TRUE;

		// encryption established, we have a go :P
		nNumPrimaries++; 
		
		while(recvit(s, Buff, 17, 0) == 17 && (strlen(ChannelName) > 1) && !ShutItDown){

			int Ref;
			dwDWKey = DecryptMXTCP((BYTE *)Buff, 17, dwDWKey);
			WORD wLen = *(BYTE *)(Buff + 15);
			if(wLen > 0){

				DWORD IP = *(DWORD*)(Buff);
				

				WORD UDPPORT = *(WORD*)(Buff + 4);
				Ref = recvit(s, Buff1, wLen, 0);
				if(Ref == wLen){

					dwDWKey = DecryptMXTCP((BYTE *)Buff1, wLen, dwDWKey);
					
					if(Buff1[0] == 0x4E){

						CSingleLock intLock( &m_pInternalData );
						intLock.Lock();

						BOOL go=TRUE;
						char temp[1024]={0};
						sprintf(temp, "%02X%02X%02X%02X",(BYTE)Buff[8],(BYTE)Buff[9],(BYTE)Buff[10],(BYTE)Buff[11]);
						for(int z=0; z<CONNECTIONS && !ShutItDown;z++){
							
							for(int y=0; y<256;y++){

								if(Cache[z][y] == temp){

									go=FALSE;
								}
							}
						}

						Cache[connumber][pos]=temp;
						intLock.Unlock();
						pos++;
						if(go){

							if(ShutItDown){

								Connected[connumber] = FALSE;
								nNumPrimaries--;
								go    = FALSE;
								ChannelName = "";
								closesocket(s);
								TRACE("Roomlisting thread exited");
								return;
							}
							if(Topic.GetLength() > 125) Topic = Topic.Left(125);
							SendRoom(IP, UDPPORT, Buff+8, (LPCTSTR)ChannelName, (LPCTSTR)CString(Topic), Users,Limit,PORT);
						}
					}
				}
				else{

					closesocket(s);
					continue;
				}
			}
			nNumPrimaries--;
			closesocket(s);
			continue;
		}
	}
	closesocket(s);
}

void StopList()
{

	for (int x=0; x<CONNECTIONS; x++){

		Connected[x]  = FALSE;
		nNumPrimaries--;
	}
}

void StartList()
{

	for (int x = 0; x < CONNECTIONS && !ShutItDown; x++){

		Sleep(100);
		_beginthread(Connector,0,(void*)x);
	}
}
