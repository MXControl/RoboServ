// RichView.cpp : implementation of the CroboServView class
//

#include "stdafx.h"
#include "roboServ.h"

#include "roboServDoc.h"
#include "CntrItem.h"
#include "roboServView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CroboServView

extern UINT UWM_SETVIEW; //   = ::RegisterWindowMessage("UWM_SETVIEW-{F2FD4D42-15C9-405a-8760-3140135EBEE1}");

IMPLEMENT_DYNCREATE(CroboServView, CRichEditView)

BEGIN_MESSAGE_MAP(CroboServView, CRichEditView)
	//{{AFX_MSG_MAP(CroboServView)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_FILE_PRINT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_DIRECT, CRichEditView::OnFilePrint)
	ON_COMMAND(ID_FILE_PRINT_PREVIEW, CRichEditView::OnFilePrintPreview)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CroboServView construction/destruction

CroboServView::CroboServView()
{

	m_bFirst = TRUE;
}

CroboServView::~CroboServView()
{
}

BOOL CroboServView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: Modify the Window class or styles here by modifying
	//  the CREATESTRUCT cs
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;

	return CRichEditView::PreCreateWindow(cs);
}

void CroboServView::OnInitialUpdate()
{
	CRichEditView::OnInitialUpdate();

	// Set the printing margins (720 twips = 1/2 inch).
	SetMargins(CRect(720, 720, 720, 720));
	
	PARAFORMAT _pf;
	_pf.cbSize = sizeof(PARAFORMAT);
	_pf.dwMask = PFM_TABSTOPS ;
	_pf.cTabCount = MAX_TAB_STOPS;
	for( int nTabIdx = 0 ; nTabIdx < _pf.cTabCount; nTabIdx++ )
		_pf.rgxTabs[nTabIdx] = (nTabIdx + 1) * 1440/5;
	
	GetRichEditCtrl().SetParaFormat( _pf );

	CHARFORMAT _cf;
	memset( &_cf, 0, sizeof(CHARFORMAT) );
	_cf.cbSize = sizeof(_cf);
	_cf.dwEffects = 0; // CFE_PROTECTED; 
	_cf.dwMask =
		CFM_FACE|CFM_BOLD|CFM_ITALIC|CFM_UNDERLINE|CFM_STRIKEOUT
		|CFM_SIZE|CFM_CHARSET|CFM_PROTECTED|CFM_COLOR;
	_cf.yHeight = 180;
	_cf.bCharSet = DEFAULT_CHARSET;
	
	strcpy( _cf.szFaceName, "Courier New" );

	_cf.crTextColor = RGB(0,0,0);
	
	GetRichEditCtrl().SetDefaultCharFormat( _cf );
	GetRichEditCtrl().SetWordCharFormat( _cf );
	
	GetRichEditCtrl().SetBackgroundColor(FALSE, RGB(255,255,255));
	
	GetRichEditCtrl().SetSel(-1, -1);

	WriteText("_____________________________________________________\n", RGB(0, 128, 255));
	WriteText("                              __\n", RGB(0, 0, 255));
	WriteText("                /           /    )\n", RGB(128, 0, 255));
	WriteText("---)__----__---/__----__----\\--------__---)__-----------__-\n", RGB(255, 0, 255));
	WriteText("  /   ) /   ) /   ) /   )    \\     /___) /   )  | /   /___)\n", RGB(255, 0, 128));
	WriteText("_/_____(___/_(___/_(___/_(____/___(___ _/_______|/___(___ _\n", RGB(255, 0, 0));
	WriteText("© by Thees Schwab. Thanks to Nushi and KingMacro\n\n", RGB(255, 128, 0));
	
	strcpy( _cf.szFaceName, "Arial" );
	GetRichEditCtrl().SetDefaultCharFormat( _cf );
	GetRichEditCtrl().SetWordCharFormat( _cf );

	CWnd* pWnd = AfxGetMainWnd();
	if(pWnd){

		pWnd->SendMessage(UWM_SETVIEW, 0, (LPARAM)this);
	}
}

/////////////////////////////////////////////////////////////////////////////
// CroboServView printing

BOOL CroboServView::OnPreparePrinting(CPrintInfo* pInfo)
{
	// default preparation
	return DoPreparePrinting(pInfo);
}


void CroboServView::OnDestroy()
{
	// Deactivate the item on destruction; this is important
	// when a splitter view is being used.
   CRichEditView::OnDestroy();
   COleClientItem* pActiveItem = GetDocument()->GetInPlaceActiveItem(this);
   if (pActiveItem != NULL && pActiveItem->GetActiveView() == this)
   {
      pActiveItem->Deactivate();
      ASSERT(GetDocument()->GetInPlaceActiveItem(this) == NULL);
   }
}


/////////////////////////////////////////////////////////////////////////////
// CroboServView diagnostics

#ifdef _DEBUG
void CroboServView::AssertValid() const
{
	CRichEditView::AssertValid();
}

void CroboServView::Dump(CDumpContext& dc) const
{
	CRichEditView::Dump(dc);
}

CroboServDoc* CroboServView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CroboServDoc)));
	return (CroboServDoc*)m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CroboServView message handlers

void CroboServView::WriteText(CString strText, COLORREF crText, BOOL bBold)
{


	int nFirstLine1 = GetRichEditCtrl().GetFirstVisibleLine();
	
	CHARRANGE crOld1, crOld2;
	GetRichEditCtrl().GetSel( crOld1 );
	GetRichEditCtrl().SetSel( -1, -1 );
	GetRichEditCtrl().GetSel( crOld2 );

	bool bResetSel = true;
	
	if((crOld1.cpMin == crOld2.cpMin)	&&	(crOld1.cpMax == crOld2.cpMax)){

		bResetSel = false;
	}
	
	GetRichEditCtrl().ReplaceSel(strText);

	CHARFORMAT _cf;
	_cf.cbSize = sizeof(CHARFORMAT);
	_cf.dwMask = CFM_COLOR|CFM_BOLD;
	_cf.dwEffects = bBold ? CFE_BOLD : 0;
	_cf.crTextColor = crText;
	
	GetRichEditCtrl().SetSel(crOld2.cpMax, -1);
	GetRichEditCtrl().SetSelectionCharFormat(_cf);

	if(bResetSel){

		GetRichEditCtrl().SetSel( crOld1 );
		int nFirstLine2 = GetRichEditCtrl().GetFirstVisibleLine();
		
		if(nFirstLine1 != nFirstLine2){

			GetRichEditCtrl().LineScroll(nFirstLine1-nFirstLine2, 0) ;
		}
	}

	int nLineCount = GetRichEditCtrl().GetLineCount();

	if(nLineCount > 300){

		int nEndRemovePos =	GetRichEditCtrl().LineIndex(nLineCount > 300);
		
		CHARRANGE crRestore;
		
		GetRichEditCtrl().GetSel(crRestore);
		crRestore.cpMin -= nEndRemovePos;
		crRestore.cpMax -= nEndRemovePos;
		GetRichEditCtrl().SetSel( 0, nEndRemovePos );
		GetRichEditCtrl().ReplaceSel( _T("") );
		GetRichEditCtrl().SetSel(crRestore);
	}

	if(!bResetSel){

		GetRichEditCtrl().SetSel( -1, -1 );
	}

	GetRichEditCtrl().SetRedraw(TRUE);


	GetRichEditCtrl().SendMessage(EM_SCROLLCARET, 0, 0);
	Invalidate();
}


