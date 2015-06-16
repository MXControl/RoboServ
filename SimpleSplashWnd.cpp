// SimpleSplashWnd.cpp : implementation file
//

#include "stdafx.h"
#include "roboServ.h"
#include "SimpleSplashWnd.h"
#include "util.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static bool g_bSplashWndClassRegistered = false;

CSimpleSplashWnd::CSimpleSplashWnd()
	: m_sStatusText( _T("Initializing ...") )
{
	VERIFY( RegisterSplashWndClass() );
}

CSimpleSplashWnd::CSimpleSplashWnd(
	CWnd * pWndParent,
	UINT nBitmapID
	)
	: m_sStatusText( _T("Initializing ...") )
{
	VERIFY( Create( pWndParent, nBitmapID ) );
}

CSimpleSplashWnd::~CSimpleSplashWnd()
{
}


BEGIN_MESSAGE_MAP(CSimpleSplashWnd, CWnd)
	//{{AFX_MSG_MAP(CSimpleSplashWnd)
	ON_WM_ERASEBKGND()
	ON_WM_PAINT()
	ON_WM_NCCALCSIZE()
	ON_WM_NCHITTEST()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CSimpleSplashWnd message handlers

void CSimpleSplashWnd::SetStatusText(
	LPCTSTR sStatusText
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() != NULL );
	ASSERT( ::IsWindow( GetSafeHwnd() ) );
LPCTSTR sText = (sStatusText == NULL) ? _T("") : sStatusText;
	if( m_sStatusText == sText )
		return;
	m_sStatusText = sText;
	if( (GetStyle() & WS_VISIBLE) == 0 )
		return;
	Invalidate();
	UpdateWindow();
}

bool CSimpleSplashWnd::Create(
	CWnd * pWndParent,
	UINT nBitmapID
	)
{
	ASSERT_VALID( this );
	ASSERT( GetSafeHwnd() == NULL );
	ASSERT( m_bitmap.GetSafeHandle() == NULL );

	if( !RegisterSplashWndClass() )
	{
		ASSERT( FALSE );
		return false;
	}

	if( !m_bitmap.LoadBitmap(nBitmapID) )
	{
		ASSERT( FALSE );
		return false;
	}
BITMAP _bmpInfo;
	::memset( &_bmpInfo, 0, sizeof(BITMAP) );
	m_bitmap.GetBitmap( &_bmpInfo );
	m_sizeBitmap.cx = _bmpInfo.bmWidth;
	m_sizeBitmap.cy = _bmpInfo.bmHeight;

CRect rcDesktop;
	VERIFY(
		::SystemParametersInfo(
			SPI_GETWORKAREA,
			0,
			PVOID(&rcDesktop),
			0
			)
		);
CRect rcWnd( 0, 0, m_sizeBitmap.cx, m_sizeBitmap.cy );
	rcWnd.OffsetRect(
		( rcDesktop.Width() - m_sizeBitmap.cx ) / 2,
		( rcDesktop.Height() - m_sizeBitmap.cy ) / 2
		);
	if( !CWnd::CreateEx(
			0,
			SPLASHWND_WNDCLASS,
			_T("GLViews initizlizing ..."),
			WS_POPUP|WS_VISIBLE,
			rcWnd.left, rcWnd.top, m_sizeBitmap.cx, m_sizeBitmap.cy,
			pWndParent->GetSafeHwnd(),
			(HMENU)NULL
			)
		)
	{
		ASSERT( FALSE );
		m_bitmap.DeleteObject();
		return false;
	}

	VERIFY(
		::SetWindowPos(
			m_hWnd, HWND_TOP, 0, 0, 0, 0,
			SWP_NOSIZE|SWP_NOMOVE|SWP_SHOWWINDOW
			)
		);

	CExtPopupMenuWnd::PassMsgLoop( false );

	return true;
}

BOOL CSimpleSplashWnd::OnEraseBkgnd(CDC* pDC) 
{
	pDC;
	return TRUE;
}

