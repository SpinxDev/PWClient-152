// Filename	: DlgMiniMap.h
// Creator	: Tom Zhou
// Date		: October 15, 2005

#pragma once

#include "hashmap.h"
#include "DlgBase.h"
#include "AUIImagePicture.h"
#include "AUICheckBox.h"
#include "EC_Counter.h"
#include "EC_PlayerLevelRankRealmChangeCheck.h"

class A3DStream;
class A3DTexture;
struct USER_LAYOUT;
class CELPrecinct;

class CDlgMiniMap : public CDlgBase, public CECObserver<CECPlayerLevelRankRealmChangeCheck>
{
	friend class CDlgSettingVideo;

	AUI_DECLARE_EVENT_MAP();
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgMiniMap();
	virtual ~CDlgMiniMap();
	virtual bool Render(void);
	virtual void Resize(A3DRECT rcOld, A3DRECT rcNew);

	void OnCommand_treasurebag(const char * szCommand);
	void OnCommand_closeall(const char * szCommand);
	void OnCommand_openmarkwin(const char * szCommand);
	void OnCommand_bigmap(const char * szCommand);
	void OnCommand_roomout(const char * szCommand);
	void OnCommand_roomin(const char * szCommand);
	void OnCommand_mode(const char * szCommand);
	void OnCommand_makemark(const char * szCommand);
	void OnCommand_listregion(const char * szCommand);
	void OnCommand_listchoose(const char *szCommand);
	void OnCommand_edit(const char * szCommand);
	void OnCommand_radar(const char * szCommand);
	void OnCommand_arrow(const char * szCommand);
	void OnCommand_delete(const char * szCommand);
	void OnCommand_deleteall(const char * szCommand);
	void OnCommand_wiki(const char *szCommand);
	void OnCommand_CANCEL(const char * szCommand);
	void OnCommand_Radio(const char *szCommand);
	void OnCommand_MailToFriends(const char * szCommand);
	void OnCommand_TouchShop(const char * szCommand);
	void OnCommand_TitleChallenge(const char * szCommand);
	void OnCommand_Activity(const char * szCommand);	
	void OnCommand_mapmonster(const char * szCommand);

	void OnEventMouseMove_Hide_2(WPARAM wParam, LPARAM lParam, AUIObject *pObj);
	void OnEventLButtonUp_Hide_2(WPARAM wParam, LPARAM lParam, AUIObject *pObj);

	void SetNewMail(int nRemainTime = 0);
	void ChangeWorldInstance(int idInstance);
	static void MiniMapRenderCallback(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3);

	void CheckMailToFriendsButton();
	void CheckTouchButton();
	void OnAddTitleChallenge();

	void UpdateProfitUI();

	void OnModelChange(const CECPlayerLevelRankRealmChangeCheck *p, const CECObservableChange *q);

public:
	struct MARK
	{
		int nNPC;
		ACString strName;
		A3DVECTOR3 vecPos;
		int mapID;
		
		MARK():mapID(1){}
	};
	struct MAPTLVERTEX
	{
		FLOAT x, y, z, rhw;
		DWORD diffuse;
		DWORD specular;
		FLOAT tu, tv, tu2, tv2;
	};
	enum
	{
		MINIMAP_UL = 0,
		MINIMAP_UR,
		MINIMAP_LL,
		MINIMAP_LR,
		MINIMAP_MAX
	};
	enum
	{
		MODE_A = 0,
		MODE_B
	};
	enum
	{
		MAPMARK_TEAMMATE = 0,
		MAPMARK_CAPTAIN,
		MAPMARK_TARGET
	};
	enum
	{
		TIME_DAY = 0,
		TIME_MORNING,
		TIME_DUSK,
		TIME_NIGHT
	};

	// handle mark on mini map
	const abase::vector<MARK>& GetMarks() const { return m_vecMark; }
	void SetMarkName(int nIndex, const ACString& strName);
	void SetMarkPos(int nIndex, const A3DVECTOR3& vec);
	void AddMark(const MARK& mark);
	void ClearMark(int nIndex = -1); // -1 means clear

	void OnNameInput(int type, const ACString& strName);
	void OnMessageBox(AUIDialog* pDlg, int iRetVal);

	void LoadPlayerMarks(void* pData);
	void LoadMapMarks(CELPrecinct* pPrecinct);

	// draw the hostplayer mark on minimap
	bool DrawHostMark(int nDegree, const A3DPOINT2& ptPos);

	// draw a path
	void DrawPath(int FromX, int FromY, int ToX, int ToY, int Step);
	void DrawPathFromCenter(int CenterX, int CenterY, int ToX, int ToY, int Step, float fMaxRadius);
	void DrawPathNode(int x, int y);

	// change radar mode
	int GetMode() const { return m_nMode; }
	void SetMode(int val) { m_nMode = val; SetItemPos();}

	// whether show the target arrow
	bool IsShowTargetArrow() const { return m_bShowTargetArrow; }

	// get current target position
	A3DVECTOR3 GetCurTarget() const { return m_vecNowTarget; }

	// get the mark desc
	ACString GetMarkDesc(const MARK& mark, int idInstance);

protected:
	static int m_nMode;
	static float m_fZoom;
	static bool m_bShowMark;
	static bool m_bShowTargetArrow;
	static A3DTexture *m_pA3DTexMask;
	static A3DTexture *m_pA3DRadarBack;
	static abase::vector<MARK> m_vecMark;
	static abase::vector<MARK> m_vecNPCMark;
	static MAPTLVERTEX m_vertexMiniMap[MINIMAP_MAX][4];
	static A3DStream * m_pMiniMapStream;
	static abase::hash_map<AString, A3DTexture *> m_TexMap;
	static A2DSprite *m_pA2DSpriteHostDir;
	static A2DSprite *m_pA2DSpriteCompass;
	static A2DSprite *m_pA2DMapMark;
	static A2DSprite *m_pA2DRadar_NPC;
	static A2DSprite *m_pA2DRadar_TaskNPC;
	static A2DSprite *m_pA2DRadar_FTaskNPC;
	static A2DSprite *m_pA2DSpritePet;
	static A2DSprite *m_pA2DSpriteAutoMove;
	static abase::vector<MARK> m_vecTeamMate;
	static A3DVECTOR3 m_vecNowTarget;

	PAUIIMAGEPICTURE	m_pImg_SystemTime;
	PAUICHECKBOX		m_pChk_Mail;
	PAUICHECKBOX        m_pCheck_Mark;
	PAUICHECKBOX		m_pChk_MapMonster;
	PAUICHECKBOX		m_pChk_MapMode;
	PAUIIMAGEPICTURE	m_pImg_CoverSquare;
	PAUIIMAGEPICTURE	m_pImg_CoverCircle;
	static int			m_nHostDirFrames;
	
	bool m_bActivityClicked;
	int  m_iUpdateProfitTime;
	int  m_iUpdateTitleTime;

	CECCounter m_TreasureUpdateCounter;
	CECCounter m_factionPVPMapUpdateCounter;

	bool UpdateMiniMap();

	virtual bool OnInitDialog();
	virtual void OnTick();

	void UpdateMarks(PAUIOBJECT pObjMiniMap);
	void UpdateTreasureMapHintGfx();
	static void RenderFactionPVPIcon(const void * posConverter, int hostX, int hostY);

	void ShowNewImg(bool bIsShow);
	void SetItemPos();
};
