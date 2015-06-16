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


#if !defined(AFX_MYSOCKET_H__56B83F29_B3D6_4BF4_BC85_5DD89C6DE20C__INCLUDED_)
#define AFX_MYSOCKET_H__56B83F29_B3D6_4BF4_BC85_5DD89C6DE20C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <WinSock2.h>

// Return values for GetLastError()
#define SOCK_NOERROR		0x00
#define	SOCK_TIMEOUT		0x01
#define SOCK_SOCKINVALID	0x02

class CMySocket  
{
public:
	CMySocket();
	CMySocket(SOCKET sSocket);
	CMySocket(int nProtocol, u_short uPort);

	virtual ~CMySocket();

	BOOL Connect(LPCTSTR lpszIP, WORD wPort, int nWait = 0, int nProtocol = IPPROTO_TCP);
	BOOL Connect(DWORD dwIP, WORD wPort, int nWait = 0, int nProtocol = IPPROTO_TCP);

	SOCKET Accept(sockaddr_in* from = NULL);
	
	int Recv(char *pBuff, int nLen, int nWait = 0);
	int RecvOnce(char *pBuff, int nMaxLen, int nWait = 0);
	int RecvFrom(char* pBuff, int nLen, int nWait = 0);
	
	int Send(char *pBuff, int nLen, int nWait = 0);
	int SendOnce(char *pBuff, int nMaxLen, int nWait = 0);
	
	BOOL Close(void);
	DWORD GetLastError(void);
	CString GetLastErrorStr(void);
	static CString GetErrorString(DWORD dwError);

	BOOL GetLocalAddr(DWORD *pIP, WORD *pPort);
	BOOL GetPeerAddr(DWORD *pIP, WORD *pPort);

	SOCKET m_sSocket;			
	DWORD  m_dwLastError;
	int Listen(u_short uPort);
	void SetSocket(SOCKET sSocket);
	static CString GetHostName(CString strIP);
	static DWORD GetIP(CString strHostname);
};

#endif // !defined(AFX_MYSOCKET_H__56B83F29_B3D6_4BF4_BC85_5DD89C6DE20C__INCLUDED_)
