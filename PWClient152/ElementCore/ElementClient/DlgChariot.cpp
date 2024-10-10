// File		: DlgCrossSevActivity.cpp
// Creator	: WYD
// Date		: 2013/8/12

#include "DlgChariot.h"
#include "DlgQuickBar.h"
#include "tankbattleplayerapply_re.hpp"
#include "tankbattleplayergetrank_re.hpp"
#include "gnetdef.h"
#include "EC_HostPlayer.h"
#include "EC_GameSession.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "elementdataman.h"
#include <A2DSprite.h>
#include <A3DEngine.h>
#include "ElementSkill.h"
#include <AFI.h>
#include "EC_GameUIMan.h"
#include "EC_Skill.h"
#include "EC_HostPlayer.h"
#include "EC_UIConfigs.h"
#include "EC_NPC.h"
#include "DlgGeneralCard.h"

#define new A_DEBUG_NEW

const char* szAction = "站立_通用";
const char* szCampIcon[] = {"2013\\战车\\暗之圣隐会.dds","2013\\战车\\光之圣隐会.dds"};
//////////////////////////////////////////////////////////////////////////
CChariot::CChariot(CECHostPlayer* pHost):m_pHostPlayer(pHost)
{
	m_iCurChariotID = 0;
	m_bUpdateState = false;
//	memset(m_chariot_vec,0,sizeof(m_chariot_vec));
//	memset(m_pCurChariotSkill,0,sizeof(m_pCurChariotSkill));
}
CChariot::~CChariot()
{
}
void CChariot::PrepareChariot(int id)
{
	if (m_iCurChariotID != id)
	{
		m_iCurChariotID = id;
		const CHARIOT_CONFIG* pChariot = GetChariot(m_iCurChariotID);
		if (pChariot)
		{
			m_pCurChariotSkill.clear();
			for (int i=0;i<sizeof(pChariot->skill)/sizeof(pChariot->skill[0]);i++)
			{
				if(pChariot->skill[i]<1) continue;
				CECSkill* pSkill = m_pHostPlayer->GetPositiveSkillByID(pChariot->skill[i]);
				m_pCurChariotSkill.push_back(pSkill);
			}			
		}
	}
}
bool CChariot::ChariotHasSkill(int chariot_id,int skill_id)
{
	const CHARIOT_CONFIG* pChariot = GetChariot(chariot_id);
	if (pChariot)
	{
		for (int i=0;i<sizeof(pChariot->skill)/sizeof(pChariot->skill[0]);i++)
		{
			if(pChariot->skill[i]<1) continue;
			if (pChariot->skill[i] == skill_id)
				return true;
		}			
	}
	return false;
}
int CChariot::GetChariotTypeByTid(int tid)
{
	for (unsigned int i=0;i<m_chariot_vec.size();i++)
	{
		unsigned int c = m_chariot_vec[i].size();
		for (unsigned int j=0;j<c;j++)
		{
			if (m_chariot_vec[i][j]->id == (unsigned int)tid)
				return i;
		}
	}

	return -1; // no chariot
}
bool CChariot::IsUpdateChariot(int oldid, int newid)
{
	if (oldid != 0 && oldid != newid)
	{
		const CHARIOT_CONFIG* pChariot = GetChariot(newid);
		if (pChariot)
		{
			return pChariot->pre_chariot == oldid;		
		}
	}
	return false;
}
const CHARIOT_CONFIG* CChariot::GetChariot(int tid)
{
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	const void* p = pDataMan->get_data_ptr(tid,ID_SPACE_CONFIG,DataType);
	if (DataType == DT_CHARIOT_CONFIG)
	{
		return (const CHARIOT_CONFIG*)p;
	}
	return NULL;
}
bool CChariot::CanGatherMine()
{	
	if(!m_pHostPlayer) return false;

	const CECHostPlayer::BATTLEINFO& info = m_pHostPlayer->GetBattleInfo();	
	const CHARIOT_CONFIG* pChariot = GetChariot(info.iChariot);
	
	if(!pChariot) return false;
	
	const CHARIOT_CONFIG* pNextChariot = GetNextChariot(info.iChariot);
	if(!pNextChariot) return false;

	int updateCost = 0;
	if (pNextChariot)	
		updateCost = pNextChariot->upgrade_cost;	
	
	if(info.iEnergy >= updateCost) return false;

	if (IsTopLevelChariot(info.iChariot))
		return false;

	return true;
}
bool CChariot::IsTopLevelChariot(int tid)
{
	for (unsigned int i=0;i<m_chariot_vec.size();i++)
	{
		unsigned int c = m_chariot_vec[i].size();
		for (unsigned int j=0;j<c;j++)
		{
			if (m_chariot_vec[i][j]->id == (unsigned int)tid && j==c-1)
			{
				return true;
			}
		}
	}
	return false;
}
const CHARIOT_CONFIG* CChariot::GetNextChariot(int tid)
{
	for (unsigned int i=0;i<m_chariot_vec.size();i++)
	{
		unsigned int c = m_chariot_vec[i].size();
		for (unsigned int j=0;j<c;j++)
		{
			if (m_chariot_vec[i][j]->id == (unsigned int)tid && j<c-1)
			{
				return m_chariot_vec[i][j+1];
			}
		}
	}
	return NULL;
}
void CChariot::ExtractChariotFromData()
{
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE DataType;
	unsigned int tid = pDataMan->get_first_data_id(ID_SPACE_CONFIG,DataType);
	
	int iType = 0;

	m_chariot_vec.clear();
	
	abase::vector<CHARIOT_CONFIG*> chariot;
	
	while(tid)
	{
		if(DataType == DT_CHARIOT_CONFIG)
		{
			CHARIOT_CONFIG* pChariot = (CHARIOT_CONFIG*)pDataMan->get_data_ptr(tid,ID_SPACE_CONFIG, DataType);
			if (pChariot)
			{
				if (pChariot->pre_chariot ==0)
				{
					CHARIOTLIST l;
					l.push_back(pChariot);
					m_chariot_vec.push_back(l);
				}
				else
					chariot.push_back(pChariot);
			}
		}
		tid = pDataMan->get_next_data_id(ID_SPACE_CONFIG, DataType);
	}
	
	// 组织战车列表
	for (unsigned int i=0;i<m_chariot_vec.size();i++)
	{
		for (unsigned int j=0;j<m_chariot_vec[i].size();j++)
		{
			for (unsigned int k=0;k<chariot.size();k++)
			{
				if (m_chariot_vec[i][j] &&  m_chariot_vec[i][j]->id == (unsigned int)chariot[k]->pre_chariot && chariot[k]->pre_chariot !=0)
				{
					m_chariot_vec[i].push_back(chariot[k]);
					break;
				}
			}
		}
	}
	
	for (i=0;i<m_chariot_vec.size();i++)
	{
		for (unsigned int j=0;j<m_chariot_vec[i].size();j++)
		{
			if (!m_chariot_vec[i][j])
			{
				ASSERT(FALSE);
				a_LogOutput(1,"CChariot::ExtractChariotFromData: pre_chariot error %d,%d",i,j);
			}
		}
	}
}
int CChariot::GetChariotTid(int iSel) 
{ 
	if(iSel<0 || iSel>= (int)m_chariot_vec.size()) return 0;
	return m_chariot_vec[iSel][0] ? m_chariot_vec[iSel][0]->id : 0;
}
CECSkill* CChariot::GetCurChariotSkillByIndex(int index)
{
	if (index<0 || index >= (int)m_pCurChariotSkill.size())
		return NULL;

	return m_pCurChariotSkill[index];
}
CECSkill* CChariot::GetCurChariotSkillByID(int skill_id)
{
	for (int i=0;i<(int)m_pCurChariotSkill.size();i++)
	{
		if (m_pCurChariotSkill[i] && m_pCurChariotSkill[i]->GetSkillID() == skill_id)
		{
			return m_pCurChariotSkill[i];
		}
	}
	return NULL;
}
void CChariot::SetChariotImgHint(int iType,int iLevel, CECGameUIMan* pGameUIMan,PAUIIMAGEPICTURE pImg)
{
	if(!pImg) return;

	CHARIOT_CONFIG* pChariot = GetChariot(iType,iLevel);
	if(!pChariot) return;

	pImg->SetCover(NULL, -1, 1);
	pGameUIMan->SetCover(pImg,pChariot->file_icon,CECGameUIMan::ICONS_INVENTORY);	

	ACString desc;
	ACString str;
	// 名字，战车血量，战车物理攻击力，战车移动速度，战车技能名称
	desc = pChariot->name;
	desc += _AL("\r");
	str.Format(pGameUIMan->GetStringFromTable(10892),pChariot->hp);
	desc += str;
	desc += _AL("\r");
	str.Format(pGameUIMan->GetStringFromTable(10893),pChariot->damage);
	desc += str;
	desc += _AL("\r");
	str.Format(pGameUIMan->GetStringFromTable(10893),pChariot->speed);
	desc += str;
	desc += _AL("\r");
	desc += g_pGame->GetSkillDesc()->GetWideString(pChariot->skill[0] * 10);
	desc += _AL("\r");
	desc += g_pGame->GetSkillDesc()->GetWideString(pChariot->skill[1] * 10);
	desc += _AL("\r");
	desc += g_pGame->GetSkillDesc()->GetWideString(pChariot->skill[2] * 10);

	pImg->SetHint(desc);
}

