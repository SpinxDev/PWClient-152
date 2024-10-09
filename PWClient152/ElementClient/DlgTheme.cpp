// Filename	: DlgTheme.cpp
// Creator	: Feng Ning
// Date		: October 19, 2010

#include "DlgTheme.h"
#include "AF.h"
#include "AUISubDialog.h"
#include "AUILabel.h"
#include "AUIStillImageButton.h"
#include "EC_BaseUIMan.h"
#include "EC_UIAnimation.h"
#include "EC_Game.h"
#include "EC_Global.h"
#include "EC_Configs.h"

#include <algorithm>

#define new A_DEBUG_NEW

template<typename T>
inline void AUI_Swap(T& a, T& b) { T tmp = a; a = b; b = tmp; }
#define AUI_SWAP(target, res) AUI_Swap(res, target->res);

CDlgTheme::CDlgTheme()
{
	//	切换界面时，pTmpMgr 加载的对话框也使用此类，因此应避免在此处使用
	//SetOptimizeResourceLoad(true);
}

CDlgTheme::~CDlgTheme()
{
}

bool CDlgTheme::OnInitDialog()
{
	if (!AUIDialog::OnInitDialog())
		return false;

	// load the init animation
	StartAnimation("init");
	
	return true;
}

void CDlgTheme::OnShowDialog()
{
	AUILuaDialog::OnShowDialog();

	// load the show animation
	StartAnimation("show");

	if (!m_ctrlDelayArray.empty()){
		for (size_t u(0); u < m_ctrlDelayArray.size(); ++ u)
		{
			m_ctrlDelayArray[u].Restart();
		}
	}
}

void CDlgTheme::OnHideDialog()
{
	CECBaseUIMan* pUIMan = dynamic_cast<CECBaseUIMan*>(m_pAUIManager);
	pUIMan->RemoveAnimation(this);
	
	AUILuaDialog::OnHideDialog();
}

// Load specific animation
CECUIAnimation* CDlgTheme::StartAnimation(const char* suffix)
{
	CECUIAnimation* pAni = NULL;
	CECBaseUIMan* pUIMan = dynamic_cast<CECBaseUIMan*>(m_pAUIManager);
	if(pUIMan)
	{
		const char* pName = strstr(this->GetName(), "_");
		if(!pName) pName = this->GetName();
		else pName++;

		AString strFile;
		strFile.Format("interfaces\\ani\\%s_%s.dat", pName, suffix);
		pAni = CECUIAnimation::Load(strFile);
		if(pAni)
		{
			pUIMan->StartAnimation(this, pAni);
		}
	}

	return pAni;
}

AString CDlgTheme::FindNextControl(const AString& name)
{
	PAUIOBJECTLISTELEMENT pElement;
	PAUIOBJECT pObj;
	pElement = m_pListHead;
	while( pElement )
	{
		pObj = pElement->pThis;
		if(name != pObj->GetName())
		{
			pElement = pElement->pNext;
			continue;
		}

		return pElement->pNext ? pElement->pNext->pThis->GetName() : "";
	}

	return "";
}

bool CDlgTheme::AddControlBefore(PAUIOBJECT pAUIObject, const AString& objName)
{
	bool bval = false;

	if(objName.IsEmpty() || m_nListSize == 0)
	{
		bval = AddObjectToList(pAUIObject);
	}
	else
	{
		PAUIOBJECTLISTELEMENT pElement;
		PAUIOBJECT pObj;
		pElement = m_pListHead;

		while( pElement )
		{
			pObj = pElement->pThis;

			if(objName != pObj->GetName())
			{
				pElement = pElement->pNext;
				continue;
			}

			PAUIOBJECTLISTELEMENT pNewElement = new AUIOBJECTLISTELEMENT;
			if( !pNewElement ) return false;
			bval = m_AUIObjectTable.put(pAUIObject->GetName(), pAUIObject);
			
			if(bval)
			{
				m_nListSize++;
				
				PAUIOBJECTLISTELEMENT pLast = pElement->pLast;
				if(pLast)
				{
					pLast->pNext = pNewElement;
				}
				else
				{
					m_pListHead = pNewElement;
				}
				
				pNewElement->id = -1; // need to refresh id
				pNewElement->pLast = pLast;
				pNewElement->pThis = pAUIObject;
				pNewElement->pNext = pElement;
				pElement->pLast = pNewElement;
			}
			
			break;
		}
	}

	return bval;
}

