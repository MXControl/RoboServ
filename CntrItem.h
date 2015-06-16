// CntrItem.h : interface of the CroboServCntrItem class
//

#if !defined(AFX_CNTRITEM_H__10455059_6E4A_472B_BD48_D7EDA6CCAB82__INCLUDED_)
#define AFX_CNTRITEM_H__10455059_6E4A_472B_BD48_D7EDA6CCAB82__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CroboServDoc;
class CRichView;

class CroboServCntrItem : public CRichEditCntrItem
{
	DECLARE_SERIAL(CroboServCntrItem)

// Constructors
public:
	CroboServCntrItem(REOBJECT* preo = NULL, CroboServDoc* pContainer = NULL);
		// Note: pContainer is allowed to be NULL to enable IMPLEMENT_SERIALIZE.
		//  IMPLEMENT_SERIALIZE requires the class have a constructor with
		//  zero arguments.  Normally, OLE items are constructed with a
		//  non-NULL document pointer.

// Attributes
public:
	CroboServDoc* GetDocument()
		{ return (CroboServDoc*)CRichEditCntrItem::GetDocument(); }
	CRichView* GetActiveView()
		{ return (CRichView*)CRichEditCntrItem::GetActiveView(); }

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CroboServCntrItem)
	public:
	protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	~CroboServCntrItem();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CNTRITEM_H__10455059_6E4A_472B_BD48_D7EDA6CCAB82__INCLUDED_)
