// RoomLister.h: interface for the CRoomLister class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ROOMLISTER_H__FF9FF0EE_2C3C_4826_B641_5949782DC9CE__INCLUDED_)
#define AFX_ROOMLISTER_H__FF9FF0EE_2C3C_4826_B641_5949782DC9CE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <winsock2.h>

#define PRIMARIES		8

class CRoomLister  
{
public:
	CRoomLister();
	virtual ~CRoomLister();

public: // structures / types

	typedef struct TAG_NODEINFO {

		DWORD dwNodeIP;		// Primary Node IP Addresses
		WORD wUDPPort;		// Primary Node UDP Port
		WORD wTCPPort;		// Primary Node TCP Port
		BYTE bFreePri;		// Free Primary Connections
		BYTE bFreeSec;		// Free Secondary Connections
		WORD wDummy;		// Padding
	} NODEINFO, *PNODEINFO;

	typedef struct TAG_ROOMDATA {

		CString strName;
		CString strTopic;
		WORD    wPort;
		DWORD   dwUsers;
		DWORD   dwLimit;
	} ROOMDATA, *PROOMDATA;
	
	typedef CArray<ROOMDATA, ROOMDATA> CRoomArray;

public: // interface
	void SendRoom(const DWORD dwIP, const int nPort, LPCTSTR pzData, LPCTSTR pzName, LPCTSTR pzTopic, const DWORD dwUsers, const DWORD dwLimit, const int nTCPPort);
	BOOL GetPrimaryNode(PNODEINFO pNodeInfo);
	int Recieve(SOCKET s, char* buffer, int nLen, int nFlags);
	void ChangeLimit(CString strRoom, DWORD dwLimit);
	void ChangeUsers(CString strRoom, DWORD dwUser);
	void ChangeTopic(CString strRoom, CString strTopic);
	void ChangeName(CString strOldName, CString strNewName);
	void RemoveRoom(CString strRoom);
	void AddRoom(CString strName, CString strTopic, DWORD dwUsers, DWORD dwLimit, WORD wPort);
	void StartList();
	void StopList();
	static UINT PrimaryThread(LPVOID pParam);

public: // attributes
	int GetPrimaryNum();

	CRoomArray  m_aRooms;

	volatile BOOL m_bShutdown;
	volatile UINT m_nMaxPrimaries;
	UINT m_nMaxListed;
	CCriticalSection m_csLock; // = new CCriticalSection;
	CCriticalSection m_csChangeLock;

	BOOL		m_bConnected[PRIMARIES];
	CEvent		m_eEvents[PRIMARIES];
	CWinThread*	m_pThreads[PRIMARIES];
	static CRoomLister* m_pThis;


};

#endif // !defined(AFX_ROOMLISTER_H__FF9FF0EE_2C3C_4826_B641_5949782DC9CE__INCLUDED_)
