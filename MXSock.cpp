/****************************************************************/
/*																*/
/*  MXSock.c													*/
/*																*/
/*  Implementation of WinMX Crypt API							*/
/*																*/
/*  Programmed by [Nushi]@[2Sen]								*/
/*  [Nushi]@[2Sen]	http://2sen.dip.jp							*/
/*	Special thanks to 'Krishean'								*/
/*																*/
/****************************************************************/

#include "stdafx.h"
#include "MXSock.h"

#include <stdio.h>
#include "MXTables.h"


void DecryptFrontCode(const BYTE *pSrc, BYTE *pDst)
{
	BYTE Work[12];
	BYTE Crypt[12];
	int i, j;

	memcpy(Crypt, pSrc, 12);
	memcpy(pDst, pSrc + 12, 120);

	for(i=0 ; i < 10 ; i++)
	{
		memcpy(Work, pSrc + 12 + i * 12, 12);
		pDst[i * 12 + 0] -= Crypt[0];
		pDst[i * 12 + 1] -= Crypt[1];
		pDst[i * 12 + 2] ^= Crypt[2];
		pDst[i * 12 + 3] -= Crypt[3];
		pDst[i * 12 + 4] ^= Crypt[4];
		pDst[i * 12 + 5] ^= Crypt[5];
		pDst[i * 12 + 6] ^= Crypt[6];
		pDst[i * 12 + 7] -= Crypt[7];
		pDst[i * 12 + 8] ^= Crypt[8];
		pDst[i * 12 + 9] -= Crypt[9];
		pDst[i * 12 + 10] -= Crypt[10];
		pDst[i * 12 + 11] ^= Crypt[11];

		for (j=0 ; j < 12 ; j++) Crypt[j] = FCTable[Work[j] * 12 + j];
	}
}

void CreateCryptKeyID(const WORD wID, BYTE *pBlock)
{
	int i;

	for(i=0 ; i < 16 ; i++)pBlock[i] = rand() % 255;

	pBlock[4] = pBlock[15] ^ pBlock[13] ^ pBlock[5] ^ pBlock[2];
	pBlock[9] = (pBlock[14] ^ pBlock[8] ^ pBlock[1]) + wID;
	pBlock[11] = (pBlock[12] ^ pBlock[10] ^ pBlock[7] ^ pBlock[6] ^ pBlock[3] ^ pBlock[0]) + (wID >> 7);

	for(i=0 ; i < 105 ; i++)
	{
		BYTE K = (i)? pBlock[(i - 1) & 0x0F] : 0x57;
		if(i % 5)pBlock[i & 0x0F] += K;
		else pBlock[i & 0x0F] ^= K;
	}
}

const WORD GetCryptKeyID(const BYTE *pBlock)
{
	BYTE kb[16];
	int i;

	memcpy(kb, pBlock, 16);

	for(i=104 ; i >= 0 ; i--)
	{
		BYTE b = (i)? kb[(i - 1) & 0x0F] : 0x57;
		if(i % 5)kb[i & 0x0F] -= b;
		else kb[i & 0x0F] ^= b;
	}

	BYTE KeyID = (BYTE)(kb[9] - (kb[14] ^ kb[8] ^ kb[1]));
	if(kb[4] != (kb[15] ^ kb[13] ^ kb[5] ^ kb[2]) || (kb[12] ^ kb[10] ^ kb[7] ^ kb[6] ^ kb[3] ^ kb[0]) != (BYTE)(kb[11]-(KeyID >> 7))) return 0xFFFF;

	return KeyID;
}

inline const BYTE Decrypt1(const BYTE s, BYTE* ckey)
{
	register BYTE r;

	switch(ckey[2] & 0x03)
	{
	case 0:
		r = MXTCPTable_2[ckey[1] ^ s];
		break;
	case 1:
		r = MXTCPTable_6[ckey[1] ^ s];
		break;
	case 2:
		r = ckey[0] ^ MXTCPTable_0[s];
		break;
	case 3:
		r = ckey[3] ^ MXTCPTable_6[s];
		break;
	}

	ckey[0] += ckey[1];
	ckey[1] ^= ckey[3];
	ckey[2] += MXTCPTable_4[s];
	ckey[3] ^= ckey[2];
	ckey[1]++;
	ckey[2]++;
	ckey[3]++;
	return r;
}