void CChariot::OnChangeUI(CECGameUIMan* pUIMan,bool bBackup)
{
	if(!pUIMan) return;
	PAUIDIALOG pDlg = NULL;

	if (bBackup)
	{
		m_dlgShowState.clear();
		abase::vector<AString> dlgName;
		
		CDlgQuickBar::GetQuickBarNameAndSC(m_pHostPlayer,dlgName);
	//	dlgName.push_back("Win_ELF");
	//	dlgName.push_back("Win_ELFZoom");
		
		for (unsigned int i=0;i<dlgName.size();i++)
		{
			pDlg = pUIMan->GetDialog(dlgName[i]);
			if(pDlg)
			{
				m_dlgShowState[dlgName[i]] = pDlg->IsShow();
			//	if(pDlg->IsShow())
					pDlg->Show(false);
			}
		}
	}
	else
	{
		DLGSTATE::iterator itr = m_dlgShowState.begin();
		for (;itr!=m_dlgShowState.end();++itr)
		{
			pDlg = pUIMan->GetDialog(itr->first);
			if (pDlg)
			{
				if(pDlg->IsShow() != itr->second)
					pDlg->Show(itr->second);
			}
		}

		PAUIDIALOG pDlgRank = pUIMan->GetDialog("Win_BattleMsg01");
		if (pDlgRank)
		{
			CDlgChariotScore* pRank = dynamic_cast<CDlgChariotScore*>(pDlgRank);
			if (pRank)
			{
				pRank->OnCommandRankList(NULL) ; // 退出战车时请求一次
			}
		}
	}
}
CECModel * CChariot::LoadModel(int id)
{	
	CECModel *pModel = NULL;
	if (id > 0)
	{
		const CHARIOT_CONFIG* pConfig = GetChariot(id);
		if (pConfig)
		{
			int iShape = (pConfig->shape & 0x3f) | 0x80;
			if (CECPlayer::LoadDummyModel(iShape, &pModel))
			{				
				pModel->SetPos(A3DVECTOR3(0));
				pModel->SetDirAndUp(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0));
				pModel->SetAutoUpdateFlag(false);
			}
		}
	}
	
	return pModel;
}
//////////////////////////////////////////////////////////////////////////

