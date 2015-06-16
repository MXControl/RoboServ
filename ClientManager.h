// ClientManager.h: interface for the CClientManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CLIENTMANAGER_H__3913F08C_C373_430C_99D8_65E789BBFF43__INCLUDED_)
#define AFX_CLIENTMANAGER_H__3913F08C_C373_430C_99D8_65E789BBFF43__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if !defined(__EXT_TEMPL_H)
	#include <ExtTempl.h>
#endif

#include <queue>
using std::queue;

#include "MySocket.h"
#include "ClientSocket.h"

// Channel modes:
#define CM_NORMAL	        0x0001 
#define CM_KEYWORD          0x0002 // room is password protected
#define CM_MODERATED        0x0003 // room is moderated
#define CM_EXILEENABLE      0x0004 // enable exile room
#define CM_NOID			    0x0008 // hide user id
#define CM_SUBCHANNELS		0x0010	// room may have subchannels
#define CM_MAINCHANNEL      0x0020 // this is the mainroom



typedef struct TAG_BAN
{

	CString strName;
	CString strIP;
	CString strHost;
} BAN, *PBAN;


typedef  CArray<CClientSocket*, CClientSocket*> CClients;
typedef  CArray<BAN, BAN> CBans;

class CClientManager  
{
public:
	static void RemoveColor(CString &rString);
	static void HandleColorPermission(UINT uMode, CString &strName, CString& strText);
	virtual UINT AddClient(CClientSocket* pClient);
	CClientManager(HWND& hWnd);
	virtual ~CClientManager();

	BOOL	  m_bHosted;
	CEvent	  m_eDone;
	CEvent    m_eHosted;
	CEvent    m_eNotifyDone;
	BOOL	  m_bShutdown;
	CString   m_strRoomBase;
	CString   m_strRoomFull;
	CString   m_strGodName;
	DWORD	  m_dwIP;
	WORD	  m_wPort;
	DWORD	  m_dwLimit;
	DWORD     m_dwUsers;
	CString	  m_strTopic;
	CString   m_strMotd;
	CClients  m_aClients;
	CBans	  m_aBans;
	UINT      m_uMode;
	CString   m_strKeyword;
	CString   m_strExile;
	SOCKET    m_sIn;
	CWinThread *m_pServerThread;
	CWinThread *m_pNotifyThread;
	
	queue<ClientNotify*> m_qNotifies;

	__int64 m_nTotalMessages;
	unsigned __int64 m_nTotalJoins;
	unsigned __int64 m_nFailedJoins;

	CString			 m_strStartDate;
	CString			 m_strStartTime;
	CTime			 m_cStartTime;
	CStringArray	 m_aCommands;

	CRITICAL_SECTION m_csLock;
	CRITICAL_SECTION m_csWriteLock;

	virtual void SetTopic(const CString strTopic);
	virtual void SetRoom(const CString strName);
	void SetMotd(const CString strMotd);
	virtual void SetLimit(DWORD uLimit);
	virtual void SetUsers(DWORD uUsers);

	// Broadcast Send
	void SendTopic();
	void SendMotd();
	void SendMsg(CString strUser, CString strMsg);
	void SendOpMsg(CString strUser, CString strMsg, UINT uColor = 0, UINT uRecpMode = UM_ADMIN);
	void SendAction(CString strUser, CString strMsg);
	virtual void SendJoin(const CString strUser, DWORD dwIP, WORD wPort, WORD wLine, DWORD dwFiles, WORD wUserLevel, DWORD dwRealIP);
	virtual void SendPart(const CString strUser, DWORD dwIP, WORD wPort, BOOL bRealPart = TRUE);
	virtual BOOL SendRename(CClientSocket* pClient, const CString strOldName, DWORD dwOldIP, WORD wOldPort, const CString strNewName, DWORD dwNewIP, WORD wNewPort, WORD wLine, DWORD dwFiles, WORD wUserLevel);
	void SendMode(const CString strSender, const CString strMode);
	void SendIPClient(CClientSocket* pAdmin, BOOL bAdd);
	void SendWelcome(CString strMsg);
	void ServerAction(const CString strSender, const CString strMsg, UINT uColor = 0, UINT uMode = UM_ADMIN);
	void Stop(BOOL bRedirect = FALSE, CString strTarget = "");
	void CheckClients(void);

	static UINT NotifyThread(LPVOID pParam);

	DWORD TranslateIP(CString strIP);

	BOOL CheckUserName(const CString strName, CString strHost, DWORD dwIP);
	BOOL CheckUserName(const CString strName, DWORD dwIP, WORD wPort);
	BOOL CheckCommands(const CString strUser, const CString strMsg);

	virtual BOOL ExecuteChannelCommand(const CString strUser, const CString strMsg, const UINT uMode);
	void FixString(CString& strFix);
	int  GetByID(GUID guid);
	void SendChannelRename(void);
	BOOL HasSpeakPermission(UINT uMode);
	void SendNoVoice(GUID guid);
	void SendCmdNotUnderstood(const CString strUser, const CString strCmd);
	void SendPermissionDenied(const CString strUser, const CString strCmd);
	BOOL CheckPermission(UINT uMode, UINT uRequired);
	void SendChannelStats(void);
	void HandleVisible(CClientSocket* pClient, WORD wMode, BOOL bSetVisible);


	virtual void EchoChat(CString strName, CString strMsg) = 0;
	virtual void WriteText(CString strText, COLORREF crText = RGB(0,0,128), BOOL bBold = TRUE, BOOL bAppendNewLine = TRUE) = 0;
	virtual void WriteText(COLORREF crText, UINT nID, ...) = 0;
	virtual void UpdateModeUserlist(CString strUser, UINT uMode) = 0;


	HWND& m_hWndTarget;
};

#endif // !defined(AFX_CLIENTMANAGER_H__3913F08C_C373_430C_99D8_65E789BBFF43__INCLUDED_)