inline const BYTE Encrypt1(const BYTE s, BYTE* ckey)
{
	register BYTE r;

	switch(ckey[2] & 0x03)
	{
	case 0:
		r = ckey[1] ^ MXTCPTable_3[s];
		break;
	case 1:
		r = ckey[1] ^ MXTCPTable_7[s];
		break;
	case 2:
		r = MXTCPTable_1[ckey[0] ^ s];
		break;
	case 3:
		r = MXTCPTable_7[ckey[3] ^ s];
		break;
	}

	ckey[0] += ckey[1];
	ckey[1] ^= ckey[3];
	ckey[2] += MXTCPTable_4[r];
	ckey[3] ^= ckey[2];
	ckey[1]++;
	ckey[2]++;
	ckey[3]++;
	return r;
}

const DWORD DecryptMXTCP(BYTE *pBuf, const int iLen, const DWORD dwKey)
{
	register DWORD dwRef = dwKey;
	int i;

	for(i=0 ; i < iLen ; i++) pBuf[i] = Decrypt1(pBuf[i], (BYTE *)&dwRef);

	return dwRef;
}

const DWORD EncryptMXTCP(BYTE *pBuf, const int iLen, const DWORD dwKey)
{
	register DWORD dwRef = dwKey;
	int i;

	for(i=0 ; i < iLen ; i++) pBuf[i] = Encrypt1(pBuf[i], (BYTE *)&dwRef);

	return dwRef;
}

void EncryptMXUDP(BYTE *pBuf, const int iLen)
{
	DWORD dwMax = (DWORD)iLen + ((DWORD)iLen << 4);
	if(!dwMax) return;
	DWORD dwCnt = 1;

	while(dwCnt < dwMax)
	{
		pBuf[dwCnt % (DWORD)iLen] = MXUDPTable_3[pBuf[dwCnt % (DWORD)iLen]] ^ MXUDPTable_1[MXUDPTable_0[(dwCnt - 1) & 0xFF] ^ pBuf[(dwCnt - 1) % (DWORD)iLen]];
		dwCnt++;
	}
}

void DecryptMXUDP(BYTE *pBuf, const int iLen)
{
	if(iLen > 5)
	{
		DWORD dwCnt = (DWORD)iLen + ((DWORD)iLen << 4);
		while(dwCnt-- > 1)pBuf[dwCnt % (DWORD)iLen] = MXUDPTable_2[MXUDPTable_1[pBuf[(dwCnt - 1) % (DWORD)iLen] ^ MXUDPTable_0[(dwCnt - 1) & 0xFF]] ^ pBuf[dwCnt % (DWORD)iLen]];
	}
}