AUI_BEGIN_COMMAND_MAP(CDlgChariotEnter, CDlgBase)
AUI_ON_COMMAND("IDCANCEL",		OnCommandCancel)
AUI_ON_COMMAND("Btn_Apply",		OnCommandEnterBattle)
AUI_ON_COMMAND("Btn_More",		OnCommandAboutChariot)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgChariotEnter, CDlgBase)
AUI_ON_EVENT("Img_Chariot0*",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_END_EVENT_MAP()

CDlgChariotEnter::~CDlgChariotEnter()
{
}
void CDlgChariotEnter::DoDataExchange(bool bSave)
{
	CDlgBase::DoDataExchange(bSave);
}

bool CDlgChariotEnter::OnInitDialog()
{
	m_pImg_Chariot.clear();
	int i=1;
	while(true)
	{
		AString str;
		str.Format("Img_Chariot%02d",i++);
		PAUIOBJECT p = GetDlgItem(str);
		if(!p) break;
		m_pImg_Chariot.push_back((PAUIIMAGEPICTURE)p);
	}

	return true;
}
void CDlgChariotEnter::OnShowDialog()
{
	CChariot* pChariot = GetHostPlayer()->GetChariot();
	if(!pChariot) return;

	for (unsigned int i=0;i<m_pImg_Chariot.size();i++)
	{
		pChariot->SetChariotImgHint(i,0,GetGameUIMan(),m_pImg_Chariot[i]);
	}

	SetData(0,"SelChariotIndex");
	m_pImg_Chariot[0]->SetCover(GetGameUIMan()->m_pA2DSpriteMask, 0, 1);

	GetDlgItem("Lbl_Condition01")->SetText(GetGameUIMan()->GetStringFromTable(10897));
}

void CDlgChariotEnter::OnTick()
{
	// 判断条件
	int level = CECUIConfig::Instance().GetGameUI().nChariotApplyLevel;
	int level2 = CECUIConfig::Instance().GetGameUI().nChariotApplyLevel2;
	int reincarnt = CECUIConfig::Instance().GetGameUI().nChariotApplyReincarnation;
	bool cond1 = (GetHostPlayer()->GetBasicProps().iLevel >= level && GetHostPlayer()->GetBasicProps().iLevel2 >= level2) || GetHostPlayer()->GetReincarnationCount() >=reincarnt; // 修真 升仙入魔
	bool cond2 = GetHostPlayer()->GetCoolTime(GP_CT_TRICKBATTLE_APPLY) <= 0; // 不冷却

	ACString str;
	str.Format(GetGameUIMan()->GetStringFromTable(10898), GetGameUIMan()->GetStringFromTable(cond2 ? 9773:9772));
	GetDlgItem("Lbl_Condition02")->SetText(str);

	GetDlgItem("Lbl_Condition01")->SetColor(cond1 ? A3DCOLORRGB(255,255,255):A3DCOLORRGB(255,0,0));
	GetDlgItem("Lbl_Condition02")->SetColor(cond2 ? A3DCOLORRGB(255,255,255):A3DCOLORRGB(255,0,0));

	GetDlgItem("Btn_Apply")->Enable(cond1 && cond2);
}
void CDlgChariotEnter::OnCommandCancel(const char * szCommand)
{
	GetHostPlayer()->EndNPCService();
	Show(false);
}
void CDlgChariotEnter::OnCommandEnterBattle(const char* szCommand)
{
	CChariot* pChariot = GetHostPlayer()->GetChariot();
	if(!pChariot) return;

	int iSel = GetData("SelChariotIndex");
	GetGameSession()->c2s_CmdNPCSevTrickBattleRequest(pChariot->GetChariotTid(iSel));
}

void CDlgChariotEnter::OnCommandAboutChariot(const char* szCommand)
{
	PAUIDIALOG pDlg = GetGameUIMan()->GetDialog("Win_ChariotInfo");
	if (pDlg && !pDlg->IsShow())
		pDlg->Show(true);
}
void CDlgChariotEnter::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int curSel = GetData("SelChariotIndex");
	int toSel = atoi(pObj->GetName() + strlen("Img_Chariot0"));
	if(toSel == curSel+1) return;

	m_pImg_Chariot[curSel]->SetCover(NULL, -1, 1);
	PAUIIMAGEPICTURE pImg = (PAUIIMAGEPICTURE)pObj;
	pImg->SetCover(GetGameUIMan()->m_pA2DSpriteMask, 0, 1);
	
	SetData(toSel-1,"SelChariotIndex");
}

//////////////////////////////////////////////////////////////////////////
AUI_BEGIN_EVENT_MAP(CDlgChariotInfo, CDlgBase)
AUI_ON_EVENT("Img_Model",		WM_LBUTTONDOWN,	OnEventLButtonDown_Model)
AUI_ON_EVENT("Img_Model",		WM_LBUTTONUP,	OnEventLButtonUp_Model)
AUI_ON_EVENT("Tv_ChariotName",	WM_LBUTTONDOWN,	OnEventLButtonDown_Tree)
AUI_ON_EVENT("Img_Model",		WM_MOUSEMOVE,	OnEventMouseMove)
AUI_END_EVENT_MAP()

