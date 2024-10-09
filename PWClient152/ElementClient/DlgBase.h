// Filename	: DlgBase.h
// Creator	: Tom Zhou
// Date		: October 11, 2005

#pragma once

#include "AMemory.h"
#include "A3DMacros.h"
#include "DlgTheme.h"
#include <vector>
class CECGame;
class CECGameRun;
class CECGameSession;
class CECWorld;
class CECHostPlayer;
class CECGameUIMan;
class CECLoginUIMan;
class CECBaseUIMan;
class CECIvtrItem;
class elementdataman;

class CRenderBuffer
{
public:
	unsigned char* m_Buffer;
	int m_iWidth, m_iHeight, m_iRowSpan;
	
	CRenderBuffer(int width, int height);
	
	~CRenderBuffer();
	
	void Reserve(int width, int height);
	void CopyArea(RECT srcRect, unsigned char* srcBuffer, int srcRowSpan);
	void BlitBGR(unsigned char* destBuffer, int destRowSpan);
	void ToGray();
	void FlipX();
	void FlipY();
};

class CDlgBase : public CDlgTheme  
{
	AUI_DECLARE_COMMAND_MAP()
	AUI_DECLARE_EVENT_MAP()

public:
	CDlgBase();
	virtual ~CDlgBase();

	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Close(const char * szCommand);
	void OnCommand_Help(const char * szCommand);
	
	void OnEventKillFocus(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void DisableForCrossServer(const char *szObjName);

	// called when a item drag-drop on this dialog
	virtual void OnItemDragDrop(CECIvtrItem* pIvtrSrc, int iSrc, PAUIOBJECT pObjSrc, PAUIOBJECT pObjOver);

	virtual void SwitchShow(){ Show(!IsShow()); }
	
	//	从 "Item_1" 中获取 1
	static int GetNameIndex(const char *szName, const char *szPrefix);
	static int GetNameIndex(PAUIOBJECT pObj, const char *szPrefix);
	
	static bool CheckNamePrefix(const char *szName, const char *szPrefix);
	static bool CheckNamePrefix(PAUIOBJECT pObj, const char *szPrefix);

	static A2DSprite* GetCoverDX(const char* str_image, int width, int height);
    
	void AlignToDlgItem(AUIObject *pObject, int nInsideX, int nAlignX,
		         int nInsideY, int nAlignY, int nOffsetX = 0, int nOffsetY = 0);

	void AlignToRect(RECT rect, int nInsideX, int nAlignX,
		         int nInsideY, int nAlignY, int nOffsetX = 0, int nOffsetY = 0);

protected:
	virtual bool OnInitDialog();

	CECGame * GetGame();
	CECGameRun * GetGameRun();
	CECGameSession * GetGameSession();
	CECWorld * GetWorld();
	CECHostPlayer * GetHostPlayer();
	CECGameUIMan * GetGameUIMan();
	CECLoginUIMan * GetLoginUIMan();
	CECBaseUIMan * GetBaseUIMan();
	elementdataman * GetElementDataMan();
};

class CDlgDynamicControl : public CDlgBase
{
public:
	CDlgDynamicControl();
	virtual ~CDlgDynamicControl();

	class ControlCommand
	{
	public:
		virtual bool operator()() = 0;
		virtual ~ControlCommand(){};
	};
	void ClearControls();

protected:

	virtual bool OnCommandMap(const char *szCommand);
	PAUIOBJECT CreateControlWithCommand(const ACString& hint, int type, ControlCommand* command);

	struct DynamicControl{
		PAUIOBJECT control;
		AString name;
		ControlCommand* command;

		DynamicControl():control(NULL), command(NULL){}
	};
	std::vector<DynamicControl> m_Controls;
};