bool CDlgTheme::AddControlBySequence(PAUIOBJECT pObj, CDlgTheme* pTheirDlg)
{
	AString pNextName = pTheirDlg->FindNextControl(pObj->GetName());

	if(!pNextName.IsEmpty() && !GetDlgItem(pNextName))
	{
		PAUIOBJECT pNextObj = pTheirDlg->GetDlgItem(pNextName);
		// should in the new dialog item table
		ASSERT( pNextObj );
		if( pNextObj )
		{
			AddControlBySequence(pNextObj, pTheirDlg);
			// should already been added
			ASSERT( GetDlgItem(pNextName) );
		}
	}

	bool bShow = OnChangeLayout(NULL, pObj); // add callback
	pTheirDlg->RemoveControl(pObj);
	pObj->SetParent(this);
	pObj->Show(bShow);

	// remove the resource from old manager
	if(pTheirDlg->GetAUIManager() != this->GetAUIManager())
	{
		// refresh font property because font was connected to old manager
		AUIOBJECT_SETPROPERTY prop;
		pObj->GetProperty("Font", &prop);
		pObj->SetProperty("Font", &prop);

		if(pObj->GetType() == AUIOBJECT_TYPE_IMAGEPICTURE)
		{
			// NOTICE: AUIImagePicture may use this parameter from old manager:
			// FONT_PARAM* fp = GetParent()->GetAUIManager()->GetImagePictureFontParam();
		}		
		else if(pObj->GetType() == AUIOBJECT_TYPE_EDITBOX)
		{
			((PAUIEDITBOX)pObj)->
				SetEmotionList(GetAUIManager()->GetDefaultSpriteEmotion(), GetAUIManager()->GetDefaultEmotion());
		}
		else if(pObj->GetType() == AUIOBJECT_TYPE_LABEL)
		{
			((PAUILABEL)pObj)->
				SetEmotionList(GetAUIManager()->GetDefaultSpriteEmotion(), GetAUIManager()->GetDefaultEmotion());
		}
		else if(pObj->GetType() == AUIOBJECT_TYPE_TEXTAREA)
		{
			((PAUITEXTAREA)pObj)->
				SetEmotionList(GetAUIManager()->GetDefaultSpriteEmotion(), GetAUIManager()->GetDefaultEmotion());
		}
	}

	return AddControlBefore(pObj, pNextName);
}

void CDlgTheme::Resort()
{
	// sort by bubble
	PAUIOBJECTLISTELEMENT pEnd = NULL;
	int maxid = m_nListSize;

	while(pEnd != m_pListHead)
	{
		PAUIOBJECTLISTELEMENT pElement = m_pListHead;
		while(pElement)
		{
			if(pElement->pNext == pEnd)
			{
				if(pEnd == NULL)
				{
					m_pListTail = pElement;
				}

				pEnd = pElement;				
				pEnd->id = --maxid;
				break;
			}
			
			if(pElement->id > pElement->pNext->id)
			{
				if(pElement == m_pListHead)
				{
					m_pListHead = pElement->pNext;
				}

				// swap these two node
				PAUIOBJECTLISTELEMENT lhs = pElement->pLast;
				PAUIOBJECTLISTELEMENT rhs = pElement->pNext->pNext;
				
				pElement->pNext->pLast = lhs;
				pElement->pNext->pNext = pElement;
				
				pElement->pLast = pElement->pNext;
				pElement->pNext = rhs;
				
				if(lhs) lhs->pNext = pElement->pLast;
				if(rhs) rhs->pLast = pElement;
			}
			else
			{
				pElement = pElement->pNext;
			}
		}
	}

	ASSERT(maxid == 0);
}