CDlgChariotInfo::~CDlgChariotInfo()
{
	A3DRELEASE(m_pModel);
}
void CDlgChariotInfo::OnTick()
{
	UpdateRenderCallback();
}
void CDlgChariotInfo::UpdateRenderCallback()
{	
	if (m_pImg_Chariot && m_pModel)
	{		
		m_pModel->Tick(GetGame()->GetTickTime());
		m_pImg_Chariot->SetRenderCallback(NPCRenderDemonstration, (DWORD)m_pModel, m_nMouseOffset + m_nMouseOffsetThis, (DWORD)&m_aabb);
	}
}
void CDlgChariotInfo::OnEventMouseMove(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( (wParam & MK_LBUTTON) )
	{
		m_nMouseOffsetThis = GET_X_LPARAM(lParam) - m_nMouseLastX;
		if( m_nMouseOffset + m_nMouseOffsetThis > 70 )
			m_nMouseOffsetThis = 70 - m_nMouseOffset;
		else if( m_nMouseOffset + m_nMouseOffsetThis < -70 )
			m_nMouseOffsetThis = -70 - m_nMouseOffset;
	}
}
void CDlgChariotInfo::OnEventLButtonDown_Tree(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	P_AUITREEVIEW_ITEM pItem = m_pTree->GetSelectedItem();
	if( !pItem || m_pTree->GetParentItem(pItem) == m_pTree->GetRootItem())
		return;
	
	POINT ptPos = pObj->GetPos();
	A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();
	int x = GET_X_LPARAM(lParam) - ptPos.x - p->X;
	int y = GET_Y_LPARAM(lParam) - ptPos.y - p->Y;

	int id = m_pTree->GetItemData(pItem);
	if(id && m_pTree->GetHitArea(x,y) == AUITREEVIEW_RECT_FRAME)
		SelectChariot(id);
}
void CDlgChariotInfo::OnEventLButtonDown_Model(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SetCanMove(false);
	SetCaptureObject(pObj);
	ChangeFocus(GetDlgItem("Tv_ChariotName"));
	m_nMouseLastX = GET_X_LPARAM(lParam);
	m_nMouseOffset += m_nMouseOffsetThis;
	m_nMouseOffsetThis = 0;
	
}
void CDlgChariotInfo::OnEventLButtonUp_Model(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	SetCaptureObject(NULL);
	ChangeFocus(GetDlgItem("Tv_ChariotName"));
	SetCanMove(true);
}

void CDlgChariotInfo::SelectChariot(int tid)
{
	if(m_curChariotID == tid) return;
	m_curChariotID = tid;

	CChariot* pHostChariot = GetHostPlayer()->GetChariot();
	if(!pHostChariot) return;
	const CHARIOT_CONFIG* pChariot = pHostChariot->GetChariot(tid);
	if(!pChariot)
		return;

	CECModel *pModel = pHostChariot->LoadModel(tid);
	if (pModel)	
	{
		A3DRELEASE(m_pModel);
		pModel->SetPos(A3DVECTOR3(0));
		pModel->SetDirAndUp(A3DVECTOR3(0, 0, -1.0f), A3DVECTOR3(0, 1.0f, 0));
		pModel->SetAutoUpdateFlag(false);
		m_pModel = pModel;

		m_pModel->PlayActionByName(szAction,1.0f);
		m_pModel->Tick(1);
		m_aabb = m_pModel->GetModelAABB();		
		m_aabb.Extents *= 1.8f;
		m_aabb.CompleteMinsMaxs();
	}

	// set other ui info

	int skill_cnt = min(sizeof(pChariot->skill)/sizeof(pChariot->skill[0]),(int)m_pImg_Skill.size());
	
	for (int j=0;j<skill_cnt;j++)
	{
		int idSkill = pChariot->skill[j];
		if (idSkill)
		{
			const char* szIcon = GNET::ElementSkill::GetIcon(idSkill);
			AString strFile;
			af_GetFileTitle(szIcon, strFile);
			strFile.MakeLower();
			m_pImg_Skill[j]->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
			
			ACHAR szText[1024] = {0};
			CECSkill::GetDesc(idSkill, 1, szText, 1000);
			
			m_pImg_Skill[j]->SetHint(szText);
		}
		else
		{
			m_pImg_Skill[j]->ClearCover();			
			m_pImg_Skill[j]->SetHint(_AL(""));
		}
	}
	
	m_pTxt_Info->SetText(pChariot->desc);	
	m_pLbl_Name->SetText(pChariot->name);

	int hp = GetHostPlayer()->GetBasicProps().iCurHP;
	float speed = GetHostPlayer()->GetExtendProps().mv.run_speed;

	ACString val;
	float valF = (pChariot->hp+hp)*(1+pChariot->hp_inc_ratio);
	val.Format(_AL("%d"),(int)valF);
	m_pLbl_HP->SetText(val);
//	val.Format(_AL("%d"),pChariot->damage);
//	m_pLbl_Attack->SetText(val);

	val.Format(_AL("%f"),pChariot->speed + speed);
	m_pLbl_Speed->SetText(val);

}
bool CDlgChariotInfo::OnInitDialog()
{
	m_curChariotID = 0;
	m_pModel = NULL;
	
	m_pImg_Chariot = NULL;;

	m_pLbl_Name = NULL;
	m_pTxt_Info = NULL;
	m_pTree = NULL;
	
	m_pLbl_HP = NULL;
	m_pLbl_Speed = NULL;
	//m_pLbl_Attack = NULL;

	DDX_Control("Img_Model",m_pImg_Chariot);
	DDX_Control("Txt_Name",m_pLbl_Name);
	DDX_Control("Txt_ChariotDesc",m_pTxt_Info);
	DDX_Control("Tv_ChariotName",m_pTree);
	DDX_Control("Txt_Hp",m_pLbl_HP);
	DDX_Control("Txt_Speed",m_pLbl_Speed);
//	DDX_Control("Txt_Attack",m_pLbl_Attack);

	m_pImg_Skill.clear();
	int i=1;
	while(true)
	{
		AString str;
		str.Format("Img_Skill%02d",i++);
		PAUIOBJECT p = GetDlgItem(str);
		if(!p) break;

		m_pImg_Skill.push_back((PAUIIMAGEPICTURE)p);
	}

	m_pImg_Chariot->ClearCover();
	m_pImg_Chariot->ClearTexture();

	return CDlgBase::OnInitDialog();
}
void CDlgChariotInfo::OnShowDialog()
{
	CChariot* pHostChariot = GetHostPlayer()->GetChariot();
	if(!pHostChariot) return;

	m_nMouseOffset = 0;
	m_nMouseOffsetThis = 0;

	m_pTree->DeleteAllItems();
	for (int i = 0;i<pHostChariot->GetChariotTypeCnt();i++)
	{
		P_AUITREEVIEW_ITEM pItem = m_pTree->InsertItem(GetGameUIMan()->GetStringFromTable(10900+i));
		for (int j=0;j<pHostChariot->GetChariotCnt(i);j++)
		{
			CHARIOT_CONFIG* pConfig = pHostChariot->GetChariot(i,j);
			if (pConfig)
			{
				P_AUITREEVIEW_ITEM pChariotItem = m_pTree->InsertItem(pConfig->name,pItem);
				m_pTree->SetItemData(pChariotItem,pConfig->id);				
			}
		}
		m_pTree->Expand(pItem, AUITREEVIEW_EXPAND_EXPAND);
	}

	CHARIOT_CONFIG* pConfig = pHostChariot->GetChariot(0,0);
	if(pConfig)
	{
		SelectChariot(pConfig->id);
		int vec[] = {0,0};
		P_AUITREEVIEW_ITEM p =	m_pTree->GetItemByIndexArray(2,vec);
		m_pTree->SelectItem(p);
	}
}

