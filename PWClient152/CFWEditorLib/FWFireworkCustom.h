#ifndef _FWFIREWORKCUSTOM_H_
#define _FWFIREWORKCUSTOM_H_

#include "CodeTemplate.h"
#include "FWFireworkLeaf.h"
#include "FWAssemblyProfile.h"

class FWModifier;

class FWFireworkCustom : public FWFireworkLeaf
{
	FW_DECLARE_SERIAL(FWFireworkCustom)
	PROPERTY_INT(ComponentCount);
protected:
	FWModifier * m_pModifier;
public:
	FWFireworkCustom();
	virtual ~FWFireworkCustom();

	// property Modifier
	const FWModifier * GetModifier() const {return m_pModifier;}
	void SetModifier(FWModifier *pSrc);

	// base class interface
#ifdef _FW_EDITOR
	virtual const char * GetDlgToShow();
	virtual void PrepareDlg(FWDialogPropBase *pDlg);
	virtual void UpdateFromDlg(FWDialogPropBase * pDlg);
#endif
	virtual int GetParticalCount();
	virtual bool CopyTo(FWAssemblySet *pSet, const A3DMATRIX4& matTM, float fStartTime);
	virtual void Serialize(FWArchive &ar);
};

#endif 