bool CDlgTheme::ChangeLayout(CDlgTheme* pDlg)
{
	if(!OnChangeLayoutBegin())
	{
		// skip the whole change action
		return true;
	}

	ScopedDialogSpriteModify _dummy(this);

	// for rollback
	typedef abase::hashtab<PAUIOBJECTLISTELEMENT, AString, abase::_hash_function> AUIElementTable;
	AUIElementTable mineHistory(16), theirHistory(16);

	// for transfer
	typedef abase::vector<PAUIOBJECT> AUIObjectVector;
	AUIObjectVector mineOnly, theirOnly;
	
	bool bAllDone = true;

	// change size first
	SIZE oldSize = GetDefaultSize();
	SIZE curSize = pDlg->GetDefaultSize();
	SetDefaultSize(curSize.cx, curSize.cy);
	
	PAUIOBJECTLISTELEMENT their = pDlg->m_pListHead;
	while(their)
	{
		PAUIOBJECT pObj = their->pThis;

		// search for object with same name
		PAUIOBJECTLISTELEMENT mine = m_pListHead;
		while(mine)
		{
			if(AString(pObj->GetName()) != mine->pThis->GetName())
			{
				mine = mine->pNext;
				continue;
			}
			break;
		}

		// try to switch these object
		if(mine)
		{
			bAllDone &= SwitchResource(mine->pThis, pObj);
			if(!bAllDone )
			{
				// assume the failed SwitchResource() will 
				// rollback automatically before return.
				break;
			}

			// store the changed object
			mineHistory.put(pObj->GetName(), mine);
			theirHistory.put(pObj->GetName(), their);
		}
		else
		{
			// object only in new layout
			theirOnly.push_back(their->pThis);
		}

		their = their->pNext;
	}

	AUIElementTable::iterator iter;
	// modified controls
	for(iter = mineHistory.begin(); iter != mineHistory.end(); ++iter)
	{
		PAUIOBJECTLISTELEMENT mine = *iter.value();
		PAUIOBJECT pObj = mine->pThis;

		AUIElementTable::pair_type p;
		p = theirHistory.get(pObj->GetName());
		PAUIOBJECTLISTELEMENT their = *p.first;

		// should always got the matched object
		ASSERT(p.second);
	
		if(!bAllDone || !OnChangeLayout(pObj, their->pThis)) // modify callback
		{
			// rollback the change
			SwitchResource(pObj, their->pThis) ;
		}
		else
		{
			// change the id
			AUI_Swap(mine->id, their->id);
		}
	}

	if(bAllDone)
	{
		// search for object only in current dialog
		PAUIOBJECTLISTELEMENT mine = m_pListHead;
		while(mine)
		{
			PAUIOBJECT pObj = mine->pThis;
			
			AUIDIALOG_AUIOBJECTTABLE::pair_type p;
			p = pDlg->m_AUIObjectTable.get(pObj->GetName());
			if(	!p.second )
			{
				// object that will be remove
				mineOnly.push_back(mine->pThis);
			}

			mine = mine->pNext;
		}

		Resort();
		
		AUIObjectVector::iterator iter;
		// transfer
		for(iter = theirOnly.begin(); iter != theirOnly.end(); ++iter)
		{
			PAUIOBJECT pObj = *iter;

			if(!GetDlgItem(pObj->GetName()))
			{
				AddControlBySequence(pObj, pDlg);
			}
		}
		
		// transfer
		for(iter = mineOnly.begin(); iter != mineOnly.end(); ++iter)
		{
			PAUIOBJECT pObj = *iter;

			if(OnChangeLayout(pObj, NULL)) // remove call back
			{
				TransferControl(pObj, pDlg);
			}
		}

		// refresh the id
		PAUIOBJECTLISTELEMENT pElement = m_pListHead;
		int curid = -1;
		while(pElement)
		{
			pElement->id = ++curid;
			pElement = pElement->pNext;
		}

		// switch other resource on current dialog
		AUI_SWAP(pDlg, m_szFilename);
		AUI_SWAP(pDlg, m_pAUIFrameDlg);
		AUI_SWAP(pDlg, m_pAUIFrameTitle);
		AUI_SWAP(pDlg, m_pA2DSpriteHilight);
		for(size_t i = 0; i < AUIDIALOG_FRAME_NUM; i++ )
		{
			AUI_SWAP(pDlg, m_pA2DSprite[i]);
		}

		// if cannot move then transfer pos
		if(!IsCanMove() || !pDlg->IsCanMove())
		{
			AUI_SWAP(pDlg, m_bCanMove);
			POINT oldPos = GetPos();
			POINT newPos = pDlg->GetPos();
			SetPosEx(newPos.x, newPos.y);
			pDlg->SetPosEx(oldPos.x, oldPos.y);
		}
	}
	else
	{
		SetDefaultSize(oldSize.cx, oldSize.cy);
	}
	
	OnChangeLayoutEnd(bAllDone);

	return bAllDone;
}


