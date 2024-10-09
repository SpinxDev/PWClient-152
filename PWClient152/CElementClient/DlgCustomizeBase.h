#pragma once

#include "DlgBase.h"
#include "EC_Face.h"
#include "EC_Player.h"

class CECFace;
class CECCustomizeMgr;
class CDlgCustomizeFace;
class CECPlayer;
class CECFace;
class CECGame;
class CECCustomizeBound;
class AUIStillImageButton;

struct AppearanceDataNode
{
	int tid;
	ACString hintName;
	A2DSprite* p2DIcon;
	
	bool operator > (const AppearanceDataNode& pANode) const
	{
		return tid > pANode.tid;
	}
	bool operator < (const AppearanceDataNode& pANode) const
	{
		return tid < pANode.tid;
	}
};

class CDlgCustomizeBase : public CDlgBase
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()
protected:
	bool m_bContextReady;
	CECCustomizeMgr * m_pCustomize;
protected:
	CECCustomizeMgr * GetCustomizeMan();
	CECFace * GetFace();
	CECFace::FACE_CUSTOMIZEDATA * GetFaceData();
	CECPlayer * GetPlayer();
	CECCustomizeBound * GetSliderBound();
	CECPlayer::PLAYER_CUSTOMIZEDATA * GetCustomizeData();

	void OnCommand_Cancel(const char * szCommand);
	
	virtual bool OnInitContext() { return true; }
	virtual void OnShowDialog();
public:
	bool IsContextReady() { return m_bContextReady; }
	virtual AUIStillImageButton * GetSwithButton() { ASSERT(false);  return NULL; }
	bool InitContext(CECCustomizeMgr *pCustomize);
	void OnCommandSwitchDialog(const char * szCommand);

	CDlgCustomizeBase();
	virtual ~CDlgCustomizeBase();

};

//////////////////////////////////////////////////////////////////////////
// some helper for customize
//////////////////////////////////////////////////////////////////////////
#define BEGIN_SLIDER_TIME_CHECKED \
{ \
	static DWORD nLastTime = 0; \
 \
	DWORD nThisTime = (DWORD)(ACounter::GetMicroSecondNow()/1000); \
	if(  nThisTime - nLastTime > 30 ) \
	{ 

#define END_SLIDER_TIME_CHECKED \
		nLastTime = nThisTime; \
	} \
}

