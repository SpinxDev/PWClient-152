#ifndef _FWGLYPHTEXT_H_
#define _FWGLYPHTEXT_H_

#include "FWGlyphGraphBase.h"
#include "FWTextMesh.h"

class FWArt;

class FWGlyphText : public FWGlyphGraphBase
{
	friend class FWStateCreateText;
	FW_DECLARE_SERIAL(FWGlyphText)
protected:
	// handles, its value will change by the text	
	POINT m_points[4];
	POINT m_pointsRotated[4];

	// text data
	ACString m_strText;
	ACString m_strFontName;
	int m_nPointSize;
	int m_nFontStyle;

	FWArt * m_pArt;
	int m_nLastSelIndexOfArt;
protected:
	void MakeOutlineArtistic(CCharOutline *pOutline) const;
	
	// return the outline which is measured  in
	// font coordinate
	CCharOutline * CreateOriginalOutline() const;

public:
	// update m_points according to the original outline
	void UpdateRect(CCharOutline *pOutline);

	// transpose original outline to current position
	void TransposeOutline(CCharOutline *pOutline) const;

	// base class interface
#ifdef _FW_EDITOR
	virtual const char * GetDlgToShow();
	virtual void PrepareDlg(FWDialogPropBase * pDlg);
	virtual void UpdateFromDlg(FWDialogPropBase * pDlg);
	virtual void DrawHandles(FWView *pView) const;
#endif
	virtual void SetAngle(double angle);
	virtual void GenerateMeshParam();
	virtual CCharOutline * CreateOutline() const;
	virtual ARectI GetHandleRect(int nIndex) const;
	virtual void MoveHandleTo(int nHandle, APointI newPoint);
	virtual ARectI GetSizeRect() const;
	virtual void SetSizeRect(ARectI rect);
	virtual double GetAngle() const;
	virtual ARectI GetTrueRect() const;
	virtual int GetHitHandle(APointI point) const;
	virtual int GetHandleCount() const;
	virtual APointI GetHandle(int nPoint) const;
	virtual bool IsHit(const APointI& point) const;
	virtual void SetCentralPoint(APointI point);
	virtual void Serialize(FWArchive& ar);

	FWGlyphText();
	virtual ~FWGlyphText();


	void SetText(const ACHAR * szText);
	void SetFontName(const ACHAR * szFontName);
	void SetPointSize(int nPoint);
	void SetFontStyle(int nStyle); // see STYLE_xxx in "CodeTemplate.h"

};

#endif 