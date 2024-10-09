// Filename	: DlgRandomMap.h
// Creator	: WYD
// Date		: 2014/8/19

#ifndef _DLGRANDOMMAP_H_
#define _DLGRANDOMMAP_H_

#include "DlgBase.h"
#include "AUIImagePicture.h"

class CDlgRandomMap : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP()
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgRandomMap();
	virtual ~CDlgRandomMap();

	void OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnResize();
	
protected:
	bool GetRandomMapTex(const AString& strFile,unsigned char*& pData,int& iLen);
	bool UpdateRandomMap(int iInst);

	virtual void OnTick(){}
	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnHideDialog();
	virtual bool Render();
protected:
	PAUIIMAGEPICTURE m_pImg_Map;
	A2DSprite* m_pMapSprite;
};

#endif //_DLGCOUNTRYMAP_H_ 