bool CDlgTheme::TransferControl(PAUIOBJECT pAUIObject, CDlgTheme* pDlg)
{
	RemoveControl(pAUIObject);
	pAUIObject->SetParent(pDlg);
	return pDlg->AddObjectToList(pAUIObject);
}

bool CDlgTheme::RemoveControl(PAUIOBJECT pAUIObject)
{
	bool bGot = false;
	PAUIOBJECTLISTELEMENT pElement = m_pListHead;
	while( pElement )
	{
		if( pElement->pThis == pAUIObject )
		{
			bGot = true;

			// the mouse on object should be cleared
			//if( m_pAUIManager&& pAUIObject == m_pAUIManager->m_pObjMouseOn)
				//m_pAUIManager->m_pObjMouseOn = NULL;

			if( pAUIObject == m_pObjFocus )
				ChangeFocus(NULL);
			if( pAUIObject == m_pObjCapture)
				m_pObjCapture = NULL;
			if( pAUIObject == m_pObjEvent )
				m_pObjEvent = NULL;
			
			PAUIOBJECTLISTELEMENT pDelete = pElement;
			
			if( pDelete->pLast ) pDelete->pLast->pNext = pDelete->pNext;
			if( pDelete->pNext ) pDelete->pNext->pLast = pDelete->pLast;
			if( pDelete == m_pListHead ) m_pListHead = pDelete->pNext;
			if( pDelete == m_pListTail ) m_pListTail = pDelete->pLast;
			
			size_t s = m_AUIObjectTable.erase(pAUIObject->GetName());
			ASSERT(s);
			
			// do not release when remove only
			//A3DRELEASE(pDelete->pThis);
			delete pDelete;
			
			m_nListSize--;
			if( m_nListSize <= 0 )
			{
				m_pListHead = NULL;
				m_pListTail = NULL;
			}
			
			break;
		}
		pElement = pElement->pNext;
	}
	
	return bGot;
}