//////////////////////////////////////////////////////////////////////////
AUI_BEGIN_COMMAND_MAP(CDlgChariotRevive, CDlgBase)
AUI_ON_COMMAND("Btn_Revive",		 OnCommandRevive)
AUI_END_COMMAND_MAP()

AUI_BEGIN_EVENT_MAP(CDlgChariotRevive, CDlgBase)
AUI_ON_EVENT("Img_Chariot0*",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_END_EVENT_MAP()


bool CDlgChariotRevive::OnInitDialog()
{
	m_pImg_Chariot.clear();
	int i =1;
	while(true)
	{
		AString str;
		str.Format("Img_Chariot0%d",i++);
		PAUIOBJECT p = GetDlgItem(str);
		if(!p) break;
		m_pImg_Chariot.push_back((PAUIIMAGEPICTURE)p);
	}
	m_pLbl_Time = NULL;
	DDX_Control("Txt_Time",m_pLbl_Time);

	m_iTimeToRevive = 0;

	return CDlgBase::OnInitDialog();
}
void CDlgChariotRevive::OnShowDialog()
{
	CChariot* pHostChariot = GetHostPlayer()->GetChariot();
	if(!pHostChariot) return;

	for (unsigned int i=0;i<m_pImg_Chariot.size();i++)
	{
		pHostChariot->SetChariotImgHint(i,0,GetGameUIMan(), m_pImg_Chariot[i]);
		m_pImg_Chariot[i]->SetCover(NULL, -1, 1);
	}
	SetData(INT_MAX,"SelChariotIndex"); // invalid value

	m_iTimeToRevive = CECUIConfig::Instance().GetGameUI().nChariotReviveTimeout;
}
void CDlgChariotRevive::OnTick()
{
	m_iTimeToRevive -= g_pGame->GetRealTickTime();
	if(m_iTimeToRevive<0)
		m_iTimeToRevive = 0;

	ACString str;
	str.Format(_AL("%d"),m_iTimeToRevive/1000);
	m_pLbl_Time->SetText(str);

	if (GetDlgItem("Btn_Revive"))
	{
		GetDlgItem("Btn_Revive")->Enable(GetData("SelChariotIndex") < (int)m_pImg_Chariot.size());
	}
}
void CDlgChariotRevive::OnCommandRevive(const char* szCommand)
{
	CChariot* pHostChariot = GetHostPlayer()->GetChariot();
	if(!pHostChariot) return;

	// revive
	int curSel = GetData("SelChariotIndex");
	GetGameSession()->c2s_CmdReviveVillage(pHostChariot->GetChariotTid(curSel));
}
void CDlgChariotRevive::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject * pObj)
{
	int curSel = GetData("SelChariotIndex");
	int toSel = atoi(pObj->GetName()+strlen("Img_Chariot0")); // 计算序号
	if(toSel == curSel+1) return;

	if (curSel < (int)m_pImg_Chariot.size())
		m_pImg_Chariot[curSel]->SetCover(NULL, -1, 1);
	PAUIIMAGEPICTURE pImg = (PAUIIMAGEPICTURE)pObj;
	pImg->SetCover(GetGameUIMan()->m_pA2DSpriteMask, 0, 1);
	
	SetData(toSel-1,"SelChariotIndex");
}

////////////////////////////////////

