// Filename	: EC_ModelAsynLoadHelper.cpp
// Creator	: Xu Wenbin
// Date		: 2014/11/28

#include "EC_ModelAsynLoadHelper.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_World.h"
#include "EC_ManPlayer.h"
#include "EC_ManNPC.h"
#include "EC_ManMatter.h"
#include "EC_ManOrnament.h"
#include "EC_ElsePlayer.h"
#include "EC_HostPlayer.h"

#include <A3DFuncs.h>

extern CECGame *g_pGame;

//	class CECModelAsynLoadContent

CECModelAsynLoadContent::CECModelAsynLoadContent(int id, DWORD dwBornStamp)
: m_id(id)
, m_dwBornStamp(dwBornStamp)
, m_fDistToHost(0.0f)
{
}

void CECModelAsynLoadContent::SetDistToHost(float fDistToHost){
	m_fDistToHost = fDistToHost;
}

bool CECModelAsynLoadContent::operator == (const CECAsynLoadContent &rhs)const{
	const CECModelAsynLoadContent &rhs2 = dynamic_cast<const CECModelAsynLoadContent &>(rhs);
	return GetID() == rhs2.GetID()
		&& GetBornStamp() == rhs2.GetBornStamp();
}

//	class CECPlayerModelAsynLoadContent
CECPlayerModelAsynLoadContent::CECPlayerModelAsynLoadContent(int id, DWORD dwBornStamp)
: CECModelAsynLoadContent(id, dwBornStamp)
{
}

void CECPlayerModelAsynLoadContent::UpdateSortWeight(){
	CECWorld* pWorld = g_pGame->GetGameRun()->GetWorld();
	if (!pWorld){
		return;
	}
	CECHostPlayer *pHost = g_pGame->GetGameRun()->GetHostPlayer();
	if (!pHost){
		return;
	}
	CECElsePlayer* pPlayer = pWorld->GetPlayerMan()->GetElsePlayer(GetID());
	if (!pPlayer){
		return;
	}
	SetDistToHost(SquareMagnitude(pPlayer->GetPos() - pHost->GetPos()));
}

//	class CECPlayerBasicModelAsynLoadContent
CECPlayerBasicModelAsynLoadContent::CECPlayerBasicModelAsynLoadContent(int id, DWORD dwBornStamp, int iProfession, int iGender, int iBodyID, const int *aEquips, const char *szPetPath, bool bSimpleModel, bool bSimpleFace, bool bClientModel)
: CECPlayerModelAsynLoadContent(id, dwBornStamp)
, m_iProfession(iProfession)
, m_iGender(iGender)
, m_iBodyID(iBodyID)
, m_strPetPath(szPetPath)
, m_bSimpleModel(bSimpleModel)
, m_bSimpleFace(bSimpleFace)
, m_bClientModel(bClientModel)
{
	if (aEquips){
		memcpy(m_aEquips, aEquips, sizeof(m_aEquips));
	}else{
		memset(m_aEquips, 0xff, sizeof(m_aEquips));
	}
}

CECAsynLoadContent * CECPlayerBasicModelAsynLoadContent::Clone()const{
	return new CECPlayerBasicModelAsynLoadContent(*this);
}

bool CECPlayerBasicModelAsynLoadContent::operator == (const CECAsynLoadContent &rhs)const{
	const CECPlayerBasicModelAsynLoadContent &rhs2 = dynamic_cast<const CECPlayerBasicModelAsynLoadContent &>(rhs);
	return CECPlayerModelAsynLoadContent::operator == (rhs2)
		&& GetProfession() == rhs2.GetProfession()
		&& GetGender() == rhs2.GetGender()
		&& GetBodyID() == rhs2.GetBodyID()
		&& GetPetPath() == rhs2.GetPetPath()
		&& GetSimpleModel() == rhs2.GetSimpleModel()
		&& GetSimpleFace() == rhs2.GetSimpleFace()
		&& GetClientModel() == rhs2.GetClientModel()
		&& !memcmp(GetEquips(), rhs2.GetEquips(), sizeof(m_aEquips));
}

bool CECPlayerBasicModelAsynLoadContent::ThreadLoad(){
	CECPlayerMan* pManager = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
	return pManager->ThreadLoadPlayerModel(
		GetID(),
		GetBornStamp(),
		GetProfession(),
		GetGender(),
		GetBodyID(),
		GetEquips(),
		GetPetPath(),
		GetSimpleModel(),
		GetSimpleFace(),
		GetClientModel());
}

