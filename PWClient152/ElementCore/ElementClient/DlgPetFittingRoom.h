// Filename	: DlgPetFittingRoom.h
// Creator	: Xu Wenbin
// Date		: 2010/3/9

#pragma once

#include "DlgBase.h"
#include <AUIImagePicture.h>
#include <AUIStillImageButton.h>
#include <A3DTypes.h>
#include <A3DGeometry.h>

class CECModel;

class CDlgPetFittingRoom : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgPetFittingRoom();
	virtual ~CDlgPetFittingRoom();

	void OnCommand_CANCEL(const char * szCommand);

	void SetPet(int tid, A3DCOLOR clr = A3DCOLORRGB(255, 255, 255));
	void SetPetColor(A3DCOLOR clr);

	bool IsPetLoaded() {return m_pModel != NULL; }

protected:
	PAUIIMAGEPICTURE m_pImg_Pet;
	PAUISTILLIMAGEBUTTON m_pBtn_LeftTurn;
	PAUISTILLIMAGEBUTTON m_pBtn_RightTurn;

	int m_nAngle;
	int	m_idPet;
	A3DCOLOR m_color;
	CECModel * m_pModel;
	A3DAABB		m_aabb;

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	CECModel *LoadPetModel(int idPet, A3DCOLOR clr);
	void Reset();
	void UpdateRenderCallback();
};
