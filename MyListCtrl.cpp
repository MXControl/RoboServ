// MyListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "roboServ.h"
#include "MyListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl

CMyListCtrl::CMyListCtrl()
{
}

CMyListCtrl::~CMyListCtrl()
{
}


BEGIN_MESSAGE_MAP(CMyListCtrl, CListCtrl)
	//{{AFX_MSG_MAP(CMyListCtrl)
	ON_WM_CONTEXTMENU()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMyListCtrl message handlers


void CMyListCtrl::OnContextMenu(CWnd* pWnd, CPoint point) 
{

#ifndef _DEBUG
	int n = -1;
	if((n = GetNextItem(-1, LVNI_SELECTED)) >= 0){
#endif		
	
		CExtPopupMenuWnd* mPopupWnd = new CExtPopupMenuWnd;

		VERIFY(
			mPopupWnd->LoadMenu(
				AfxGetMainWnd()->m_hWnd,
				IDR_USERLIST
			)
		);

		VERIFY( mPopupWnd->TrackPopupMenu(0, point.x, point.y ) );
#ifndef _DEBUG
	}	
#endif
}
