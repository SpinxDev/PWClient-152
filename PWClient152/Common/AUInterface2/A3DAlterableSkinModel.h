// Filename	: A3DAlterableSkinModel.h
// Creator	: Tom Zhou
// Date		: July 31, 2004
// Desc		: A3DAlterableSkinModel is the class of an alterable skin model.

#ifndef _A3DALTERABLESKINMODEL_H_
#define _A3DALTERABLESKINMODEL_H_

#include "A3DSkinModel.h"

class A3DAlterableSkinModel : public A3DSkinModel
{
public:
	A3DAlterableSkinModel();
	virtual ~A3DAlterableSkinModel();

	bool AdjustFace_NoseLength(float fValue);
	bool AdjustFace_NoseWidth(float fValue);
	bool AdjustFace_NoseHeight(float fValue);
	bool AdjustFace_JawLength(float fValue);
	bool AdjustFace_EyeBrowDistance(float fValue);
	bool AdjustFace_EyeWidth(float fValue);
	bool AdjustFace_EyeDistance(float fValue);
	bool AdjustFace_EyeType(float fValue);
	bool AdjustFace_MouseWidth(float fValue);
	bool AdjustFace_HairType(float fValue);
	bool AdjustFace_LipType(float fValue);
	bool AdjustFace_ForeheadHeight(float fValue);
	bool AdjustFace_CheekHeight(float fValue);

	bool AdjustBody_Height(float fValue);
	bool AdjustBody_ShoulderWidth(float fValue);
	bool AdjustBody_ChestGirth(float fValue);
	bool AdjustBody_WaistGirth(float fValue);
	bool AdjustBody_HipGirth(float fValue);
	bool AdjustBody_BraSize(float fValue);
};

#endif //_A3DALTERABLESKINMODEL_H_