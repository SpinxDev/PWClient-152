/*
 * FILE: EC_HPWorkForceNavigate.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: 
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */


#include "EC_HPWorkForceNavigate.h"
#include "EC_Bezier.h"
#include "EC_Scene.h"
#include "EC_TaskInterface.h"
#include "EC_HostPlayer.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Global.h"
#include "EC_World.h"
#include "EC_UIHelper.h"
#include "EC_GameUIMan.h"
#include "EC_PlayerClone.h"

#include "CSplit.h"

#include <A3DFuncs.h>
#include <AScriptFile.h>

#define new A_DEBUG_NEW


//////////////////////////////////////////////////////////////////////////

CECNavigateCtrl::~CECNavigateCtrl(){
	if (m_pBezierWalker){
		delete m_pBezierWalker;
		m_pBezierWalker = NULL;
	}
	m_configInfo.clear();
}
bool CECNavigateCtrl::LoadConfig(const char* szFile){
	AScriptFile sf;
	if (!sf.Open(szFile))	{
		a_LogOutput(1, "CECNaviageCtrl::LoadConfig, failed to open file %s",szFile);
		return false;
	}
	
	m_configInfo.clear();
	while (sf.PeekNextToken(true))	{
		sf.GetNextToken(true);
		
		CSplit split(sf.m_szToken);
		CSplit::VectorAString v = split.Split(",");
		if (v.size()<6){ // 6个值： taskid,bezierid,speed,angle,dir,model file		
			ASSERT(0);
			sf.Close();
			return false;
		}
		INFO navi;
		navi.taskID = v[0].ToInt();
		navi.bezierID = v[1].ToInt();
		navi.speed = v[2].ToFloat();
		navi.angleWithH = v[3].ToFloat();
		navi.bezierDir = (v[4].ToInt()==1);
		navi.strModelPath = v[5];
		
		m_configInfo.push_back(navi);
	}
	
	sf.Close();	
	return true;
}
bool CECNavigateCtrl::GetNavigateInfo(int task,CECNavigateCtrl::INFO& info){
	if(task<=0) return false;

	for(unsigned int i=0;i<m_configInfo.size();i++){
		if(m_configInfo[i].taskID == task){
			info = m_configInfo[i];
			return true;
		}
	}
	return false;		
}

