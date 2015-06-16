// RichDoc.h : interface of the CroboServDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RICHDOC_H__4B67E249_C6AD_4907_BD9E_E3B6FFC6E7CB__INCLUDED_)
#define AFX_RICHDOC_H__4B67E249_C6AD_4907_BD9E_E3B6FFC6E7CB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CroboServDoc : public CRichEditDoc
{
protected: // create from serialization only
	CroboServDoc();
	DECLARE_DYNCREATE(CroboServDoc)

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CroboServDoc)
	public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
	//}}AFX_VIRTUAL
	virtual CRichEditCntrItem* CreateClientItem(REOBJECT* preo) const;

// Implementation
public:
	virtual ~CroboServDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CroboServDoc)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHDOC_H__4B67E249_C6AD_4907_BD9E_E3B6FFC6E7CB__INCLUDED_)
