#include "DlgCustomizeEar.h"
#include "EC_CustomizeMgr.h"
#include "AUIListBox.h"
#include "AUISlider.h"
#include "FaceBoneController.h"
#include "EC_CustomizeBound.h"
#include "EC_CustomizeMgr.h"
#include "EC_UIManager.h"
#include "EC_LoginUIMan.h"
#include "EC_Game.h"
#include "elementdataman.h"

#include "A3DViewport.h"
#include "WindowsX.h"
#include "A2DSprite.h"
#include "EC_GameRun.h"


AUI_BEGIN_COMMAND_MAP(CDlgCustomizeEar, CDlgCustomizeBase)

AUI_ON_COMMAND("ChooseBaseEar*", OnCommandChooseEarShape)
AUI_ON_COMMAND("MoveSlider", OnCommandMoveSlider)
AUI_ON_COMMAND("Reset", OnCommandReset)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgCustomizeEar, CDlgCustomizeBase)

AUI_ON_EVENT("Img_*", WM_LBUTTONUP, OnLButtonUpListboxEarShape)
AUI_ON_EVENT("Scroll_*", WM_MOUSEWHEEL, OnMouseWheelListboxEarShape)
AUI_ON_EVENT("Edit_*", WM_MOUSEWHEEL, OnMouseWheelListboxEarShape)
AUI_ON_EVENT("Img_*", WM_MOUSEWHEEL, OnMouseWheelListboxEarShape)

AUI_END_EVENT_MAP()



CDlgCustomizeEar::CDlgCustomizeEar()
{
}

CDlgCustomizeEar::~CDlgCustomizeEar()
{
	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		(*iter)->Release();
		delete *iter;
	}
	m_vecA2DSprite.clear();
}

void CDlgCustomizeEar::OnShowDialog()
{
}

AUIStillImageButton * CDlgCustomizeEar::GetSwithButton()
{
	return GetCustomizeMan()->m_pBtnChooseEar;
}


void CDlgCustomizeEar::LoadDefault()
{

	UpdateData(false);
}

bool CDlgCustomizeEar::InitResource()
{
	return true;
}

void CDlgCustomizeEar::SetSliderBound()
{
	if (!IsContextReady()) return;
	
	CECCustomizeBound * pSliderBound = GetSliderBound();

}


bool CDlgCustomizeEar::OnInitContext()
{
	if (!CDlgCustomizeBase::OnInitContext())
		return false;

	ClearListCtrlAndA2DSprite();

	if (!m_ListBoxEar.Init(this, "Img_01", 2, 3, "Scroll_01", "Edit_01"))
		return false;

	if (!InitResource())
		return false;

	SetSliderBound();

	OnCommandChooseEarShape("ChooseBaseEar1");
	
	// init member
	LoadDefault();
	
	return true;
}

void CDlgCustomizeEar::DoDataExchange(bool bSave)
{
	CDlgCustomizeBase::DoDataExchange(bSave);

	// ddx control
	
	// ddx data
	if (!IsContextReady()) return;

}


void CDlgCustomizeEar::OnCommandChooseEarShape(const char *pCommand)
{
	// only one button avaliable, always make it pushed
	CheckStillImageButton(0, 1, "Btm_ChooseBaseEar1");
}

void CDlgCustomizeEar::OnCommandMoveSlider(const char *szCommand)
{
	BEGIN_SLIDER_TIME_CHECKED;

	UpdateData(true);

	DoCalculate();

	END_SLIDER_TIME_CHECKED;
}

void CDlgCustomizeEar::OnLButtonUpListboxEarShape(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	if (!IsContextReady()){
		return;
	}

	// 判断是否点中列表框中的选项
	PAUIOBJECT pObjUseful = m_ListBoxEar.OnMouse_Clicked(wParam, lParam, pObj);
	if (pObjUseful)
	{
		// 选中耳形更新
		GetFaceData()->idEarShape = (int)pObjUseful->GetData();
		
		ResetEarToNormal();
		
		UpdateData(false);
		DoCalculate();
	}
}

void CDlgCustomizeEar::OnMouseWheelListboxEarShape(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	m_ListBoxEar.OnMouse_Wheel(wParam, lParam, pObj);
}

void CDlgCustomizeEar::OnTick()
{
	m_ListBoxEar.OnTick();
}

void CDlgCustomizeEar::ResetEarToNormal(void)
{
	GetFaceData()->offsetEarV = 128;
	GetFaceData()->scaleEar = 128;
}

void CDlgCustomizeEar::OnCommandReset(const char *szCommand)
{
	LoadDefault();
	DoCalculate();
}

void CDlgCustomizeEar::ClearListCtrlAndA2DSprite()
{
	m_ListBoxEar.ResetContent();

	VecA2DSpriteIter_t iter = m_vecA2DSprite.begin();
	for( ; iter != m_vecA2DSprite.end(); ++iter)
	{
		if(*iter)
		{
			(*iter)->Release();
			delete *iter;
		}
	}
	m_vecA2DSprite.clear();
}