// switch the resources on these two object
bool CDlgTheme::SwitchResource(PAUIOBJECT lhs, PAUIOBJECT rhs)
{
	if(!lhs || !rhs)
	{
		return false;
	}

	ASSERT( lhs->GetType() == rhs->GetType() );
	
	AUIOBJECT_SETPROPERTY lhsProp;
	AUIOBJECT_SETPROPERTY rhsProp;

	SIZE oldSize = lhs->GetDefaultSize();
	SIZE newSize = rhs->GetDefaultSize();
	//lhs->SetDefaultSize(newSize.cx, newSize.cy);
	//rhs->SetDefaultSize(oldSize.cx, oldSize.cy);

	// hack code because the above methods don't work...
	oldSize = lhs->GetSize();
	float scale = (GetAUIManager()->GetWindowScale() * GetScale());
	newSize.cx = (long)(newSize.cx * scale);
	newSize.cy = (long)(newSize.cy * scale);
	lhs->SetSize(newSize.cx, newSize.cy);
	rhs->SetSize(oldSize.cx, oldSize.cy);

	POINT oldPos = lhs->GetDefaultPos();
	POINT newPos = rhs->GetDefaultPos();
	lhs->SetDefaultPos(newPos.x, newPos.y);
	rhs->SetDefaultPos(oldPos.x, oldPos.y);
	
	switch(lhs->GetType())
	{
	case AUIOBJECT_TYPE_CHECKBOX:
		AUI_SWAP_RES("Normal Image File");
		AUI_SWAP_RES("Checked Image File");
		AUI_SWAP_RES("OnHover Image File");
		AUI_SWAP_RES("CheckedOnHover Image File");
		AUI_SWAP_RES("Font");
		AUI_SWAP_RES("Align");
		AUI_SWAP_RES("Text Offset X");
		AUI_SWAP_RES("Text Offset Y");
		break;

	case AUIOBJECT_TYPE_COMBOBOX:
		AUI_SWAP_RES("Frame Image");
		AUI_SWAP_RES("Frame Expand Image");
		AUI_SWAP_RES("Frame Pushed Image");
		AUI_SWAP_RES("Hilight Image");
		AUI_SWAP_RES("Frame Mode");
		AUI_SWAP_RES("Font");
		AUI_SWAP_RES("Align");
		break;

	case AUIOBJECT_TYPE_EDITBOX:
	case AUIOBJECT_TYPE_WINDOWPICTURE:
		AUI_SWAP_RES("Frame Image");
		AUI_SWAP_RES("Up Button Image");
		AUI_SWAP_RES("Down Button Image");
		AUI_SWAP_RES("Scroll Area Image");
		AUI_SWAP_RES("Scroll Bar Image");
		AUI_SWAP_RES("Frame Mode");
		AUI_SWAP_RES("Font");
		break;

	case AUIOBJECT_TYPE_IMAGEPICTURE:
		AUI_SWAP_RES("Image File");
		AUI_SWAP_RES("Gfx File");

		// HACK: frame number == 0 will cause data release
		//AUI_SWAP_RES("Frames Number");
		lhs->GetProperty("Frames Number", &lhsProp);
		rhs->GetProperty("Frames Number", &rhsProp);
		if(rhsProp.i == 0) rhsProp.i = 1;
		if(lhsProp.i == 0) lhsProp.i = 1;
		lhs->SetProperty("Frames Number", &rhsProp);
		rhs->SetProperty("Frames Number", &lhsProp);

		AUI_SWAP_RES("Frame Interval");
		AUI_SWAP_RES("Render Top");
		break;

	case AUIOBJECT_TYPE_LABEL:
		AUI_SWAP_RES("Font");
		AUI_SWAP_RES("Align");
		break;

	case AUIOBJECT_TYPE_LISTBOX:
		AUI_SWAP_RES("Frame Image");
		AUI_SWAP_RES("Hilight Image");
		AUI_SWAP_RES("Up Button Image");
		AUI_SWAP_RES("Down Button Image");
		AUI_SWAP_RES("Scroll Area Image");
		AUI_SWAP_RES("Scroll Bar Image");
		AUI_SWAP_RES("Frame Mode");
		AUI_SWAP_RES("Font");
		AUI_SWAP_RES("Align");
		break;

	case AUIOBJECT_TYPE_PROGRESS:
		AUI_SWAP_RES("Frame Image File");
		AUI_SWAP_RES("Fill Image File");
		AUI_SWAP_RES("Pos Image File");
		AUI_SWAP_RES("Inc Image File");
		AUI_SWAP_RES("Dec Image File");
		AUI_SWAP_RES("Frame Mode");
		AUI_SWAP_RES("Fill Margin Left");
		AUI_SWAP_RES("Fill Margin Top");
		AUI_SWAP_RES("Fill Margin Right");
		AUI_SWAP_RES("Fill Margin Bottom");
		break;

	case AUIOBJECT_TYPE_RADIOBUTTON:
		AUI_SWAP_RES("Text Content"); // we switch the text on a button
		AUI_SWAP_RES("Normal Image File");
		AUI_SWAP_RES("Checked Image File");
		AUI_SWAP_RES("OnHover Image File");
		AUI_SWAP_RES("CheckedOnHover Image File");
		AUI_SWAP_RES("Font");
		AUI_SWAP_RES("Align");
		AUI_SWAP_RES("Text Offset X");
		AUI_SWAP_RES("Text Offset Y");
		break;

	case AUIOBJECT_TYPE_SCROLL:
		AUI_SWAP_RES("Up Button Image");
		AUI_SWAP_RES("Down Button Image");
		AUI_SWAP_RES("Scroll Area Image");
		AUI_SWAP_RES("Scroll Bar Image");
		AUI_SWAP_RES("Frame Mode");
		break;

	case AUIOBJECT_TYPE_SLIDER:
		AUI_SWAP_RES("Frame Image File");
		AUI_SWAP_RES("Bar Image File");
		AUI_SWAP_RES("Frame Mode");
		break;

	case AUIOBJECT_TYPE_STILLIMAGEBUTTON:
		AUI_SWAP_RES("Up Frame File");
		AUI_SWAP_RES("Down Frame File");
		AUI_SWAP_RES("OnHover Frame File");
		AUI_SWAP_RES("Disabled Frame File");
		AUI_SWAP_RES("Frame Mode");
		AUI_SWAP_RES("Text Content"); // we switch the text on a button
		AUI_SWAP_RES("Down Gfx File");
		AUI_SWAP_RES("OnHover Gfx File");
		AUI_SWAP_RES("Font");
		AUI_SWAP_RES("Align");
		AUI_SWAP_RES("Text Offset X");
		AUI_SWAP_RES("Text Offset Y");

		// support different color on button
		AUI_SWAP_RES("Text Color");
		AUI_SWAP_RES("Outline");
		AUI_SWAP_RES("Outline Color");
		AUI_SWAP_RES("Inner Text Color");
		break;

	case AUIOBJECT_TYPE_SUBDIALOG: // NOTICE: currently SubDialog didn't support rollback
		{
			rhs->GetProperty("Dialog File", &rhsProp);

			// release the dialog immediately
			PAUISUBDIALOG pMine = (PAUISUBDIALOG)lhs;
			pMine->RemoveDialog();
			m_pAUIManager->Tick(0);
			lhs->SetProperty("Dialog File", &rhsProp);
			m_pAUIManager->Tick(0);

			//
			// NOTICE: enable this code when rhs and lhs use same AUIManager.
			//
			// delete the legacy dialog because it didn't belongs to any AUIManager
			//PAUISUBDIALOG pTheir = (PAUISUBDIALOG)rhs;
			//PAUIDIALOG pRemoved = pTheir->GetSubDialog();
			//A3DRELEASE(pRemoved);
		}

		AUI_SWAP_RES("Up Button Image");
		AUI_SWAP_RES("Down Button Image");
		AUI_SWAP_RES("VScroll Area Image");
		AUI_SWAP_RES("Left Button Image");
		AUI_SWAP_RES("Right Button Image");
		AUI_SWAP_RES("HScroll Area Image");
		AUI_SWAP_RES("VScroll Bar Image");
		AUI_SWAP_RES("HScroll Bar Image");
		break;

	case AUIOBJECT_TYPE_TEXTAREA:
		AUI_SWAP_RES("Frame Image");
		AUI_SWAP_RES("Up Button Image");
		AUI_SWAP_RES("Down Button Image");
		AUI_SWAP_RES("Scroll Area Image");
		AUI_SWAP_RES("Scroll Bar Image");
		AUI_SWAP_RES("Frame Mode");
		AUI_SWAP_RES("Font");
		break;

	case AUIOBJECT_TYPE_TREEVIEW:
		AUI_SWAP_RES("Frame Image");
		AUI_SWAP_RES("Hilight Image");
		AUI_SWAP_RES("Up Button Image");
		AUI_SWAP_RES("Down Button Image");
		AUI_SWAP_RES("Scroll Area Image");
		AUI_SWAP_RES("Scroll Bar Image");
		AUI_SWAP_RES("Plus Symbol Image");
		AUI_SWAP_RES("Minus Symbol Image");
		AUI_SWAP_RES("Leaf Symbol Image");
		AUI_SWAP_RES("Frame Mode");
		AUI_SWAP_RES("Font");
		break;

	case AUIOBJECT_TYPE_VERTICALTEXT:
		AUI_SWAP_RES("Frame Image");
		AUI_SWAP_RES("Left Button Image");
		AUI_SWAP_RES("Right Button Image");
		AUI_SWAP_RES("Scroll Area Image");
		AUI_SWAP_RES("Scroll Bar Image");
		AUI_SWAP_RES("Frame Mode");
		AUI_SWAP_RES("Font");
		AUI_SWAP_RES("Align");
		break;

	default:
		// unknown type
		break;
	}

	return true;
}

