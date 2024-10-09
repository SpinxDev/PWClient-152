/*
 * FILE: EC_CustomizeMgr.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/8/18
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

//	For WM_MOUSEWHEEL message
#pragma warning (disable: 4284)

#define _WIN32_WINDOWS	0x0500

#include "EC_CustomizeMgr.h"
#include "EC_Player.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_ShadowRender.h"
#include "EC_GameSession.h"
#include "EC_Viewport.h"
#include "EC_World.h"
#include "EC_Player.h"
#include "EC_LoginPlayer.h"
#include "EC_Face.h"
#include "EC_UIManager.h"
#include "EC_CommandLine.h"
#include "EC_UIHelper.h"
#include "EC_CustomizePolicy.h"

#include "AUIManager.h"
#include "DlgCustomizeFace.h"
#include "EC_Face.h"
#include "EC_CustomizeBound.h"
#include "elementdataman.h"
#include "EC_Model.h"

#include "AUIStillImageButton.h"
#include "DlgCustomizeEar.h"
#include "DlgCustomizeEye.h"
#include "DlgCustomizeThirdEye.h"
#include "DlgCustomizeEyebrow.h"
#include "DlgCustomizeFace.h"
#include "DlgCustomizeHair.h"
#include "DlgCustomizeMouth.h"
#include "DlgCustomizeNose.h"
#include "DlgCustomizePaint.h"
#include "DlgCustomizePre.h"
#include "DlgCustomizePreForFree.h"
#include "DlgCustomizeStature.h"
#include "DlgCustomizeAdv.h"
#include "DlgCustomizeUserPic.h"
#include "DlgCustomizeMoveCamera.h"
#include "DlgCustomize.h"
#include "DlgCustomizeChsPre.h"
#include "DlgFacename.h"

#include <AFI.h>
#include "A3DEngine.h"
#include "A3DViewport.h"
#include "windowsx.h"

//-------------------------------------------------------------------
CECCustomizeMgr::CECCustomizeMgr()
: m_pSliderBound(NULL)
, m_pAUIManager(NULL)
, m_pCurPlayer(NULL)
, m_vNPCPos(0.0f)
, m_vNPCDir(g_vAxisY)
, m_bUnsymmetrical(false)
, m_bLoadFacePillData(false)
, m_bDragRole(false)
, m_bDragCamera(false)
, m_pSpriteImageSelect(NULL)
, m_pSpriteImageMouseOn(NULL)
, m_pDlgCustomizePre(NULL)
, m_pDlgCustomizeStature(NULL)
, m_pDlgCustomizeEye(NULL)
, m_pDlgCustomizeEye2(NULL)
, m_pDlgCustomizeFace(NULL)
, m_pDlgCustomizeFace2(NULL)
, m_pDlgCustomizeHair(NULL)
, m_pDlgCustomizeNoseMouth(NULL)
, m_pDlgCustomizeNoseMouth2(NULL)
, m_pDlgCustomizePaint(NULL)
, m_pCurDialog(NULL)
, m_pDlgCustomize(NULL)
, m_pDlgFacename(NULL)
, m_pBtnChoosePreCustomize(NULL)
, m_pBtnChooseFace(NULL)
, m_pBtnChooseEye(NULL)
, m_pBtnChooseNoseMouth(NULL)
, m_pBtnChooseHair(NULL)
, m_pBtnChooseFacePainting(NULL)
, m_pBtnCurChoose(NULL)
, m_pCustomizePolicy(NULL)
{
	m_fMoveZDist = 0;
	m_fMoveXDist = 0;
	m_fMoveYDist = 0;

	m_vPosDaltaM = A3DVECTOR3(0.0f, 0.7f, -12.0f);
	m_vPosDaltaF = A3DVECTOR3(0.0f, 0.5f, -13.0f);
}
//-------------------------------------------------------------------
void CECCustomizeMgr::Release()
{
	A3DRELEASE(m_pSpriteImageSelect);
	A3DRELEASE(m_pSpriteImageMouseOn);
}

bool CECCustomizeMgr::Test(const CustomizeInitParam_t &curInitParam)const
{
	bool bSame(false);
	while (IsWorking())
	{
		if (curInitParam.pAUImanager != m_pAUIManager ||
			curInitParam.pCurPlayer != m_pCurPlayer){
			break;
		}
		CECCustomizeBound *pSliderBound = GetSliderBound(curInitParam.nFaceTicketID);
		if (pSliderBound != m_pSliderBound){
			break;
		}
		bSame = true;
		break;
	}
	return bSame;
}

CECCustomizeBound * CECCustomizeMgr::GetSliderBound(int nFaceTicketID)const
{
	CECCustomizeBound * pSliderBound = NULL;
	if (nFaceTicketID == FACE_TICKET_DEFAULT){
		pSliderBound = g_CustomizeBoundMgr.GetCustomizeBound("facedata\\整容卷\\普通整容卷限制参数.ini");
	}else{			
		elementdataman* pElementDataMan = g_pGame->GetElementDataMan ();
		DATA_TYPE dtFaceData;
		FACETICKET_ESSENCE* pFaceTicket = (FACETICKET_ESSENCE*)pElementDataMan->get_data_ptr(nFaceTicketID, ID_SPACE_ESSENCE, dtFaceData);		
		if(pFaceTicket){
			pSliderBound = g_CustomizeBoundMgr.GetCustomizeBound(pFaceTicket->bound_file);
		}else{
			assert(0 && "Face Ticket not found !");
		}
	}
	return pSliderBound;
}

//--------------------------------------------------------------------
bool CECCustomizeMgr::Init(CustomizeInitParam_t cusInitParam, CECCustomizePolicy *policy)
{
	m_pCustomizePolicy = policy;
	m_pAUIManager = cusInitParam.pAUImanager;
	m_pCurPlayer = cusInitParam.pCurPlayer;
	
	m_vPlayerSavedPos = m_pCurPlayer->GetPos();
	m_vPlayerSavedDir = m_pCurPlayer->GetDir();
	m_vPlayerSavedUp = m_pCurPlayer->GetUp();

	m_vNPCPos = cusInitParam.vPos;
	m_vNPCDir = cusInitParam.vDir;	
	
	if (m_pCustomizePolicy->ShouldHideAllDialog()){
		HideAllOldDlg();
	}
	
	do {		
		m_pSliderBound = GetSliderBound(cusInitParam.nFaceTicketID);		
		if (!m_pSliderBound) 
		{
			assert(0 && "Couldn't Load the CustomizeBound File!");
			break;
		}

		if( cusInitParam.nFaceTicketID == FACE_TICKET_DEFAULT)
		{
			m_bUnsymmetrical = false;
		}
		else 
		{
			elementdataman* pElementDataMan;
			pElementDataMan= g_pGame->GetElementDataMan ();
			DATA_TYPE dtFaceData;
			FACETICKET_ESSENCE* pFaceTicket = (FACETICKET_ESSENCE*)pElementDataMan->get_data_ptr(cusInitParam.nFaceTicketID, ID_SPACE_ESSENCE, dtFaceData);
			
			if(pFaceTicket)
			{				
				if( pFaceTicket->unsymmetrical == 0)
				{
					m_bUnsymmetrical = false;
				}
				else
				{
					m_bUnsymmetrical = true;
				}
			}
			else
			{
				assert(0 && "Face Ticket not found !");
			}
		}
		
		//判断是否载入变形丸特有的数据
		if (CECCommandLine::GetLoadFacePillData())
		{
			m_bLoadFacePillData = true;
		}

		m_pCurPlayer->StoreCustomizeData();
		
		if (!InitDefaultData()) break;
		
		if (!InitDialogs()) break;
		
		if (!InitSwithButtons()) break;
		
		return true;

	} while(false);

	return false;
}

void CECCustomizeMgr::HideRelatedDialogs()
{
	PAUIDIALOG dlgs[] = {
		m_pDlgCustomizePre,
		m_pDlgCustomizeStature,
		m_pDlgCustomizeEye,
		m_pDlgCustomizeEye2,
		m_pDlgCustomizeFace,
		m_pDlgCustomizeFace2,
		m_pDlgCustomizeHair,
		m_pDlgCustomizeNoseMouth,
		m_pDlgCustomizeNoseMouth2,
		m_pDlgCustomizePaint,
		m_pDlgCustomize,
		m_pDlgFacename,
	};
	for (int i(0); i < ARRAY_SIZE(dlgs); ++ i)
	{
		PAUIDIALOG pDlg = dlgs[i];
		if (pDlg->IsShow()){
			pDlg->Show(false);
		}
	}
}

bool CECCustomizeMgr::IsWorking()const
{
	return (m_pCurDialog && m_pCurDialog->IsShow());
}

void CECCustomizeMgr::Finish()
{
	m_pCustomizePolicy = NULL;
	m_pAUIManager = NULL;
	m_pCurPlayer = NULL;
	m_pSliderBound = NULL;
	m_vecOldDlg.clear();
}

void CECCustomizeMgr::RestorePlayer(CECPlayer *pPlayer)const
{
	if (pPlayer){
		pPlayer->SetPos(m_vPlayerSavedPos);
		pPlayer->ChangeModelMoveDirAndUp(m_vPlayerSavedDir, m_vPlayerSavedUp);
	}
}

void CECCustomizeMgr::SwitchDialog(const char * szCommand)
{
	AString strCommand = szCommand;

	CDlgCustomizeBase *pDlg = NULL;
	if (strCommand == "ChoosePreCustomize")
		pDlg = m_pDlgCustomizePre;
	else if (strCommand == "ChooseFace")
		pDlg = m_pDlgCustomizeFace;
	else if (strCommand == "ChooseFace2")
		pDlg = m_pDlgCustomizeFace2;
	else if (strCommand == "ChooseEye")
		pDlg = m_pDlgCustomizeEye;
	else if (strCommand == "ChooseEye2")
		pDlg = m_pDlgCustomizeEye2;
	else if (strCommand == "ChooseNoseMouth")
		pDlg = m_pDlgCustomizeNoseMouth;
	else if (strCommand == "ChooseNoseMouth2")
		pDlg = m_pDlgCustomizeNoseMouth2;
	else if (strCommand == "ChooseHair")
		pDlg = m_pDlgCustomizeHair;
	else if (strCommand == "ChooseFacePainting")
		pDlg = m_pDlgCustomizePaint;
	else if (strCommand == "ChooseStature")
		pDlg = m_pDlgCustomizeStature;

	ASSERT(pDlg);

	// set button state
	m_pBtnCurChoose->SetPushed(false);
	
	if( pDlg->GetSwithButton())
	{
		m_pBtnCurChoose = pDlg->GetSwithButton();
	}

	m_pBtnCurChoose->SetPushed(true);

	// hide/show dialogs
	m_pCurDialog->Show(false);
	m_pCurDialog = pDlg;
	m_pCurDialog->Show(true);

	//Set move camera var
	m_fMoveZDist = 0;
	m_fMoveXDist = 0;
	m_fMoveYDist = 0;
}


bool CECCustomizeMgr::InitDefaultData(void)
{
	CECFace* pCurFace = m_pCurPlayer->GetFaceModel();

	CECFace::FACE_CUSTOMIZEDATA* pFaceData = pCurFace->GetFaceData();
	
	//脸盘融合
	

	//3庭3
	m_nDefaultScaleUp = MakeSureSlider(pFaceData->scaleUp, m_pSliderBound->m_nScaleUpPartMax, m_pSliderBound->m_nScaleUpPartMin);
	m_nDefaultScaleMiddle = MakeSureSlider(pFaceData->scaleMiddle, m_pSliderBound->m_nScaleMiddlePartMax, m_pSliderBound->m_nScaleMiddlePartMin);
	m_nDefaultScaleDown = MakeSureSlider(pFaceData->scaleDown, m_pSliderBound->m_nScaleDownPartMax, m_pSliderBound->m_nScaleDownPartMin);

	//脸盘缩放2
	m_nDefaultScaleFaceH = MakeSureSlider(pFaceData->scaleFaceH, m_pSliderBound->m_nScaleFaceHMax, m_pSliderBound->m_nScaleFaceHMin);
	m_nDefaultScaleFaceV = MakeSureSlider(pFaceData->scaleFaceV, m_pSliderBound->m_nScaleFaceVMax, m_pSliderBound->m_nScaleFaceVMin);

	//眼睛14
	m_nDefaultScaleEyeH = MakeSureSlider(pFaceData->scaleEyeH, m_pSliderBound->m_nScaleEyeHMax, m_pSliderBound->m_nScaleEyeHMin);
	m_nDefaultScaleEyeV = MakeSureSlider(pFaceData->scaleEyeV, m_pSliderBound->m_nScaleEyeVMax, m_pSliderBound->m_nScaleEyeVMin);
	m_nDefaultRotateEye = MakeSureSlider(pFaceData->rotateEye, m_pSliderBound->m_nRotateEyeMax, m_pSliderBound->m_nRotateEyeMin);
	m_nDefaultOffsetEyeH = MakeSureSlider(pFaceData->offsetEyeH, m_pSliderBound->m_nOffsetEyeHMax, m_pSliderBound->m_nOffsetEyeHMin);
	m_nDefaultOffsetEyeV = MakeSureSlider(pFaceData->offsetEyeV, m_pSliderBound->m_nOffsetEyeVMax, m_pSliderBound->m_nOffsetEyeVMin);
	m_nDefaultOffsetEyeZ = MakeSureSlider(pFaceData->offsetEyeZ, m_pSliderBound->m_nOffsetEyeZMax, m_pSliderBound->m_nOffsetEyeZMin);
	m_nDefaultScaleEyeBall = MakeSureSlider(pFaceData->scaleEyeBall, m_pSliderBound->m_nScaleEyeBallMax, m_pSliderBound->m_nScaleEyeBallMin);

	m_nDefaultScaleEyeH2 = MakeSureSlider(pFaceData->scaleEyeH2, m_pSliderBound->m_nScaleEyeHMax, m_pSliderBound->m_nScaleEyeHMin);
	m_nDefaultScaleEyeV2 = MakeSureSlider(pFaceData->scaleEyeV2, m_pSliderBound->m_nScaleEyeVMax, m_pSliderBound->m_nScaleEyeVMin);
	m_nDefaultRotateEye2 = MakeSureSlider(pFaceData->rotateEye2, m_pSliderBound->m_nRotateEyeMax, m_pSliderBound->m_nRotateEyeMin);
	m_nDefaultOffsetEyeH2 = MakeSureSlider(pFaceData->offsetEyeH2, m_pSliderBound->m_nOffsetEyeHMax, m_pSliderBound->m_nOffsetEyeHMin);
	m_nDefaultOffsetEyeV2 = MakeSureSlider(pFaceData->offsetEyeV2, m_pSliderBound->m_nOffsetEyeVMax, m_pSliderBound->m_nOffsetEyeVMin);
	m_nDefaultOffsetEyeZ2 = MakeSureSlider(pFaceData->offsetEyeZ2, m_pSliderBound->m_nOffsetEyeZMax, m_pSliderBound->m_nOffsetEyeZMin);
	m_nDefaultScaleEyeBall2 = MakeSureSlider(pFaceData->scaleEyeBall2, m_pSliderBound->m_nScaleEyeBallMax, m_pSliderBound->m_nScaleEyeBallMin);


	//眉毛6
	m_nDefaultScaleBrowH = MakeSureSlider(pFaceData->scaleBrowH, m_pSliderBound->m_nScaleBrowHMax, m_pSliderBound->m_nScaleBrowHMin);
	m_nDefaultScaleBrowV = MakeSureSlider(pFaceData->scaleBrowV, m_pSliderBound->m_nScaleBrowVMax, m_pSliderBound->m_nScaleBrowVMin);
	m_nDefaultRotateBrow = MakeSureSlider(pFaceData->rotateBrow, m_pSliderBound->m_nRotateBrowMax, m_pSliderBound->m_nRotateBrowMin);
	m_nDefaultOffsetBrowH = MakeSureSlider(pFaceData->offsetBrowH, m_pSliderBound->m_nOffsetBrowHMax, m_pSliderBound->m_nOffsetBrowHMin);
	m_nDefaultOffsetBrowV = MakeSureSlider(pFaceData->offsetBrowV, m_pSliderBound->m_nOffsetBrowVMax, m_pSliderBound->m_nOffsetBrowVMin);
	m_nDefaultOffsetBrowZ = MakeSureSlider(pFaceData->offsetBrowZ, m_pSliderBound->m_nOffsetBrowZMax, m_pSliderBound->m_nOffsetBrowZMin);
	

	m_nDefaultScaleBrowH2 = MakeSureSlider(pFaceData->scaleBrowH2, m_pSliderBound->m_nScaleBrowHMax, m_pSliderBound->m_nScaleBrowHMin);
	m_nDefaultScaleBrowV2 = MakeSureSlider(pFaceData->scaleBrowV2, m_pSliderBound->m_nScaleBrowVMax, m_pSliderBound->m_nScaleBrowVMin);
	m_nDefaultRotateBrow2 = MakeSureSlider(pFaceData->rotateBrow2, m_pSliderBound->m_nRotateBrowMax, m_pSliderBound->m_nRotateBrowMin);
	m_nDefaultOffsetBrowH2 = MakeSureSlider(pFaceData->offsetBrowH2, m_pSliderBound->m_nOffsetBrowHMax, m_pSliderBound->m_nOffsetBrowHMin);
	m_nDefaultOffsetBrowV2 = MakeSureSlider(pFaceData->offsetBrowV2, m_pSliderBound->m_nOffsetBrowVMax, m_pSliderBound->m_nOffsetBrowVMin);
	m_nDefaultOffsetBrowZ2 = MakeSureSlider(pFaceData->offsetBrowZ2, m_pSliderBound->m_nOffsetBrowZMax, m_pSliderBound->m_nOffsetBrowZMin);

	//鼻子5
	m_nDefaultScaleNoseTipH = MakeSureSlider(pFaceData->scaleNoseTipH, m_pSliderBound->m_nScaleNoseTipHMax, m_pSliderBound->m_nScaleNoseTipHMin);
	m_nDefaultScaleNoseTipV = MakeSureSlider(pFaceData->scaleNoseTipV, m_pSliderBound->m_nScaleNoseTipVMax, m_pSliderBound->m_nScaleNoseTipVMin);
	m_nDefaultScaleNoseTipZ = MakeSureSlider(pFaceData->scaleNoseTipZ, m_pSliderBound->m_nScaleNoseTipZMax, m_pSliderBound->m_nScaleNoseTipZMin);
	m_nDefaultOffsetNoseTipV = MakeSureSlider(pFaceData->offsetNoseTipV, m_pSliderBound->m_nOffsetNoseTipVMax, m_pSliderBound->m_nOffsetNoseTipVMin);
	m_nDefaultScaleBridgeTipH = MakeSureSlider(pFaceData->scaleBridgeTipH, m_pSliderBound->m_nScaleBridgeTipHMax, m_pSliderBound->m_nScaleBridgeTipHMin);
	m_nDefaultOffsetBridegTipZ = MakeSureSlider(pFaceData->offsetBridgeTipZ, m_pSliderBound->m_nOffsetBridgeTipZMax, m_pSliderBound->m_nOffsetBridgeTipZMin);
	
	//嘴7
	m_nDefaultThickUpLip = MakeSureSlider(pFaceData->thickUpLip, m_pSliderBound->m_nThickUpLipMax, m_pSliderBound->m_nThickUpLipMin);
	m_nDefaultThickDownLip = MakeSureSlider(pFaceData->thickDownLip, m_pSliderBound->m_nThickDownLipMax, m_pSliderBound->m_nThickDownLipMin);
	m_nDefaultScaleMouthH = MakeSureSlider(pFaceData->scaleMouthH, m_pSliderBound->m_nScaleMouthHMax, m_pSliderBound->m_nScaleMouthHMin);
	m_nDefaultOffsetMouthV = MakeSureSlider(pFaceData->offsetMouthV, m_pSliderBound->m_nOffsetMouthVMax, m_pSliderBound->m_nOffsetMouthVMin);
	m_nDefaultOffsetMouthZ = MakeSureSlider(pFaceData->offsetMouthZ, m_pSliderBound->m_nOffsetMouthZMax, m_pSliderBound->m_nOffsetMouthZMin);
	m_nDefaultOffsetCornerOfMouthSpecial = MakeSureSlider(pFaceData->offsetCornerOfMouthSpecial, m_pSliderBound->m_nOffsetCornerOfMouthSpecialMax, m_pSliderBound->m_nOffsetCornerOfMouthSpecialMin);

	m_nDefaultScaleMouthH2 = MakeSureSlider(pFaceData->scaleMouthH2, m_pSliderBound->m_nScaleMouthH2Max, m_pSliderBound->m_nScaleMouthH2Min);
	m_nDefaultOffsetCornerOfMouthSpecial2 = MakeSureSlider(pFaceData->offsetCornerOfMouthSpecial2, m_pSliderBound->m_nOffsetCornerOfMouthSpecial2Max, m_pSliderBound->m_nOffsetCornerOfMouthSpecial2Min);

	//耳朵2
	m_nDefaultScaleEar = MakeSureSlider(pFaceData->scaleEar, m_pSliderBound->m_nScaleEarMax, m_pSliderBound->m_nScaleEarMin);
	m_nDefaultOffsetEarV = MakeSureSlider(pFaceData->offsetEarV, m_pSliderBound->m_nOffsetEarVMax, m_pSliderBound->m_nOffsetEarVMin);
	
	//额头5
	m_nDefaultOffsetForeheadH = MakeSureSlider(pFaceData->offsetForeheadH, m_pSliderBound->m_nOffsetForeheadHMax, m_pSliderBound->m_nOffsetForeheadHMin);
	m_nDefaultOffsetForeheadV = MakeSureSlider(pFaceData->offsetForeheadV, m_pSliderBound->m_nOffsetForeheadVMax, m_pSliderBound->m_nOffsetForeheadVMin);
	m_nDefaultOffsetForeheadZ = MakeSureSlider(pFaceData->offsetForeheadZ, m_pSliderBound->m_nOffsetForeheadZMax, m_pSliderBound->m_nOffsetForeheadZMin);
	m_nDefaultRotateForehead = MakeSureSlider(pFaceData->rotateForehead, m_pSliderBound->m_nRotateForeheadMax, m_pSliderBound->m_nRotateForeheadMin);
	m_nDefaultScaleForehead = MakeSureSlider(pFaceData->scaleForehead, m_pSliderBound->m_nScaleForeheadMax, m_pSliderBound->m_nScaleForeheadMin);

	//颧骨5
	m_nDefaultOffsetYokeBoneH = MakeSureSlider(pFaceData->offsetYokeBoneH, m_pSliderBound->m_nOffsetYokeBoneHMax, m_pSliderBound->m_nOffsetYokeBoneHMin);
	m_nDefaultOffsetYokeBoneV = MakeSureSlider(pFaceData->offsetYokeBoneV, m_pSliderBound->m_nOffsetYokeBoneVMax, m_pSliderBound->m_nOffsetYokeBoneVMin);
	m_nDefaultOffsetYokeBoneZ = MakeSureSlider(pFaceData->offsetYokeBoneZ, m_pSliderBound->m_nOffsetYokeBoneZMax, m_pSliderBound->m_nOffsetYokeBoneZMin);
	m_nDefaultRotateYokeBone = MakeSureSlider(pFaceData->rotateYokeBone, m_pSliderBound->m_nRotateYokeBoneMax, m_pSliderBound->m_nRotateYokeBoneMin);
	m_nDefaultScaleYokeBone = MakeSureSlider(pFaceData->scaleYokeBone, m_pSliderBound->m_nScaleYokeBoneMax, m_pSliderBound->m_nScaleYokeBoneMin);

	//脸颊4
	m_nDefaultOffsetCheekH = MakeSureSlider(pFaceData->offsetCheekH, m_pSliderBound->m_nOffsetCheekHMax, m_pSliderBound->m_nOffsetCheekHMin);
	m_nDefaultOffsetCheekV = MakeSureSlider(pFaceData->offsetCheekV, m_pSliderBound->m_nOffsetCheekVMax, m_pSliderBound->m_nOffsetCheekVMin);
	m_nDefaultOffsetCheekZ = MakeSureSlider(pFaceData->offsetCheekZ, m_pSliderBound->m_nOffsetCheekZMax, m_pSliderBound->m_nOffsetCheekZMin);
	m_nDefaultScaleCheek = MakeSureSlider(pFaceData->scaleCheek, m_pSliderBound->m_nScaleCheekMax, m_pSliderBound->m_nScaleCheekMin);

	//下巴4
	m_nDefaultOffsetChainV = MakeSureSlider(pFaceData->offsetChainV, m_pSliderBound->m_nOffsetChainVMax, m_pSliderBound->m_nOffsetChainVMin);
	m_nDefaultOffsetChainZ = MakeSureSlider(pFaceData->offsetChainZ, m_pSliderBound->m_nOffsetChainZMax, m_pSliderBound->m_nOffsetChainZMin);
	m_nDefaultRotateChain = MakeSureSlider(pFaceData->rotateChain, m_pSliderBound->m_nRotateBrowMax, m_pSliderBound->m_nRotateBrowMin);
	m_nDefaultScaleChainH = MakeSureSlider(pFaceData->scaleChainH, m_pSliderBound->m_nScaleChainHMax, m_pSliderBound->m_nScaleChainHMin);

	//颌骨6
	m_nDefaultOffsetJawH = MakeSureSlider(pFaceData->offsetJawH, m_pSliderBound->m_nOffsetJawHMax, m_pSliderBound->m_nOffsetJawHMin);
	m_nDefaultOffsetJawV = MakeSureSlider(pFaceData->offsetJawV, m_pSliderBound->m_nOffsetJawVMax, m_pSliderBound->m_nOffsetJawVMin);
	m_nDefaultOffsetJawZ = MakeSureSlider(pFaceData->offsetJawZ, m_pSliderBound->m_nOffsetJawZMax, m_pSliderBound->m_nOffsetJawZMin);
	m_nDefaultScaleJawSpecial = MakeSureSlider(pFaceData->scaleJawSpecial, m_pSliderBound->m_nScaleJawSpecialMax, m_pSliderBound->m_nScaleJawSpecialMin);
	m_nDefaultScaleJawH = MakeSureSlider(pFaceData->scaleJawH, m_pSliderBound->m_nScaleJawHMax, m_pSliderBound->m_nScaleJawHMin);
	m_nDefaultScaleJawV = MakeSureSlider(pFaceData->scaleJawV, m_pSliderBound->m_nScaleJawVMax, m_pSliderBound->m_nScaleJawVMin);
	

	CECPlayer::PLAYER_CUSTOMIZEDATA pCustomizeData = m_pCurPlayer->GetCustomizeData();

	//身材
	m_nDefaultHeadScale =MakeSureSlider(pCustomizeData.headScale, m_pSliderBound->m_nHeadScaleMax, m_pSliderBound->m_nHeadScaleMin);
	m_nDefaultUpScale = MakeSureSlider(pCustomizeData.upScale, m_pSliderBound->m_nUpScaleMax, m_pSliderBound->m_nUpScaleMin);
	m_nDefaultWaistScale = MakeSureSlider(pCustomizeData.waistScale, m_pSliderBound->m_nWaistScaleMax, m_pSliderBound->m_nWaistScaleMin);
	m_nDefaultArmWidth =MakeSureSlider(pCustomizeData.armWidth, m_pSliderBound->m_nArmWidthMax, m_pSliderBound->m_nArmWidthMin);
	m_nDefaultLegWidth = MakeSureSlider(pCustomizeData.legWidth, m_pSliderBound->m_nLegWidthMax, m_pSliderBound->m_nLegWidthMin);
	m_nDefaultBreastScale = MakeSureSlider(pCustomizeData.breastScale, m_pSliderBound->m_nBreastScaleMax, m_pSliderBound->m_nBreastScaleMin);

	return true;
}


int CECCustomizeMgr::MakeSureSlider(unsigned char &level, int nMax, int nMin)
{
	if( level < nMin)
	{
		level = nMin;
		
	}
	if( level > nMax)
	{
		level = nMax;
		
	}
	
	return level;
}

#define INIT_BUTTON_POINTER(variableName, dialogPointer, buttonName) \
{ \
	variableName = dynamic_cast<AUIStillImageButton *>( \
		dialogPointer->GetDlgItem(buttonName)); \
	ASSERT(variableName); \
}

bool CECCustomizeMgr::InitSwithButtons()
{
	INIT_BUTTON_POINTER(m_pBtnChoosePreCustomize, m_pDlgCustomizePre, "Btm_ChoosePreCustomize");
	INIT_BUTTON_POINTER(m_pBtnChooseFace, m_pDlgCustomizeFace, "Btm_ChooseFace");
	INIT_BUTTON_POINTER(m_pBtnChooseEye, m_pDlgCustomizeEye, "Btm_ChooseEye");
	INIT_BUTTON_POINTER(m_pBtnChooseNoseMouth, m_pDlgCustomizeNoseMouth, "Btm_ChooseNoseMouth");
	INIT_BUTTON_POINTER(m_pBtnChooseHair, m_pDlgCustomizeHair, "Btm_ChooseHair");
	INIT_BUTTON_POINTER(m_pBtnChooseFacePainting, m_pDlgCustomizePaint, "Btm_ChooseFacePainting");
	
	m_pBtnChoosePreCustomize->SetPushLike(true);
	m_pBtnChooseFace ->SetPushLike(true);
	m_pBtnChooseEye->SetPushLike(true); 
	m_pBtnChooseNoseMouth->SetPushLike(true);
	m_pBtnChooseHair->SetPushLike(true); 
	m_pBtnChooseFacePainting->SetPushLike(true); 

	m_pBtnCurChoose = m_pBtnChoosePreCustomize;
	m_pBtnCurChoose->SetPushed(true);

	return true;
}

bool CECCustomizeMgr::CalculateAdvaceOption(void)
{
	CECFace* pCurFace = m_pCurPlayer->GetFaceModel();

	pCurFace->CalculateForehead();//计算额头
	
	pCurFace->CalculateYokeBone();//计算颧骨
	
	pCurFace->CalculateCheek();//计算脸颊
	
	pCurFace->CalculateChain();//计算下巴
	
	pCurFace->CalculateJaw();//计算颌骨
	
	return true;
}

#define INIT_DIALOG_POINTER(variableName, className, dialogName) \
{ \
	variableName = dynamic_cast<className *>(m_pAUIManager->GetDialog(dialogName)); \
	ASSERT(variableName); \
}

bool CECCustomizeMgr::InitDialogs()
{
	INIT_DIALOG_POINTER(m_pDlgCustomizeEye, CDlgCustomizeEye, "Win_ChooseEye");
	INIT_DIALOG_POINTER(m_pDlgCustomizeEye2, CDlgCustomizeEye2, "Win_ChooseEye2");
	INIT_DIALOG_POINTER(m_pDlgCustomizeFace, CDlgCustomizeFace, "Win_ChooseFace");
	INIT_DIALOG_POINTER(m_pDlgCustomizeFace2, CDlgCustomizeFace2, "Win_ChooseFace2");
	INIT_DIALOG_POINTER(m_pDlgCustomizeHair, CDlgCustomizeHair, "Win_ChooseHair");
	INIT_DIALOG_POINTER(m_pDlgCustomizeNoseMouth, CDlgCustomizeNoseMouth, "Win_ChooseNoseMouth");
	INIT_DIALOG_POINTER(m_pDlgCustomizeNoseMouth2, CDlgCustomizeNoseMouth2, "Win_ChooseNoseMouth2");
	INIT_DIALOG_POINTER(m_pDlgCustomizePaint, CDlgCustomizePaint, "Win_ChooseFacePainting");
	INIT_DIALOG_POINTER(m_pDlgCustomizePre, CDlgCustomizePre, "Win_ChoosePreCustomize");
	INIT_DIALOG_POINTER(m_pDlgCustomizeStature, CDlgCustomizeStature, "Win_ChooseStature");
	INIT_DIALOG_POINTER(m_pDlgCustomize, CDlgCustomize, "Win_Customize");
	INIT_DIALOG_POINTER(m_pDlgFacename, CDlgFaceName, "Win_Facename");
	
	bool bRet = true;
	
	if (!m_pDlgCustomizeEye->InitContext(this)) bRet = false;
	if (!m_pDlgCustomizeEye2->InitContext(this)) bRet = false;
	if (!m_pDlgCustomizeFace->InitContext(this)) bRet = false;
	if (!m_pDlgCustomizeFace2->InitContext(this)) bRet = false;
	if (!m_pDlgCustomizeHair->InitContext(this)) bRet = false;
	if (!m_pDlgCustomizeNoseMouth->InitContext(this)) bRet = false;
	if (!m_pDlgCustomizeNoseMouth2->InitContext(this)) bRet = false;
	if (!m_pDlgCustomizePaint->InitContext(this)) bRet = false;
	if (!m_pDlgCustomizePre->InitContext(this)) bRet = false;
	if (!m_pDlgCustomizeStature->InitContext(this)) bRet = false;
	if (!m_pDlgCustomize->InitContext(this)) bRet = false;
	if (!m_pDlgFacename->InitContext(this)) bRet = false;

	m_pCurDialog = m_pDlgCustomizePre;

	m_pDlgCustomizePre->Show(true);

	if (m_pCustomizePolicy->ShouldShowOKCancel()){
		m_pDlgCustomize->Show(true);
	}

	return bRet;
}

//-------------------------------------------------------------------------------
void CECCustomizeMgr::HideAllOldDlg()
{
	m_vecOldDlg.clear();
	int nNumDlg = m_pAUIManager->GetDialogCount();
	for( int i = 0; i < nNumDlg; i++)
	{
		AUIDialog* pDlg = m_pAUIManager->GetDialog(i);
		if( pDlg->IsShow())
		{
			m_vecOldDlg.push_back(pDlg);
			pDlg->Show(false);
		}
	}
}

bool CECCustomizeMgr::AddRestoreDialog(PAUIDIALOG pDlg)
{
	if (!pDlg || std::find(m_vecOldDlg.begin(), m_vecOldDlg.end(), pDlg) != m_vecOldDlg.end()){
		return false;
	}
	m_vecOldDlg.push_back(pDlg);
	return true;
}

bool CECCustomizeMgr::RemoveRestoreDialog(PAUIDIALOG pDlg)
{
	if (!pDlg){
		return false;
	}
	OldDlgVecIter_t it = std::find(m_vecOldDlg.begin(), m_vecOldDlg.end(), pDlg);
	if (it == m_vecOldDlg.end()){
		return false;
	}
	m_vecOldDlg.erase(it);
	return true;
}

//-------------------------------------------------------------------------------
void CECCustomizeMgr::ShowAllOldDlg()
{
	OldDlgVecIter_t iter = m_vecOldDlg.begin();
	for( ; iter != m_vecOldDlg.end(); iter++)
	{
		(*iter)->Show(true);
	}
	m_vecOldDlg.clear();
}

//------------------------------------------------------------------------------
void CECCustomizeMgr::ChangeScene(int nScene)
{
		A3DMATRIX4 matNPC = TransformMatrix(m_vNPCDir, A3DVECTOR3(0.0f, 1.0f, 0.0f), m_vNPCPos);
		A3DVECTOR3 vecPlayerPos = A3DVECTOR3(0.0f, -9.0f, 0.0f) * matNPC;
		A3DVECTOR3 vecCamPos;
		if( nScene == FACE_SCENE_STATURE)
		{
			vecCamPos = A3DVECTOR3(0.0f, -8.0f, 3.0f) * matNPC;
		}
		else if( nScene == FACE_SCENE_HEAD)
		{
			if( m_pCurPlayer->GetGender() == 0)//男
			{
				vecCamPos = (A3DVECTOR3(0.0f, -8.0f, 17.0f) + m_vPosDaltaM)* matNPC;
			}
			else//女
			{
				vecCamPos = (A3DVECTOR3(0.0f, -8.0f, 17.0f) + m_vPosDaltaF) * matNPC;
			}
		}

		A3DVECTOR3 vecCamToPlayer = a3d_VectorMatrix3x3(A3DVECTOR3(0.0f, 0.0f, -1.0f), matNPC);

		A3DCamera * pCamera = g_pGame->GetViewport()->GetA3DCamera();
		pCamera->SetPos(vecCamPos);
		pCamera->SetDirAndUp(vecCamToPlayer, A3DVECTOR3(0.0f, 1.0f, 0.0f));

		if( nScene == FACE_SCENE_STATURE)
		{
			pCamera->SetFOVSet(DEG2RAD(DEFCAMERA_FOV));
			pCamera->SetFOV(DEG2RAD(DEFCAMERA_FOV));
		}
		else if ( nScene == FACE_SCENE_HEAD)
		{
			pCamera->SetFOVSet(DEG2RAD(10.0f));
			pCamera->SetFOV(DEG2RAD(10.0f));
		}

		if(m_pCurPlayer && m_pCurPlayer->GetPlayerModel() )
		{
			m_pCurPlayer->SetPos(vecPlayerPos);
			m_pCurPlayer->ChangeModelMoveDirAndUp(-vecCamToPlayer, g_vAxisY);
		}
}

A2DSprite * CECCustomizeMgr::GetImageSelected()
{
	A2DSprite * & pSprite = m_pSpriteImageSelect;
	if (!pSprite){
		const char * szFile = "surfaces\\图标\\个性化选中框-1.tga";
		if (af_IsFileExist(szFile)){
			pSprite = new A2DSprite;
			if (!pSprite->Init(g_pGame->GetA3DDevice(), szFile+strlen("surfaces\\"), AUI_COLORKEY)){
				A3DRELEASE(pSprite);
			}else{
				pSprite->SetLinearFilter(true);
				pSprite->SetLocalCenterPos(pSprite->GetWidth()/2, pSprite->GetHeight()/2);
			}
		}
	}
	return pSprite;
}

A2DSprite * CECCustomizeMgr::GetImageMouseOn()
{
	A2DSprite * & pSprite = m_pSpriteImageMouseOn;
	if (!pSprite){
		const char * szFile = "surfaces\\图标\\个性化选中框-2.tga";
		if (af_IsFileExist(szFile)){
			pSprite = new A2DSprite;
			if (!pSprite->Init(g_pGame->GetA3DDevice(), szFile+strlen("surfaces\\"), AUI_COLORKEY)){
				A3DRELEASE(pSprite);
			}else{
				pSprite->SetLinearFilter(true);
				pSprite->SetLocalCenterPos(pSprite->GetWidth()/2, pSprite->GetHeight()/2);
			}
		}
	}
	return pSprite;
}

//------------------------------------------------------------------------------
void CECCustomizeMgr::ResetDragRole(bool bflag) 
{ 
	m_bDragRole = bflag; 
	g_pGame->ShowCursor(true); 
}

void CECCustomizeMgr::ResetDragCamera(bool bflag) 
{ 
	m_bDragCamera = bflag; 
	g_pGame->ShowCursor(true); 
}

void CECCustomizeMgr::InitDragRole(LPARAM lParam)
{
	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	
	m_bDragRole = true;
	m_ptDragStart.x = GET_X_LPARAM(lParam) - p->X;
	m_ptDragStart.y = GET_Y_LPARAM(lParam) - p->Y;
	g_pGame->ShowCursor(false);	
}

void CECCustomizeMgr::InitDragCamera(LPARAM lParam)
{
	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	
	m_bDragCamera = true;
	m_ptDragStart.x = GET_X_LPARAM(lParam) - p->X;
	m_ptDragStart.y = GET_Y_LPARAM(lParam) - p->Y;
	g_pGame->ShowCursor(false);	
}

void CECCustomizeMgr::DragRole(LPARAM lParam)
{
	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	int mx = GET_X_LPARAM(lParam) - p->X;
	int my = GET_Y_LPARAM(lParam) - p->Y;
				
	A3DMATRIX4 matRotate = RotateY((m_ptDragStart.x - mx) * 0.003f);
				
	A3DVECTOR3 vecDir = m_pCurPlayer->GetDir();
	A3DVECTOR3 vecUp = m_pCurPlayer->GetUp();
	vecDir = a3d_VectorMatrix3x3(vecDir, matRotate);
	vecUp = a3d_VectorMatrix3x3(vecUp, matRotate);
	m_pCurPlayer->ChangeModelMoveDirAndUp(vecDir, vecUp);
}

void CECCustomizeMgr::DragCamera(LPARAM lParam)
{
	A3DVIEWPORTPARAM *p = g_pGame->GetA3DEngine()->GetActiveViewport()->GetParam();
	A3DCamera *pCamera = g_pGame->GetViewport()->GetA3DCamera();
	int mx = GET_X_LPARAM(lParam) - p->X;
	int my = GET_Y_LPARAM(lParam) - p->Y;

	const float fDeltaDist = 0.003f;
	const float fDeltaDist2 = 0.2f;
	
	float fDeltaDist_X = (m_ptDragStart.x - mx) * fDeltaDist;
	float fDeltaDist_Y = (m_ptDragStart.y - my) * fDeltaDist;
				
	if( m_pBtnChoosePreCustomize->IsPushed() )   
	{
		if( m_fMoveXDist < 1.0f && fDeltaDist_X > 0.0f ) // Drag left and right
		{
			m_fMoveXDist += fDeltaDist_X;
			pCamera->MoveRight(fDeltaDist_X);
		}
		else if( m_fMoveXDist > -1.0f && fDeltaDist_X < 0.0f )
		{
			m_fMoveXDist += fDeltaDist_X;
			pCamera->MoveLeft(-fDeltaDist_X);
		}
					
		if( m_fMoveYDist < 0.3f && fDeltaDist_Y > 0.0f ) // Drag up and down
		{
			m_fMoveYDist += fDeltaDist_Y;
			pCamera->Move(A3DVECTOR3(0.0f, -fDeltaDist_Y, 0.0f));
		}
		else if( m_fMoveYDist > -1.0f && fDeltaDist_Y < 0.0f)
		{
			m_fMoveYDist += fDeltaDist_Y;
			pCamera->Move(A3DVECTOR3(0.0f, -fDeltaDist_Y, 0.0f));
		}
	}
	else
	{
		if( m_fMoveXDist < 1.0f && fDeltaDist_X > 0.0f ) // Drag left and right
		{
			m_fMoveXDist += fDeltaDist_X * fDeltaDist2;
			pCamera->MoveRight(fDeltaDist_X * fDeltaDist2);
		}
		else if( m_fMoveXDist > -1.0f && fDeltaDist_X < 0.0f )
		{
			m_fMoveXDist += fDeltaDist_X * fDeltaDist2;
			pCamera->MoveLeft(-fDeltaDist_X * fDeltaDist2);
		}
					
		if( m_fMoveYDist < 0.3f && fDeltaDist_Y > 0.0f ) // Drag up and down
		{
			m_fMoveYDist += fDeltaDist_Y * fDeltaDist2;
			pCamera->Move(A3DVECTOR3(0.0f, -fDeltaDist_Y * fDeltaDist2, 0.0f));
		}
		else if( m_fMoveYDist > -1.0f && fDeltaDist_Y < 0.0f)
		{
			m_fMoveYDist += fDeltaDist_Y * fDeltaDist2;
			pCamera->Move(A3DVECTOR3(0.0f, -fDeltaDist_Y * fDeltaDist2, 0.0f));
		}
	}
} 

void CECCustomizeMgr::MouseWheel(WPARAM wParam)
{
	int zDelta = (short)HIWORD(wParam);
	if( zDelta > 0 )
		zDelta = 1;
	else
		zDelta = -1;
	
	if( m_fMoveZDist < 1.0f && zDelta > 0 )
	{
		A3DCamera *pCamera = g_pGame->GetViewport()->GetA3DCamera();
		m_fMoveZDist += zDelta * 0.1f;
		pCamera->MoveFront(zDelta * 0.1f);
	} 
	else if( m_fMoveZDist > -1.0f && zDelta < 0)
	{
		A3DCamera *pCamera = g_pGame->GetViewport()->GetA3DCamera();
		m_fMoveZDist += zDelta * 0.1f;
		pCamera->MoveBack(-zDelta * 0.1f);
	}
}

void CECCustomizeMgr::ResetCursor()
{
	// Restore cursor position to start position
	g_pGame->ShowCursor(false);
	POINT pt = {m_ptDragStart.x, m_ptDragStart.y};
	::ClientToScreen(g_pGame->GetGameInit().hWnd, &pt);
	::SetCursorPos(pt.x, pt.y);
}

void CECCustomizeMgr::Tick()
{
	if( m_bDragRole || m_bDragCamera )
	{
		ResetCursor();
	}
}

bool CECCustomizeMgr::CustomizeDealWindowsMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if( uMsg == WM_LBUTTONDOWN )
	{
		InitDragCamera(lParam);	
	}
	else if( uMsg == WM_RBUTTONDOWN )
	{
		InitDragRole(lParam);
	}
	else if( uMsg == WM_MOUSEMOVE )
	{
		if( GetDragRole() ) // Drag Role
		{
			if( !(wParam & MK_RBUTTON) )
			{
				ResetDragRole(false);
			}
			else
			{
				DragRole(lParam);
			}
		}
		else if( GetDragCamera() )  // Drag Camera
		{
			if( !(wParam & MK_LBUTTON))
			{
				ResetDragCamera(false);
			}
			else
			{
				DragCamera(lParam);
			}
		} 
	} 
	else if( uMsg == WM_MOUSEWHEEL )
	{
		MouseWheel(wParam);
	}
	else if( uMsg == WM_RBUTTONUP )
	{
		ResetDragRole(false);
	}
	else if( uMsg == WM_LBUTTONUP )
	{
		ResetDragCamera(false);
	} 
	else
	{
		return false;
	}
	return true;
}

void CECCustomizeMgr::ConfirmPlayerCustomizeData(){	
	CECPlayer *pPlayer = GetPlayer();
	CECPlayer::PLAYER_CUSTOMIZEDATA data = pPlayer->GetCustomizeData();
	data.faceData = *pPlayer->GetFaceModel()->GetFaceData();
	pPlayer->ChangeCustomizeData(data);
	pPlayer->SaveAsDefaultCustomizeData();
}

void CECCustomizeMgr::RestoreCamera(){
	A3DCamera * pCamera = g_pGame->GetViewport()->GetA3DCamera();
	pCamera->SetFOVSet(DEG2RAD(DEFCAMERA_FOV));
	pCamera->SetFOV(DEG2RAD(DEFCAMERA_FOV));
}

void CECCustomizeMgr::OnOK(){
	m_pCustomizePolicy->OnOK(*this);
}

void CECCustomizeMgr::OnCancel(){
	m_pCustomizePolicy->OnCancel(*this);
}