void CECNavigateCtrl::OnPrepareNavigate(int task){
	m_bForceNavigateState = true;
	m_taskID = task;

	CECUIHelper::GetGameUIMan()->SetShowAllPanlesFlag(false);

	if(GetNavigateInfo(task,m_curNavigateInfo))	{
		CECHostNavigatePlayer* player = m_pHost->GetNavigatePlayer();
		player->SetNavigateModelFile(m_curNavigateInfo.strModelPath);
		player->Init();
	}
	else  {
		//m_pHost->ReleaseNavigatePlayer();
		m_pHost->GetTaskInterface()->GiveUpTask(m_taskID);
	}
}
void CECNavigateCtrl::OnBeginNavigate(){	
 	CECScene* pScene = g_pGame->GetGameRun()->GetWorld()->GetScene();
 	CECBezier* pBezier = dynamic_cast<CECBezier*>(pScene->GetBezierObjectByGlobalID(m_curNavigateInfo.bezierID));
 	if(!pBezier) {
 		m_pHost->GetTaskInterface()->GiveUpTask(m_taskID);
 		return;
 	}

	if (!m_pBezierWalker)	
		m_pBezierWalker = new CECBezierWalker();
	
	m_pBezierWalker->BindBezier(pBezier);	
	m_pBezierWalker->SetSpeed(m_curNavigateInfo.speed);		
	m_pBezierWalker->StartWalk(false,true);

	CECHPWorkNavigate* pWork = (CECHPWorkNavigate*)m_pHost->GetWorkMan()->CreateWork(CECHPWork::WORK_FORCENAVIGATEMOVE);
	pWork->BeginNavigate();
	m_pHost->GetWorkMan()->StartWork_p2(pWork);
}
void CECNavigateCtrl::OnEndNavigate(){
	if(!m_bForceNavigateState) return;
	m_taskID = 0;
//	m_pHost->ReleaseNavigatePlayer();
	m_pHost->GetWorkMan()->FinishRunningWork(CECHPWork::WORK_FORCENAVIGATEMOVE);	
	CECUIHelper::GetGameUIMan()->SetShowAllPanlesFlag(true);	
	m_bForceNavigateState = false;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECHPWorkNavigate
//	
///////////////////////////////////////////////////////////////////////////

float glb_VectorToYaw(const A3DVECTOR3& v){
	A3DVECTOR3 v1 = v;
	v1.y = 0.0f;
	v1.Normalize();
	return RAD2DEG((float)atan2(v1.z, v1.x));
}

A3DVECTOR3 glb_YawToVector(float fYaw){
	float fRad = DEG2RAD(fYaw);
	
	A3DVECTOR3 v;
	v.x = 10.0f * cosf(fRad);
	v.z = 10.0f * sinf(fRad);
	v.y = 0.0f;
	v.Normalize();
	
	return v;
}

//	Clamp yaw to [-180.0f, 180.0f]
void glb_ClampYaw(float& fYaw){
	if (fYaw >= -180.0f && fYaw <= 180.0f)	{
		//	Below clamp calculation will bring little error to original number, this will 
		//	cause problem in yaw's equal comparing. So if original yaw has been in valid
		//	range, return directly.
		return;
	}
	
	static float fInv = 1.0f / 360.0f;
	
	fYaw += 180.0f;
	if (fYaw < 0.0f){
		int n = (int)(-fYaw * fInv) + 1;
		fYaw = fYaw + 360.0f * n;
	}
	else{
		int n = (int)(fYaw * fInv);
		fYaw = fYaw - 360.0f * n;
	}
	
	fYaw -= 180.0f;
	ASSERT(fYaw >= -180.0f && fYaw <= 180.0f);
}
float glb_GetPitch(const A3DVECTOR3& v){
	return RAD2DEG(acosf(v.y));
}

//////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////

CECHPWorkNavigate::CECHPWorkNavigate(CECHPWorkMan* pWorkMan) :
CECHPWork(WORK_FORCENAVIGATEMOVE, pWorkMan){
	m_dwMask		= MASK_FORCENAVIGATE;
	m_dwTransMask	= MASK_STAND;	
	CECHPWorkNavigate::Reset();
}

CECHPWorkNavigate::~CECHPWorkNavigate(){}

//	Reset work
void CECHPWorkNavigate::Reset(){
	CECHPWork::Reset();
	m_bMove = false;
}

//	Copy work data
bool CECHPWorkNavigate::CopyData(CECHPWork* pWork){
	if (!CECHPWork::CopyData(pWork))
		return false;

	CECHPWorkNavigate* pSrc = (CECHPWorkNavigate*)pWork;	

	m_fSpeed = pSrc->m_fSpeed;
	m_bMove	 = pSrc->m_bMove; 

	return true;
}

void CECHPWorkNavigate::BeginNavigate(){
	m_bMove = true;
}
//	Stop move
void CECHPWorkNavigate::Finish(){
	CECHPWorkNavigate::Reset();
}
void CECHPWorkNavigate::OnFirstTick(){

	A3DVECTOR3 dir = m_pHost->GetNavigatePlayer()->GetDir();
	dir.Normalize();

	CECNavigateCtrl* pNaviCtrl = m_pHost->GetNavigatePlayer()->GetNavigateCtrl();
	
	A3DVECTOR3 vRight = CrossProduct(g_vAxisY,dir);
	dir = a3d_RotatePosAroundAxis(dir,vRight,DEG2RAD(pNaviCtrl->GetCurrentNavigateInfo().angleWithH));
		
	float fDesYaw = glb_VectorToYaw(dir);
	glb_ClampYaw(fDesYaw);
		
	m_pHost->GetCameraCtrl()->SetDgree(90.f-fDesYaw);
			
	float fDesPitch = glb_GetPitch(dir);
	m_pHost->GetCameraCtrl()->SetPitch(90.f-fDesPitch);
}
//	Tick routine
bool CECHPWorkNavigate::Tick(DWORD dwDeltaTime){
	CECNavigateCtrl* pNaviCtrl = m_pHost->GetNavigatePlayer()->GetNavigateCtrl();
	CECBezierWalker* pBezierWalker = pNaviCtrl->GetBezierWalker();

	if(!m_bMove || !pBezierWalker) return true;

	DWORD dwRealTime = g_pGame->GetRealTickTime();

	if (pBezierWalker->IsWalking()) {
		A3DVECTOR3 vCurPos = m_pHost->GetPos();
		A3DVECTOR3 vMoveDir;

		pBezierWalker->Tick(dwRealTime);
		vCurPos = pBezierWalker->GetPos();

		A3DVECTOR3 vDir = pBezierWalker->GetDir();
		vDir.Normalize();
		A3DVECTOR3 vRight = CrossProduct(g_vAxisY,vDir);
		A3DVECTOR3 vUp = CrossProduct(vDir,vRight);

		A3DVECTOR3 vDirH = CrossProduct(vRight,g_vAxisY);
		vUp.Normalize();
		CECHostNavigatePlayer* pClone = m_pHost->GetNavigatePlayer();
		if(pClone){
			pClone->SetPos(vCurPos);
			if(pNaviCtrl->GetCurrentNavigateInfo().bezierDir)
				pClone->ChangeModelMoveDirAndUp(vDir,vUp);
			else
				pClone->ChangeModelMoveDirAndUp(vDirH,g_vAxisY);
		}
	}  
	else {
		if (m_bMove)		
			m_bMove = false;		
		else
			ASSERT(FALSE);
		
		// 任务完成
		m_pHost->GetTaskInterface()->SetForceNavigateFinishFlag(true);
	}

	CECHPWork::Tick(dwDeltaTime);
	
	return true;
}