//	class CECPlayerFaceAsynLoadContent
CECPlayerFaceAsynLoadContent::CECPlayerFaceAsynLoadContent(int id, DWORD dwBornStamp, int iProfession, int iGender, int iFaceID)
: CECPlayerModelAsynLoadContent(id, dwBornStamp)
, m_iProfession(iProfession)
, m_iGender(iGender)
, m_iFaceID(iFaceID)
{
}

CECAsynLoadContent * CECPlayerFaceAsynLoadContent::Clone()const{
	return new CECPlayerFaceAsynLoadContent(*this);
}

bool CECPlayerFaceAsynLoadContent::operator == (const CECAsynLoadContent &rhs)const{
	const CECPlayerFaceAsynLoadContent &rhs2 = dynamic_cast<const CECPlayerFaceAsynLoadContent &>(rhs);
	return CECPlayerModelAsynLoadContent::operator == (rhs2)
		&& GetProfession() == rhs2.GetProfession()
		&& GetGender() == rhs2.GetGender()
		&& GetFaceID() == rhs2.GetFaceID();
}

bool CECPlayerFaceAsynLoadContent::ThreadLoad(){
	CECPlayerMan* pManager = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
	return pManager->ThreadLoadPlayerFace(
		GetID(),
		GetBornStamp(),
		GetProfession(),
		GetGender(),
		GetFaceID());
}

//	class CECPlayerShapeAsynLoadContent
CECPlayerShapeAsynLoadContent::CECPlayerShapeAsynLoadContent(int id, DWORD dwBornStamp, int iShape, bool bClientModel)
: CECPlayerModelAsynLoadContent(id, dwBornStamp)
, m_iShape(iShape)
, m_bClientModel(bClientModel)
{
}

CECAsynLoadContent * CECPlayerShapeAsynLoadContent::Clone()const{
	return new CECPlayerShapeAsynLoadContent(*this);
}

bool CECPlayerShapeAsynLoadContent::operator == (const CECAsynLoadContent &rhs)const{
	const CECPlayerShapeAsynLoadContent &rhs2 = dynamic_cast<const CECPlayerShapeAsynLoadContent &>(rhs);
	return CECPlayerModelAsynLoadContent::operator == (rhs2)
		&& GetShape() == rhs2.GetShape()
		&& GetClientModel() == rhs2.GetClientModel();
}

bool CECPlayerShapeAsynLoadContent::ThreadLoad(){
	CECPlayerMan* pManager = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
	return pManager->ThreadLoadDummyModel(
		GetID(),
		GetBornStamp(),
		GetShape(),
		GetClientModel());
}

//	class CECPlayerEquipmentAsynLoadContent
CECPlayerEquipmentAsynLoadContent::CECPlayerEquipmentAsynLoadContent(int id, DWORD dwBornStamp, int iProfession, int iGender, __int64 iEquipMask, const int *aEquips, bool bSimpleModel)
: CECPlayerModelAsynLoadContent(id, dwBornStamp)
, m_iProfession(iProfession)
, m_iGender(iGender)
, m_iEquipMask(iEquipMask)
, m_bSimpleModel(bSimpleModel)
{
	if (aEquips){
		memcpy(m_aEquips, aEquips, sizeof(m_aEquips));
	}else{
		memset(m_aEquips, 0xff, sizeof(m_aEquips));
	}
}

CECAsynLoadContent * CECPlayerEquipmentAsynLoadContent::Clone()const{
	return new CECPlayerEquipmentAsynLoadContent(*this);
}

bool CECPlayerEquipmentAsynLoadContent::operator == (const CECAsynLoadContent &rhs)const{
	const CECPlayerEquipmentAsynLoadContent &rhs2 = dynamic_cast<const CECPlayerEquipmentAsynLoadContent &>(rhs);
	return CECPlayerModelAsynLoadContent::operator == (rhs2)
		&& GetProfession() == rhs2.GetProfession()
		&& GetGender() == rhs2.GetGender()
		&& GetEquipMask() == rhs2.GetEquipMask()
		&& GetSimpleModel() == rhs2.GetSimpleModel()
		&& !memcmp(GetEquips(), rhs2.GetEquips(), sizeof(m_aEquips));
}

bool CECPlayerEquipmentAsynLoadContent::ThreadLoad(){
	CECPlayerMan* pManager = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
	return pManager->ThreadLoadPlayerEquips(
		GetID(),
		GetBornStamp(),
		GetProfession(),
		GetGender(),
		GetEquips(),
		GetEquipMask(),
		GetSimpleModel());
}

