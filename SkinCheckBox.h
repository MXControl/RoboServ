#ifndef __SKINCHECKBOX_H__INCLUDED__
#define __SKINCHECKBOX_H__INCLUDED__

class CHighImageList : public CImageList
{
public:
	

	CHighImageList(){}
	
	CHighImageList(UINT nResourceID, COLORREF clrTransparencyMask = RGB(255,0,255),
					int nButtonWidth = 16,	HINSTANCE hInstResource = AfxGetInstanceHandle())
	{
		
		
		VERIFY(LoadHighImgLst(nResourceID, clrTransparencyMask, nButtonWidth, hInstResource));
	}
	
	
	BOOL LoadHighImgLst(UINT nResourceID,COLORREF clrTransparencyMask = RGB(255,0,255),
						 int nButtonWidth = 16,	HINSTANCE hInstResource = AfxGetInstanceHandle())
	{
		
		if(hInstResource == NULL)
		{

			hInstResource = ::AfxGetInstanceHandle();
			if(hInstResource == NULL)
			{

				ASSERT(FALSE);
				return FALSE;
			}
		}

		CBitmap objBitmap;

		if(!objBitmap.Attach(LoadImage(hInstResource, MAKEINTRESOURCE(nResourceID),
										IMAGE_BITMAP, 0, 0,	LR_DEFAULTSIZE|LR_CREATEDIBSECTION)))
		{

			ASSERT(FALSE);
			return FALSE;
		}

		BITMAP infoBitmap;

		if(!objBitmap.GetBitmap(&infoBitmap))
		{

			ASSERT(FALSE);
			return FALSE;
		}

		CSize sizeBitmap(infoBitmap.bmWidth, infoBitmap.bmHeight); 

		int nCountOfButtons = sizeBitmap.cx / nButtonWidth;

		if(!Create(nButtonWidth, sizeBitmap.cy, ILC_COLOR24|ILC_MASK, nCountOfButtons, 0))
		{

			ASSERT(FALSE);
			return FALSE;
		}

		if(Add(&objBitmap, clrTransparencyMask) == -1)
		{

			ASSERT(FALSE);
			return FALSE;
		}

		return TRUE;
	}
};

class CSkinCheckBox : public CExtCheckBox
{

public
:
	CSkinCheckBox()
		: CExtCheckBox()
		, m_nBitmapResource(0)
	{
	}

	int m_nBitmapResource;
	
protected:

	virtual bool OnPaintBox(CDC & dc, CRect & rcBox, eCheckBoxState eState){

		if(m_nBitmapResource == 0)
			return CExtCheckBox::OnPaintBox(dc,rcBox,eState);
		
		CHighImageList il(m_nBitmapResource, RGB(255,0,255), __EXT_DEFAULT_CHECK_BOX_SIZE);
		
		bool bRet = il.Draw(&dc, eState, rcBox.TopLeft(), ILD_NORMAL) == TRUE;
		return bRet;
	}
};// class CSkinCheckBox

/////////////////////////////////////////////////////////////////////////////
// CSkinRadioButton window
/////////////////////////////////////////////////////////////////////////////

class CSkinRadioButton : public CExtRadioButton
{
public:
	CSkinRadioButton()
		: CExtRadioButton()
		, m_nBitmapResource( 0 )
	{
	}
	int m_nBitmapResource;
	
protected:
	virtual bool OnPaintBox(
		CDC & dc,
		CRect & rcBox,
		eRadioBoxState eState
		)
	{
		if( m_nBitmapResource == 0 )
			return CExtRadioButton::OnPaintBox(dc,rcBox,eState);
		
		CHighImageList il(m_nBitmapResource, RGB(255,0,255), __EXT_DEFAULT_RADIO_BOX_SIZE);
		bool bRet = il.Draw(
			&dc, 
			eState, 
			rcBox.TopLeft(), 
			ILD_NORMAL
			) == TRUE;
		return bRet;
	}
};// class CSkinRadioButton

#endif