/*
 * FILE: CustomizeData.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2012/12/26
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once

#include <ABaseDef.h>
#include <A3DTypes.h>


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


// 人脸个性化数据定义82
struct FACE_CUSTOMIZEDATA_1
{
	// 头部比例总体参数
	unsigned char			scaleUp;			// 上庭比例
	unsigned char			scaleMiddle;		// 中庭比例
	unsigned char			scaleDown;			// 下庭比例

	// 脸型调整所需的参数
	unsigned short			idFaceShape1;		// 脸型1
	unsigned short			idFaceShape2;		// 脸型2
	unsigned char			blendFaceShape;		// 脸型融合系数, 0.0~1.0, 为0.0时即是脸型1,为1.0时即是脸型2
	unsigned char			scaleFaceH;			// 脸盘左右比例调节(中心为鼻01)
	unsigned char			scaleFaceV;			// 脸盘上下比例调节(中心为鼻01)
	unsigned short			idFaceTex;			// 脸的贴图

	// 脸型高级设定调整所需的参数
	unsigned char			offsetForeheadH;	// 额头的横移
	unsigned char			offsetForeheadV;	// 额头的纵移
	unsigned char			offsetForeheadZ;	// 额头的前后移动
	unsigned char			rotateForehead;		// 额头的旋转
	unsigned char			scaleForehead;		// 额头的比例缩放(以自己为中心缩放)
	unsigned char			offsetYokeBoneH;	// 颧骨的横移
	unsigned char			offsetYokeBoneV;	// 颧骨的纵移
	unsigned char			offsetYokeBoneZ;	// 颧骨的前后移动
	unsigned char			rotateYokeBone;		// 颧骨的旋转(z轴)
	unsigned char			scaleYokeBone;		// 颧骨的比例缩放
	unsigned char			offsetCheekH;		// 脸颊的横移
	unsigned char			offsetCheekV;		// 脸颊的纵移
	unsigned char			offsetCheekZ;		// 脸颊的前后移动
	unsigned char			scaleCheek;			// 脸颊的比例缩放(以各自为中心)
	unsigned char			offsetChainV;		// 下巴的纵移
	unsigned char			offsetChainZ;		// 下巴的前后移动
	unsigned char			rotateChain;		// 下巴的旋转(以x轴旋转)
	unsigned char			scaleChainH;		// 下巴的左右比例缩放(以脸盘120为中)
	unsigned char			offsetJawH;			// 颌骨的横移
	unsigned char			offsetJawV;			// 颌骨的纵移
	unsigned char			offsetJawZ;			// 颌骨的前后移动
	unsigned char			scaleJawSpecial;	// 颌骨的特殊缩放系数
	unsigned char			scaleJawH;			// 颌骨的左右比例缩放
	unsigned char			scaleJawV;			// 颌骨的上下比例缩放

	
	// 眼型调整所需要的参数
	unsigned short			idEyeShape;			// 眼型id
	unsigned short			idEyeBaseTex;		// 眼皮纹理id
	unsigned short			idEyeHighTex;		// 眼影纹理id
	unsigned short			idEyeBallTex;		// 眼球纹理id
	unsigned char			scaleEyeH;			// 左眼部水平比例调节(位置缩放,几何中心)
	unsigned char			scaleEyeV;			// 左眼部竖直比例调节
	unsigned char			rotateEye;			// 左眼部旋转调节, 只绕z轴旋转
	unsigned char			offsetEyeH;			// 左眼部水平位移, 相对头所在的世界中心
	unsigned char			offsetEyeV;			// 左眼部垂直位移, 相对头所在的世界中心
	unsigned char			offsetEyeZ;			// 左眼部垂直位移, 相对头所在的世界中心
	unsigned char			scaleEyeBall;		// 左眼球缩放
	unsigned char			scaleEyeH2;			// 右眼部水平比例调节(位置缩放,几何中心)
	unsigned char			scaleEyeV2;			// 右眼部竖直比例调节
	unsigned char			rotateEye2;			// 右眼部旋转调节, 只绕z轴旋转
	unsigned char			offsetEyeH2;		// 右眼部水平位移, 相对头所在的世界中心
	unsigned char			offsetEyeV2;		// 右眼部垂直位移, 相对头所在的世界中心
	unsigned char			offsetEyeZ2;		// 右眼部垂直位移, 相对头所在的世界中心
	unsigned char			scaleEyeBall2;		// 右眼球缩放
	

	// 眉型调整所需要的参数
	unsigned short			idBrowTex;			// 眉的贴图
	unsigned short			idBrowShape;		// 眉型id
	unsigned char			scaleBrowH;			// 左眉部水平比例调节(位置缩放,几何中心)
	unsigned char			scaleBrowV;			// 左眉部上下比例调节(各自中心,变大小)
	unsigned char			rotateBrow;			// 左眉部旋转调节, 只绕z轴旋转
	unsigned char			offsetBrowH;		// 左眉部水平位移, 相对头所在的世界中心
	unsigned char			offsetBrowV;		// 左眉部垂直位移, 相对头所在的世界中心
	unsigned char			offsetBrowZ;		// 左眉部前后位移, 相对头所在的世界中心
	unsigned char			scaleBrowH2;		// 右眉部水平比例调节(位置缩放,几何中心)
	unsigned char			scaleBrowV2;		// 右眉部上下比例调节(各自中心,变大小)
	unsigned char			rotateBrow2;		// 右眉部旋转调节, 只绕z轴旋转
	unsigned char			offsetBrowH2;		// 右眉部水平位移, 相对头所在的世界中心
	unsigned char			offsetBrowV2;		// 右眉部垂直位移, 相对头所在的世界中心
	unsigned char			offsetBrowZ2;		// 右眉部前后位移, 相对头所在的世界中心
	

	//鼻型调整所需要的参数
	unsigned short			idNoseTex;			// 鼻的贴图
	unsigned short			idNoseTipShape;		// 鼻头形状id
	unsigned char			scaleNoseTipH;		// 鼻头左右比例调节(中心为53)
	unsigned char			scaleNoseTipV;		// 鼻头上下比例调节(以各自为中心)
	unsigned char			offsetNoseTipV;		// 鼻头垂直位移, 相对头所在的世界中心
	unsigned char			scaleNoseTipZ;		// 鼻头垂直位置缩放,(中心是070)
	unsigned short			idNoseBridgeShape;	// 鼻梁形状id
	unsigned char			scaleBridgeTipH;	// 鼻梁左右比例调节
	unsigned char			offsetBridgeTipZ;	// 鼻梁前后位移调节
	
	// 嘴型调整所需要的参数
	unsigned short			idMouthUpLipLine;	// 上唇线id
	unsigned short			idMouthMidLipLine;	// 唇沟id
	unsigned short			idMouthDownLipLine;	// 下唇线id
	unsigned char			thickUpLip;			// 上嘴唇厚度(比例)(位置缩放,中心是051)
	unsigned char			thickDownLip;		// 下嘴唇厚度(比例)(位置缩放,中心是051)
	unsigned char			offsetMouthV;		// 嘴部整体垂直位移, 相对头所在的世界中心
	unsigned char			offsetMouthZ;		// 嘴部整体沿Z轴的位移, 相对头所在的世界中心
	unsigned short			idMouthTex;			// 嘴的贴图	
	unsigned char			scaleMouthH;		// 左嘴部水平比例调节
	unsigned char			scaleMouthH2;		// 右嘴部水平比例调节
	unsigned char			offsetCornerOfMouthSpecial;		//左嘴角的特殊位移
	unsigned char			offsetCornerOfMouthSpecial2;	//右嘴角的特殊位移

	// 耳型调整所需要的参数
	unsigned short			idEarShape;			// 耳型id
	unsigned char			scaleEar;			// 耳朵的整体大小(位置家大小缩放,中心061)
	unsigned char			offsetEarV;			// 耳朵的上下移动

	// 发型调整所需要的参数
	unsigned short			idHairModel;		// 头发模型id
	unsigned short			idHairTex;			// 头发贴图id

	// 胡子调整所需要的参数 
	unsigned short			idMoustacheTex;		// 小胡子贴图id
	unsigned short			idMoustacheSkin;	// 大胡子模型id
	unsigned short			idGoateeTex;		// 大胡子贴图id
	
	// 法令调整所需要的参数
	unsigned short			idFalingSkin;		// 法令模型id
	unsigned short			idFalingTex;		// 法令的贴图id

	A3DCOLOR				colorFace;			// 脸的颜色
	A3DCOLOR				colorEye;			// 眼影的颜色
	A3DCOLOR				colorBrow;			// 眉毛的颜色
	A3DCOLOR				colorMouth;			// 嘴唇的颜色
	A3DCOLOR				colorHair;			// 头发的颜色
	A3DCOLOR				colorEyeBall;		// 瞳孔的颜色
	A3DCOLOR				colorMoustache;		// 胡子的颜色
};

struct FACE_CUSTOMIZEDATA : public FACE_CUSTOMIZEDATA_1
{
	unsigned short idThirdEye;					//	附眼模板id

	static FACE_CUSTOMIZEDATA From(const FACE_CUSTOMIZEDATA_1 &rhs)
	{
		FACE_CUSTOMIZEDATA result;
		*(FACE_CUSTOMIZEDATA_1 *)(&result) = rhs;
		result.idThirdEye = 0;
		return result;
	}
};


#define CUSTOMIZE_DATA_VERSION_1		0x10007000
struct PLAYER_CUSTOMIZEDATA_1
{
	DWORD							dwVersion;		// version
	
	FACE_CUSTOMIZEDATA_1			faceData;		// data defined face
	
	unsigned short					bodyID;			// 妖兽或妖精使用的形象ID
	A3DCOLOR						colorBody;		// 身体的颜色
	
	unsigned char					headScale;		// 头大小
	unsigned char					upScale;		// 上身健壮度
	unsigned char					waistScale;		// 腰部围度
	unsigned char					armWidth;		// 上肢围度
	unsigned char					legWidth;		// 下肢围度
	unsigned char					breastScale;	// 胸部高低（女性才有）
};

#define CUSTOMIZE_DATA_VERSION			0x10007001
struct PLAYER_CUSTOMIZEDATA
{
	DWORD							dwVersion;		// version
	
	FACE_CUSTOMIZEDATA				faceData;		// data defined face
	
	unsigned short					bodyID;			// 妖兽或妖精使用的形象ID
	A3DCOLOR						colorBody;		// 身体的颜色
	
	unsigned char					headScale;		// 头大小
	unsigned char					upScale;		// 上身健壮度
	unsigned char					waistScale;		// 腰部围度
	unsigned char					armWidth;		// 上肢围度
	unsigned char					legWidth;		// 下肢围度
	unsigned char					breastScale;	// 胸部高低（女性才有）
	
	static PLAYER_CUSTOMIZEDATA From(const PLAYER_CUSTOMIZEDATA_1 &rhs)
	{
		PLAYER_CUSTOMIZEDATA result;
		
		result.dwVersion	= rhs.dwVersion;
		
		result.faceData		= FACE_CUSTOMIZEDATA::From(rhs.faceData);
		
		result.bodyID		= rhs.bodyID;
		result.colorBody	= rhs.colorBody;
		
		result.headScale	= rhs.headScale;
		result.upScale		= rhs.upScale;
		result.waistScale	= rhs.waistScale;
		result.armWidth		= rhs.armWidth;
		result.legWidth		= rhs.legWidth;
		result.breastScale	= rhs.breastScale;
		
		return result;
	}
	
	static PLAYER_CUSTOMIZEDATA From(const void *rhs, size_t size)
	{
		if (rhs)
		{
			if (size == sizeof(PLAYER_CUSTOMIZEDATA_1))
				return From(*(PLAYER_CUSTOMIZEDATA_1*)rhs);
			if (size == sizeof(PLAYER_CUSTOMIZEDATA))
				return *(PLAYER_CUSTOMIZEDATA *)rhs;
		}
		PLAYER_CUSTOMIZEDATA result;
		::memset(&result, 0, sizeof(result));
		return result;
	}
};

///////////////////////////////////////////////////////////////////////////
//  
//  Global functions
//  
///////////////////////////////////////////////////////////////////////////

bool LoadDefaultCustomizeData(int iProfession, int iGender, PLAYER_CUSTOMIZEDATA& data);


///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////
