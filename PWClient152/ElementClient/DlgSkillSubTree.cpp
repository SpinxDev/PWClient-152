// Filename	: DlgSkillSubTree.cpp
// Creator	: zhangyitian
// Date		: 2014/7/2

#include "DlgSkillSubTree.h"
#include "DlgSkillAction.h"
#include "DlgSkillSubList.h"

#include "EC_HostSkillModel.h"
#include "EC_GameUIMan.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Configs.h"
#include "EC_Viewport.h"

#include "AUIImagePicture.h"
#include "AFI.h"
#include "A3DViewport.h"
#include "../../CFWEditorLib/FWFlatCollector.h"

AUI_BEGIN_COMMAND_MAP(CDlgSkillSubTree, CDlgBase)
AUI_ON_COMMAND("Btn_TreeClose", OnCommand_TreeClose)
AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgSkillSubTree, CDlgBase)
AUI_ON_EVENT("skill*",	WM_LBUTTONDOWN, OnEventLButtonDown)
AUI_END_EVENT_MAP()

extern CECGame * g_pGame;

// 所有技能图标obj的名字
const char* SkillImageObjName = "skill%d";

//////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDlgSkillSubTree::SkillTreeNode::SkillTreeNode(int skillID, int xPos, int yPos, int imgWidth, int imgHeight, 
											   int zoneLeft, int zoneRight, int verticalDistanceToChild) {
	m_skillID	= skillID;
	m_xPos		= xPos;
	m_yPos		= yPos;
	m_imgWidth	= imgWidth;
	m_imgHeight	= imgHeight;
	m_zoneLeft	= zoneLeft;
	m_zoneRight	= zoneRight;
	m_verticalDistanceToChild	= verticalDistanceToChild;
	m_childrenCount	= CECHostSkillModel::Instance().GetJunior(m_skillID).size();
}

int CDlgSkillSubTree::SkillTreeNode::GetSkillID() const {
	return m_skillID;
}

int CDlgSkillSubTree::SkillTreeNode::GetCenterX() const {
	return m_xPos;
}

int CDlgSkillSubTree::SkillTreeNode::GetCenterY() const {
	return m_yPos;
}

int CDlgSkillSubTree::SkillTreeNode::GetBottom() const {
	return m_yPos + m_imgHeight / 2;
}

int CDlgSkillSubTree::SkillTreeNode::GetTop() const {
	return m_yPos - m_imgHeight / 2;
}

int CDlgSkillSubTree::SkillTreeNode::GetLeft() const {
	return m_xPos - m_imgWidth / 2;
}

int CDlgSkillSubTree::SkillTreeNode::GetRight() const {
	return m_xPos + m_imgWidth / 2;
}

int CDlgSkillSubTree::SkillTreeNode::GetChildrenCount() const {
	return m_childrenCount;
}

CDlgSkillSubTree::SkillTreeNode CDlgSkillSubTree::SkillTreeNode::GetChild(int index) const {
	ASSERT(index >= 0 && index < m_childrenCount);
	int childSkillID = CECHostSkillModel::Instance().GetJunior(m_skillID)[index].first;
	int childZoneLeft = m_zoneLeft + (m_zoneRight - m_zoneLeft) / m_childrenCount * index;
	int childZoneRight = m_zoneLeft + (m_zoneRight - m_zoneLeft) / m_childrenCount * (index + 1);
	int childXPos = (childZoneLeft + childZoneRight) / 2;
	int childYPos = m_yPos - m_verticalDistanceToChild;

	return SkillTreeNode(childSkillID, childXPos, childYPos, m_imgWidth, m_imgHeight, childZoneLeft, childZoneRight, m_verticalDistanceToChild);
}

int CDlgSkillSubTree::SkillTreeNode::GetMidYCoordWithChild() const {
	return m_yPos - m_verticalDistanceToChild / 2;
}



///////////////////////////////////////////////////////////////////////////////////////////////////////////////

CDlgSkillSubTree::CDlgSkillSubTree() : m_usedImgPicCount(0), m_templateImgPic(NULL) {

	m_rootID = 0;
	m_pFlatCollector = new FWFlatCollector;
	m_pFlatCollector->Init(g_pGame->GetA3DEngine());

}

CDlgSkillSubTree::~CDlgSkillSubTree() {
}

bool CDlgSkillSubTree::OnInitDialog() {
	if (CDlgBase::OnInitDialog()) {
		DDX_Control("Img_Skill", m_templateImgPic);
		m_imgPicWidth = m_templateImgPic->GetSize().cx;
		m_imgPicHeight = m_templateImgPic->GetSize().cy;
		return true;
	}
	return false;
}

