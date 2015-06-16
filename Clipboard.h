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


#ifndef __CCLIPBOARD_H
#define __CCLIPBOARD_H

class CClipboard
{
public:
	static CString GetText(HWND hWnd = NULL);
	static BOOL GetText (LPSTR lpszBuffer,
						 int nBufSize,
						 HWND hWnd = NULL);
	
	static BOOL GetTextLength (unsigned long *pnSize,
							   HWND hWnd = NULL);
	
	static BOOL SetText (LPSTR lpszBuffer,
						 HWND hWND = NULL);
};

#endif