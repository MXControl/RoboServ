/*
 * BendMX - Chat extension library for WinMX.
 * Copyright (C) 2003-2004 by Thees Ch. Schwab
 * 
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Contact: Bender979@yahoo.com
 *
*/

#if !defined(AFX_SETTINGS_H__25ADA47A_C2DF_42B2_B3AE_6C12BA658D11__INCLUDED_)
#define AFX_SETTINGS_H__25ADA47A_C2DF_42B2_B3AE_6C12BA658D11__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSettings  
{
public:
	void Save(CString strFile = "roboServ.ini");
	void Load(CString strFile = "roboServ.ini");
	CSettings();
	virtual ~CSettings();

	// Set Functions
	void	SetWorkingDir(CString strValue){ m_strWd = strValue; }
	CString GetWorkingDir(BOOL bCached = TRUE);
protected:
	CString m_strWd;
	CString m_strLogDir;

public:
	void LoadFromServer(CString strURL);
	POSITION GetLoginIterator();
	void IterateLogins(POSITION& pos, CString& strPass, UINT& uMode);
	UINT GetLogin(CString strPass);
	void DeleteLogin(CString strPass);
	void AddLogin(CString strPass, UINT uMode);
	void SaveLogins(CString strFile);
	void LoadLogins(CString strFile);
	void	SetDefaultUserMode(UINT uMode){ m_uDefMode = uMode; }
	void	SetTopic(CString strValue){ m_strTopic = strValue; }
	void	SetMotd(CString strValue){ m_strMotd = strValue; }
	void	SetKeyMsg(CString strValue){ m_strKeyMsg = strValue; }
	void	SetGodName(CString strValue){ m_strGodName = strValue; }
	void	SetGodFiles(DWORD dwValue){ m_dwGodFiles = dwValue; }
	void	SetGodLine(WORD wValue){ m_wGodLine = wValue; }
	void	SetGodVisible(BOOL bValue){ m_bGodVisible = bValue; }
	void	SetLimit(DWORD dwValue){ m_dwLimit = dwValue; }
	void	SetMultiIPOk(BOOL bValue){ m_bMultiIPOk = bValue; }
	void	SetBlockNushi(BOOL bValue){ m_bBLockNushi = bValue; }
	void	SetLocalIsOp(BOOL bValue){ m_bLocalIsOp = bValue; }
	void	SetListed(BOOL bValue){ m_bListed = bValue; }
	void	SetColor(UINT nValue){m_nColor = nValue; }
	void	SetWelcome(CString strValue){ m_strWelcome = strValue; }
	void	SetRoomname(CString strValue){ m_strRoom = strValue; }
	void	SetKeyword(CString strValue){ m_strKeyword = strValue; }
	void    SetChannelMode(UINT uMode){ m_uMode = uMode; }
	void    SetStyle(int nStyle){ m_nStyle = nStyle; }
	void	SetExile(CString strValue){m_strExile = strValue; }
	void	SetSubPermission(UINT uValue){ m_uOpenSub = uValue; }
	void	SetDisplaySubText(BOOL bValue){ m_bDisplaySubText = bValue; }
	void	SetAutoCloseSub(BOOL bValue){ m_bAutoCloseSub = bValue; }
	void	SetLimitSub(DWORD dwValue){ m_wLimitSub = dwValue; }
	void    SetIP(DWORD dwValue){ m_dwIP = dwValue; }
	void    SetPort(WORD wValue){ m_wPort = wValue; }
	void    SetAdvertiseRose(BOOL bValue){ m_bAdvertise = bValue; }
	void	SetSubFirstIsSupoer(BOOL bValue){ m_bFirstIsSuper = bValue; }
	void	SetAdminWatch(UINT uValue){ m_uAdminWatch = uValue; }
	void	SetVoiceWatch(UINT uValue){ m_uVoiceWatch = uValue; }
	void    SetSubListed(BOOL bValue){ m_bSubListed = bValue; }
	void	SetNormalIcon(int nValue){ m_nNormalIcon = nValue; }
	void	SetVoiceIcon(int nValue){ m_nVoiceIcon = nValue; }
	void	SetAdminIcon(int nValue){ m_nAdminIcon = nValue; }
	void    SetAgentIcon(int nValue){ m_nAgentIcon = nValue; }

	CString    GetTopic(){ return m_strTopic; }
	CString    GetMotd(){ return m_strMotd; }
	CString    GetKeyMsg(){ return m_strKeyMsg; }
	CString    GetGodName(){ return m_strGodName; }
	DWORD	   GetGodFiles(){ return m_dwGodFiles; }
	WORD	   GetGodLine(){ return m_wGodLine; }
	BOOL       GetGodVisible(){ return m_bGodVisible; }
	DWORD	   GetLimit(){ return m_dwLimit; }
	BOOL       GetMultiIPOk(){ return m_bMultiIPOk; }
	BOOL       GetBlockNushi(){ return m_bBLockNushi; }
	BOOL       GetLocalIsOp(){ return m_bLocalIsOp; }
	BOOL	   GetListed(){ return m_bListed; }
	int		   GetColor(){ return m_nColor; }
	CString	   GetWelcome(){ return m_strWelcome; }
	CString	   GetRoomname(){ return m_strRoom; }
	CString	   GetKeyword(){ return m_strKeyword; }
	UINT	   GetChannelMode(){ return m_uMode; }
	int		   GetStyle(){ return m_nStyle; }
	CString	   GetExile(){ return m_strExile; }
	UINT	   GetSubPermission(){ return m_uOpenSub; }
	BOOL	   GetDisplaySubText(){ return m_bDisplaySubText; }
	BOOL	   GetAutoCloseSub(){ return m_bAutoCloseSub; }
	DWORD	   GetLimitSub(){ return m_wLimitSub; }
	DWORD      GetIP(){ return m_dwIP; }
	WORD       GetPort(){ return m_wPort; }
	BOOL	   GetAdvertiseRose(){ return m_bAdvertise; }
	UINT	   GetDefaultUserMode(){ return m_uDefMode; }
	BOOL	   GetSubFirstIsSuper(){ return m_bFirstIsSuper; }
	UINT	   GetAdminWatch(){ return m_uAdminWatch; }
	UINT	   GetVoiceWatch(){ return m_uVoiceWatch; }
	BOOL	   GetSubListed(){ return m_bSubListed; }
	int		   GetNormalIcon(){ return m_nNormalIcon; }
	int		   GetVoiceIcon(){ return m_nVoiceIcon; }
	int		   GetAdminIcon(){ return m_nAdminIcon; }
	int		   GetAgentIcon(){ return m_nAgentIcon; }

protected:
	// Server shit
	CString	   m_strExile;
	CString	   m_strRoom;
	CString    m_strTopic;
	CString    m_strMotd;
	CString    m_strKeyMsg;
	CString    m_strGodName;
	CString	   m_strWelcome;
	CString	   m_strKeyword;
	DWORD	   m_dwGodFiles;
	WORD	   m_wGodLine;
	BOOL       m_bGodVisible;
	DWORD	   m_dwLimit;
	BOOL       m_bMultiIPOk;
	BOOL       m_bBLockNushi;
	BOOL       m_bLocalIsOp;
	BOOL	   m_bListed;
	int		   m_nColor;
	UINT	   m_uMode;
	UINT	   m_uOpenSub;
	int		   m_nStyle;
	BOOL	   m_bDisplaySubText;
	BOOL	   m_bAutoCloseSub;
	DWORD	   m_wLimitSub;
	BOOL	   m_bAdvertise;
	DWORD      m_dwIP;
	WORD       m_wPort;
	UINT	   m_uDefMode;
	BOOL	   m_bFirstIsSuper;
	UINT	   m_uAdminWatch;
	UINT	   m_uVoiceWatch;
	BOOL       m_bSubListed;
	int  m_nNormalIcon;
	int  m_nVoiceIcon;
	int  m_nAdminIcon;
	int  m_nAgentIcon;
	CMap<CString, LPCTSTR, UINT, UINT&> m_aLogins;
};

#endif // !defined(AFX_SETTINGS_H__25ADA47A_C2DF_42B2_B3AE_6C12BA658D11__INCLUDED_)
