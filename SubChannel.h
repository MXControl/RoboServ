// SubChannel.h: interface for the CSubChannel class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_SUBCHANNEL_H__117D673F_B2C2_43E2_821E_7F1D2BAAB490__INCLUDED_)
#define AFX_SUBCHANNEL_H__117D673F_B2C2_43E2_821E_7F1D2BAAB490__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ClientManager.h"


typedef struct TAG_WRITE_STRUCT
{

	CString strRoom;
	CString strText;
	COLORREF crColor;
	BOOL	bBold;
	BOOL	bNewLine;

} WRITE_STRUCT, *PWRITE_STRUCT;

class CSubChannel : 
	public CWnd,
	public CClientManager  
{
public:
	CString m_strParentRoom;
	virtual BOOL ExecuteChannelCommand(const CString strUser, const CString strMsg, const UINT uMode);
	void SendPart(const CString strUser, DWORD dwIP, WORD wPort, BOOL bRealPart = TRUE);
	CSubChannel(HWND& hWnd);
	virtual ~CSubChannel();
	virtual void EchoChat(CString strName, CString strMsg);
	virtual void WriteText(CString strText, COLORREF crText = RGB(0,0,128), BOOL bBold = TRUE, BOOL bAppendNewLine = TRUE);
	virtual void WriteText(COLORREF crText, UINT nID, ...);
	virtual void UpdateModeUserlist(CString strUser, UINT uMode){}; // we dont need that...
	virtual UINT AddClient(CClientSocket* pClient);
	virtual void SetTopic(const CString strTopic);
	virtual void SetLimit(DWORD uLimit);
	virtual void SetUsers(DWORD uUsers);
	virtual void SetRoom(const CString strName);
	HWND &m_hWndWrite;

protected:
	afx_msg LRESULT ClientCallback(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()
};

#endif // !defined(AFX_SUBCHANNEL_H__117D673F_B2C2_43E2_821E_7F1D2BAAB490__INCLUDED_)