void CSimpleSplashWnd::OnPaint() 
{
	ASSERT( m_bitmap.GetSafeHandle() != NULL );
CPaintDC dcPaint( this );
CDC dcMem;
	if( !dcMem.CreateCompatibleDC( &dcPaint ) )
	{
		ASSERT( FALSE );
		return;
	}
CBitmap * pOldBmp = dcMem.SelectObject( &m_bitmap );
	dcPaint.BitBlt(
		0, 0, m_sizeBitmap.cx, m_sizeBitmap.cy,
		&dcMem,
		0, 0,
		SRCCOPY
		);
	if( !m_sStatusText.IsEmpty() )
	{
		CRect rcText;
		GetClientRect( &rcText );
		rcText.bottom -= 6;
		rcText.top = rcText.bottom - 10;
		rcText.DeflateRect( 16, 0 );
		int nOldBkMode = dcPaint.SetBkMode( TRANSPARENT );
		COLORREF clrTextOld = dcPaint.SetTextColor( RGB(255,255,255) );
		CFont * pOldFont = dcPaint.SelectObject( &g_PaintManager->m_FontNormal );
		dcPaint.DrawText(
			m_sStatusText,
			&rcText,
			DT_SINGLELINE|DT_CENTER|DT_VCENTER|DT_END_ELLIPSIS  
			);
		dcPaint.SelectObject( pOldFont );
		dcPaint.SetTextColor( clrTextOld );
		dcPaint.SetBkMode( nOldBkMode );
	} // if( !m_sStatusText.IsEmpty() )
	dcMem.SelectObject( pOldBmp );
}

void CSimpleSplashWnd::OnNcCalcSize(BOOL bCalcValidRects, NCCALCSIZE_PARAMS FAR* lpncsp) 
{
	bCalcValidRects;
	lpncsp;
}

UINT CSimpleSplashWnd::OnNcHitTest(CPoint point) 
{
	point;
	return HTCLIENT;
}

void CSimpleSplashWnd::OnClose() 
{
}

BOOL CSimpleSplashWnd::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( ( !RegisterSplashWndClass() )
		|| ( !CWnd::PreCreateWindow(cs) )
		)
	{
		ASSERT( FALSE );
		return FALSE;
	}

	cs.lpszClass = SPLASHWND_WNDCLASS;

	return TRUE;
}

bool CSimpleSplashWnd::RegisterSplashWndClass()
{
	if( g_bSplashWndClassRegistered )
		return true;

WNDCLASS wndclass;
HINSTANCE hInst = AfxGetInstanceHandle();
	if( ! ::GetClassInfo(
			hInst,
			SPLASHWND_WNDCLASS,
			&wndclass
			)
		)
	{
		// otherwise we need to register a new class
		wndclass.style =
				CS_GLOBALCLASS
				//|CS_DBLCLKS
				|CS_HREDRAW|CS_VREDRAW
				|CS_NOCLOSE
				|CS_SAVEBITS
				;
		wndclass.lpfnWndProc = ::DefWindowProc;
		wndclass.cbClsExtra = wndclass.cbWndExtra = 0;
		wndclass.hInstance = hInst;
		wndclass.hIcon = NULL;
		wndclass.hCursor =
				::LoadCursor(
					NULL, //hInst,
					IDC_WAIT
					)
				;
		ASSERT( wndclass.hCursor != NULL );
		wndclass.hbrBackground = NULL; 
		wndclass.lpszMenuName = NULL;
		wndclass.lpszClassName = SPLASHWND_WNDCLASS;
		if( !::AfxRegisterClass( &wndclass ) )
		{
			ASSERT( FALSE );
			//AfxThrowResourceException();
			return false;
		}
	}

	g_bSplashWndClassRegistered = true;
	return true;
}

BOOL CSimpleSplashWnd::DestroyWindow() 
{
	ShowWindow( SW_HIDE );
	CExtPopupMenuWnd::PassMsgLoop( false );
	
	return CWnd::DestroyWindow();
}


/////////////////////////////////////////////////////////////////////////////
// CSimpleSplashWnd message handlers

void CSimpleSplashWnd::SetStatusText(UINT nID)
{

	SetStatusText(Util::LoadString(nID));
}
