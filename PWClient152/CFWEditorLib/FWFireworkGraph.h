#ifndef _FWFIREWORKGRAPH_H_
#define _FWFIREWORKGRAPH_H_

#include "FWFireworkLeaf.h"
#include "CodeTemplate.h"

struct MeshParam;
class CCharOutline;

class FWFireworkGraph : public FWFireworkLeaf  
{
	FW_DECLARE_SERIAL(FWFireworkGraph)
	PROPERTY_FLOAT(Thickness)
protected:
	// data used to fill the multi-plane gfx
	MeshParam * m_pMeshParam;
protected:
#ifdef _FW_EDITOR

	virtual void PrepareDlg(FWDialogPropBase *pDlg);
	virtual void UpdateFromDlg(FWDialogPropBase * pDlg);
#endif
public:
	FWFireworkGraph();
	virtual ~FWFireworkGraph();

	bool BuildDataFromOutline(const CCharOutline *pOutline);

	virtual bool CopyTo(FWAssemblySet *pSet, const A3DMATRIX4& matTM, float fStartTime);
	virtual void Serialize(FWArchive &ar);
};

typedef FWFireworkGraph FWFireworkText;

#endif 