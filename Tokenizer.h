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

#if !defined(__TOKENIZER_H__)
#define __TOKENIZER_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if !defined(_BITSET_)
#	include <bitset>
#endif // !defined(_BITSET_)

class CTokenizer
{
public:
	CTokenizer(const CString& str, const CString& strDelim);
	void SetDelimiters(const CString& strDelim);

	BOOL Next(CString& str);
	BOOL Next(CString& str, BOOL& bDelim);
	CString	Tail() const;

private:
	CString m_str;
	std::bitset<256> m_bsDelim;
	int m_nCurPos;
};

#endif  // !defined(__TOKENIZER_H__)
