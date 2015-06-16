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

#include "StdAfx.h"
#include "Tokenizer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CTokenizer::CTokenizer(const CString& str, const CString& strDelim):
	m_str(str),
	m_nCurPos(0)
{
	SetDelimiters(strDelim);
}

void CTokenizer::SetDelimiters(const CString& strDelim)
{
	for(int i = 0; i < strDelim.GetLength(); ++i){

		m_bsDelim.set(static_cast<BYTE>(strDelim[i]));
	}
}

BOOL CTokenizer::Next(CString& str)
{

	str.Empty();

	// get the first non-delimiter character
	while((m_nCurPos < m_str.GetLength()) && m_bsDelim[static_cast<BYTE>(m_str[m_nCurPos])]){

		++m_nCurPos;
	}

	if(m_nCurPos >= m_str.GetLength()){

		// out of scope, this is no element to return
		return FALSE;
	}

	int nStartPos = m_nCurPos; // this is the first non-delimiter charactrer 
	
	// get string until next delimiter or end of string
	while((m_nCurPos < m_str.GetLength()) && !m_bsDelim[static_cast<BYTE>(m_str[m_nCurPos])]){

		++m_nCurPos;
	}
	
	str = m_str.Mid(nStartPos, m_nCurPos - nStartPos); // copy string from start position to end ;)

	return TRUE;
}

BOOL CTokenizer::Next(CString& str, BOOL& bDelim)
{

	str.Empty();

	bDelim	 = FALSE;
	int nOld = m_nCurPos;
	int nCut = m_nCurPos;
	// get the first non-delimiter character
	while((m_nCurPos < m_str.GetLength()) && m_bsDelim[static_cast<BYTE>(m_str[m_nCurPos])]){

		++m_nCurPos;
	}

	if(m_nCurPos >= m_str.GetLength()){

		// out of scope, there is no element to return
		return FALSE;
	}

	int nStartPos = m_nCurPos; // this is the first non-delimiter charactrer 
	
	bDelim = nStartPos != nOld;
	if(bDelim) nCut = nOld+1;
	// get string until next delimiter or end of string
	while((m_nCurPos < m_str.GetLength()) && !m_bsDelim[static_cast<BYTE>(m_str[m_nCurPos])]){

		++m_nCurPos;
	}
	
	str = m_str.Mid(nCut, m_nCurPos - nCut); // copy string from start position to end ;)

	return TRUE;
}

CString	CTokenizer::Tail() const
{
	int nCurPos = m_nCurPos;

	while((nCurPos < m_str.GetLength()) && m_bsDelim[static_cast<BYTE>(m_str[nCurPos])]){

		++nCurPos;
	}

	CString strResult;

	if(nCurPos < m_str.GetLength()){

		strResult = m_str.Mid(nCurPos);
	}

	return strResult;
}
