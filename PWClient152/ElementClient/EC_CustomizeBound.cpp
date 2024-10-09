/*
 * FILE: EC_CustomizeBound.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Liyi, 2005/6/6
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_CustomizeBound.h"

//
CECCustomizeBound::CECCustomizeBound()
{
	m_nVersion = 1;
	
	m_nScaleFaceHMin = 0;			m_nScaleFaceHMax = 255;
	m_nScaleFaceVMin = 0;			m_nScaleFaceVMax = 255;

	m_nScaleUpPartMin = 0;			m_nScaleUpPartMax = 255;
	m_nScaleMiddlePartMin = 0;		m_nScaleMiddlePartMax = 255;
	m_nScaleDownPartMin = 0;		m_nScaleDownPartMax = 255;

	m_nOffsetForeheadHMin = 0;		m_nOffsetForeheadHMax = 255;
	m_nOffsetForeheadVMin = 0;		m_nOffsetForeheadVMax = 255;
	m_nOffsetForeheadZMin = 0;		m_nOffsetForeheadZMax = 255;
	m_nRotateForeheadMin = 0;		m_nRotateForeheadMax = 255;
	m_nScaleForeheadMin = 0;		m_nScaleForeheadMax = 255;

	m_nOffsetYokeBoneHMin = 0;		m_nOffsetYokeBoneHMax = 255;
	m_nOffsetYokeBoneVMin = 0;		m_nOffsetYokeBoneVMax = 255;
	m_nOffsetYokeBoneZMin = 0;		m_nOffsetYokeBoneZMax = 255;
	m_nRotateYokeBoneMin = 0;		m_nRotateYokeBoneMax = 255;
	m_nScaleYokeBoneMin = 0;		m_nScaleYokeBoneMax = 255;

	m_nOffsetCheekHMin = 0;			m_nOffsetCheekHMax = 255;
	m_nOffsetCheekVMin = 0;			m_nOffsetCheekVMax = 255;
	m_nOffsetCheekZMin = 0;			m_nOffsetCheekZMax = 255;
	m_nScaleCheekMin = 0;			m_nScaleCheekMax = 255;

	m_nOffsetChainZMin = 0;			m_nOffsetChainZMax = 255;
	m_nOffsetChainVMin = 0;			m_nOffsetChainVMax = 255;
	m_nRotateChainMin = 0;			m_nRotateChainMax = 255;
	m_nScaleChainHMin = 0;			m_nScaleChainHMax = 255;
	
	m_nOffsetJawHMin = 0;			m_nOffsetJawHMax = 255;
	m_nOffsetJawVMin = 0;			m_nOffsetJawVMax = 255;
	m_nOffsetJawZMin = 0;			m_nOffsetJawZMax = 255;
	m_nScaleJawSpecialMin = 0;		m_nScaleJawSpecialMax = 255;
	m_nScaleJawHMin = 0;			m_nScaleJawHMax = 255;
	m_nScaleJawVMin = 0;			m_nScaleJawVMax = 255;

	m_nScaleEyeHMin = 0;			m_nScaleEyeHMax = 255;
	m_nScaleEyeVMin = 0;			m_nScaleEyeVMax = 255;
	m_nRotateEyeMin = 0;			m_nRotateEyeMax = 255;
	m_nOffsetEyeHMin = 0;			m_nOffsetEyeHMax = 255;
	m_nOffsetEyeVMin = 0;			m_nOffsetEyeVMax = 255;
	m_nOffsetEyeZMin = 0;			m_nOffsetEyeZMax = 255;
	m_nScaleEyeBallMin = 0;			m_nScaleEyeBallMax = 255;


	
	m_nScaleBrowHMin = 0;			m_nScaleBrowHMax = 255;
	m_nScaleBrowVMin = 0;			m_nScaleBrowVMax = 255;
	m_nRotateBrowMin = 0;			m_nRotateBrowMax = 255;
	m_nOffsetBrowHMin = 0;			m_nOffsetBrowHMax = 255;
	m_nOffsetBrowVMin = 0;			m_nOffsetBrowVMax = 255;
	m_nOffsetBrowZMin = 0;			m_nOffsetBrowZMax = 255;
	


	m_nScaleNoseTipHMin = 0;		m_nScaleNoseTipHMax = 255;
	m_nScaleNoseTipVMin = 0;		m_nScaleNoseTipVMax = 255;
	m_nOffsetNoseTipVMin = 0;		m_nOffsetNoseTipVMax = 255;
	m_nScaleNoseTipZMin = 0;		m_nScaleNoseTipZMax = 255;
	m_nScaleBridgeTipHMin = 0;		m_nScaleBridgeTipHMax = 255;
	m_nOffsetBridgeTipZMin = 0;		m_nOffsetBridgeTipZMax = 255;

	m_nThickUpLipMin = 0;			m_nThickUpLipMax = 255;
	m_nThickDownLipMin = 0;			m_nThickDownLipMax = 255;
	m_nScaleMouthHMin = 0;			m_nScaleMouthHMax = 255;
	m_nOffsetMouthVMin = 0;			m_nOffsetMouthVMax = 255;
	m_nOffsetMouthZMin = 0;			m_nOffsetMouthZMax = 255;
	m_nOffsetCornerOfMouthSpecialMin = 0; m_nOffsetCornerOfMouthSpecialMax = 255;
	m_nScaleMouthH2Min = 0;			m_nScaleMouthH2Max = 255;
	m_nOffsetCornerOfMouthSpecial2Min = 0; m_nOffsetCornerOfMouthSpecial2Max = 255;

	m_nScaleEarMin = 0;				m_nScaleEarMax = 255;
	m_nOffsetEarVMin = 0;			m_nOffsetEarVMax = 255;
	
	m_nHeadScaleMin = 0;			m_nHeadScaleMax = 255;
	m_nUpScaleMin = 0;				m_nUpScaleMax = 255;
	m_nWaistScaleMin = 0;			m_nWaistScaleMax = 255;
	m_nArmWidthMin = 0;				m_nArmWidthMax = 255;
	m_nLegWidthMin = 0;				m_nLegWidthMax = 255;
	m_nBreastScaleMin = 0;			m_nBreastScaleMax = 255;

}

CECCustomizeBound::~CECCustomizeBound()
{

}

//获取调节范围
bool CECCustomizeBound::LoadCustomizeBound(const char* pszFileName)
{
	AIniFile IniFile;

	
	if (!IniFile.Open(pszFileName))
	{
		a_LogOutput(1, "CECCustomize::LoadCustomizeBound(), Failed to open file Configs\\CustomizeBound.ini");
		return false;
	}
	AString strSection = "Version";
	m_nVersion = IniFile.GetValueAsInt(strSection, "Version", m_nVersion);
	
	strSection = "Face";
	m_nScaleFaceHMin = IniFile.GetValueAsInt(strSection, "ScaleFaceHMin", m_nScaleFaceHMin);
	m_nScaleFaceHMax = IniFile.GetValueAsInt(strSection, "ScaleFaceHMax", m_nScaleFaceHMax);
	m_nScaleFaceVMin = IniFile.GetValueAsInt(strSection, "ScaleFaceVMin", m_nScaleFaceVMin);
	m_nScaleFaceVMax = IniFile.GetValueAsInt(strSection, "ScaleFaceVMax", m_nScaleFaceVMax);
	
	strSection = "3Parts";
	m_nScaleUpPartMin = IniFile.GetValueAsInt(strSection, "ScaleUpPartMin", m_nScaleUpPartMin);
	m_nScaleUpPartMax = IniFile.GetValueAsInt(strSection, "ScaleUpPartMax", m_nScaleUpPartMax);
	m_nScaleMiddlePartMin = IniFile.GetValueAsInt(strSection, "ScaleMiddlePartMin", m_nScaleMiddlePartMin);
	m_nScaleMiddlePartMax = IniFile.GetValueAsInt(strSection, "ScaleMiddlePartMax", m_nScaleMiddlePartMax);
	m_nScaleDownPartMin = IniFile.GetValueAsInt(strSection, "ScaleDownPartMin", m_nScaleDownPartMin);
	m_nScaleDownPartMax = IniFile.GetValueAsInt(strSection, "ScaleDownpartMax", m_nScaleDownPartMax);

	strSection = "Forehead";
	m_nOffsetForeheadHMin = IniFile.GetValueAsInt(strSection, "OffsetForeheadHMin", m_nOffsetForeheadHMin);
	m_nOffsetForeheadHMax = IniFile.GetValueAsInt(strSection, "OffsetForeheadHMax", m_nOffsetForeheadHMax);
	m_nOffsetForeheadVMin = IniFile.GetValueAsInt(strSection, "OffsetForeheadVMin", m_nOffsetForeheadVMin);
	m_nOffsetForeheadVMax = IniFile.GetValueAsInt(strSection, "OffsetForeheadVMax", m_nOffsetForeheadVMax);	
	m_nOffsetForeheadZMin = IniFile.GetValueAsInt(strSection, "OffsetForeheadZMin", m_nOffsetForeheadZMin);
	m_nOffsetForeheadZMax = IniFile.GetValueAsInt(strSection, "OffsetForeheadZMax", m_nOffsetForeheadZMax);
	m_nRotateForeheadMin = IniFile.GetValueAsInt(strSection, "RotateForeheadMin", m_nRotateForeheadMin);
	m_nRotateForeheadMax = IniFile.GetValueAsInt(strSection, "RotateForeheadMax", m_nRotateForeheadMax);
	m_nScaleForeheadMin = IniFile.GetValueAsInt(strSection, "ScaleForeheadMin", m_nScaleForeheadMin);
	m_nScaleForeheadMax = IniFile.GetValueAsInt(strSection, "ScaleForeheadMax", m_nScaleForeheadMax);

	strSection = "YokeBone";
	m_nOffsetYokeBoneHMin = IniFile.GetValueAsInt(strSection, "OffsetYokeBoneHMin", m_nOffsetYokeBoneHMin);
	m_nOffsetYokeBoneHMax = IniFile.GetValueAsInt(strSection, "OffsetYokeBoneHMax", m_nOffsetYokeBoneHMax);
	m_nOffsetYokeBoneVMin = IniFile.GetValueAsInt(strSection, "OffsetYokeBoneVMin", m_nOffsetYokeBoneVMin);
	m_nOffsetYokeBoneVMax = IniFile.GetValueAsInt(strSection, "OffsetYokeBoneVMax", m_nOffsetYokeBoneVMax);
	m_nOffsetYokeBoneZMin = IniFile.GetValueAsInt(strSection, "OffsetYokeBoneZMin", m_nOffsetYokeBoneZMin);
	m_nOffsetYokeBoneZMax = IniFile.GetValueAsInt(strSection, "OffsetYokeBoneZMax", m_nOffsetYokeBoneZMax);
	m_nRotateYokeBoneMin = IniFile.GetValueAsInt(strSection, "RotateYokeBoneMin", m_nRotateYokeBoneMin);
	m_nRotateYokeBoneMax = IniFile.GetValueAsInt(strSection, "RotateYokeBoneMax", m_nRotateYokeBoneMax);
	m_nScaleYokeBoneMin = IniFile.GetValueAsInt(strSection, "ScaleYokeBoneMin", m_nScaleYokeBoneMin);
	m_nScaleYokeBoneMax = IniFile.GetValueAsInt(strSection, "ScaleYokeBoneMax", m_nScaleYokeBoneMax);

	strSection = "Cheek";
	m_nOffsetCheekHMin = IniFile.GetValueAsInt(strSection, "OffsetCheekHMin", m_nOffsetCheekHMin);
	m_nOffsetCheekHMax = IniFile.GetValueAsInt(strSection, "OffsetCheekHMax", m_nOffsetCheekHMax);
	m_nOffsetCheekVMin = IniFile.GetValueAsInt(strSection, "OffsetCheekVMin", m_nOffsetCheekVMin);
	m_nOffsetCheekVMax = IniFile.GetValueAsInt(strSection, "OffsetCheekVMax", m_nOffsetCheekVMax);
	m_nOffsetCheekZMin = IniFile.GetValueAsInt(strSection, "OffsetCheekZMin", m_nOffsetCheekZMin);
	m_nOffsetCheekZMax = IniFile.GetValueAsInt(strSection, "OffsetCheekZMax", m_nOffsetCheekZMax);
	m_nScaleCheekMin = IniFile.GetValueAsInt(strSection, "ScaleCheekMin", m_nScaleCheekMin);
	m_nScaleCheekMax = IniFile.GetValueAsInt(strSection, "ScaleCheekMax", m_nScaleCheekMax);

	strSection = "Chain";
	m_nOffsetChainZMin = IniFile.GetValueAsInt(strSection, "OffsetChainZMin", m_nOffsetChainZMin);
	m_nOffsetChainZMax = IniFile.GetValueAsInt(strSection, "OffsetChainZMax", m_nOffsetChainZMax);
	m_nOffsetChainVMin = IniFile.GetValueAsInt(strSection, "OffsetChainVMin", m_nOffsetChainVMin);
	m_nOffsetChainVMax = IniFile.GetValueAsInt(strSection, "OffsetChainVMax", m_nOffsetChainVMax);
	m_nRotateChainMin = IniFile.GetValueAsInt(strSection, "RotateChainMin", m_nRotateChainMin);
	m_nRotateChainMax = IniFile.GetValueAsInt(strSection, "RotateChainMax", m_nRotateChainMax);
	m_nScaleChainHMin = IniFile.GetValueAsInt(strSection, "ScaleChainHMin", m_nScaleChainHMin);
	m_nScaleChainHMax = IniFile.GetValueAsInt(strSection, "ScaleChainHMax", m_nScaleChainHMax);

	strSection = "Jaw";
	m_nOffsetJawHMin = IniFile.GetValueAsInt(strSection, "OffsetJawHMin", m_nOffsetJawHMin);
	m_nOffsetJawHMax = IniFile.GetValueAsInt(strSection, "OffsetJawHMax", m_nOffsetJawHMax);
	m_nOffsetJawVMin = IniFile.GetValueAsInt(strSection, "OffsetJawVMin", m_nOffsetJawVMin);
	m_nOffsetJawVMax = IniFile.GetValueAsInt(strSection, "OffsetJawVMax", m_nOffsetJawVMax);
	m_nOffsetJawZMin = IniFile.GetValueAsInt(strSection, "OffsetJawZMin", m_nOffsetJawZMin);
	m_nOffsetJawZMax = IniFile.GetValueAsInt(strSection, "OffsetJawZMax", m_nOffsetJawZMax);
	m_nScaleJawSpecialMin = IniFile.GetValueAsInt(strSection, "ScaleJawSpecialMin", m_nScaleJawSpecialMin);
	m_nScaleJawSpecialMax = IniFile.GetValueAsInt(strSection, "ScaleJawSpecialMax", m_nScaleJawSpecialMax);
	m_nScaleJawHMin = IniFile.GetValueAsInt(strSection, "ScaleJawHMin", m_nScaleJawHMin);
	m_nScaleJawHMax = IniFile.GetValueAsInt(strSection, "ScaleJawHMax", m_nScaleJawHMax);
	m_nScaleJawVMin = IniFile.GetValueAsInt(strSection, "ScaleJawVMin", m_nScaleJawVMin);
	m_nScaleJawVMax = IniFile.GetValueAsInt(strSection, "ScaleJawVMax", m_nScaleJawVMax);
	
	strSection = "Eye";
	m_nScaleEyeHMin = IniFile.GetValueAsInt(strSection, "ScaleEyeHMin", m_nScaleEyeHMin);
	m_nScaleEyeHMax = IniFile.GetValueAsInt(strSection, "ScaleEyeHMax", m_nScaleEyeHMax);
	m_nScaleEyeVMin = IniFile.GetValueAsInt(strSection, "ScaleEyeVMin", m_nScaleEyeVMin);
	m_nScaleEyeVMax = IniFile.GetValueAsInt(strSection, "ScaleEyeVMax", m_nScaleEyeVMax);
	m_nRotateEyeMin = IniFile.GetValueAsInt(strSection, "RotateEyeMin", m_nRotateEyeMin);
	m_nRotateEyeMax = IniFile.GetValueAsInt(strSection, "RotateEyeMax", m_nRotateEyeMax);
	m_nOffsetEyeHMin = IniFile.GetValueAsInt(strSection, "OffsetEyeHMin", m_nOffsetEyeHMin);
	m_nOffsetEyeHMax = IniFile.GetValueAsInt(strSection, "OffsetEyeHMax", m_nOffsetEyeHMax);
	m_nOffsetEyeVMin = IniFile.GetValueAsInt(strSection, "OffsetEyeVMin", m_nOffsetEyeVMin);
	m_nOffsetEyeVMax = IniFile.GetValueAsInt(strSection, "OffsetEyeVMax", m_nOffsetEyeVMax);
	m_nOffsetEyeZMin = IniFile.GetValueAsInt(strSection, "OffsetEyeZMin", m_nOffsetEyeZMin);
	m_nOffsetEyeZMax = IniFile.GetValueAsInt(strSection, "OffsetEyeZMax", m_nOffsetEyeZMax);
	m_nScaleEyeBallMin = IniFile.GetValueAsInt(strSection, "ScaleEyeBallMin", m_nScaleEyeBallMin);
	m_nScaleEyeBallMax = IniFile.GetValueAsInt(strSection, "ScaleEyeBallMax", m_nScaleEyeBallMax);


	strSection = "Brow";
	m_nScaleBrowHMin = IniFile.GetValueAsInt(strSection, "ScaleBrowHMin", m_nScaleBrowHMin);
	m_nScaleBrowHMax = IniFile.GetValueAsInt(strSection, "ScaleBrowHMax", m_nScaleBrowHMax);
	m_nScaleBrowVMin = IniFile.GetValueAsInt(strSection, "ScaleBrowVMin", m_nScaleBrowVMin);
	m_nScaleBrowVMax = IniFile.GetValueAsInt(strSection, "ScaleBrowVMax", m_nScaleBrowVMax);
	m_nRotateBrowMin = IniFile.GetValueAsInt(strSection, "RotateBrowMin", m_nRotateBrowMin);
	m_nRotateBrowMax = IniFile.GetValueAsInt(strSection, "RotateBrowMax", m_nRotateBrowMax);
	m_nOffsetBrowHMin = IniFile.GetValueAsInt(strSection, "OffsetBrowHMin", m_nOffsetBrowHMin);
	m_nOffsetBrowHMax = IniFile.GetValueAsInt(strSection, "OffsetBrowHMax", m_nOffsetBrowHMax);
	m_nOffsetBrowVMin = IniFile.GetValueAsInt(strSection, "OffsetBrowVMin", m_nOffsetBrowVMin);
	m_nOffsetBrowVMax = IniFile.GetValueAsInt(strSection, "OffsetBrowVMax", m_nOffsetBrowVMax);
	m_nOffsetBrowZMin = IniFile.GetValueAsInt(strSection, "OffsetBrowZMin", m_nOffsetBrowZMin);
	m_nOffsetBrowZMax = IniFile.GetValueAsInt(strSection, "OffsetBrowZMax", m_nOffsetBrowZMax);


	strSection = "Nose";
	m_nScaleNoseTipHMin = IniFile.GetValueAsInt(strSection, "ScaleNoseTipHMin", m_nScaleNoseTipHMin);
	m_nScaleNoseTipHMax = IniFile.GetValueAsInt(strSection, "ScaleNoseTipHMax", m_nScaleNoseTipHMax);
	m_nScaleNoseTipVMin = IniFile.GetValueAsInt(strSection, "ScaleNoseTipVMin", m_nScaleNoseTipVMin);
	m_nScaleNoseTipVMax = IniFile.GetValueAsInt(strSection, "ScaleNoseTipVMax", m_nScaleNoseTipVMax);
	m_nOffsetNoseTipVMin = IniFile.GetValueAsInt(strSection, "OffsetNoseTipVMin", m_nOffsetNoseTipVMin);
	m_nOffsetNoseTipVMax = IniFile.GetValueAsInt(strSection, "OffsetNoseTipVMax", m_nOffsetNoseTipVMax);
	m_nScaleNoseTipZMin = IniFile.GetValueAsInt(strSection, "ScaleNoseTipZMin", m_nScaleNoseTipZMin);
	m_nScaleNoseTipZMax = IniFile.GetValueAsInt(strSection, "ScaleNoseTipZMax", m_nScaleNoseTipZMax);
	m_nScaleBridgeTipHMin = IniFile.GetValueAsInt(strSection, "ScaleBridgeTipHMin", m_nScaleBridgeTipHMin);
	m_nScaleBridgeTipHMax = IniFile.GetValueAsInt(strSection, "ScaleBridgeTipHMax", m_nScaleBridgeTipHMax);
	m_nOffsetBridgeTipZMin = IniFile.GetValueAsInt(strSection, "OffsetBridgeTipZMin", m_nOffsetBridgeTipZMin);
	m_nOffsetBridgeTipZMax = IniFile.GetValueAsInt(strSection, "OffsetBridgeTipZMax", m_nOffsetBridgeTipZMax);
	
	strSection = "Mouth";
	m_nThickUpLipMin = IniFile.GetValueAsInt(strSection, "ThickUpLipMin", m_nThickUpLipMin);
	m_nThickUpLipMax = IniFile.GetValueAsInt(strSection, "ThickUpLipMax", m_nThickUpLipMax);
	m_nThickDownLipMin = IniFile.GetValueAsInt(strSection, "ThickDownLipMin", m_nThickDownLipMin);
	m_nThickDownLipMax = IniFile.GetValueAsInt(strSection, "ThickDownLipMax", m_nThickDownLipMax);
	m_nScaleMouthHMin = IniFile.GetValueAsInt(strSection, "ScaleMouthHMin", m_nScaleMouthHMin);
	m_nScaleMouthHMax = IniFile.GetValueAsInt(strSection, "ScaleMouthHMax", m_nScaleMouthHMax);
	m_nOffsetMouthVMin = IniFile.GetValueAsInt(strSection, "OffsetMouthVMin", m_nOffsetMouthVMin);
	m_nOffsetMouthVMax = IniFile.GetValueAsInt(strSection, "OffsetMouthVMax", m_nOffsetMouthVMax);
	m_nOffsetMouthZMin = IniFile.GetValueAsInt(strSection, "OffsetMouthZMin", m_nOffsetMouthZMin);
	m_nOffsetMouthZMax = IniFile.GetValueAsInt(strSection, "OffsetMouthZMax", m_nOffsetMouthZMax);
	m_nOffsetCornerOfMouthSpecialMin = IniFile.GetValueAsInt(strSection, "OffsetCornerOfMouthSpecialMin", m_nOffsetCornerOfMouthSpecialMin);
	m_nOffsetCornerOfMouthSpecialMax = IniFile.GetValueAsInt(strSection, "OffsetCornerOfMouthSpecialMax", m_nOffsetCornerOfMouthSpecialMax);
	m_nScaleMouthH2Min = IniFile.GetValueAsInt(strSection, "ScaleMouthH2Min", m_nScaleMouthH2Min);
	m_nScaleMouthH2Max = IniFile.GetValueAsInt(strSection, "ScaleMouthH2Max", m_nScaleMouthH2Max);
	m_nOffsetCornerOfMouthSpecial2Min = IniFile.GetValueAsInt(strSection, "OffsetCornerOfMouthSpecial2Min", m_nOffsetCornerOfMouthSpecial2Min);
	m_nOffsetCornerOfMouthSpecial2Max = IniFile.GetValueAsInt(strSection, "OffsetCornerOfMouthSpecial2Max", m_nOffsetCornerOfMouthSpecial2Max);

	strSection = "Ear";
	m_nScaleEarMin = IniFile.GetValueAsInt(strSection, "ScaleEarMin", m_nScaleEarMin);
	m_nScaleEarMax = IniFile.GetValueAsInt(strSection, "ScaleEarMax", m_nScaleEarMax);
	m_nOffsetEarVMin = IniFile.GetValueAsInt(strSection, "OffsetEarVMin", m_nOffsetEarVMin);
	m_nOffsetEarVMax = IniFile.GetValueAsInt(strSection, "OffsetEarVMax", m_nOffsetEarVMax);

	strSection = "Body";
	m_nHeadScaleMin = IniFile.GetValueAsInt(strSection, "HeadScaleMin", m_nHeadScaleMin);
	m_nHeadScaleMax = IniFile.GetValueAsInt(strSection, "HeadScaleMax", m_nHeadScaleMax);
	m_nUpScaleMin = IniFile.GetValueAsInt(strSection, "UpScaleMin", m_nUpScaleMin);
	m_nUpScaleMax = IniFile.GetValueAsInt(strSection, "UpScaleMax", m_nUpScaleMax);
	m_nWaistScaleMin = IniFile.GetValueAsInt(strSection, "WaistScaleMin", m_nWaistScaleMin);
	m_nWaistScaleMax = IniFile.GetValueAsInt(strSection, "WaistScaleMax", m_nWaistScaleMax);
	m_nArmWidthMin = IniFile.GetValueAsInt(strSection, "ArmWidthMin", m_nArmWidthMin);
	m_nArmWidthMax = IniFile.GetValueAsInt(strSection, "ArmWidthMax", m_nArmWidthMax);
	m_nLegWidthMin = IniFile.GetValueAsInt(strSection, "LegWidthMin", m_nLegWidthMin);
	m_nLegWidthMax = IniFile.GetValueAsInt(strSection, "LegWidthMax", m_nLegWidthMax);
	m_nBreastScaleMin = IniFile.GetValueAsInt(strSection, "BreastScaleMin", m_nBreastScaleMin);
	m_nBreastScaleMax = IniFile.GetValueAsInt(strSection, "BreastScaleMax", m_nBreastScaleMax);

	IniFile.Close();
	
	return true;
	
}

void CECCustomizeBound::ClampCustomizeDatas(CECPlayer::PLAYER_CUSTOMIZEDATA& data)
{
	ClampData(data.faceData.scaleFaceH, m_nScaleFaceHMin, m_nScaleFaceHMax);
	ClampData(data.faceData.scaleFaceV, m_nScaleFaceVMin, m_nScaleFaceVMax);

	ClampData(data.faceData.scaleUp, m_nScaleUpPartMin, m_nScaleUpPartMax);
	ClampData(data.faceData.scaleMiddle, m_nScaleMiddlePartMin, m_nScaleMiddlePartMax);
	ClampData(data.faceData.scaleDown, m_nScaleDownPartMin, m_nScaleDownPartMax);
	
	ClampData(data.faceData.offsetForeheadH, m_nOffsetForeheadHMin, m_nOffsetForeheadHMax);
	ClampData(data.faceData.offsetForeheadV, m_nOffsetForeheadVMin, m_nOffsetForeheadVMax);
	ClampData(data.faceData.offsetForeheadZ, m_nOffsetForeheadZMin, m_nOffsetForeheadZMax);
	ClampData(data.faceData.rotateForehead, m_nRotateForeheadMin, m_nRotateForeheadMax);
	ClampData(data.faceData.scaleForehead, m_nScaleForeheadMin, m_nScaleForeheadMax);

	ClampData(data.faceData.offsetYokeBoneH, m_nOffsetYokeBoneHMin, m_nOffsetYokeBoneHMax);
	ClampData(data.faceData.offsetYokeBoneV, m_nOffsetYokeBoneVMin, m_nOffsetYokeBoneVMax);
	ClampData(data.faceData.offsetYokeBoneZ, m_nOffsetYokeBoneZMin, m_nOffsetYokeBoneZMax);
	ClampData(data.faceData.rotateYokeBone, m_nRotateYokeBoneMin, m_nRotateYokeBoneMax);
	ClampData(data.faceData.scaleYokeBone, m_nScaleYokeBoneMin, m_nScaleYokeBoneMax);

	ClampData(data.faceData.offsetCheekH, m_nOffsetCheekHMin, m_nOffsetCheekHMax);
	ClampData(data.faceData.offsetCheekV, m_nOffsetCheekVMin, m_nOffsetCheekVMax);
	ClampData(data.faceData.offsetCheekZ, m_nOffsetCheekZMin, m_nOffsetCheekZMax);
	ClampData(data.faceData.scaleCheek, m_nScaleCheekMin, m_nScaleCheekMax);

	ClampData(data.faceData.offsetChainZ, m_nOffsetChainZMin, m_nOffsetChainZMax);
	ClampData(data.faceData.offsetChainV, m_nOffsetChainVMin, m_nOffsetChainVMax);
	ClampData(data.faceData.rotateChain, m_nRotateChainMin, m_nRotateChainMax);
	ClampData(data.faceData.scaleChainH, m_nScaleChainHMin, m_nScaleChainHMax);
	
	ClampData(data.faceData.offsetJawH, m_nOffsetJawHMin, m_nOffsetJawHMax);
	ClampData(data.faceData.offsetJawV, m_nOffsetJawVMin, m_nOffsetJawVMax);
	ClampData(data.faceData.offsetJawZ, m_nOffsetJawZMin, m_nOffsetJawZMax);
	ClampData(data.faceData.scaleJawSpecial, m_nScaleJawSpecialMin, m_nScaleJawSpecialMax);
	ClampData(data.faceData.scaleJawH, m_nScaleJawHMin, m_nScaleJawHMax);
	ClampData(data.faceData.scaleJawV, m_nScaleJawVMin, m_nScaleJawVMax);
	
	ClampData(data.faceData.scaleEyeH, m_nScaleEyeHMin, m_nScaleEyeHMax);
	ClampData(data.faceData.scaleEyeV, m_nScaleEyeVMin, m_nScaleEyeVMax);
	ClampData(data.faceData.rotateEye, m_nRotateEyeMin, m_nRotateEyeMax);
	ClampData(data.faceData.offsetEyeH, m_nOffsetEyeHMin, m_nOffsetEyeHMax);
	ClampData(data.faceData.offsetEyeV, m_nOffsetEyeVMin, m_nOffsetEyeVMax);
	ClampData(data.faceData.offsetEyeZ, m_nOffsetEyeZMin, m_nOffsetEyeZMax);
	ClampData(data.faceData.scaleEyeBall, m_nScaleEyeBallMin, m_nScaleEyeBallMax);

	ClampData(data.faceData.scaleEyeH2, m_nScaleEyeHMin, m_nScaleEyeHMax);
	ClampData(data.faceData.scaleEyeV2, m_nScaleEyeVMin, m_nScaleEyeVMax);
	ClampData(data.faceData.rotateEye2, m_nRotateEyeMin, m_nRotateEyeMax);
	ClampData(data.faceData.offsetEyeH2, m_nOffsetEyeHMin, m_nOffsetEyeHMax);
	ClampData(data.faceData.offsetEyeV2, m_nOffsetEyeVMin, m_nOffsetEyeVMax);
	ClampData(data.faceData.offsetEyeZ2, m_nOffsetEyeZMin, m_nOffsetEyeZMax);
	ClampData(data.faceData.scaleEyeBall2, m_nScaleEyeBallMin, m_nScaleEyeBallMax);

	ClampData(data.faceData.scaleBrowH, m_nScaleBrowHMin, m_nScaleBrowHMax);
	ClampData(data.faceData.scaleBrowV, m_nScaleBrowVMin, m_nScaleBrowVMax);
	ClampData(data.faceData.rotateBrow, m_nRotateBrowMin, m_nRotateBrowMax);
	ClampData(data.faceData.offsetBrowH, m_nOffsetBrowHMin, m_nOffsetBrowHMax);
	ClampData(data.faceData.offsetBrowV, m_nOffsetBrowVMin, m_nOffsetBrowVMax);
	ClampData(data.faceData.offsetBrowZ, m_nOffsetBrowZMin, m_nOffsetBrowZMax);
	
	ClampData(data.faceData.scaleBrowH2, m_nScaleBrowHMin, m_nScaleBrowHMax);
	ClampData(data.faceData.scaleBrowV2, m_nScaleBrowVMin, m_nScaleBrowVMax);
	ClampData(data.faceData.rotateBrow2, m_nRotateBrowMin, m_nRotateBrowMax);
	ClampData(data.faceData.offsetBrowH2, m_nOffsetBrowHMin, m_nOffsetBrowHMax);
	ClampData(data.faceData.offsetBrowV2, m_nOffsetBrowVMin, m_nOffsetBrowVMax);
	ClampData(data.faceData.offsetBrowZ2, m_nOffsetBrowZMin, m_nOffsetBrowZMax);
	
	ClampData(data.faceData.scaleNoseTipH, m_nScaleNoseTipHMin, m_nScaleNoseTipHMax);
	ClampData(data.faceData.scaleNoseTipV, m_nScaleNoseTipVMin, m_nScaleNoseTipVMax);
	ClampData(data.faceData.offsetNoseTipV, m_nOffsetNoseTipVMin, m_nOffsetNoseTipVMax);
	ClampData(data.faceData.scaleNoseTipZ, m_nScaleNoseTipZMin, m_nScaleNoseTipZMax); 
	ClampData(data.faceData.scaleBridgeTipH, m_nScaleBridgeTipHMin, m_nScaleBridgeTipHMax);
	ClampData(data.faceData.offsetBridgeTipZ, m_nOffsetBridgeTipZMin, m_nOffsetBridgeTipZMax);

	ClampData(data.faceData.thickUpLip, m_nThickUpLipMin, m_nThickUpLipMax);
	ClampData(data.faceData.thickDownLip, m_nThickDownLipMin, m_nThickDownLipMax);
	ClampData(data.faceData.scaleMouthH, m_nScaleMouthHMin, m_nScaleMouthHMax);
	ClampData(data.faceData.offsetMouthV, m_nOffsetMouthVMin, m_nOffsetMouthVMax);
	ClampData(data.faceData.offsetMouthZ, m_nOffsetMouthZMin, m_nOffsetMouthZMax);
	ClampData(data.faceData.offsetCornerOfMouthSpecial, m_nOffsetCornerOfMouthSpecialMin, m_nOffsetCornerOfMouthSpecialMax);
	ClampData(data.faceData.scaleMouthH2, m_nScaleMouthH2Min, m_nScaleMouthH2Max);
	ClampData(data.faceData.offsetCornerOfMouthSpecial2, m_nOffsetCornerOfMouthSpecial2Min, m_nOffsetCornerOfMouthSpecial2Max);
	
	ClampData(data.faceData.scaleEar, m_nScaleEarMin, m_nScaleEarMax);
	ClampData(data.faceData.offsetEarV, m_nOffsetEarVMin, m_nOffsetEarVMax);
	
	ClampData(data.headScale, m_nHeadScaleMin, m_nHeadScaleMax);
	ClampData(data.upScale, m_nUpScaleMin, m_nUpScaleMax);
	ClampData(data.waistScale, m_nWaistScaleMin, m_nWaistScaleMax);
	ClampData(data.armWidth, m_nArmWidthMin, m_nArmWidthMax);
	ClampData(data.legWidth, m_nLegWidthMin, m_nLegWidthMax);
	ClampData(data.breastScale, m_nBreastScaleMin, m_nBreastScaleMax);
}

void CECCustomizeBound::ClampData(unsigned char &data, int nMin, int nMax)
{
	assert(nMin < nMax);
	
	if( data < nMin)
		data = nMin;
	if( data > nMax)
		data = nMax;
}

CECCustomizeBoundMgr g_CustomizeBoundMgr;

CECCustomizeBoundMgr::CECCustomizeBoundMgr()
{

}

CECCustomizeBoundMgr::~CECCustomizeBoundMgr()
{
	CustomizeBoundMapIter_t iter = m_CustomizeBoundMap.begin();
	CustomizeBoundMapIter_t iterEnd = m_CustomizeBoundMap.end();

	for(; iter != iterEnd; ++iter)
	{
		delete iter->second;
	}

}

CECCustomizeBound* CECCustomizeBoundMgr::GetCustomizeBound(const char* pszFileName)
{
	CustomizeBoundMapIter_t iter = m_CustomizeBoundMap.find(pszFileName);
	
	if( iter == m_CustomizeBoundMap.end())
	{
		CECCustomizeBound *pCustomizeBound = new CECCustomizeBound;
		if(pCustomizeBound->LoadCustomizeBound(pszFileName))
		{
			m_CustomizeBoundMap.insert(CustomizeBoundMap_t::value_type(pszFileName, pCustomizeBound));
			return pCustomizeBound;
		}
		else
		{
			delete pCustomizeBound;

			return NULL;
		}

		
	}

	return iter->second;
}