bool CDlgSkillSubTree::Release() {
	A3DRELEASE(m_pFlatCollector);
	if (!CDlgBase::Release()) {
		return false;
	}
	return true;
}

void CDlgSkillSubTree::OnCommand_TreeClose(const char *pszCommand) {
	GetGameUIMan()->m_pDlgSkillSubList->SelectSkill(0);
}

AUIImagePicture* CDlgSkillSubTree::AllocImgPic() {
	
	// 如果已经用完了，则再创建一个
	if (m_usedImgPicCount == m_imgPics.size()) {
		AUIImagePicture* imgPic = dynamic_cast<AUIImagePicture*>(CloneControl(m_templateImgPic));
		char name[10];
		sprintf(name, SkillImageObjName, m_usedImgPicCount);
		imgPic->SetName(name);
		m_imgPics.push_back(imgPic);
	}
	return m_imgPics[m_usedImgPicCount++];
}

void CDlgSkillSubTree::DrawSkillTree(const SkillTreeNode& rootNode) {

	CECHostSkillModel& model = CECHostSkillModel::Instance();
	
	// 设置技能图标位置
	AUIImagePicture* imgPic = AllocImgPic();
	imgPic->SetPos(rootNode.GetLeft(), rootNode.GetTop());
	
	// 设置技能图标文件
	SetImagePicture(rootNode.GetSkillID(), imgPic);

	// 如果没有前置技能，结束
	if (rootNode.GetChildrenCount() == 0) {
		return;
	}
	
	// 如果只有一个前置技能，直接处理，结束
	if (rootNode.GetChildrenCount() == 1) {
		SkillTreeNode childNode = rootNode.GetChild(0);
		DrawSkillTree(childNode);
		APointI p1(rootNode.GetCenterX(), rootNode.GetTop());
		APointI p2(rootNode.GetCenterX(), childNode.GetBottom());
		m_linesToDraw.push_back(std::pair<APointI, APointI>(p1, p2));
		return;
	}

	// 处理多个前置技能的情况
	APointI p1 = APointI(rootNode.GetCenterX(), rootNode.GetTop());
	APointI p2 = APointI(rootNode.GetCenterX(), rootNode.GetMidYCoordWithChild());
	m_linesToDraw.push_back(std::pair<APointI, APointI>(p1, p2));

	SkillTreeNode firstChild = rootNode.GetChild(0);
	SkillTreeNode lastChild = rootNode.GetChild(rootNode.GetChildrenCount() - 1);
	p1 = APointI(firstChild.GetCenterX(), rootNode.GetMidYCoordWithChild());
	p2 = APointI(lastChild.GetCenterX(),  rootNode.GetMidYCoordWithChild());
	m_linesToDraw.push_back(std::pair<APointI, APointI>(p1, p2));

	for (size_t i = 0; i < rootNode.GetChildrenCount(); i++) {		
		SkillTreeNode childNode = rootNode.GetChild(i);
		p1 = APointI(childNode.GetCenterX(), childNode.GetBottom());
		p2 = APointI(childNode.GetCenterX(), rootNode.GetMidYCoordWithChild());
		m_linesToDraw.push_back(std::pair<APointI, APointI>(p1, p2));
		DrawSkillTree(childNode); 
	}
}

void CDlgSkillSubTree::ShowSkillTree(int rootID) {
		
	m_rootID = rootID;

	if (rootID == 0) {
		return;
	}

	CECHostSkillModel& model = CECHostSkillModel::Instance();
	for (size_t i = 0; i < m_imgPics.size(); i++) {
		m_imgPics[i]->Show(false);
	}
	int treeHeight = model.GetTreeHeight(rootID);
	int verticalDistanceToChild = GetSize().cy / (treeHeight + 1);
	m_linesToDraw.clear();

	SkillTreeNode rootNode(rootID, GetSize().cx / 2, GetSize().cy - verticalDistanceToChild, 
		m_imgPicWidth, m_imgPicHeight, 0, GetSize().cx, verticalDistanceToChild);

	DrawSkillTree(rootNode);
}

void CDlgSkillSubTree::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj) {
	int skillID = pObj->GetData();
	if (GetGameUIMan()->m_pDlgSkillSubList->IsSkillOrConvertSkillExist(skillID)) {
		GetGameUIMan()->m_pDlgSkillSubList->SelectSkill(skillID);
		GetGameUIMan()->m_pDlgSkillSubList->ScrollToShowSelectedSkill();
	}	
}

