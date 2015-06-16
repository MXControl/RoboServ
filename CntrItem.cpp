// CntrItem.cpp : implementation of the CroboServCntrItem class
//

#include "stdafx.h"
#include "roboServ.h"

#include "roboServDoc.h"
#include "roboServView.h"
#include "CntrItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CroboServCntrItem implementation

IMPLEMENT_SERIAL(CroboServCntrItem, CRichEditCntrItem, 0)

CroboServCntrItem::CroboServCntrItem(REOBJECT* preo, CroboServDoc* pContainer)
	: CRichEditCntrItem(preo, pContainer)
{
	// TODO: add one-time construction code here
	
}

CroboServCntrItem::~CroboServCntrItem()
{
	// TODO: add cleanup code here
	
}

/////////////////////////////////////////////////////////////////////////////
// CroboServCntrItem diagnostics

#ifdef _DEBUG
void CroboServCntrItem::AssertValid() const
{
	CRichEditCntrItem::AssertValid();
}

void CroboServCntrItem::Dump(CDumpContext& dc) const
{
	CRichEditCntrItem::Dump(dc);
}
#endif

/////////////////////////////////////////////////////////////////////////////