const WORD GetCryptKey(const BYTE *pBlock, DWORD *pUpKey, DWORD *pDwKey)
{
	const WORD wKeyID = GetCryptKeyID(pBlock);
	BOOL bUPlus, bDPlus;
	BYTE bUK0, bUK1, bUK2, bUK3, bUD;
	BYTE bDK0, bDK1, bDK2, bDK3, bDD;
	DWORD dwUPKey = 0;
	DWORD dwDWKey = 0;

	switch(wKeyID)
	{
	case 0x0050 :
	case 0x0051 :
		bUK0 = 2; bUK1 = 5; bUK2 = 9; bUK3 = 11; bUD = 0x68; bUPlus = FALSE; // SV PR
		bDK0 = 4; bDK1 = 12; bDK2 = 10; bDK3 = 7; bDD = 0x67; bDPlus = FALSE; // SV PR
		break;

	case 0x0052 :
	case 0x0053 :
		bUK0 = 7; bUK1 = 3; bUK2 = 9; bUK3 = 5; bUD = 0x54; bUPlus = TRUE; // SV SE
		bDK0 = 2; bDK1 = 8; bDK2 = 13; bDK3 = 6; bDD = 0x55; bDPlus = TRUE; // SV SE
		break;

	case 0x0057 :
	case 0x0058 :
		bUK0 = 7; bUK1 = 3; bUK2 = 9; bUK3 = 5; bUD = 0x22; bUPlus = FALSE; // SV CH
		bDK0 = 2; bDK1 = 8; bDK2 = 13; bDK3 = 6; bDD = 0x7A; bDPlus = TRUE; // SV CH
		break;

	default :
		return wKeyID;
	}

	dwUPKey = pBlock[bUK3]; dwUPKey <<= 8;
	dwUPKey |= pBlock[bUK2]; dwUPKey <<= 8;
	dwUPKey |= pBlock[bUK1]; dwUPKey <<= 8;
	dwUPKey |= (BYTE)((bUPlus)? (pBlock[bUK0] + bUD) : (pBlock[bUK0] - bUD));

	dwDWKey = pBlock[bDK3]; dwDWKey <<= 8;
	dwDWKey |= pBlock[bDK2]; dwDWKey <<= 8;
	dwDWKey |= pBlock[bDK1]; dwDWKey <<= 8;
	dwDWKey |= (BYTE)((bDPlus)? (pBlock[bDK0] + bDD) : (pBlock[bDK0] - bDD));

	if(wKeyID == 0x0051 || wKeyID == 0x0053 || wKeyID == 0x0058)
	{
		*pUpKey = dwDWKey;
		*pDwKey = dwUPKey;
	} else {
		*pUpKey = dwUPKey;
		*pDwKey = dwDWKey;
	}

	return wKeyID;
}

inline void MXHashSub(BYTE *pBuf, const DWORD dwLen, BYTE *pHash)
{
	BYTE b, n;
	BYTE *bp;

	if(dwLen <= 16) return;

	b = 0xC9;
	n = 0;
	bp = pBuf;

	while(bp < pBuf + dwLen - 16)
	{
		pHash[0] += bp[0];
		pHash[1] += bp[1];
		pHash[2] += bp[2];
		pHash[3] += bp[3];
		pHash[4] += bp[4];
		pHash[5] += bp[5];
		pHash[6] += bp[6];
		pHash[7] += bp[7];
		pHash[8] += bp[8];
		pHash[9] += bp[9];
		pHash[10] += bp[10];
		pHash[11] += bp[11];
		pHash[12] += bp[12];
		pHash[13] += bp[13];
		pHash[14] += bp[14];
		pHash[15] += bp[15];

		pHash[0] ^= b;
		pHash[1] ^= pHash[0];
		pHash[2] ^= pHash[1];
		pHash[3] ^= pHash[2];
		pHash[4] ^= pHash[3];
		pHash[5] ^= pHash[4];
		pHash[6] ^= pHash[5];
		pHash[7] ^= pHash[6];
		pHash[8] ^= pHash[7];
		pHash[9] ^= pHash[8];
		pHash[10] ^= pHash[9];
		pHash[11] ^= pHash[10];
		pHash[12] ^= pHash[11];
		pHash[13] ^= pHash[12];
		pHash[14] ^= pHash[13];
		pHash[15] ^= pHash[14];

		b = n + pHash[15];
		b = (b << 1) + ( b >> 7);
		bp += 0x10;
		n++;
	}
}

const BOOL GetFileHash(LPCTSTR lpszFileName, DWORD *pHash, DWORD *pFileLen)
{
	FILE *in;
	DWORD len;
	DWORD blockskip;

	if((in = fopen(lpszFileName, "rb")) == NULL) return FALSE;

	BYTE buf[0x20000];

	fseek(in, 0 , SEEK_END);

	*pFileLen = ftell(in);

	fseek(in, 0, SEEK_SET);

	blockskip = *pFileLen / 0x20000 / 10;
	if(blockskip < 5) blockskip = 5;

	pHash[0] = pHash[1] = pHash[2] = pHash[3] = 0;

	len = fread(buf, 1, 0x20000, in);

	while(1)
	{
		MXHashSub(buf, len, (BYTE *)pHash);
		if(len < 0x20000) break;

		fseek(in, 0x20000 * (blockskip - 1), SEEK_CUR);

		len = fread(buf, 1, 0x20000, in);
	}

	fclose(in);

	return TRUE;
}

