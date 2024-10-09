// Filename	: DlgPalette2.h
// Creator	: Xu Wenbin
// Date		: 2014/5/29

#ifndef _ELEMENTCLIENT_DLGPALETTE2_H_
#define _ELEMENTCLIENT_DLGPALETTE2_H_

#include "DlgPaletteBase.h"

class AUIImagePicture;
class AUIRadioButton;
class CPaletteControl;
class A2DSprite;

class CDlgPalette2 : public CDlgPaletteFashion  
{
	AUI_DECLARE_COMMAND_MAP()
		
public:
	CDlgPalette2();
	 
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_BestColor(const char * szCommand);

	virtual void OnSetColor(const A3DCOLOR& color);
	virtual void OnChangeTarget(enumChangeTarget);
	
protected:	
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	int  GetEquipSlotForTarget(int changeTarget);
	int  GetEnabledTargetCount()const;

	void UpdateBestColorButton();

protected: 
	PAUIOBJECT			m_pBtn_BestColor;

	unsigned short		m_bestColor[CHANGE_ALL];	//	各部位保存的最佳颜色、避免反复查询
	bool				m_hasBestColor[CHANGE_ALL];	//	各部位是否有最佳色，避免反复查询
};

#endif	//	_ELEMENTCLIENT_DLGPALETTE2_H_