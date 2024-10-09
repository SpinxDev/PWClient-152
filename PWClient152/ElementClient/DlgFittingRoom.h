// Filename	: DlgFittingRoom.h
// Creator	: Xiao Zhou
// Date		: 2007/4/10

#pragma once

#include "DlgBase.h"
#include "EC_IvtrTypes.h"
#include "AUIImagePicture.h"
#include "AUIStillImageButton.h"

class CECLoginPlayer;
class CECIvtrItem;
class CECFashionModel;
class CECComputeMoveParamFunction;

class CDlgFittingRoom : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP();

public:
	CDlgFittingRoom();
	virtual ~CDlgFittingRoom();

	void OnCommand_CANCEL(const char * szCommand);

	void OnCommand_Palette(const char * szCommand);
	void OnCommand_ResetPlayer(const char * szCommand);

	void OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void OnEventLButtonDown_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonDown_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseWheel_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventMouseMove_Char(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	
	bool SetEquipIcon(CECIvtrItem* pItem, PAUIOBJECT pObj);
	PAUIOBJECT GetEquipIcon(CECIvtrItem* pItem);

	void SetEquipColor(PAUIOBJECT pObj, A3DCOLOR clr);
	bool GetEquipColor(PAUIOBJECT pObj, A3DCOLOR &clr);

	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

protected:
	PAUIIMAGEPICTURE m_pImgEquip[SIZE_EQUIPIVTR];
	PAUIIMAGEPICTURE m_pImg_Char;

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void ResetPlayer();
	void UpdatePlayer();

	void CapturePlayerParams(int mouseX, int mouseY);

	virtual bool Release();

	void CreatePlayer();

protected:
	CECFashionModel* m_pFashionModel;
	CECComputeMoveParamFunction * m_pMoveParamFunction;
};