void CDlgTheme::ControlEnableDelay::Tick(DWORD dwTickTime)
{
	if (m_dwLeftTime > 0){
		if (m_dwLeftTime <= dwTickTime){
			m_dwLeftTime = 0;
			m_pObj->Enable(true);
		}else{
			m_dwLeftTime -= dwTickTime;
		}
	}
}

void CDlgTheme::ControlEnableDelay::Restart()
{
	m_pObj->Enable(false);
	m_dwLeftTime = m_dwDelayTime;
}

void CDlgTheme::OnTick()
{
	if (!m_ctrlDelayArray.empty()){
		extern CECGame * g_pGame;
		DWORD dwTickTime = g_pGame->GetTickTime();
		for (size_t u(0); u < m_ctrlDelayArray.size(); ++ u)
		{
			m_ctrlDelayArray[u].Tick(dwTickTime);
		}
	}
	AUILuaDialog::OnTick();
}

void CDlgTheme::RegisterEnableDelay(const char *szObjName, DWORD dwDelayTime /* = 300 */)
{
	//	注意，szObjName 的 Enable 和 Disable 应不受其它方式控制，否则不宜注册
	AUIObject *pObj = GetDlgItem(szObjName);
	if (!pObj){
		assert(false);
		return;
	}
	ControlEnableDelayArray::iterator it = std::find(m_ctrlDelayArray.begin(), m_ctrlDelayArray.end(), pObj);
	if (it == m_ctrlDelayArray.end()){
		m_ctrlDelayArray.push_back(ControlEnableDelay(pObj, dwDelayTime));
	}
}

