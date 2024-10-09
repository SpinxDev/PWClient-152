#ifndef _FWFIREWORK_H_
#define _FWFIREWORK_H_

#include "CodeTemplate.h"
#include <AList2.h>
#include "FWConfig.h"
#include "FWObject.h"
#include <A3DVector.h>

class FWAssemblyProfile;
class FWDialogPropBase;
class FWAssemblySet;
class A3DGFXElement;
class A3DGFXEx;
typedef A3DGFXEx FWAssembly;
class A3DMATRIX4;


class FWFirework : public FWObject
{
	FW_DECLARE_DYNAMIC(FWFirework)
public:
	enum
	{
		OFFSET_INDEX_BY_GLYPH = 0,
		OFFSET_COUNT
	};

	// this member is only used for create assemblyset
	// so set it as static, client should set its value
	// before use the firework to create assemblyset
	// the default value is (0, 1, 0)
	A3DVECTOR3 s_vecDir;
protected:
	// offset values used to modify the firework
	// in the process of generating final gfx
	float m_fOffsetTime[OFFSET_COUNT];
	A3DVECTOR3 m_vOffsetPos[OFFSET_COUNT];

protected:
	// update gfx start time and pos according to the offset values
	bool UpdateAssemblyStartParam(FWAssembly *pAssembly, const A3DMATRIX4& matTM, float fStartTime);

public:
	FWFirework();
	virtual ~FWFirework();

	// param dialog helper
	// determine which dialog to show
	virtual const char * GetDlgToShow() { return NULL; }
	// param dialog helper
	// called before show the dialog
	virtual void PrepareDlg(FWDialogPropBase *pDlg) {}
	// param dialog helper
	// called after the dialog is closed by BTN_OK
	virtual void UpdateFromDlg(FWDialogPropBase * pDlg) {}

	// return used partical count
	virtual int GetParticalCount() { return 0; }
	// return used Sound count
	virtual int GetSoundCount() { return 0; }

	// calculate sum of offset pos
	A3DVECTOR3 GetSumOfOffsetPos();
	// calculate sum of offset time
	float GetSumOfOffsetTime();

	float GetOffsetTime(int nIndex) const { return m_fOffsetTime[nIndex]; }
	const A3DVECTOR3 & GetOffsetPos(int nIndex) const { return m_vOffsetPos[nIndex]; }
	virtual void SetOffsetTime(int nIndex, float fOffsetTime) { m_fOffsetTime[nIndex] = fOffsetTime; }
	virtual void SetOffsetPos(int nIndex, const A3DVECTOR3 & vOffsetPos) { m_vOffsetPos[nIndex] = vOffsetPos; }	

	// profile property
	virtual const FWAssemblyProfile & GetAssemblyProfile() const = 0;
	virtual void SetAssemblyProfile(const FWAssemblyProfile &src) = 0;

	// convert to gfx
	virtual bool CopyTo(FWAssemblySet *pSet, const A3DMATRIX4& matTM, float fStartTime) = 0;

	virtual void Serialize(FWArchive &ar);
};

#endif 