/*
 * RoboMX - Chatclient for WinMX.
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

#include "stdafx.h"
#include "roboServ.h"
#include "Settings.h"
#include "Ini.h"
#include "Tokenizer.h"
#include ".\settings.h"
#include "ClientSocket.h"
#include "ClientManager.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CSettings::CSettings()
{


	m_strGodName	= "roboServe.100_00000";
	m_dwGodFiles	= 0;
	m_wGodLine		= 0;
	m_bGodVisible	= FALSE;
	m_dwLimit		= 40;
	m_bMultiIPOk	= TRUE;
	m_bBLockNushi	= FALSE;
	m_bLocalIsOp	= FALSE;
	m_uOpenSub		= 0x00100000L; // operator
	m_bDisplaySubText = TRUE;
	m_bAutoCloseSub = FALSE;
	m_wLimitSub	    = 10;
	m_bSubListed    = FALSE;
	m_nNormalIcon   = 0;
	m_nVoiceIcon	= 2;
	m_nAdminIcon	= 1;
	m_nAgentIcon	= 1;
	srand(GetTickCount());
}

CSettings::~CSettings()
{

	try{

		CFileStatus r;
		if(CFile::GetStatus(m_strWd+"\\temp.dat", r)){

			CFile::Remove(m_strWd+"\\temp.dat");
		}
	}
	catch(CFileException){

	}
}

void CSettings::Load(CString strFile)
{

	CIni ini;
	if((strFile.Find(":", 0) < 0) && (strFile.Find("/", 0) < 0)){

		TCHAR szBuffer[_MAX_PATH]; 
		::GetModuleFileName(AfxGetInstanceHandle(), szBuffer, _MAX_PATH);
		m_strWd.Format("%s", szBuffer);
		m_strWd = m_strWd.Left(m_strWd.ReverseFind('\\'));
		strFile.Insert(0, m_strWd +  "\\");
		ini.SetIniFileName(strFile);
	}
	else{

		ini.SetIniFileName(strFile);
	}

	m_strTopic.LoadString(IDS_DEFAULT_TOPIC);
	m_strMotd.LoadString(IDS_DEFAULT_MOTD);
	m_strKeyMsg.LoadString(IDS_DEFAULT_KEYMSG);

	m_dwIP			= ini.GetValue("Server", "IP", 0x0100007F);
	m_wPort			= ini.GetValue("Server", "Port", 16699);
	m_strTopic		= ini.GetValue("Server", "Topic", m_strTopic);
	m_strMotd		= ini.GetValue("Server", "Motd", m_strMotd);
	m_strKeyMsg		= ini.GetValue("Server", "KeyMsg", m_strKeyMsg);
	m_strGodName	= ini.GetValue("Server", "GodName", m_strGodName);
	m_dwGodFiles	= ini.GetValue("Server", "GodFiles", 0);
	m_wGodLine		= ini.GetValue("Server", "GodLine", 0);
	m_bGodVisible	= ini.GetValue("Server", "GodVisible", 1);
	m_dwLimit		= ini.GetValue("Server", "Limit", 80);
	m_bMultiIPOk	= ini.GetValue("Server", "MultiIPOk", TRUE);
	m_bBLockNushi	= ini.GetValue("Server", "BlockNushi", FALSE);
	m_bLocalIsOp	= ini.GetValue("Server", "LocalIsOp", FALSE);
	m_bListed		= ini.GetValue("Server", "Listed", TRUE);
	m_strWelcome	= ini.GetValue("Server", "Welcome", "#cx#Welcome to #cx#%ROOMNAME%#cx#, #cx#%NAME%.");
	m_strRoom		= ini.GetValue("Server", "Roomname", "roboServe hosted room");
	m_strKeyword	= ini.GetValue("Server", "Keyword", "");
	m_nColor		= ini.GetValue("Server", "EnableColors", FALSE);
	m_strExile		= ini.GetValue("Server", "Exile", "");
	m_uMode		    = ini.GetValue("Server", "ChannelModes", CM_NORMAL|CM_MODERATED);
	m_uOpenSub      = ini.GetValue("Server", "SubCreatePermission", UM_HOST);
	m_wLimitSub     = ini.GetValue("Server", "SubLimit", 0);
	m_bAutoCloseSub = ini.GetValue("Server", "AutoCloseSub", FALSE);
	m_nStyle		= ini.GetValue("UI", "Style", 1);
	m_bDisplaySubText = ini.GetValue("UI", "DisplaySub", TRUE);
	m_uDefMode		= ini.GetValue("Server", "DefaultUserMode", UM_NORMAL|UM_VOICED);
	m_bAdvertise    = ini.GetValue("Server", "AdvertiseRose", TRUE);
	m_bFirstIsSuper = ini.GetValue("Server", "SubFirstIsSuper", FALSE);
	m_uAdminWatch	= ini.GetValue("Server", "AdminWatch", UM_NORMAL);
	m_uVoiceWatch	= ini.GetValue("Server", "VoiceWatch", UM_NORMAL);
	m_bSubListed    = ini.GetValue("Server", "SubListed", FALSE);
	m_nNormalIcon   = ini.GetValue("Server", "NormalIcon", 0);
	m_nVoiceIcon	= ini.GetValue("Server", "VoiceIcon", 0);
	m_nAdminIcon	= ini.GetValue("Server", "AdminIcon", 2);
	m_nAgentIcon	= ini.GetValue("Server", "AgentIcon", 1);
	LoadLogins(strFile);
}

void CSettings::Save(CString strFile)
{

	ASSERT(!m_strWd.IsEmpty());

	CIni ini;

	if((strFile.Find(":", 0) > 0) || (strFile.Find("/", 0) >= 0)){

		ini.SetIniFileName(strFile);
	}
	else{

		strFile.Insert(0, m_strWd + "\\");
		ini.SetIniFileName(strFile);
	}

	m_strMotd.Replace("\n", "\\n");
	m_strKeyMsg.Replace("\n", "\\n");

	ini.SetValue("Server", "IP", m_dwIP);
	ini.SetValue("Server", "Port", m_wPort);
	ini.SetValue("Server", "Topic", m_strTopic);
	ini.SetValue("Server", "Motd", m_strMotd);
	ini.SetValue("Server", "KeyMsg", m_strKeyMsg);
	ini.SetValue("Server", "GodName", m_strGodName);
	ini.SetValue("Server", "GodFiles", m_dwGodFiles);
	ini.SetValue("Server", "GodLine", m_wGodLine);
	ini.SetValue("Server", "GodVisible", m_bGodVisible);
	ini.SetValue("Server", "Limit", m_dwLimit);
	ini.SetValue("Server", "MultiIPOk", m_bMultiIPOk);
	ini.SetValue("Server", "BlockNushi", m_bBLockNushi);
	ini.SetValue("Server", "LocalIsOp", m_bLocalIsOp);
	ini.SetValue("Server", "Listed", m_bListed);
	ini.SetValue("Server", "Welcome", m_strWelcome);
	ini.SetValue("Server", "Roomname", m_strRoom);
	ini.SetValue("Server", "Keyword", m_strKeyword);
	ini.SetValue("Server", "EnableColors", m_nColor);
	ini.SetValue("Server", "ChannelModes", (DWORD)m_uMode);
	ini.SetValue("Server", "Exile", m_strExile);
	ini.SetValue("Server", "SubCreatePermission", (DWORD)m_uOpenSub);
	ini.SetValue("Server", "SubLimit", m_wLimitSub);
	ini.SetValue("Server", "AutoCloseSub", m_bAutoCloseSub);
	ini.SetValue("Server", "DefaultUserMode", (DWORD)m_uDefMode);
	ini.SetValue("Server", "AdvertiseRose", m_bAdvertise);
	ini.SetValue("Server", "SubFirstIsSuper", m_bFirstIsSuper);
	ini.SetValue("Server", "SubListed", m_bSubListed);
	ini.SetValue("Server", "AdminWatch", (DWORD)m_uAdminWatch);
	ini.SetValue("Server", "VoiceWatch", (DWORD)m_uVoiceWatch);
	ini.SetValue("Server", "NormalIcon", m_nNormalIcon);
	ini.SetValue("Server", "VoiceIcon", m_nVoiceIcon);
	ini.SetValue("Server", "AdminIcon", m_nAdminIcon);
	ini.SetValue("Server", "AgentIcon", m_nAgentIcon);
	ini.SetValue("UI", "Style", m_nStyle);
	ini.SetValue("UI", "DisplaySub", m_bDisplaySubText);

	SaveLogins(strFile);
}

CString CSettings::GetWorkingDir(BOOL bCached)
{


	if(bCached){

		return m_strWd;
	}

	TCHAR  pszAppPath[ MAX_PATH + 1 ];
	TCHAR* pszSearch = NULL;

	memset( pszAppPath, 0, sizeof( pszAppPath ) );

	GetModuleFileName( NULL, pszAppPath, sizeof( pszAppPath ) );

	// find last \ character
	pszSearch = _tcsrchr( pszAppPath, _T( '\\' ) );

	if ( pszSearch )
	{
		// strip everything after the last \ char, \ char including 
		pszSearch[ 0 ] = '\0';
	}

	return pszAppPath;
}

void CSettings::LoadLogins(CString strFile)
{

	m_aLogins.RemoveAll();

	CIni ini;
	ini.SetIniFileName(strFile);
	
	CString strTmp, strPass;
	UINT uMode = 0;
	int nCount = ini.GetValue("Logins", "Count", 0);

	for(int i = 0; i < nCount; i++){

		strTmp.Format("Password_%03d", i);
		strPass = ini.GetValue("Logins", strTmp, "");

		if(strPass.IsEmpty()) continue;

		strTmp.Format("Usermode_%03d", i);
		uMode = ini.GetValue("Logins", strTmp, (DWORD)0);

		if(uMode == 0) continue;

		m_aLogins.SetAt(strPass, uMode);
	}
	
}

void CSettings::SaveLogins(CString strFile)
{

	CIni ini;
	ini.SetIniFileName(strFile);
	
	CString strTmp, strPass;
	UINT uMode = 0;

	int nCount = m_aLogins.GetCount();
	ini.SetValue("Logins", "Count", nCount);

	if(nCount <= 0) return;

	POSITION pos = m_aLogins.GetStartPosition();
	nCount = 0;
	while(pos){
		

		m_aLogins.GetNextAssoc(pos, strPass, uMode);

		strTmp.Format("Password_%03d", nCount);
		ini.SetValue("Logins", strTmp, strPass);
		strTmp.Format("Usermode_%03d", nCount++);
		ini.SetValue("Logins", strTmp, (DWORD)uMode);
	}
}

void CSettings::AddLogin(CString strPass, UINT uMode)
{

	m_aLogins.SetAt(strPass, uMode);
}

void CSettings::DeleteLogin(CString strPass)
{

	m_aLogins.RemoveKey(strPass);
}

UINT CSettings::GetLogin(CString strPass)
{

	UINT uMode = 0;
	if(!m_aLogins.Lookup(strPass, uMode)){

		uMode = 0;
	}
	return uMode;
}

void CSettings::IterateLogins(POSITION &pos, CString &strPass, UINT &uMode)
{

	m_aLogins.GetNextAssoc(pos, strPass, uMode);
}

POSITION CSettings::GetLoginIterator()
{

	return m_aLogins.GetStartPosition();
}

void CSettings::LoadFromServer(CString strURL)
{

	CStdioFile			localTmp;
	CInternetSession	is;
	CString				strTmp;
	
	try{

		CHttpFile* pFile = (CHttpFile*) is.OpenURL(strURL);
		pFile->Close();
		delete pFile;
		pFile = 0;

		pFile = (CHttpFile*) is.OpenURL(strURL);
		
		if(!localTmp.Open(GetWorkingDir(FALSE) + "\\temp.dat", CFile::typeText|CFile::modeWrite|CFile::modeCreate)) return;
		
		if(pFile == NULL) return;

		while(pFile->ReadString(strTmp)){

			localTmp.WriteString(strTmp+"\n");
		}

		pFile->Close();
		delete pFile;
		is.Close();
	}
	catch(CException* pEx){
		
		TCHAR   szCause[255];
		CString strFormatted;

		pEx->GetErrorMessage(szCause, 255);
		strFormatted.Format("Error during Update Query: %s\n", szCause);
		TRACE(strFormatted);
		return;
	}
	
	Load("temp.dat");
}
