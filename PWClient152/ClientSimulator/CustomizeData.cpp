/*
 * FILE: CustomizeData.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/12/26
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#include "Common.h"
#include "CustomizeData.h"
#include <AIniFile.h>


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macros
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Implement the Global functions
//  
///////////////////////////////////////////////////////////////////////////

// 载入形象的默认个性化数据
bool LoadDefaultCustomizeData(int iProfession, int iGender, PLAYER_CUSTOMIZEDATA& data)
{
	memset(&data, 0, sizeof(data));
	data.dwVersion = CUSTOMIZE_DATA_VERSION;
	data.colorBody = 0xffffffff;
	data.headScale = 128;
	data.upScale = 128;
	data.waistScale = 128;
	data.armWidth = 128;
	data.legWidth = 128;
	data.breastScale = 128;

	AIniFile IniFile;
	char szFile[256];
	sprintf(szFile, "userdata\\character\\character%d%d.ini", iProfession, iGender);
	
	if(!IniFile.Open(szFile))
	{
		a_LogOutput(1, "LoadDefaultCustomizeData, Failed to open file %s", szFile);
		return false;
	}

	//3庭3
	AString strSection = "3Parts";
	data.faceData.scaleUp = IniFile.GetValueAsInt(strSection, "scaleUp", data.faceData.scaleUp);
	data.faceData.scaleMiddle = IniFile.GetValueAsInt(strSection, "scaleMiddle", data.faceData.scaleMiddle);
	data.faceData.scaleDown = IniFile.GetValueAsInt(strSection, "scaleDown", data.faceData.scaleDown);

	//脸型融合3
	strSection = "BlendFace";
	data.faceData.idFaceShape1 = IniFile.GetValueAsInt(strSection, "idFaceShape1", data.faceData.idFaceShape1);
	data.faceData.idFaceShape2 = IniFile.GetValueAsInt(strSection, "idFaceShape2", data.faceData.idFaceShape2);
	data.faceData.blendFaceShape = IniFile.GetValueAsInt(strSection, "blendFaceShape", data.faceData.blendFaceShape);

	//脸盘调整3
	strSection = "Face";
	data.faceData.scaleFaceH = IniFile.GetValueAsInt(strSection, "scaleFaceH", data.faceData.scaleFaceH);
	data.faceData.scaleFaceV = IniFile.GetValueAsInt(strSection, "scaleFaceV", data.faceData.scaleFaceV);
	data.faceData.idFaceTex = IniFile.GetValueAsInt(strSection, "idFaceTex", data.faceData.idFaceTex);

	//法令
	strSection = "Faling";
	data.faceData.idFalingSkin = IniFile.GetValueAsInt(strSection, "idFalingSkin", data.faceData.idFalingSkin);
	data.faceData.idFalingTex = IniFile.GetValueAsInt(strSection, "idFalingTex", data.faceData.idFalingTex);

	//额头5
	strSection = "Forehead";
	data.faceData.offsetForeheadH = IniFile.GetValueAsInt(strSection, "offsetForeheadH", data.faceData.offsetForeheadH);
	data.faceData.offsetForeheadV = IniFile.GetValueAsInt(strSection, "offsetForeheadV", data.faceData.offsetForeheadV);
	data.faceData.offsetForeheadZ = IniFile.GetValueAsInt(strSection, "offsetForeheadZ", data.faceData.offsetForeheadZ);
	data.faceData.rotateForehead = IniFile.GetValueAsInt(strSection, "rotateForehead", data.faceData.rotateForehead);
	data.faceData.scaleForehead = IniFile.GetValueAsInt(strSection, "scaleForehead", data.faceData.scaleForehead);

	//颧骨5
	strSection = "YokeBone";
	data.faceData.offsetYokeBoneH = IniFile.GetValueAsInt(strSection, "offsetYokeBoneH", data.faceData.offsetYokeBoneH);
	data.faceData.offsetYokeBoneV = IniFile.GetValueAsInt(strSection, "offsetYokeBoneV", data.faceData.offsetYokeBoneV);
	data.faceData.offsetYokeBoneZ = IniFile.GetValueAsInt(strSection, "offsetYokeBoneZ", data.faceData.offsetYokeBoneZ);
	data.faceData.rotateYokeBone = IniFile.GetValueAsInt(strSection, "rotateYokeBone", data.faceData.rotateYokeBone);
	data.faceData.scaleYokeBone = IniFile.GetValueAsInt(strSection, "scaleYokeBone", data.faceData.scaleYokeBone);
	
	//脸颊4
	strSection = "Cheek";
	data.faceData.offsetCheekH = IniFile.GetValueAsInt(strSection, "offsetCheekH", data.faceData.offsetCheekH);
	data.faceData.offsetCheekV = IniFile.GetValueAsInt(strSection, "offsetCheekV", data.faceData.offsetCheekV);
	data.faceData.offsetCheekZ = IniFile.GetValueAsInt(strSection, "offsetCheekZ", data.faceData.offsetCheekZ);
	data.faceData.scaleCheek = IniFile.GetValueAsInt(strSection, "scaleCheek", data.faceData.scaleCheek);

	//下巴4
	strSection = "Chain";
	data.faceData.offsetChainV = IniFile.GetValueAsInt(strSection, "offsetChainV", data.faceData.offsetChainV);
	data.faceData.offsetChainZ = IniFile.GetValueAsInt(strSection, "offsetChainZ", data.faceData.offsetChainZ);
	data.faceData.rotateChain = IniFile.GetValueAsInt(strSection, "rotateChain", data.faceData.rotateChain);
	data.faceData.scaleChainH = IniFile.GetValueAsInt(strSection, "scaleChainH", data.faceData.scaleChainH);

	//颌骨6
	strSection = "Jaw";
	data.faceData.offsetJawH = IniFile.GetValueAsInt(strSection, "offsetJawH", data.faceData.offsetJawH);
	data.faceData.offsetJawV = IniFile.GetValueAsInt(strSection, "offsetJawV", data.faceData.offsetJawV);
	data.faceData.offsetJawZ = IniFile.GetValueAsInt(strSection, "offsetJawZ", data.faceData.offsetJawZ);
	data.faceData.scaleJawSpecial = IniFile.GetValueAsInt(strSection, "scaleJawSpecial", data.faceData.scaleJawSpecial);
	data.faceData.scaleJawH = IniFile.GetValueAsInt(strSection, "scaleJawH", data.faceData.scaleJawH);
	data.faceData.scaleJawV = IniFile.GetValueAsInt(strSection, "scaleJawV", data.faceData.scaleJawV);

	//眼睛18
	strSection = "Eye";
	data.faceData.idThirdEye	= IniFile.GetValueAsInt(strSection, "idThirdEye", data.faceData.idThirdEye);
	data.faceData.idEyeBaseTex = IniFile.GetValueAsInt(strSection, "idEyeBaseTex", data.faceData.idEyeBaseTex);
	data.faceData.idEyeHighTex = IniFile.GetValueAsInt(strSection, "idEyeHighTex", data.faceData.idEyeHighTex);
	data.faceData.idEyeBallTex = IniFile.GetValueAsInt(strSection, "idEyeBallTex", data.faceData.idEyeBallTex);
	data.faceData.idEyeShape = IniFile.GetValueAsInt(strSection, "idEyeShape", data.faceData.idEyeShape);
	data.faceData.scaleEyeH = IniFile.GetValueAsInt(strSection, "scaleEyeH", data.faceData.scaleEyeH);
	data.faceData.scaleEyeV = IniFile.GetValueAsInt(strSection, "scaleEyeV", data.faceData.scaleEyeV);
	data.faceData.rotateEye = IniFile.GetValueAsInt(strSection, "rotateEye", data.faceData.rotateEye);
	data.faceData.offsetEyeH = IniFile.GetValueAsInt(strSection, "offsetEyeH", data.faceData.offsetEyeH);
	data.faceData.offsetEyeV = IniFile.GetValueAsInt(strSection, "offsetEyeV", data.faceData.offsetEyeV);
	data.faceData.offsetEyeZ = IniFile.GetValueAsInt(strSection, "offseteyeZ", data.faceData.offsetEyeZ);
	data.faceData.scaleEyeBall = IniFile.GetValueAsInt(strSection, "scaleEyeBall", data.faceData.scaleEyeBall);
	
	data.faceData.scaleEyeH2 = IniFile.GetValueAsInt(strSection, "scaleEyeH2", data.faceData.scaleEyeH2);
	data.faceData.scaleEyeV2 = IniFile.GetValueAsInt(strSection, "scaleEyeV2", data.faceData.scaleEyeV2);
	data.faceData.rotateEye2= IniFile.GetValueAsInt(strSection, "rotateEye2", data.faceData.rotateEye2);
	data.faceData.offsetEyeH2 = IniFile.GetValueAsInt(strSection, "offsetEyeH2", data.faceData.offsetEyeH2);
	data.faceData.offsetEyeV2 = IniFile.GetValueAsInt(strSection, "offsetEyeV2", data.faceData.offsetEyeV2);
	data.faceData.offsetEyeZ2 = IniFile.GetValueAsInt(strSection, "offseteyeZ2", data.faceData.offsetEyeZ2);
	data.faceData.scaleEyeBall2 = IniFile.GetValueAsInt(strSection, "scaleEyeBall2", data.faceData.scaleEyeBall2);

	//眉毛14
	strSection = "Brow";
	data.faceData.idBrowTex = IniFile.GetValueAsInt(strSection, "idBrowTex", data.faceData.idBrowTex);
	data.faceData.idBrowShape = IniFile.GetValueAsInt(strSection, "idBrowShape", data.faceData.idBrowShape);
	data.faceData.scaleBrowH = IniFile.GetValueAsInt(strSection, "scaleBrowH", data.faceData.scaleBrowH);
	data.faceData.scaleBrowV = IniFile.GetValueAsInt(strSection, "scaleBrowV", data.faceData.scaleBrowV);
	data.faceData.rotateBrow = IniFile.GetValueAsInt(strSection, "rotateBrow", data.faceData.rotateBrow);
	data.faceData.offsetBrowH = IniFile.GetValueAsInt(strSection, "offsetBrowH", data.faceData.offsetBrowH);
	data.faceData.offsetBrowV = IniFile.GetValueAsInt(strSection, "offsetBrowV", data.faceData.offsetBrowV);
	data.faceData.offsetBrowZ = IniFile.GetValueAsInt(strSection, "offsetBrowZ", data.faceData.offsetBrowZ);

	data.faceData.scaleBrowH2 = IniFile.GetValueAsInt(strSection, "scaleBrowH2", data.faceData.scaleBrowH2);
	data.faceData.scaleBrowV2 = IniFile.GetValueAsInt(strSection, "scaleBrowV2", data.faceData.scaleBrowV2);
	data.faceData.rotateBrow2 = IniFile.GetValueAsInt(strSection, "rotateBrow2", data.faceData.rotateBrow2);
	data.faceData.offsetBrowH2 = IniFile.GetValueAsInt(strSection, "offsetBrowH2", data.faceData.offsetBrowH2);
	data.faceData.offsetBrowV2 = IniFile.GetValueAsInt(strSection, "offsetBrowV2", data.faceData.offsetBrowV2);
	data.faceData.offsetBrowZ2 = IniFile.GetValueAsInt(strSection, "offsetBrowZ2", data.faceData.offsetBrowZ2);

	//鼻子9
	strSection = "Nose";
	data.faceData.idNoseTex = IniFile.GetValueAsInt(strSection, "idNoseTex", data.faceData.idNoseTex);
	data.faceData.idNoseTipShape = IniFile.GetValueAsInt(strSection, "idNoseTipShape", data.faceData.idNoseTipShape);
	data.faceData.scaleNoseTipH = IniFile.GetValueAsInt(strSection, "scaleNoseTipH", data.faceData.scaleNoseTipH);
	data.faceData.scaleNoseTipV = IniFile.GetValueAsInt(strSection, "scaleNoseTipV", data.faceData.scaleNoseTipV);
	data.faceData.scaleNoseTipZ = IniFile.GetValueAsInt(strSection, "scaleNoseTipZ", data.faceData.scaleNoseTipZ);
	data.faceData.offsetNoseTipV = IniFile.GetValueAsInt(strSection, "offsetNoseTipV", data.faceData.offsetNoseTipV);
	data.faceData.idNoseBridgeShape = IniFile.GetValueAsInt(strSection, "idNoseBridgeShape", data.faceData.idNoseBridgeShape);
	data.faceData.scaleBridgeTipH = IniFile.GetValueAsInt(strSection, "scaleBridgeTipH", data.faceData.scaleBridgeTipH);
	data.faceData.offsetBridgeTipZ = IniFile.GetValueAsInt(strSection, "OffsetBridgeTipZ", data.faceData.offsetBridgeTipZ);
	
	//嘴9
	strSection = "Mouth";
	data.faceData.idMouthUpLipLine = IniFile.GetValueAsInt (strSection, "idMouthUpLipLine", data.faceData.idMouthUpLipLine);
	data.faceData.idMouthMidLipLine = IniFile.GetValueAsInt(strSection, "idMouthMidLipLine", data.faceData.idMouthMidLipLine);
	data.faceData.idMouthDownLipLine = IniFile.GetValueAsInt(strSection, "idMouthDownLipLine", data.faceData.idMouthDownLipLine);
	data.faceData.thickUpLip = IniFile.GetValueAsInt(strSection, "thickUpLip", data.faceData.thickUpLip);
	data.faceData.thickDownLip = IniFile.GetValueAsInt(strSection, "thickDownLip", data.faceData.thickDownLip);
	data.faceData.scaleMouthH = IniFile.GetValueAsInt(strSection, "scaleMouthH", data.faceData.scaleMouthH);
	
	data.faceData.offsetMouthV = IniFile.GetValueAsInt(strSection, "offsetMouthV", data.faceData.offsetMouthV);
	data.faceData.offsetMouthZ = IniFile.GetValueAsInt(strSection, "offsetMOuthZ", data.faceData.offsetMouthZ);
	data.faceData.idMouthTex = IniFile.GetValueAsInt(strSection, "idMouthTex", data.faceData.idMouthTex);
	data.faceData.offsetCornerOfMouthSpecial = IniFile.GetValueAsInt(strSection, "offsetCornerOfMouthSpecial", data.faceData.offsetCornerOfMouthSpecial);

	data.faceData.scaleMouthH2 = IniFile.GetValueAsInt(strSection, "scaleMouthH2", data.faceData.scaleMouthH2);
	data.faceData.offsetCornerOfMouthSpecial2 = IniFile.GetValueAsInt(strSection, "offsetCornerOfMouthSpecial2", data.faceData.offsetCornerOfMouthSpecial2);

	//耳朵3
	strSection = "Ear";
	data.faceData.idEarShape = IniFile.GetValueAsInt(strSection, "idEarShape", data.faceData.idEarShape);
	data.faceData.scaleEar = IniFile.GetValueAsInt(strSection, "scaleEar", data.faceData.scaleEar);
	data.faceData.offsetEarV = IniFile.GetValueAsInt(strSection, "offsetEarV", data.faceData.offsetEarV);
	
	//头发1
	strSection = "Hair";
	data.faceData.idHairModel = IniFile.GetValueAsInt(strSection, "idHairModel", data.faceData.idHairModel);
	data.faceData.idHairTex = IniFile.GetValueAsInt(strSection, "idHairTex", data.faceData.idHairTex);

	//胡子2
	strSection = "Moustache";
	data.faceData.idMoustacheTex = IniFile.GetValueAsInt(strSection, "idMoustacheTex", data.faceData.idMoustacheTex);
	data.faceData.idMoustacheSkin = IniFile.GetValueAsInt(strSection, "idMoustacheSkin", data.faceData.idMoustacheSkin);
	data.faceData.idGoateeTex = IniFile.GetValueAsInt(strSection, "idGoateeTex", data.faceData.idGoateeTex);

	//各部分颜色7
	strSection = "Color";
	data.faceData.colorHair = IniFile.GetValueAsInt(strSection, "colorHair", data.faceData.colorHair);
	data.faceData.colorFace = IniFile.GetValueAsInt(strSection, "colorFace", data.faceData.colorFace);
	data.faceData.colorEye = IniFile.GetValueAsInt(strSection, "colorEye", data.faceData.colorEye);
	data.faceData.colorBrow = IniFile.GetValueAsInt(strSection, "colorBrow", data.faceData.colorBrow);
	data.faceData.colorMouth = IniFile.GetValueAsInt(strSection, "colorMouth", data.faceData.colorMouth);
	data.faceData.colorEyeBall = IniFile.GetValueAsInt(strSection, "colorEyeBall", data.faceData.colorEyeBall);
	data.faceData.colorMoustache = IniFile.GetValueAsInt(strSection, "colorMoustache", data.faceData.colorMoustache);

	// 身体参数
	strSection = "Body";
	data.bodyID = IniFile.GetValueAsInt(strSection, "bodyID", data.bodyID);
	data.colorBody = IniFile.GetValueAsInt(strSection, "colorBody", data.colorBody);
	data.headScale = IniFile.GetValueAsInt(strSection, "headScale", data.headScale);
	data.upScale = IniFile.GetValueAsInt(strSection, "upScale", data.upScale);
	data.waistScale = IniFile.GetValueAsInt(strSection, "waistScale", data.waistScale);
	data.armWidth = IniFile.GetValueAsInt(strSection, "armWidth", data.armWidth);
	data.legWidth = IniFile.GetValueAsInt(strSection, "legWidth", data.legWidth);
	data.breastScale = IniFile.GetValueAsInt(strSection, "breastScale", data.breastScale);
         
	IniFile.Close();

	return true;
}