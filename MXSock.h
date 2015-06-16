/****************************************************************/
/*																*/
/*  MXSock.h													*/
/*																*/
/*  Declaration of WinMX Crypt API								*/
/*																*/
/*  Programmed by [Nushi]@[2Sen]								*/
/*  [Nushi]@[2Sen]	http://2sen.dip.jp							*/
/*	Crypt Table reconstructed by Bender		                	*/
/*																*/
/****************************************************************/

#ifndef _MX_SOCK_H__INCLUDED
#define _MX_SOCK_H__INCLUDED

#pragma once


void DecryptFrontCode(const BYTE *pSrc, BYTE *pDst);
void CreateCryptKeyID(const WORD wID, BYTE *pBlock);
const WORD GetCryptKeyID(const BYTE *pBlock);
const WORD GetCryptKey(const BYTE *pBlock, DWORD *pUpKey, DWORD *pDwKey);
const DWORD DecryptMXTCP(BYTE *pBuf, const int iLen, const DWORD dwKey);
const DWORD EncryptMXTCP(BYTE *pBuf, const int iLen, const DWORD dwKey);
void DecryptMXUDP(BYTE *pBuf, const int iLen);
void EncryptMXUDP(BYTE *pBuf, const int iLen);
const BOOL GetFileHash(LPCTSTR lpszFileName, DWORD *pHash, DWORD *pFileLen);

#endif // _MX_SOCK_H__INCLUDED