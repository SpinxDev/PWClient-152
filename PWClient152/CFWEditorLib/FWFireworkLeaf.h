#ifndef _FWFIREWORKLEAF_H_
#define _FWFIREWORKLEAF_H_

#include "CodeTemplate.h"
#include "FWFirework.h"
#include "FWAssemblyProfile.h"

class FWAssemblyProfile;
class A3DGFXEx;

typedef A3DGFXEx FWAssembly;

class FWFireworkLeaf : public FWFirework  
{
	FW_DECLARE_SERIAL(FWFireworkLeaf)
protected:
	FWAssemblyProfile m_profile;

protected:
	// part of the template method CopyTo()
	FWAssembly * LoadAndInitAssembly();
	// part of the template method CopyTo()
	bool DoCopy(FWAssemblySet *pSet, FWAssembly *pAssembly, const A3DMATRIX4& matTM, float fStartTime);

public:
	FWFireworkLeaf();
	virtual ~FWFireworkLeaf();
	
	virtual const FWAssemblyProfile & GetAssemblyProfile() const {return m_profile;}
	virtual void SetAssemblyProfile(const FWAssemblyProfile &src){m_profile = src;}

	// base class interface
#ifdef _FW_EDITOR
	virtual void PrepareDlg(FWDialogPropBase *pDlg);
	virtual void UpdateFromDlg(FWDialogPropBase * pDlg);
#endif
	virtual int GetParticalCount();
	virtual int GetSoundCount();
	virtual bool CopyTo(FWAssemblySet *pSet, const A3DMATRIX4& matTM, float fStartTime);
	virtual void Serialize(FWArchive &ar);
};

#endif 