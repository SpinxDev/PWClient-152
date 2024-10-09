// Filename	: DlgChangeSkirt.cpp
// Creator	: Xu Wenbin
// Date		: 2009/06/01

#include "AFI.h"
#include "AUIImagePicture.h"
#include "AUIDef.h"
#include "AUICTranslate.h"
#include "DlgChangeSkirt.h"
#include "EC_HostPlayer.h"
#include "EC_GameUIMan.h"
#include "EC_IvtrFashion.h"

#define new A_DEBUG_NEW

AUI_BEGIN_COMMAND_MAP(CDlgChangeSkirt, CDlgBase)

AUI_ON_COMMAND("IDCANCEL", OnCommand_CANCEL)
AUI_ON_COMMAND("beginchange", OnCommand_BeginChange)
AUI_ON_COMMAND("stopchange", OnCommand_StopChange)

AUI_END_COMMAND_MAP()


AUI_BEGIN_EVENT_MAP(CDlgChangeSkirt, CDlgBase)

AUI_ON_EVENT("*",  WM_MOUSEWHEEL, OnEventMouseWheel)
AUI_ON_EVENT(NULL, WM_MOUSEWHEEL, OnEventMouseWheel)
AUI_ON_EVENT("Item_*",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("Img_No*",	WM_LBUTTONDOWN,	OnEventLButtonDown)
AUI_ON_EVENT("Item_*",	WM_RBUTTONDOWN,	OnEventRButtonDown)

AUI_END_EVENT_MAP()

CDlgChangeSkirt::CDlgChangeSkirt()
{
}

CDlgChangeSkirt::~CDlgChangeSkirt()
{
}

bool CDlgChangeSkirt::OnInitDialog()
{
	m_pEdit_Hour = GetDlgItem("Edt_Hours");
	m_pEdit_Minute = GetDlgItem("Edt_Minutes");

	int minCoolTime = GetMinAutoFashionTime();
	ACHAR szText[40];

	a_sprintf(szText, _AL("%d"), minCoolTime/60);
	m_pEdit_Hour->SetText(szText);

	a_sprintf(szText, _AL("%d"), minCoolTime%60);
	m_pEdit_Minute->SetText(szText);

	m_pButton_BeginChange = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_BeginChange");
	
	m_pButton_StopChange = (PAUISTILLIMAGEBUTTON)GetDlgItem("Btn_StopChange");
	m_pButton_StopChange->Enable(false);
	
	m_pScroll_Suite = (PAUISCROLL)GetDlgItem("Scl_Right");

	for (int i = 0; i < SuiteSizeHeight; ++ i)
	{
		AString str;
		str.Format("Img_No0%d", i+1);
		m_pImage_Suite[i] = (PAUIIMAGEPICTURE)GetDlgItem(str);
	}

	return true;
}

void CDlgChangeSkirt::OnCommand_CANCEL(const char * szCommand)
{
	Show(false);
}

void CDlgChangeSkirt::OnCommand_BeginChange(const char * szCommand)
{
	// Start auto fashion
	//

	// Check if is already started
	CECHostPlayer *pHost = GetHostPlayer();
	if (pHost->GetAutoFashion())
	{
		return;
	}

	// Get auto fashion time
	int hours = a_atoi(m_pEdit_Hour->GetText());
	int minutes = a_atoi(m_pEdit_Minute->GetText());
	int totalMinutes = hours * 60 + minutes;
	int minAutoFashionTime = GetMinAutoFashionTime();
	int maxAutoFashionTime = GetMaxAutoFashionTime();
	if (hours < 0  ||
		minutes < 0  ||
		minutes > 59 ||
		totalMinutes < minAutoFashionTime ||
		totalMinutes > maxAutoFashionTime)
	{
		ACHAR szText[20];

		a_Clamp(hours, 0, hours);
		a_Clamp(minutes, 0, 59);

		totalMinutes = hours * 60 + minutes;
		a_Clamp(totalMinutes, minAutoFashionTime, maxAutoFashionTime);
		hours = totalMinutes/60;
		minutes = totalMinutes%60;
		
		a_sprintf(szText, _AL("%d"), hours);
		m_pEdit_Hour->SetText(szText);

		a_sprintf(szText, _AL("%d"), minutes);
		m_pEdit_Minute->SetText(szText);

		GetGameUIMan()->AddChatMessage(GetStringFromTable(7833), GP_CHAT_MISC);
		return;
	}

	// Check if we can start auto fashion
	if (pHost->CheckAutoFashionCondition())
		return;

	// Start auto fashion
	pHost->SetCurFashionSuitID(0);
	pHost->StartAutoFashion(totalMinutes);
}

void CDlgChangeSkirt::OnCommand_StopChange(const char * szCommand)
{
	// Stop auto fashion
	//
	CECHostPlayer *pHost = GetHostPlayer();
	if (pHost->GetAutoFashion())
		pHost->StopAutoFashion();
}

void CDlgChangeSkirt::OnEventMouseWheel(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// Scroll fashion short cut display
	//
	if (m_pScroll_Suite->IsShow())
	{
		int zDelta = (short)HIWORD(wParam);
		zDelta /= 120;
		m_pScroll_Suite->SetBarLevel(m_pScroll_Suite->GetBarLevel() - zDelta);
	}
}

void CDlgChangeSkirt::OnEventLButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// Drag move or discard given fashion short cut
	//
	if (strstr(pObj->GetName(), "Item_") &&
		IsFashionShortCutChangeable() &&
		pObj->GetDataPtr() ||
		strstr(pObj->GetName(), "Img_No"))
	{
		A3DVIEWPORTPARAM *p = m_pA3DEngine->GetActiveViewport()->GetParam();	
		CECGameUIMan *pGameUIMan = GetGameUIMan();
		pGameUIMan->m_ptLButtonDown.x = GET_X_LPARAM(lParam) - p->X;
		pGameUIMan->m_ptLButtonDown.y = GET_Y_LPARAM(lParam) - p->Y;
		pGameUIMan->InvokeDragDrop(this, pObj, pGameUIMan->m_ptLButtonDown);
	}
}