void CDlgTheme::UnregisterEnableDelay(const char *szObjName)
{
	AUIObject *pObj = GetDlgItem(szObjName);
	if (!pObj){
		assert(false);
		return;
	}
	m_ctrlDelayArray.erase(std::remove(m_ctrlDelayArray.begin(), m_ctrlDelayArray.end(), pObj), m_ctrlDelayArray.end());
}

// 让按钮显示一个特效
// 特效文件通过Down Gfx File设置， 为了显示这个Down Gfx，需要设置按钮为按下状态，
// 并设置SetPushLike以让按钮在接受WM_LBUTTONUP消息后能切换按下状态。
// 按钮被点击后恢复按钮被修改的Perproty
void CDlgTheme::StartButtonGfx(const char* button, const char* gfx)
{
	if (!button || !gfx) return;
	PAUISTILLIMAGEBUTTON pBtn = dynamic_cast<PAUISTILLIMAGEBUTTON>(GetDlgItem(button));
	if (pBtn) {
		// 备份原来的Down Gfx和Up Frame
		ButtonBackupMap::iterator iter = m_BtnBackUp.find(pBtn);
		if (iter == m_BtnBackUp.end()) {
			AUIOBJECT_SETPROPERTY p;
			pBtn->GetProperty("Down Gfx File", &p);
			ButtonGfxBackup back;
			back.down_gfx = p.fn;			
			pBtn->GetProperty("Down Frame File", &p);
			back.down_image = p.fn;
			m_BtnBackUp[pBtn] = back;
			AString command(pBtn->GetCommand());
			if (command.GetLength() == 0) command = pBtn->GetName();
			m_CmdBtnMap[command] = pBtn;
		}
		// 设置新的Down Gfx
		AUIOBJECT_SETPROPERTY n;
		strcpy(n.fn, gfx);
		AString strTemp("gfx\\");
		strTemp += gfx;
		strTemp += "old.gfx";
		if (g_pGame->GetConfigs()->GetSystemSettings().iTheme == 0 && af_IsFileExist(strTemp)) 
			strcat(n.fn, "old.gfx");
		else strcat(n.fn, ".gfx");
		pBtn->SetProperty("Down Gfx File", &n);
		pBtn->GetProperty("Up Frame File", &n);
		pBtn->SetProperty("Down Frame File", &n);
		pBtn->SetPushLike(true);
		pBtn->SetPushed(true);
		pBtn->OnDlgItemMessage(WM_LBUTTONDOWN, 0, 0);
	}
}

