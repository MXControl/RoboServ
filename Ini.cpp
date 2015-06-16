/*
** Copyright (C) 2002-2004 Thees Schwab
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


#include "StdAfx.h"
#include "Ini.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIni::CIni(  )
{

}


CIni::~CIni()
{
}



// **********************************************************************************

BOOL CIni::SetValue(const CString& strSection, const CString& strItem, const CString& strVal)
{
	ASSERT( !m_strFileName.IsEmpty() );

	return WritePrivateProfileString( strSection, strItem, strVal, m_strFileName );
}

BOOL CIni::SetValue(const CString& strSection, const CString& strItem, const int iVal)
{
	CString strEntry;
	strEntry.Format( _T( "%d" ), iVal );
	return SetValue( strSection, strItem, strEntry );
}

BOOL CIni::SetValue(const CString& strSection, const CString& strItem, const long lVal)
{
	return SetValue( strSection, strItem, (int) lVal );
}

BOOL CIni::SetValue(const CString& strSection, const CString& strItem, const DWORD dwVal)
{
	return SetValue( strSection, strItem, (int)dwVal );
}



CString CIni::GetValue( const CString& strSection, const CString& strItem, CString strDefault )
{
	ASSERT( !m_strFileName.IsEmpty() );

	CHAR lpszValue[ 1024 ] = {'\0',};

	::GetPrivateProfileString(	strSection,
								strItem,
								strDefault,
								lpszValue,
								sizeof( lpszValue),
								m_strFileName );

	return CString( lpszValue );
}

int CIni::GetValue( const CString& strSection, const CString& strItem, const int nDefault )
{
	int nReturn = nDefault;

	CString strReturn = GetValue( strSection, strItem, "" );

	if ( !strReturn.IsEmpty() )
	{
		nReturn = _ttoi( strReturn );
	}

	return nReturn;
}


LONG CIni::GetValue( const CString& strSection, const CString& strItem, const LONG nDefault )
{
	return (LONG) GetValue ( strSection, strItem, (int) nDefault );
}

DWORD CIni::GetValue( const CString& strSection, const CString& strItem, const DWORD nDefault )
{
	return (DWORD) GetValue ( strSection, strItem, (int) nDefault );
}



