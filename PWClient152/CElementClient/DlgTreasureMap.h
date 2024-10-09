#pragma once

#include "hashmap.h"
#include "DlgBase.h"
#include "AUIImagePicture.h"

class A3DTexture;
class A3DSurface;

class CDlgTreasureMap : public CDlgBase  
{
	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:

	CDlgTreasureMap();
	virtual ~CDlgTreasureMap();
	virtual void OnTick();
	virtual bool Render(void);

	bool ShowTreasureMap(int iMapLowerLeftX,int iMapLowerLeftZ,int iTreasureLowerLeftX,int iTreasureLowerLeftZ,int iTreasureSide,int nHalfSizeMap);

	void ClearTreasureMap();
	void OnEventRButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

protected:
	virtual bool OnChangeLayoutBegin() { return false; }
	CRenderBuffer* m_pRenderBuffer;
	A3DTexture* m_pMapTexture;
	int m_iMapLowerLeftX,m_iMapLowerLeftZ,m_iTreasureLowerLeftX,m_iTreasureLowerLeftZ,m_iTreasureSide;
};
