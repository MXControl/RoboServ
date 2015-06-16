// RichView.h : interface of the CroboServView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_RICHVIEW_H__CE0DA2D5_9E8D_4365_BE3F_3FE73876C65E__INCLUDED_)
#define AFX_RICHVIEW_H__CE0DA2D5_9E8D_4365_BE3F_3FE73876C65E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CroboServCntrItem;

class CroboServView : public CRichEditView
{
protected: // create from serialization only
	CroboServView();
	DECLARE_DYNCREATE(CroboServView)

// Attributes
public:
	CroboServDoc* GetDocument();

// Operations
public:
	BOOL m_bFirst;
// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CroboServView)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	protected:
	virtual void OnInitialUpdate(); // called first time after construct
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	//}}AFX_VIRTUAL

// Implementation
public:
	void WriteText(CString strText, COLORREF crText = RGB(0,0,128), BOOL bBold = TRUE);
	virtual ~CroboServView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
	//{{AFX_MSG(CroboServView)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in RichView.cpp
inline CroboServDoc* CroboServView::GetDocument()
   { return (CroboServDoc*)m_pDocument; }
#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_RICHVIEW_H__CE0DA2D5_9E8D_4365_BE3F_3FE73876C65E__INCLUDED_)