void CDlgChangeSkirt::OnEventRButtonDown(WPARAM wParam, LPARAM lParam, AUIObject *pObj)
{
	// Discard fashion short cut with right button
	//
	if (!strstr(pObj->GetName(), "Item_") ||
		!IsFashionShortCutChangeable() ||
		!pObj->GetDataPtr())
		return;

	Discard(pObj);
}

bool CDlgChangeSkirt::Tick()
{
	if (IsShow())
	{
		CECHostPlayer *pHost = GetHostPlayer();
		CECGameUIMan *pGameUIMan = GetGameUIMan();

		// Update button enabled state
		//
		bool autoFashionEnabled = pHost->GetAutoFashion();
		m_pEdit_Hour->Enable(!autoFashionEnabled);
		m_pEdit_Minute->Enable(!autoFashionEnabled);
		m_pButton_BeginChange->Enable(!autoFashionEnabled);
		m_pButton_StopChange->Enable(autoFashionEnabled);

		// Update suite and according items
		//

		// Update scroll size
		int num = pHost->GetMaxFashionSuitNum();
		if (num <= SuiteSizeHeight)
		{
			m_pScroll_Suite->SetScrollRange(0, 0);
			m_pScroll_Suite->Show(false);
		}
		else
		{
			m_pScroll_Suite->SetScrollRange(0, num-SuiteSizeHeight);
			m_pScroll_Suite->SetScrollStep(1);
			m_pScroll_Suite->SetBarLength(-1);
			m_pScroll_Suite->Show(true);
		}

		// Update suite appearance
		int pos = m_pScroll_Suite->GetBarLevel();
		int height = min(SuiteSizeHeight, GetHostPlayer()->GetMaxFashionSuitNum());
		for (int i = 0; i < SuiteSizeHeight; ++ i)
		{
			// Update suite cover
			if (i < height)
			{
				int iconSet = CECGameUIMan::ICONS_SUITE;
				
				A2DSprite * cover = pGameUIMan->m_pA2DSpriteIcons[iconSet];
				
				AString str = GetIconFile(pos+i);
				AString iconFile;
				af_GetFileTitle(str, iconFile);
				int index = pGameUIMan->m_IconMap[iconSet][iconFile];
				m_pImage_Suite[i]->SetCover(cover, index);
				m_pImage_Suite[i]->SetDataPtr((void *)(pos+i+1)); // STORE suite offset in order to be used by short cut
			}

			// Update suite item cover
			int fashionOffset = (pos + i) * SuiteSizeWidth;
			int itemOffset = i * SuiteSizeWidth;
			for (int j = 0; j < SuiteSizeWidth; ++j, ++fashionOffset, ++itemOffset)
			{
				// Get item name, like Item_002
				AString itemName;
				itemName.Format("Item_%d%d%d", itemOffset/100, (itemOffset%100)/10, itemOffset%10);
				PAUIIMAGEPICTURE pItem = (PAUIIMAGEPICTURE)GetDlgItem(itemName);

				if (i < height)
				{
					// Get new fashion associated with the item
					int id = pHost->GetFashionSCSets(fashionOffset);
					int storage2Index(-1);
					CECIvtrFashion *pFashion = pHost->GetFashionByID(id, storage2Index);
					
					// Change item cover
					SetFashionShortCut(pItem, pFashion);
				}
				else if (pItem)	pItem->Show(false);
			}
		}
	}
	
	return CDlgBase::Tick();
}

