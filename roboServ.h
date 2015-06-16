// roboServ.h : main header file for the roboServ application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// CroboServApp:
// See roboServ.cpp for the implementation of this class
//

class CroboServApp : public CWinApp
{
public:
	CroboServApp();
	int m_nWSA;
	HINSTANCE	m_hInstLang;
// Overrides
public:
	virtual BOOL InitInstance();

// Implementation
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CroboServApp theApp;