AUI_BEGIN_EVENT_MAP(CDlgChariotOpt, CDlgBase)
AUI_ON_EVENT("Img_Skill0*",	WM_LBUTTONUP,	OnEventLButtonUp)
AUI_ON_EVENT("Img_UpData",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("Txt_UpData",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_END_EVENT_MAP()

bool CDlgChariotOpt::OnInitDialog()
{
	m_pProgressMagic = NULL;	
	m_pImg_LevelUp = NULL;
	m_pTxt_LevelUp = NULL;

	DDX_Control("Txt_UpData", m_pTxt_LevelUp);
	DDX_Control("Img_UpData", m_pImg_LevelUp);
	DDX_Control("Prgs_AP",m_pProgressMagic);

	m_pImg_Skill.clear();
	int i = 1;
	while(true)
	{
		AString str;
		str.Format("Img_Skill%02d",i++);
		PAUIOBJECT p = GetDlgItem(str);
		if(!p) break;
		m_pImg_Skill.push_back((PAUIIMAGEPICTURE)p);
	}
	return CDlgBase::OnInitDialog();
}
void CDlgChariotOpt::OnEventLButtonUp(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if( !pObj->GetDataPtr("ptr_CECSkill") )
		return;
	
	CECSkill* pSkill = (CECSkill*)pObj->GetDataPtr("ptr_CECSkill");
	if (pSkill)
	{
		GetHostPlayer()->ApplySkillShortcut(pSkill->GetSkillID());
	}
}
void CDlgChariotOpt::OnShowDialog()
{

}

void CDlgChariotOpt::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	CChariot* pChariot = GetHostPlayer()->GetChariot();
	const CHARIOT_CONFIG* p = pChariot->GetNextChariot(GetHostPlayer()->GetBattleInfo().iChariot);
	if(p)
		GetGameSession()->c2s_CmdUpdateChariot(p->id);
}

void CDlgChariotOpt::OnTick()
{
	CECHostPlayer *pHost = GetHostPlayer();
	CChariot* pHostChariot = pHost->GetChariot();
	if(!pHostChariot) return;

	ACString strText;	
	CECSkill* pSkill = NULL;
	AUIClockIcon *pClock;
	PAUIIMAGEPICTURE pCell;

	const CECHostPlayer::BATTLEINFO& info = pHost->GetBattleInfo();	
	const CHARIOT_CONFIG* pChariot = pHostChariot->GetChariot(info.iChariot);

	if(!pChariot) return;

	for (unsigned int i=0;i<m_pImg_Skill.size();i++)
	{
		pCell = m_pImg_Skill[i];		
		
		pClock = pCell->GetClockIcon();
		pClock->SetProgressRange(0, 1);
		pClock->SetProgressPos(1);
		pSkill = pHostChariot->GetCurChariotSkillByIndex(i);
		if(pSkill)
		{			
			pCell->SetDataPtr(pSkill,"ptr_CECSkill");
			
			if( pSkill->ReadyToCast() && pHost->GetPrepSkill() != pSkill )
			{
				if( !pHost->CheckSkillCastCondition(pSkill) )
					pCell->SetColor(A3DCOLORRGB(255, 255, 255));
				else
					pCell->SetColor(A3DCOLORRGB(128, 128, 128));
			}
			else
				pClock->SetColor(A3DCOLORRGBA(0, 0, 0, 128));
			
			if( (pSkill->GetCoolingTime() > 0 ||
				pHost->GetPrepSkill() == pSkill ))
			{
				pClock->SetProgressRange(0, pSkill->GetCoolingTime());
				if( pHost->GetPrepSkill() == pSkill )
					pClock->SetProgressPos(0);
				else
					pClock->SetProgressPos(pSkill->GetCoolingTime() - pSkill->GetCoolingCnt());
			}

			const char* szIcon = pSkill->GetIconFile();
			AString strFile;
			af_GetFileTitle(szIcon, strFile);
			strFile.MakeLower();
			pCell->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
				GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][strFile]);
			
			ACHAR szText[1024] = {0};
			CECSkill::GetDesc(pSkill->GetSkillID(), 1, szText, 1000);
			
			pCell->SetHint(szText);
		}
		else
		{
			pCell->SetDataPtr(NULL,"ptr_CECSkill");
			pCell->SetColor(A3DCOLORRGB(255, 255, 255));
			pCell->SetHint(_AL(""));
			pCell->ClearCover();
		}		
	}	

	const ROLEBASICPROP &rbp = pHost->GetBasicProps();
	const ROLEEXTPROP &rep = pHost->GetExtendProps();
	
	int nHP = rbp.iCurHP;
	int nMaxHP = max(rep.bs.max_hp, 1);
	
	const CHARIOT_CONFIG* pNextChariot = pHostChariot->GetNextChariot(info.iChariot);
	int updateCost = 0;
	if (pNextChariot)
	{
		updateCost = pNextChariot->upgrade_cost;
	}

//	a_sprintf(szText,  _AL("%d/%d"), info.iEnergy, updateCost);
//	GetDlgItem("Txt_AP")->SetText(szText);
	if (updateCost)
		m_pProgressMagic->SetProgress(int(100*info.iEnergy/updateCost));
	else
		m_pProgressMagic->SetProgress(0);


	bool bShow = pChariot && updateCost && info.iEnergy >= updateCost;
	m_pImg_LevelUp->Show(bShow);
//	m_pTxt_LevelUp->Show(true);
	ACString str;
	if(bShow)
		str.Format(_AL("%s"),GetGameUIMan()->GetStringFromTable(10911));
	else
		str.Format(_AL("%d/%d"),info.iEnergy,updateCost);
	m_pTxt_LevelUp->SetText(str);
	
	GetDlgItem("Txt_ChariotName")->SetText(pChariot->name);
}
//////////////////////////////////////
AUI_BEGIN_COMMAND_MAP(CDlgChariotRankList, CDlgBase)
AUI_ON_COMMAND("Btn_Refresh",				OnCommandRefresh)
AUI_END_COMMAND_MAP()