bool CDlgChangeSkirt::IsFashionShortCutChangeable()
{
	// Check if fashion short cut is editable at current state
	//
	return !GetHostPlayer()->GetAutoFashion();
}

bool CDlgChangeSkirt::IsFashionShortCutExchangeable(PAUIOBJECT pItem1, PAUIOBJECT pItem2)
{
	// Check if given items can store same kind of fashion short cut
	//
	int offset1 = GetFashionShortCutOffset(pItem1);
	int offset2 = GetFashionShortCutOffset(pItem2);
	return offset1 >= 0
		&& offset2 >= 0
		&& (offset1 % SuiteSizeWidth) == (offset2 % SuiteSizeWidth);
}

void CDlgChangeSkirt::Store(PAUIOBJECT pItem, CECIvtrFashion *pFashion)
{
	// Store fashion short cut set
	//
	if (!IsFashionShortCutChangeable())
		return;
	
	CECHostPlayer *pHost = GetHostPlayer();

	// Validate match if not empty
	if (pFashion)
	{
		if (!pHost->CanUseEquipment(pFashion))
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(7831), GP_CHAT_MISC);
			return;
		}

		if (!IsFashionMatch(pItem, pFashion))
		{
			GetGameUIMan()->AddChatMessage(GetStringFromTable(7832), GP_CHAT_MISC);
			return;
		}
	}

	// Store fashion short cut
	SetFashionShortCut(pItem, pFashion);
}

void CDlgChangeSkirt::Replace(PAUIOBJECT pSrcItem, PAUIOBJECT pDstItem)
{
	// Remove source short cut from one place to another
	//

	if (!IsFashionShortCutChangeable() ||
		pSrcItem == pDstItem)
		return;

	if (!IsFashionShortCutExchangeable(pSrcItem, pDstItem))
	{
		GetGameUIMan()->AddChatMessage(GetStringFromTable(7832), GP_CHAT_MISC);
		return;
	}

	CECIvtrFashion *fashion1 = GetFashion(pSrcItem);
	CECIvtrFashion *fashion2 = GetFashion(pDstItem);
	Store(pSrcItem, fashion2);
	Store(pDstItem, fashion1);
}

void CDlgChangeSkirt::Discard(PAUIOBJECT pItem)
{
	// Discard fashion short cut
	//
	if (!IsFashionShortCutChangeable())
		return;

	SetFashionShortCut(pItem, NULL);
}

int CDlgChangeSkirt::GetFashionShortCutOffset(PAUIOBJECT pItem)
{
	// Get corresponding fashion short cut offset with current item
	//
	int offset(-1);
	
	while (true)
	{		
		if (!IsShow())
			break;

		if (!pItem ||
			!strstr(pItem->GetName(), "Item_"))
			break;

		int temp = atoi(pItem->GetName() + strlen("Item_"));
		if (temp < 0 ||
			temp >= SuiteSizeShown)
			break;

		// Take scroll bar as offset
		offset = m_pScroll_Suite->IsShow()
			? m_pScroll_Suite->GetBarLevel() * SuiteSizeWidth
			: 0;

		offset += temp;
		break;
	}

	return offset;
}