//	class CECPetAsynLoadContent
CECPlayerPetAsynLoadContent::CECPlayerPetAsynLoadContent(int id, DWORD dwBornStamp, const char *szFile)
: CECPlayerModelAsynLoadContent(id, dwBornStamp)
, m_strFilePath(szFile)
{
}

CECAsynLoadContent * CECPlayerPetAsynLoadContent::Clone()const{
	return new CECPlayerPetAsynLoadContent(*this);
}

bool CECPlayerPetAsynLoadContent::operator == (const CECAsynLoadContent &rhs)const{
	const CECPlayerPetAsynLoadContent &rhs2 = dynamic_cast<const CECPlayerPetAsynLoadContent &>(rhs);
	return CECPlayerModelAsynLoadContent::operator == (rhs2)
		&& GetFilePath() == rhs2.GetFilePath();
}

bool CECPlayerPetAsynLoadContent::ThreadLoad(){
	CECPlayerMan* pManager = g_pGame->GetGameRun()->GetWorld()->GetPlayerMan();
	return pManager->ThreadLoadPetModel(
		GetID(),
		GetBornStamp(),
		GetFilePath());
}

//	class CECNPCModelAsynLoadContent
CECNPCModelAsynLoadContent::CECNPCModelAsynLoadContent(int id, DWORD dwBornStamp, int tid, const char *szFilePath)
: CECModelAsynLoadContent(id, dwBornStamp)
, m_tid(tid)
, m_strFilePath(szFilePath)
{
}

CECAsynLoadContent * CECNPCModelAsynLoadContent::Clone()const{
	return new CECNPCModelAsynLoadContent(*this);
}

bool CECNPCModelAsynLoadContent::operator == (const CECAsynLoadContent &rhs)const{
	const CECNPCModelAsynLoadContent &rhs2 = dynamic_cast<const CECNPCModelAsynLoadContent &>(rhs);
	return CECModelAsynLoadContent::operator == (rhs2)
		&& GetTID() == rhs2.GetTID()
		&& GetFilePath() == rhs2.GetFilePath();
}

bool CECNPCModelAsynLoadContent::ThreadLoad(){
	CECNPCMan* pManager = g_pGame->GetGameRun()->GetWorld()->GetNPCMan();
	return pManager->ThreadLoadNPCModel(
		GetID(),
		GetBornStamp(),
		GetTID(),
		GetFilePath());
}

//	class CECMatterModelAsynLoadContent
CECMatterModelAsynLoadContent::CECMatterModelAsynLoadContent(int id, const char *szFilePath)
: CECModelAsynLoadContent(id, 0)
, m_strFilePath(szFilePath)
{
}

CECAsynLoadContent * CECMatterModelAsynLoadContent::Clone()const{
	return new CECMatterModelAsynLoadContent(*this);
}

bool CECMatterModelAsynLoadContent::operator == (const CECAsynLoadContent &rhs)const{
	const CECMatterModelAsynLoadContent &rhs2 = dynamic_cast<const CECMatterModelAsynLoadContent &>(rhs);
	return CECModelAsynLoadContent::operator == (rhs2)
		&& GetFilePath() == rhs2.GetFilePath();
}

bool CECMatterModelAsynLoadContent::ThreadLoad(){
	CECMatterMan* pManager = g_pGame->GetGameRun()->GetWorld()->GetMatterMan();
	return pManager->ThreadLoadMatterModel(
		GetID(),
		GetFilePath());
}

//	class CECSceneModelAsynLoadContent
CECSceneModelAsynLoadContent::CECSceneModelAsynLoadContent(int id, const char *szFilePath)
: CECModelAsynLoadContent(id, 0)
, m_strFilePath(szFilePath)
{
}

CECAsynLoadContent * CECSceneModelAsynLoadContent::Clone()const{
	return new CECSceneModelAsynLoadContent(*this);
}

bool CECSceneModelAsynLoadContent::operator == (const CECAsynLoadContent &rhs)const{
	const CECSceneModelAsynLoadContent &rhs2 = dynamic_cast<const CECSceneModelAsynLoadContent &>(rhs);
	return CECModelAsynLoadContent::operator == (rhs2)
		&& GetFilePath() == rhs2.GetFilePath();
}

bool CECSceneModelAsynLoadContent::ThreadLoad(){
	CECOrnamentMan* pManager = g_pGame->GetGameRun()->GetWorld()->GetOrnamentMan();
	return pManager->ThreadLoadECModel(
		(DWORD)GetID(),
		GetFilePath());
}