void CDlgChariotRankList::OnCommandRefresh(const char* szCommand)
{
	GetGameSession()->tankBattle_GetRank();
}
void CDlgChariotRankList::OnTankBattleNotify(Protocol* pProtocol)
{
	int ret = 0;
	switch(pProtocol->GetType())
	{
	case PROTOCOL_TANKBATTLEPLAYERAPPLY_RE: // npc 进跨服战场 返回
		{
			TankBattlePlayerApply_Re* p = (TankBattlePlayerApply_Re*)pProtocol;
			ret = p->retcode;
			if (ret == ERR_SUCCESS)
			{
			}
			else
			{
				ACString strError;
				GetGameSession()->GetServerError(ret, strError);
				g_pGame->GetGameRun()->AddChatMessage(strError, GP_CHAT_MISC);
			}
		}
		break;
	case PROTOCOL_TANKBATTLEPLAYERGETRANK_RE: // 战场排名
		{
			TankBattlePlayerGetRank_Re* p = (TankBattlePlayerGetRank_Re*)pProtocol;
			ret = p->retcode;
			if (ret == ERR_SUCCESS)
			{
				if(!IsShow())
					Show(true);

				SetSelfRand(p->your_score);
				SetRankList(p->player_scores);
			}
			else
				ASSERT(FALSE);
		}
		break;
	default:
		{
			ASSERT(FALSE);
		}
	}
}
bool CDlgChariotRankList::OnInitDialog()
{
	m_pList = NULL;

	DDX_Control("List_Result",m_pList);

	return CDlgBase::OnInitDialog();
}
void CDlgChariotRankList::OnHideDialog()
{
	if (m_bNPCFlag)
	{
		GetGameUIMan()->EndNPCService();
		m_bNPCFlag = false;
	}
}
void CDlgChariotRankList::SetSelfRand(const GNET::TankBattlePlayerScoreInfo& info)
{
	m_selfRank = info;
}
void CDlgChariotRankList::FormatInfo(int i,const ACHAR* name,ACString& outStr)
{
	if(i<0 || i>= (int)m_rankList.size()) return;

	ACString kill;
	kill.Format(_AL("^00ff00%d/^ff0000%d"),m_rankList[i].kill_cnt,m_rankList[i].dead_cnt);
	outStr.Format(_AL("%d\t%s\t%d\t%s"),m_rankList[i].rank,name,m_rankList[i].score,kill);
}
void CDlgChariotRankList::SetRankList(const GNET::TankBattlePlayerScoreInfoVector& vec)
{
	m_pList->ResetContent();
	m_rankList.GetVector().clear();
	GNET::TankBattlePlayerScoreInfoVector::const_iterator itr = vec.begin();
	int i=0;	
	for (;itr!=vec.end();++itr)
	{
		m_rankList.push_back(*itr);

		ACString str;
		FormatInfo(i++,_AL("--"),str);
		m_pList->AddString(str);
		m_pList->SetItemData(i,1,NAME_NULL);
	}
}
void CDlgChariotRankList::OnTick()
{
	abase::vector<int> ids;
	
	for(unsigned int i=0;i<m_rankList.size();i++)
	{
		int id = m_rankList[i].roleid;
		int flag = m_pList->GetItemData(i,1);
		if(flag != NAME_DONE && id!=0)
		{
			const ACHAR* szName = g_pGame->GetGameRun()->GetPlayerName(id, false);
			if( szName )		
			{	
				ACString str;
				FormatInfo(i,szName,str);
				m_pList->SetText(i,str);				
				m_pList->SetItemData(i,NAME_DONE,1); // 已得到名字
			}	
			else if(flag == NAME_NULL)
			{
				ids.push_back(id);				
				m_pList->SetItemData(i,NAME_REQUESTING,1);	
			}
		}
	}
	
	if(ids.size())
		g_pGame->GetGameSession()->CacheGetPlayerBriefInfo(ids.size(),ids.begin(), 2);

	ACString str;
	str.Format(_AL("%d"),m_selfRank.rank);
	GetDlgItem("Txt_Rank8")->SetText(str);

	str.Format(_AL("%d"),m_selfRank.score);
	GetDlgItem("Txt_Total")->SetText(str);

	str.Format(_AL("^00ff00%d/^ff0000%d"),m_selfRank.kill_cnt,m_selfRank.dead_cnt);
	GetDlgItem("Txt_KillDeath")->SetText(str);

	GetDlgItem("Txt_Name8")->SetText(GetHostPlayer()->GetName());
}

//////////////////////////////////////////////////////////////////////////
AUI_BEGIN_COMMAND_MAP(CDlgChariotScore, CDlgBase)
AUI_ON_COMMAND("Btn_Quit",				OnCommandExitBattle)
AUI_ON_COMMAND("Btn_Rank",				OnCommandRankList)
AUI_ON_COMMAND("Btn_ChariotInfo",		OnCommandQueryAmount)
AUI_END_COMMAND_MAP()

bool CDlgChariotScore::Release()
{
	OnHideDialog();
	
	return CDlgBase::Release();
}
void CDlgChariotScore::OnCommandExitBattle(const char* szCommand)
{
	GetGameUIMan()->MessageBox("ChariotBattle",GetGameUIMan()->GetStringFromTable(10899),MB_OKCANCEL,A3DCOLORRGB(255,255,255));
}
void CDlgChariotScore::OnCommandRankList(const char* szCommand)
{
	GetGameSession()->tankBattle_GetRank();
}
void CDlgChariotScore::OnCommandQueryAmount(const char* szCommand)
{
	// Get chariot amount
	GetGameSession()->c2s_CmdQueryChariotAmount();
}
void CDlgChariotScore::UpdateScore()
{
	ACString str;
	int s = GetHostPlayer()->GetBattleInfo().iScoreSelf;

	str.Format(GetGameUIMan()->GetStringFromTable(10895),s);
	GetDlgItem("Txt_BattleMsg")->SetText(str);
}
void CDlgChariotScore::OnShowDialog()
{
	ACString str;	
	str.Format(GetGameUIMan()->GetStringFromTable(10895),0);
	GetDlgItem("Txt_BattleMsg")->SetText(str);

	PAUIIMAGEPICTURE pImg = (PAUIIMAGEPICTURE)GetDlgItem("Img_Camp");
	if (pImg && GetHostPlayer()->GetBattleCamp())
	{
		CDlgGeneralCard::SetImgCover(pImg,szCampIcon[GetHostPlayer()->GetBattleCamp()-1]);
		pImg->SetHint(GetGameUIMan()->GetStringFromTable(10912 + GetHostPlayer()->GetBattleCamp()));
	}
}
void CDlgChariotScore::OnHideDialog()
{
	PAUIIMAGEPICTURE pImg = (PAUIIMAGEPICTURE)GetDlgItem("Img_Camp");
	if (pImg)	
		CDlgGeneralCard::SetImgCover(pImg,NULL);	
}
void CDlgChariotMultiKill::OnTick()
{
	if (m_fCloseKill>0.0f)
			m_fCloseKill -= g_pGame->GetRealTickTime();
		
	if (m_fCloseKill<=0.0f)
	{
		Show(false);
	}
}

void CDlgChariotMultiKill::UpdateKill()
{
	ACString str;
	int k = GetHostPlayer()->GetBattleInfo().iMultiKill;
	if (k > 0)
	{	
		str.Format(GetGameUIMan()->GetStringFromTable(10896),k);
		GetDlgItem("Txt_DoubleHit")->SetText(str);
		m_fCloseKill = 5000.0f;
		Show(true);
	}
}