bool CDlgSkillSubTree::Render() {
	if (CDlgBase::Render()) {
		int vOffSet = GetGame()->GetViewport()->GetA3DViewport()->GetParam()->Y;				// 垂直偏移量
		int curX = GetPos().x;
		int curY = GetPos().y + vOffSet;
		const int HalfLineWidth = 1;
		for (size_t i = 0; i < m_linesToDraw.size(); i++) {
			if (m_linesToDraw[i].first.x == m_linesToDraw[i].second.x) {
				// 竖线
				ARectI rect;
				rect.left = m_linesToDraw[i].first.x - HalfLineWidth + curX;
				rect.right = m_linesToDraw[i].first.x + HalfLineWidth + curX;
				rect.top = min(m_linesToDraw[i].first.y, m_linesToDraw[i].second.y) + curY;
				rect.bottom = max(m_linesToDraw[i].first.y, m_linesToDraw[i].second.y) + curY;
				m_pFlatCollector->DrawRectangle2D(rect, COLOR_WHITE, true, A3DCOLORRGBA(255, 255, 255, 255));
			} else {
				// 横线
				ARectI rect;
				rect.left = min(m_linesToDraw[i].first.x, m_linesToDraw[i].second.x) + curX;
				rect.right = max(m_linesToDraw[i].first.x, m_linesToDraw[i].second.x) + curX;
				rect.top = m_linesToDraw[i].first.y - 2 * HalfLineWidth + curY;
				rect.bottom = m_linesToDraw[i].first.y + curY;
				m_pFlatCollector->DrawRectangle2D(rect, COLOR_WHITE, true, A3DCOLORRGBA(255, 255, 255, 255));
			}
		}
		m_pFlatCollector->Flush_2D();
		return true;
	}
	return false;
}

bool CDlgSkillSubTree::OnChangeLayoutBegin() {
	// 清除所有技能图标
	size_t i;
	for (i = 0; i < m_imgPics.size(); i++) {
		DeleteControl(m_imgPics[i]);
	}
	// 清除所有要画的线
	m_linesToDraw.clear();

	return true;
}


void CDlgSkillSubTree::Resize(A3DRECT rcOld, A3DRECT rcNew) {
	ShowSkillTree(m_rootID);
}

void CDlgSkillSubTree::SetImagePicture(int skilllD, AUIImagePicture* imgPic) {

	CECHostSkillModel& model = CECHostSkillModel::Instance();

	AString skillIconFile;
	af_GetFileTitle(model.GetSkillIcon(skilllD), skillIconFile);
	skillIconFile.MakeLower();
	CECHostSkillModel::enumSkillLearnedState learnedState = model.GetSkillLearnedState(skilllD);
	imgPic->SetCover(GetGameUIMan()->m_pA2DSpriteIcons[CECGameUIMan::ICONS_SKILL],
		GetGameUIMan()->m_IconMap[CECGameUIMan::ICONS_SKILL][skillIconFile]);
	ACString skillDsc;
	if (CECHostSkillModel::SKILL_NOT_LEARNED == learnedState) {
		imgPic->SetColor(A3DCOLORRGB(128, 128, 128));
		skillDsc = model.GetSkillDescription(skilllD, 1);
		imgPic->SetGfx(NULL);
	} else if (CECHostSkillModel::SKILL_OVERRIDDEN == learnedState) {
		imgPic->SetColor(A3DCOLORRGB(255, 255, 255));
		skillDsc = model.GetSkillDescription(skilllD, model.GetSkillMaxLevel(skilllD));
		skillDsc = GetStringFromTable(11329) + skillDsc;
		imgPic->SetGfx(NULL);
	} else {
		imgPic->SetColor(A3DCOLORRGB(255, 255, 255));
		skillDsc = model.GetSkillDescription(skilllD, model.GetSkillCurrentLevel(skilllD));
		imgPic->SetGfx(GetGfxName(skilllD));
	}
	imgPic->SetHint(skillDsc);
	imgPic->Show(true);
	imgPic->SetData(skilllD);
}

const char* CDlgSkillSubTree::GetGfxName(int skillID) {
	CECHostSkillModel& model = CECHostSkillModel::Instance();
	CECHostSkillModel::enumEvilGod skillType = model.GetSkillEvilGod(skillID);
	if (skillType == CECHostSkillModel::SKILL_BASE) {
		return "界面\\技能边框发光_黄.gfx";
	} else if (skillType == CECHostSkillModel::SKILL_EVIL) {
		return "界面\\技能边框发光_红.gfx";
	} else {
		return "界面\\技能边框发光_蓝.gfx";
	}
}