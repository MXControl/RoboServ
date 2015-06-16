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

#ifndef _WINAMP_H_INCLUDED
#define _WINAMP_H_INCLUDED


// buttons
#define WINAMP_BUTTON1          40044  // play
#define WINAMP_BUTTON2          40045  // pause
#define WINAMP_BUTTON3          40046  // prev
#define WINAMP_BUTTON4          40047  // stop
#define WINAMP_BUTTON5          40048  // next

#define WM_WA_IPC				WM_USER

#define IPC_GETVERSION			0
#define IPC_ISPLAYING			104
#define IPC_GETOUTPUTTIME		105
#define IPC_GETINFO				126
#define IPC_UPDTITLE			243

#define IPC_IS_PLAYING_VIDEO	501 // returns >1 if playing, 0 if not, 1 if old version (so who knows):)

#endif // _WINAMP_H_INCLUDED