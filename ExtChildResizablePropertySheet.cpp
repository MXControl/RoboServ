// ExtChildResizablePropertySheet.cpp : implementation file
//

#include "stdafx.h"

#include "roboServ.h"
#include "MainFrm.h"
#include "ExtChildResizablePropertySheet.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtChildResizablePropertySheet
extern UINT UWM_SETTINGS;

IMPLEMENT_DYNCREATE(CExtChildResizablePropertySheet, CExtResizablePropertySheet)

BEGIN_MESSAGE_MAP(CExtChildResizablePropertySheet, CExtResizablePropertySheet)
	//{{AFX_MSG_MAP(CExtChildResizablePropertySheet)
	ON_REGISTERED_MESSAGE(UWM_SETTINGS, OnLoadSettings)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CExtChildResizablePropertySheet construction/destruction

CExtChildResizablePropertySheet::CExtChildResizablePropertySheet()
	: m_bInitTabSingleLine( false )
	, m_bInitTabButtons( false )
	, m_bInitTabButtonsFlat( false )
	, m_bInitTabSeparatorsFlat( false )
	, m_bTabStylesInitialized( false )
{
	// TODO: add member initialization code here
	m_bShowResizingGripper = false;
}

CExtChildResizablePropertySheet::~CExtChildResizablePropertySheet()
{
}

BOOL CExtChildResizablePropertySheet::Create( CWnd * pWndParent )
{
	if( !CExtResizablePropertySheet::Create(pWndParent,WS_CHILD|WS_VISIBLE) )
	{
		ASSERT( FALSE );
		return FALSE;
	}
	ModifyStyle(
		WS_THICKFRAME|WS_BORDER|WS_CAPTION|WS_SYSMENU,
		0,
		SWP_FRAMECHANGED
		);
	ModifyStyleEx(
		WS_EX_DLGMODALFRAME|WS_EX_STATICEDGE|WS_EX_CLIENTEDGE,
		0,
		SWP_FRAMECHANGED
		);
	
	return TRUE;
}

BOOL CExtChildResizablePropertySheet::PreTranslateMessage(MSG* pMsg)
{
CMainFrame * pMainFrame =
		STATIC_DOWNCAST( CMainFrame, AfxGetMainWnd() );
	if( pMainFrame->m_wndMenuBar.TranslateMainFrameMessage(pMsg) )
		return TRUE;
	return CExtResizablePropertySheet::PreTranslateMessage(pMsg);
}

void CExtChildResizablePropertySheet::_TrickSyncActviveChild()
{
CPropertyPage * pWnd = CPropertySheet::GetActivePage();
CTabCtrl * pTabCtrl = CPropertySheet::GetTabControl();
	if(		pWnd != NULL
		&&	pWnd->GetSafeHwnd() != NULL
		&&	::IsWindow( pWnd->GetSafeHwnd() )
		&&	pTabCtrl != NULL
		&&	pTabCtrl->GetSafeHwnd() != NULL
		&&	::IsWindow( pTabCtrl->GetSafeHwnd() )
		)
	{

		if( !m_bTabStylesInitialized )
		{
			m_bTabStylesInitialized = true;
			DWORD dwStylesRemove = 0L;
			DWORD dwStylesAdd = 0L;
			if( m_bInitTabSingleLine )
			{
				dwStylesRemove |= TCS_MULTILINE;
				dwStylesAdd |= TCS_SINGLELINE;
			}
			if( m_bInitTabButtons )
			{
				dwStylesAdd |= TCS_BUTTONS;
				if( m_bInitTabButtonsFlat )
					dwStylesAdd |= TCS_FLATBUTTONS;
			}
			if( dwStylesRemove != 0 || dwStylesAdd != 0 )
				pTabCtrl->ModifyStyle(
					dwStylesRemove,
					dwStylesAdd,
					SWP_FRAMECHANGED
					);
			if( m_bInitTabButtons && m_bInitTabButtonsFlat && m_bInitTabSeparatorsFlat )
				pTabCtrl->SetExtendedStyle( TCS_EX_FLATSEPARATORS, TCS_EX_FLATSEPARATORS );
		}
		
		RECT lpRect;
		GetClientRect(&lpRect);
		// dirty fix of the access vialation bug of
		// the tab control with single item when item's
		// width/height is same as tab control width/height
		__try
		{
			pTabCtrl->AdjustRect(FALSE, &lpRect);
		}
		__except( EXCEPTION_EXECUTE_HANDLER )
		{
			lpRect.right ++;
			lpRect.bottom ++;
			__try
			{
				pTabCtrl->AdjustRect(FALSE, &lpRect);
			}
			__except( EXCEPTION_EXECUTE_HANDLER )
			{
			}
		}
		pWnd->MoveWindow(&lpRect, TRUE);   
	}
}		


LRESULT CExtChildResizablePropertySheet::OnLoadSettings(WPARAM wParam, LPARAM lParam)
{

	for(int i = 0; i < GetPageCount(); i++){

		CPropertyPage* page = GetPage(i);
		
		TRACE("Testing page %d\n", i);

		if(!page) continue;
		if(!::IsWindow(page->m_hWnd)) continue;
		
		TRACE("Page %d is ok\n", i);

		page->SendMessage(UWM_SETTINGS, wParam, lParam);
	}

	return 1;
}
