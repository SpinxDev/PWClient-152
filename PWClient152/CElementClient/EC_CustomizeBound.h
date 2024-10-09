/*
 * FILE: EC_CustomizeBound.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/6/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */
#ifndef EC_CUSTOMIZEBOUND_H
#define EC_CUSTOMIZEBOUND_H

#include "AIniFile.h"

#include "EC_Player.h"

//定义个性化限制
class CECCustomizeBound
{
public:
	CECCustomizeBound();
	virtual ~CECCustomizeBound();


	//获取调节范围
	bool LoadCustomizeBound(const char* pszFile);

	void ClampCustomizeDatas(CECPlayer::PLAYER_CUSTOMIZEDATA& data);

protected:
	void ClampData(unsigned char &data, int nMin, int nMax); 

public:
	int m_nVersion;
	
	int m_nScaleFaceHMin,			m_nScaleFaceHMax;
	int m_nScaleFaceVMin,			m_nScaleFaceVMax;

	int m_nScaleUpPartMin,			m_nScaleUpPartMax;
	int m_nScaleMiddlePartMin,		m_nScaleMiddlePartMax;
	int m_nScaleDownPartMin,		m_nScaleDownPartMax;

	int m_nOffsetForeheadHMin,		m_nOffsetForeheadHMax;
	int m_nOffsetForeheadVMin,		m_nOffsetForeheadVMax;
	int m_nOffsetForeheadZMin,		m_nOffsetForeheadZMax;
	int m_nRotateForeheadMin,		m_nRotateForeheadMax;
	int m_nScaleForeheadMin,		m_nScaleForeheadMax;

	int m_nOffsetYokeBoneHMin,		m_nOffsetYokeBoneHMax;
	int m_nOffsetYokeBoneVMin,		m_nOffsetYokeBoneVMax;
	int m_nOffsetYokeBoneZMin,		m_nOffsetYokeBoneZMax;
	int m_nRotateYokeBoneMin,		m_nRotateYokeBoneMax;
	int m_nScaleYokeBoneMin,		m_nScaleYokeBoneMax;

	int m_nOffsetCheekHMin,			m_nOffsetCheekHMax;
	int m_nOffsetCheekVMin,			m_nOffsetCheekVMax;
	int m_nOffsetCheekZMin,			m_nOffsetCheekZMax;
	int m_nScaleCheekMin,			m_nScaleCheekMax;

	int m_nOffsetChainZMin,			m_nOffsetChainZMax;
	int m_nOffsetChainVMin,			m_nOffsetChainVMax;
	int m_nRotateChainMin,			m_nRotateChainMax;
	int m_nScaleChainHMin,			m_nScaleChainHMax;
	
	int m_nOffsetJawHMin,			m_nOffsetJawHMax;
	int m_nOffsetJawVMin,			m_nOffsetJawVMax;
	int m_nOffsetJawZMin,			m_nOffsetJawZMax;
	int m_nScaleJawSpecialMin,		m_nScaleJawSpecialMax;
	int m_nScaleJawHMin,			m_nScaleJawHMax;
	int m_nScaleJawVMin,			m_nScaleJawVMax;

	int m_nScaleEyeHMin,			m_nScaleEyeHMax;
	int m_nScaleEyeVMin,			m_nScaleEyeVMax;
	int m_nRotateEyeMin,			m_nRotateEyeMax;
	int m_nOffsetEyeHMin,			m_nOffsetEyeHMax;
	int m_nOffsetEyeVMin,			m_nOffsetEyeVMax;
	int m_nOffsetEyeZMin,			m_nOffsetEyeZMax;
	int m_nScaleEyeBallMin,			m_nScaleEyeBallMax;


	
	int m_nScaleBrowHMin,			m_nScaleBrowHMax;
	int m_nScaleBrowVMin,			m_nScaleBrowVMax;
	int m_nRotateBrowMin,			m_nRotateBrowMax;
	int m_nOffsetBrowHMin,			m_nOffsetBrowHMax;
	int m_nOffsetBrowVMin,			m_nOffsetBrowVMax;
	int m_nOffsetBrowZMin,			m_nOffsetBrowZMax;
	

	int m_nScaleNoseTipHMin,		m_nScaleNoseTipHMax;
	int m_nScaleNoseTipVMin,		m_nScaleNoseTipVMax;
	int m_nOffsetNoseTipVMin,		m_nOffsetNoseTipVMax;
	int m_nScaleNoseTipZMin,		m_nScaleNoseTipZMax;
	int m_nScaleBridgeTipHMin,		m_nScaleBridgeTipHMax;
	int m_nOffsetBridgeTipZMin,		m_nOffsetBridgeTipZMax;

	int m_nThickUpLipMin,			m_nThickUpLipMax;
	int m_nThickDownLipMin,			m_nThickDownLipMax;
	int m_nScaleMouthHMin,			m_nScaleMouthHMax;
	int m_nOffsetMouthVMin,			m_nOffsetMouthVMax;
	int m_nOffsetMouthZMin,			m_nOffsetMouthZMax;
	int m_nOffsetCornerOfMouthSpecialMin, m_nOffsetCornerOfMouthSpecialMax;
	int m_nScaleMouthH2Min,			m_nScaleMouthH2Max;
	int m_nOffsetCornerOfMouthSpecial2Min, m_nOffsetCornerOfMouthSpecial2Max;

	int m_nScaleEarMin, m_nScaleEarMax;
	int m_nOffsetEarVMin, m_nOffsetEarVMax;
	
	int m_nHeadScaleMin, m_nHeadScaleMax;
	int m_nUpScaleMin,m_nUpScaleMax;
	int m_nWaistScaleMin,  m_nWaistScaleMax;
	int m_nArmWidthMin, m_nArmWidthMax;
	int m_nLegWidthMin, m_nLegWidthMax;
	int m_nBreastScaleMin, m_nBreastScaleMax;


};

class CECCustomizeBoundMgr
{
protected:
	typedef abase::hash_map<const char* , CECCustomizeBound* > CustomizeBoundMap_t;
	typedef CustomizeBoundMap_t::iterator CustomizeBoundMapIter_t;

	CustomizeBoundMap_t m_CustomizeBoundMap;
	
public:
	CECCustomizeBoundMgr();
	~CECCustomizeBoundMgr();

	CECCustomizeBound* GetCustomizeBound(const char* pszFileName);
};

extern CECCustomizeBoundMgr g_CustomizeBoundMgr;

#endif 