void CDlgChangeSkirt::SetFashionShortCut(PAUIOBJECT pItem, CECIvtrFashion *pFashion)
{
	// Save fashion short cut to given item and Host player
	//
	int fashionOffset = GetFashionShortCutOffset(pItem);
	if (fashionOffset < 0)
		return;

	CECHostPlayer *pHost = GetHostPlayer();
	pHost->SetFashionSCSets(fashionOffset, pFashion);
	int id = pHost->GetFashionSCSets(fashionOffset);

	// Set item cover, color, data, hint, etc.

	if (!pFashion)
	{
		// Clear all
		((PAUIIMAGEPICTURE)pItem)->SetCover(NULL, -1);
		pItem->SetColor(A3DCOLORRGB(255, 255, 255));
		pItem->SetText(_AL(""));
		pItem->SetDataPtr(NULL);
		pItem->SetHint(_AL(""));
		return;
	}
	
	CECGameUIMan *pGameUIMan = GetGameUIMan();
	AString iconFile;
	af_GetFileTitle(pFashion->GetIconFile(), iconFile);
	iconFile.MakeLower();
	((PAUIIMAGEPICTURE)pItem)->SetCover(
		pGameUIMan->m_pA2DSpriteIcons[CECGameUIMan::ICONS_INVENTORY],
		pGameUIMan->m_IconMap[CECGameUIMan::ICONS_INVENTORY][iconFile]
		);
	
	pItem->SetColor(A3DCOLORRGB(255, 255, 255));
	
	pItem->SetDataPtr((void *)id);  // STORE color and template id for later use
	
	const wchar_t *pszHint = pFashion->GetDesc(
		CECIvtrItem::DESC_NORMAL,
		pHost->GetEquipment()
		);
	if( pszHint )
	{
		AUICTranslate trans;
		AWString strHint = trans.Translate(pszHint);					
		pItem->SetHint(strHint);
	}
	else
	{
		pItem->SetHint(_AL(""));
	}
}

CECIvtrFashion * CDlgChangeSkirt::GetFashion(PAUIOBJECT pItem)
{
	// Get fashion associated with given item
	//
	DWORD id = (DWORD)pItem->GetDataPtr();
	int index(-1);
	return GetHostPlayer()->GetFashionByID(id, index);
}

bool CDlgChangeSkirt::IsFashionMatch(PAUIOBJECT pItem, CECIvtrFashion *pFashion)
{
	// Check if the type of the fashion matches given item
	//
	if (pItem && pFashion)
	{
		int fashionOffset = GetFashionShortCutOffset(pItem);
		if (fashionOffset >= 0)
		{
			int itemType = fashionOffset % SuiteSizeWidth;
			
			int fashionType = -1;
			__int64 fashionMask = pFashion->GetEquipMask();

			static __int64 s_fashionMask[SuiteSizeWidth] = 
			{
				EQUIP_MASK64_FASHION_HEAD,
					EQUIP_MASK64_FASHION_BODY,
					EQUIP_MASK64_FASHION_LEG,
					EQUIP_MASK64_FASHION_WRIST,
					EQUIP_MASK64_FASHION_FOOT,
					EQUIP_MASK64_FASHION_WEAPON
			};
			for (int i(0); i<SuiteSizeWidth; ++i)
			{
				if (fashionMask & s_fashionMask[i])
				{
					fashionType = i;
					break;
				}
			}
			return itemType == fashionType;
		}
	}
	return false;
}

AString CDlgChangeSkirt::GetIconFile(int iSuite)
{
	// Return suite icon file name that can be used in CECGameUIMan to give
	// AUIImagePicture appearance in Win_ChangeSkirt¡¢Quickbar fashion short cut
	//
	// iSuite starts from 0
	ASSERT(iSuite >= 0 && iSuite < GetHostPlayer()->GetMaxFashionSuitNum());
	AString iconFile;
	iconFile.Format("%d.dds", iSuite+1);
	return iconFile;
}

int CDlgChangeSkirt::GetMinAutoFashionTime()
{
	// Return minimum fashion cool time in MINUTE
	//
	return 1;
}

int CDlgChangeSkirt::GetMaxAutoFashionTime()
{
	// Return maximum fashion cool time in MINUTE
	//
	static int s_MaxCoolTime = 23 * 60 + 59;
	return s_MaxCoolTime;
}