//////////////////////////////////////////////////////////////////////////

bool CDlgChariotAmount::OnInitDialog()
{
	m_pImg_ChariotDefVec.clear();
	m_pProg_AmountDefVec.clear();
	m_pLbl_NumberDefVec.clear();

	int i=1;
	while(true)
	{
		AString str;
		str.Format("Img_DefChariot%d",i);
		PAUIOBJECT p = GetDlgItem(str);
		if(!p) break;
		m_pImg_ChariotDefVec.push_back((PAUIIMAGEPICTURE)p);

		str.Format("Prgs_Def%d",i);
		p = GetDlgItem(str);
		if(!p) break;
		m_pProg_AmountDefVec.push_back((PAUIPROGRESS)p);

		str.Format("Txt_DefNum%d",i++);
		p = GetDlgItem(str);
		if(!p) break;
		m_pLbl_NumberDefVec.push_back((PAUILABEL)p);
	}

	m_pImg_ChariotAtkVec.clear();
	m_pProg_AmountAtkVec.clear();
	m_pLbl_NumberAtkVec.clear();

	i=1;
	while(true)
	{
		AString str;
		str.Format("Img_AtkChariot%d",i);
		PAUIOBJECT p = GetDlgItem(str);
		if(!p) break;
		m_pImg_ChariotAtkVec.push_back((PAUIIMAGEPICTURE)p);
		
		str.Format("Prgs_Atk%d",i);
		p = GetDlgItem(str);
		if(!p) break;
		m_pProg_AmountAtkVec.push_back((PAUIPROGRESS)p);
		
		str.Format("Txt_AtkNum%d",i++);
		p = GetDlgItem(str);
		if(!p) break;
		m_pLbl_NumberAtkVec.push_back((PAUILABEL)p);
	}
	
	return true;
}
void CDlgChariotAmount::OnShowDialog()
{
	CChariot* pChariot = GetHostPlayer()->GetChariot();
	if(!pChariot) return;
	
	unsigned int i=0;

	for (i=0;i<m_pImg_ChariotDefVec.size();i++)
	{
		CHARIOT_CONFIG* pChariotConfig = pChariot->GetChariot(i,0);
		if(!pChariotConfig) return;
		
		GetGameUIMan()->SetCover(m_pImg_ChariotDefVec[i],pChariotConfig->file_icon,CECGameUIMan::ICONS_INVENTORY);	
		m_pImg_ChariotDefVec[i]->SetHint(GetGameUIMan()->GetStringFromTable(10900+i));
	}
	for (i=0;i<m_pImg_ChariotAtkVec.size();i++)
	{
		CHARIOT_CONFIG* pChariotConfig = pChariot->GetChariot(i,0);
		if(!pChariotConfig) return;
		
		m_pImg_ChariotAtkVec[i]->SetHint(GetGameUIMan()->GetStringFromTable(10900+i));
		GetGameUIMan()->SetCover(m_pImg_ChariotAtkVec[i],pChariotConfig->file_icon,CECGameUIMan::ICONS_INVENTORY);	
	}
}
void CDlgChariotAmount::OnHideDialog()
{
	unsigned int i=0;
	for (i=0;i<m_pProg_AmountAtkVec.size();i++)
	{
		m_pProg_AmountAtkVec[i]->SetProgress(0);
	}
	for (i=0;i<m_pProg_AmountDefVec.size();i++)
	{
		m_pProg_AmountDefVec[i]->SetProgress(0);
	}
	for (i=0;i<m_pLbl_NumberAtkVec.size();i++)
	{
		m_pLbl_NumberAtkVec[i]->SetText(_AL("0"));
	}
	for (i=0;i<m_pLbl_NumberDefVec.size();i++)
	{
		m_pLbl_NumberDefVec[i]->SetText(_AL("0"));
	}
}
void CDlgChariotAmount::SetInfo(int atk,S2C::cmd_player_query_chariots::chariot* pAtk,int def,S2C::cmd_player_query_chariots::chariot* pDef)
{
	CChariot* pChariot = GetHostPlayer()->GetChariot();
	if(!pChariot) return;

	OnHideDialog();

	int i=0;

	abase::vector<int> chariotAmountAtk(m_pImg_ChariotAtkVec.size(),0);
	abase::vector<int> chariotAmountDef(m_pImg_ChariotDefVec.size(),0);

	// 计算每种战车数量
	for (i=0;i<atk;i++)
	{
		int type = pChariot->GetChariotTypeByTid(pAtk[i].type);
		if(type<(int)chariotAmountAtk.size() && type>=0)
			chariotAmountAtk[type] += pAtk[i].count;
	}
	for (i=0;i<def;i++)
	{
		int type = pChariot->GetChariotTypeByTid(pDef[i].type);
		if(type<(int)chariotAmountDef.size() && type>=0)
			chariotAmountDef[type] += pDef[i].count;
	}

	int chariotCount = 	CECUIConfig::Instance().GetGameUI().nChariotAmount;

	//设置显示
	ACString str;
	for (i=0;i<(int)chariotAmountAtk.size();i++)
	{
		int v = chariotAmountAtk[i];
		m_pProg_AmountAtkVec[i]->SetProgress(int((float)v * AUIPROGRESS_MAX/chariotCount));
		str.Format(_AL("%d"),v);
		m_pLbl_NumberAtkVec[i]->SetText(str);
	}
	for (i=0;i<(int)chariotAmountDef.size();i++)
	{
		int v = chariotAmountDef[i];
		m_pProg_AmountDefVec[i]->SetProgress(int((float)v * AUIPROGRESS_MAX/chariotCount));
		str.Format(_AL("%d"),v);
		m_pLbl_NumberDefVec[i]->SetText(str);
	}
}