void CDlgTheme::OnGfxButtonClicked(const char *szCommand)
{
	AString command(szCommand);
	CommandButtonMap::iterator iter = m_CmdBtnMap.find(command);
	if (iter != m_CmdBtnMap.end()) {
		PAUISTILLIMAGEBUTTON pBtn = (PAUISTILLIMAGEBUTTON)(iter->second);
		ButtonBackupMap::iterator iterbackup = m_BtnBackUp.find(pBtn);
		if (iterbackup != m_BtnBackUp.end()){
			const ButtonGfxBackup& back = iterbackup->second;
			// 按钮被点击后停止gfx
			// 恢复原来的Down Gfx和Up Frame
			AUIOBJECT_SETPROPERTY before;
			strcpy(before.fn, back.down_gfx);
			pBtn->SetProperty("Down Gfx File", &before);
			strcpy(before.fn, back.down_image);
			pBtn->SetProperty("Down Frame File", &before);
			if (pBtn->IsPushed()) {
				pBtn->SetPushed(false);
				pBtn->SetState(AUISTILLIMAGEBUTTON_STATE_NORMAL);
			}
			pBtn->SetPushLike(false);
		}
	}
}

// 由于调用了SetOptimizeResourceLoad(true) 导致图片资源在Show(false)时被释放；
// 如果在Show(false)后，仍调用AUILuaDialog::Render()，会引起客户端崩溃。
// 比如CDlgELFProp::Render()内部，本来应放到OnTick中的逻辑代码放到了Render前面，而这部分逻辑代码中有Show(false)操作。
// 为了避免客户端其他地方有类似的处理，或者未来有类似的处理，此处做如下修改
bool CDlgTheme::Render()
{
	if (IsShow()) return AUILuaDialog::Render();
	else return true;
}

//	ScopedSpriteResourceOptimize
ScopedDialogSpriteModify::ScopedDialogSpriteModify(AUIDialog *p)
: m_p(p)
, m_bResourceLoaded(true)
{
	if (m_p && m_p->IsResourceOptimized()){
		m_bResourceLoaded = false;
		m_p->OptimizeSpriteResource(true);	//	加载所有资源、用于恢复现有资源的引用计数
	}
}

ScopedDialogSpriteModify::~ScopedDialogSpriteModify()
{
	if (m_p && !m_bResourceLoaded){
		m_p->OptimizeSpriteResource(false);		//	优化修改后的资源
	}
}

//	ScopedSpriteResourceOptimize
ScopedAUIControlSpriteModify::ScopedAUIControlSpriteModify(AUIObject *p)
: m_p(p)
, m_bResourceLoaded(true)
{
	if (m_p && m_p->IsResourceOptimized()){
		m_bResourceLoaded = false;
		m_p->OptimizeSpriteResource(true);
	}
}

ScopedAUIControlSpriteModify::~ScopedAUIControlSpriteModify()
{
	if (m_p && !m_bResourceLoaded){
		m_p->OptimizeSpriteResource(false);
	}
}
