/*
 * FILE: EC_Face.h
 *
 * DESCRIPTION: Face object for Element Client
 *
 * CREATED BY: Duyuxin, 2004/12/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#pragma once

#include "A3DTypes.h"
#include "FaceAnimation.h"
#include "Expression.h"
#include "EC_Model.h"
#include "EC_RoleTypes.h"
#include <A3DSkinModel.h>

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class A3DSkinModel;
class A3DShader;
class A3DSkin;
class A3DTexture;
class CECViewport;
class A3DVertexShader;

class CFaceBoneController;
class CFaceBone;
class CExpression;

class elementdataman;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class CECFace
//	
///////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////
// 
// 贴图配置方案适用的部位
//
///////////////////////////////////////////////////////////////////////////
enum FACE_TEX_PART_ID
{
	FACE_TEX_PART_FACE = 0,			// 脸
	FACE_TEX_PART_EYE = 1,			// 眼皮
	FACE_TEX_PART_EYESHADOW = 2,	// 眼影
	FACE_TEX_PART_BROW = 3,			// 眉
	FACE_TEX_PART_MOUTH = 4,		// 唇
	FACE_TEX_PART_NOSE = 5,			// 鼻
	FACE_TEX_PART_EYEBALL = 6,		// 眼珠
	FACE_TEX_PART_MOUSTACHE = 7,	// 小胡子
	FACE_TEX_PART_HAIR = 8,			// 头发
	FACE_TEX_PART_GOATEE = 9,		// 大胡子
	FACE_TEX_PART_FALING = 10,		// 法令
};

///////////////////////////////////////////////////////////////////////////
// 
// eFace配置方案适用的部位
//
///////////////////////////////////////////////////////////////////////////
enum FACE_SHAPE_PART_ID
{
	FACE_SHAPE_PART_FACE = 0,				// 脸型
	FACE_SHAPE_PART_EYE = 1,				// 眼型
	FACE_SHAPE_PART_BROW = 2,				// 眉型
	FACE_SHAPE_PART_NOSETIP = 3,			// 鼻头
	FACE_SHAPE_PART_NOSEBRIDGE = 4,			// 鼻梁
	FACE_SHAPE_PART_UPLIPLINE = 5,			// 上唇线
	FACE_SHAPE_PART_MIDLIPLINE = 6,			// 唇沟
	FACE_SHAPE_PART_DOWNLIPLINE = 7,		// 下唇线
	FACE_SHAPE_PART_EAR = 8,				// 耳型
};

enum FACE_ANIMATION
{
	FACE_ANIMATION_NULL		= -1,
	FACE_ANIMATION_BLINK	= 0,
	FACE_ANIMATION_LAUGH,
	FACE_ANIMATION_ANGRY,
	FACE_ANIMATION_SMILE,
	FACE_ANIMATION_SAD,
	FACE_ANIMATION_MAX
};

class CECSkinModelWrapper : public A3DSkinModel
{
public:

	//	Bind skeleton file
	bool BindSkeletonFile(const char* szFile);
};

class CECModelWrapper : public CECModel
{
public:

	//	Bind skin model
	void BindSkinModel(A3DSkinModel* pSkinModel);
};

class CECFace
{
private:
	struct ANIMATION_FACTOR
	{
		int nAppearTime;
		int nKeepTime;
		int nDisappearTime;
		int nRestTime;
		int nNumRepeat;
	};

public:		//	Types

	enum
	{
		PART_FACE = 0,
		PART_EYE,
		PART_BROW,
		PART_MOUTH,
		PART_NOSE,
		PART_EYEBALL,
		PART_MOUSTACHE,
		NUM_PART,
	};

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

	struct FACE_CUSTOMIZE_FACTOR
	{
		int			nVision;

		float		fScaleFaceHFactor;
		float		fScaleFaceVFactor;

		float		fScaleUpPartFactor;
		float		fScaleMiddlePartFactor;
		float		fScaleDownPartFactor;

		float		fOffsetForeheadHFactor;
		float		fOffsetForeheadVFactor;
		float		fOffsetForeheadZFactor;
		float		fRotateForeheadFactor;
		float		fScaleForeheadFactor;

		float		fOffsetYokeBoneHFactor;
		float		fOffsetYokeBoneVFactor;
		float		fOffsetYokeBoneZFactor;
		float		fRotateYokeBoneFactor;
		float		fScaleYokeBoneFactor;

		float		fOffsetCheekHFactor;
		float		fOffsetCheekVFactor;
		float		fOffsetCheekZFactor;
		float		fScaleCheekFactor;
		
		float		fOffsetChainVFactor;
		float		fOffsetChainZFactor;
		float		fRotateChainFactor;
		float		fScaleChainHFactor;

		float		fOffsetJawHFactor;
		float		fOffsetJawVFactor;
		float		fOffsetJawZFactor;
		float		fScaleJawSpecialFactor;
		float		fScaleJawHFactor;
		float		fScaleJawVFactor;

		float		fScaleEyeHFactor;
		float		fScaleEyeVFactor;
		float		fRotateEyeFactor;
		float		fOffsetEyeHFactor;
		float		fOffsetEyeVFactor;
		float		fOffsetEyeZFactor;
		float		fScaleEyeBallFactor;
		float		fScaleEyeHFactor2;
		float		fScaleEyeVFactor2;
		float		fRotateEyeFactor2;
		float		fOffsetEyeHFactor2;
		float		fOffsetEyeVFactor2;
		float		fOffsetEyeZFactor2;
		float		fScaleEyeBallFactor2;

		float		fScaleBrowHFactor;
		float		fScaleBrowVFactor;
		float		fRotateBrowFactor;
		float		fOffsetBrowHFactor;
		float		fOffsetBrowVFactor;
		float		fOffsetBrowZFactor;
		
		float		fScaleBrowHFactor2;
		float		fScaleBrowVFactor2;
		float		fRotateBrowFactor2;
		float		fOffsetBrowHFactor2;
		float		fOffsetBrowVFactor2;
		float		fOffsetBrowZFactor2;

		float		fScaleNoseTipHFactor;
		float		fScaleNoseTipVFactor;
		float		fOffsetNoseTipVFactor;
		float		fScaleNoseTipZFactor;
		float		fScaleBridgeTipHFactor;
		float		fOffsetBridgeTipZFactor;

		float		fThickUpLipFactor;
		float		fThickDownLipFactor;
		float		fScaleMouthHFactor;
		float		fOffsetMouthVFactor;
		float		fOffsetMouthZFactor;
		float		fOffsetCornerOfMouthSpecialFactor;
		float		fScaleMouthHFactor2;
		float		fOffsetCornerOfMouthSpecialFactor2;
		
		float		fScaleEarFactor;
		float		fOffsetEarVFactor;

	};
#define SCALE_FACEH_FACTOR			0.3f
#define SCALE_FACEV_FACTOR			0.3f

#define SCALE_UPPART_FACTOR			0.4f
#define SCALE_MIDDLEPART_FACTOR		0.4f
#define SCALE_DOWNPART_FACTOR		0.4f

#define OFFSET_FOREHEADH_FACTOR		0.02f
#define OFFSET_FOREHEADV_FACTOR		0.02f
#define OFFSET_FOREHEADZ_FACTOR		0.02f
#define ROTATE_FOREHEAD_FACTOR		1.0f
#define SCALE_FOREHEAD_FACTOR		0.1f

#define OFFSET_YOKEBONEH_FACTOR		0.01f
#define OFFSET_YOKEBONEV_FACTOR		0.01f
#define OFFSET_YOKEBONEZ_FACTOR		0.01f
#define ROTATE_YOKEBONE_FACTOR		1.0f
#define SCALE_YOKEBONE_FACTOR		0.1f

#define OFFSET_CHEEKH_FACTOR		0.015f
#define OFFSET_CHEEKV_FACTOR		0.02f
#define OFFSET_CHEEKZ_FACTOR		0.02f
#define SCALE_CHEEK_FACTOR			0.5f

#define OFFSET_CHAINV_FACTOR		0.02f
#define OFFSET_CHAINZ_FACTOR		0.01f
#define ROTATE_CHAIN_FACTOR			1.0f
#define SCALE_CHAINH_FACTOR			0.99f

#define OFFSET_JAWH_FACTOR			0.01f
#define OFFSET_JAWV_FACTOR			0.01f
#define OFFSET_JAWZ_FACTOR			0.01f
#define SCALE_JAWSPECIAL_FACTOR		0.01f
#define SCALE_JAWH_FACTOR			0.5f
#define SCALE_JAWV_FACTOR			0.5f

#define SCALE_EYEH_FACTOR			0.7f
#define SCALE_EYEV_FACTOR			1.0f
#define ROTATE_EYE_FACTOR			1.0f
#define OFFSET_EYEH_FACTOR			0.01f
#define OFFSET_EYEV_FACTOR			0.01f
#define OFFSET_EYEZ_FACTOR			0.01f
#define SCALE_EYEBALL_FACTOR		0.8f
#define SCALE_EYEH_FACTOR2			0.7f
#define SCALE_EYEV_FACTOR2			1.0f
#define ROTATE_EYE_FACTOR2			1.0f
#define OFFSET_EYEH_FACTOR2			0.01f
#define OFFSET_EYEV_FACTOR2			0.01f
#define OFFSET_EYEZ_FACTOR2			0.01f
#define SCALE_EYEBALL_FACTOR2		0.8f

#define SCALE_BROWH_FACTOR			0.4f
#define SCALE_BROWV_FACTOR			1.0f
#define ROTATE_BROW_FACTOR			0.8f
#define OFFSET_BROWH_FACTOR			0.01f
#define OFFSET_BROWV_FACTOR			0.01f
#define OFFSET_BROWZ_FACTOR			0.01f

#define SCALE_BROWH_FACTOR2			0.4f
#define SCALE_BROWV_FACTOR2			1.0f
#define ROTATE_BROW_FACTOR2			0.8f
#define OFFSET_BROWH_FACTOR2		0.01f
#define OFFSET_BROWV_FACTOR2		0.01f
#define OFFSET_BROWZ_FACTOR2		0.01f

#define SCALE_NOSETIPH_FACTOR		0.8f
#define SCALE_NOSETIPV_FACTOR		0.6f
#define OFFSET_NOSETIPV_FACTOR		0.01f
#define SCALE_NOSETIPZ_FACTOR		1.0f
#define SCALE_BRIDGETIPH_FACTOR		0.8f
#define OFFSET_BRIDGETIPZ_FACTOR	0.01f

#define THICK_UPLIP_FACTOR			1.0f
#define THICK_DOWNLIP_FACTOR		1.0f
#define SCALE_MOUTHH_FACTOR			0.6f
#define OFFSET_MOUTHV_FACTOR		0.01f
#define OFFSET_MOUTHZ_FACTOR		0.02f
#define OFFSET_CORNEROFMOUTHSPECIAL_FACTOR 0.02f
#define SCALE_MOUTHH_FACTOR2			0.6f
#define OFFSET_CORNEROFMOUTHSPECIAL_FACTOR2 0.02f

#define SCALE_EAR_FACTOR			1.0f
#define OFFSET_EARV_FACTOR			0.03f
	

public:		//	Constructor and Destructor

	CECFace();
	virtual ~CECFace();

public:		//	Attributes

	static const char*	m_aMeshName[CECFace::NUM_PART];
	static const char*	m_szNeckHH;
	static const char*	m_szNeckCC;
	static const char*	m_szHeadBone;
	static const char*	m_szNeckBone;
	static const char*	m_sz3rdEyeHH;

public:		//	Operations

	//	Initialize object
	bool Init(int nCharacter, int nGender, int nFaceID);
	//	Release object
	void Release();

	//	Tick routinue
	bool Tick(DWORD dwDeltaTime);
	//	Render routine
	bool Render(CECViewport* pViewport, bool bPortrait, bool bHighLight, const A3DSkinModel::LIGHTINFO *pPortraitLight=NULL);

	//	Set parent model
	void SetParent(A3DSkinModel* pParent);
	//	Change hair model
	bool ChangeHairModel(const char* szModelFile, const char * szSkinFile);

	//	Change moustache model	
	bool ChangeGoateeModel(const char * szSkinFile);

	//	Change faling model
	bool ChangeFalingModel(const char * szSkinFile);

	//	change textures
	bool ChangeFaceTexture(const char * szTexMap);
	bool ChangeEyeTexture(const char * szBaseTex, const char * szHighTex);
	bool ChangeBrowTexture(const char * szBaseTex, const char * szHighTex);
	bool ChangeMouthTexture(const char * szBaseTex, const char * szHighTex);
	bool ChangeNoseTexture(const char * szBaseTex);
	bool ChangeEyeBallTexture(const char * szBaseTex, const char * szHighTex);
	bool ChangeMoustacheTexture(const char * szBaseTex, const char * szHighTex);
	bool ChangeTattooTexture(const char * szTattooTex);
	bool ChangeHairTexture(const char * szBaseTex, const char * szHighTex);
	bool ChangeGoateeTexture(const char * szBaseTex, const char * szHighTex);
	bool ChangeFalingTexture(const char * szHighTex);

	//	修改第三只眼光效文件
	bool ChangeThirdEyeGfx(const char *szGfx);

protected:
	bool SetHairMaterial(bool bUseAlpha);
	bool SetGoateeMaterial(bool bUseAlpha);
	bool SetFalingMaterial(bool bUseAlpha);

public:
	//	tattoo matrix set and get
	void SetTattooProjMatrix(const A3DMATRIX4& mat)	{ m_tmTattoo = mat; }
	const A3DMATRIX4& GetTattooMatrix()				{ return m_tmTattoo; }

	void SetFaceColor(A3DCOLOR color)		{ m_colorFace = color; }
	A3DCOLOR GetFaceColor()					{ return m_colorFace; }

	void SetHairColor(A3DCOLOR color)		{ m_colorHair = color; }
	A3DCOLOR GetHairColor()					{ return m_colorHair; }

	void SetEyeColor(A3DCOLOR color)		{ m_colorEye = color; }
	A3DCOLOR GetEyeColor()					{ return m_colorEye; }

	void SetBrowColor(A3DCOLOR color)		{ m_colorBrow = color; }
	A3DCOLOR GetBrowColor()					{ return m_colorBrow; }

	void SetMouthColor(A3DCOLOR color)		{ m_colorMouth = color; }
	A3DCOLOR GetMouthColor() 				{ return m_colorMouth; }

	void SetEyeBallColor(A3DCOLOR color)	{ m_colorEyeBall = color; }
	A3DCOLOR GetEyeBallColor()				{ return m_colorEyeBall; }

	void SetMoustacheColor(A3DCOLOR color)	{ m_colorMoustache = color; }
	A3DCOLOR GetMoustacheColor()			{ return m_colorMoustache; }

	void AutoHairTrans(bool bTrue) { m_bHairTrans = bTrue; }

	//	Get skin model object
	A3DSkinModel* GetA3DSkinModel() { return m_pSkinModel; }
	CECModel* GetECModel() { return m_pECModel; }

	void SetTransparent(float fTrans);
	float GetTransparent()const;
	
	//---------与脸型调整相关---------new----------------
protected:
	// 初始化脸型骨骼
	bool CreateFaceController();
	void ReleaseFaceController();
	void InitEmptyFaceData();

	void InitFaceCusFactor();

	//把比例从int 转换到float
	float TransformScaleFromIntToFloat(int nScale, float fScaleFactor,float fMax);
	
	//把位移从int 转换到float
	float TransformOffsetFromIntToFloat(int nOffset, float fOffsetFactor, float fMax);

	//把旋转从int 转换到float
	float TransformRotateFromIntToFloat(int nRotate, float fRotateFactor, float fMax);



public:

	// 获取骨骼控制器
	CFaceBoneController* GetBoneController(void)	{ return m_pBoneCtrler; }
	A3DSkinModel* GetSkinModel(void)				{ return m_pSkinModel; }

	// 设置FaceData
	bool SetFaceData(const FACE_CUSTOMIZEDATA& faceData);

	// 将当前脸的数据备份起来，以备将来恢复
	bool StoreFaceData();

	// 恢复备份的脸型数据
	bool RestoreFaceData();

	// 获取脸型数据
	FACE_CUSTOMIZEDATA* GetFaceData(void)	{ return &m_FaceData; }

	// 根据FaceData进行全部的脸型更新(形状, 纹理颜色, 模型,颜色)
	bool UpdateAll(void);

	//载入脸型自定义参数
	bool LoadFaceCusFactorFromIni(void);

	//------------形状函数13--------begin------------

	// 计算全部脸部形状
	bool CalculateAllFaceShape(void);

	// 计算眼型
	bool CalculateEye(void);

	// 计算眉型
	bool CalculateBrow(void);

	// 计算鼻型
	bool CalculateNose(void);

	// 计算嘴型
	bool CalculateMouth(void);

	// 计算耳型
	bool CalculateEar(void);

	// 计算脸型融合
	bool CalculateBlendFace(void);

	// 计算脸盘
	bool CalculateFace(void);

	// 计算额头
	bool CalculateForehead(void);

	// 计算颧骨
	bool CalculateYokeBone(void);

	// 计算脸颊
	bool CalculateCheek(void);

	// 计算下巴
	bool CalculateChain(void);

	// 计算颌骨
	bool CalculateJaw(void);

	// 计算3庭
	bool Calculate3Parts(void);
	//-----------形状函数-------end------------

	//-----------纹理调整函数9----begin---------

	// 更新全部脸部相关的纹理
	bool UpdateAllFaceTexture(void);

	// 更新脸型纹理
	bool UpdateFaceTexture(void);

	// 更新眼睛纹理(眼皮,眼影)
	bool UpdateEyeTexture(void);

	// 更新眉毛纹理
	bool UpdateBrowTexture(void);

	// 更新嘴唇纹理
	bool UpdateMouthTexture(void);

	// 更新鼻子纹理
	bool UpdateNoseTexture(void);

	// 更新眼球纹理
	bool UpdateEyeBallTexture(void);
	
	// 更新小胡子纹理
	bool UpdateMoustacheTexture(void);

	// 更新头发纹理
	bool UpdateHairTexture(void);

	//更新大胡子纹理
	bool UpdateGoateeTexture(void);

	//更新法令文理
	bool UpdateFalingTexture(void);

	//-----------纹理调整函数----end-----------
	
	//-----------模型调整函数2----begin---------

	// 更新头发和胡子模型
	bool UpdateHairAndGoateeModel(void);

	// 更新头发
	bool UpdateHairModel(void);
	
	// 更新胡子模型
	bool UpdateGoateeModel(void);

	// 更新法令模型
	bool UpdateFalingModel(void);
	

	//-----------模型调整函数----end-------------

	//-----------颜色调整函数7----begin-----------

	// 更新所有脸部颜色
	bool UpdateAllFaceColor(void);

	// 更新脸颜色
	bool UpdateFaceColor(void);

	// 更新眼影颜色
	bool UpdateEyeColor(void);

	// 更新眉毛颜色
	bool UpdateBrowColor(void);

	// 更新嘴唇颜色
	bool UpdateMouthColor(void);

	// 更新头发颜色
	bool UpdateHairColor(void);

	// 更新眼球颜色
	bool UpdateEyeBallColor(void);

	// 更新胡子颜色
	bool UpdateGoateeColor(void);

	//-----------颜色调整函数----end-------------

	bool UpdateThirdEye();

protected:
	FACE_CUSTOMIZEDATA			m_FaceData;				// 脸型数据
	FACE_CUSTOMIZEDATA			m_FaceDataStored;		// 保存起来的脸型数据
	FACE_CUSTOMIZE_FACTOR		m_FaceCusFactor;		// 个性化参数
	

	CFaceBoneController*		m_pBoneCtrler;			// face bone controller

	int							m_nCharacter;			// 人物职业
	int							m_nGender;				// 人物性别
	int							m_nFaceID;				// 使用的脸的ID，供妖兽使用

	//----------与脸型调整相关---------new----------------
	
protected:	//	Attributes
	
	CECSkinModelWrapper*	m_pSkinModel;	//	A3D skin model object
	CECModelWrapper*		m_pECModel;		//	EC model wrapper

	A3DSkinModel*	m_pParent;		//	Parent model

	bool			m_bHasTicked;	//	flag must be true before we can render them
	
	A3DSkin *		m_pFaceSkin;	//	skin of face which will be customized by the user
	A3DSkin *		m_pHairSkin;	//	skin of hair which will be affected by the face bones
	A3DSkin *		m_pGoateeSkin;	//	skin of goatee mesh, which will be affected by the face bones
	A3DSkin *		m_pFalingSkin;	// 	skin of faling mesh, which will be affected by face bones
	int				m_nHairSkinIndex;	// index of hair skin in the skin model
	int				m_nGoateeSkinIndex;	// index of goatee skin in the skin model
	int				m_nFalingSkinIndex;	// index of faling skin in the skin model

	// for test usage
	A3DVECTOR3		m_vecPos;		//	relative pos in parent
	A3DVECTOR3		m_vecDir;		//	relative dir in parent
	A3DVECTOR3		m_vecUp;		//	relative up in parent

	// tattoo control
	A3DMATRIX4		m_tmTattoo;		//	tattoo projection matrix

	// colors of all parts
	A3DCOLOR		m_colorFace;		//	Face color, this will be the material color of all face parts
	A3DCOLOR		m_colorHair;		//	Hair color
	A3DCOLOR		m_colorEye;			//	eye shadow color
	A3DCOLOR		m_colorBrow;		//	brow color
	A3DCOLOR		m_colorMouth;		//	lip color
	A3DCOLOR		m_colorNose;		//	not used now
	A3DCOLOR		m_colorEyeBall;		//	color of eye apple
	A3DCOLOR		m_colorMoustache;	//	color of moustache

	A3DShader*		m_pFaceShader;		//	Face shader
	A3DShader*		m_pEyeShader;		//	Eye shader
	A3DShader*		m_pBrowShader;		//	Brow shader
	A3DShader*		m_pMouthShader;		//	Mouth shader
	A3DShader*		m_pNoseShader;		//	Nose shader
	A3DShader*		m_pEyeBallShader;	//	EyeBall shader
	A3DShader*		m_pMoustacheShader;	//	Moustache shader
	A3DShader*		m_pHairShader;		//	Hair shader
	A3DShader*		m_pGoateeShader;	//	Goatee shader
	
	A3DVertexShader*	m_aVS[NUM_PART+2];

	int		m_iNeckCC;		//	Index of neck CC hook
	int		m_iNeckHH;		//	Index of neck HH in parent model
	int		m_iHeadBone;	//	Index of head bone in parent model
	int		m_iNeckBone;	//	Index of neck bone in face model
	int		m_iThirdEyeHH;	//	附眼 hook

	A3DGFXEx * m_pThirdEyeGfx;

	bool	m_bHairTrans;	//	true, adjust hair transparent every tick

protected:
	static bool					m_bStaticDataInited;						// static data init flag
	static ANIMATION_FACTOR		m_AnimationFactors[FACE_ANIMATION_MAX];		// all animation definiton data
	static CExpression 			m_Expressions[NUM_PROFESSION][2][FACE_ANIMATION_MAX];	// all expression for all races

protected:
	CFaceAnimation 				m_Animations[FACE_ANIMATION_MAX];			// all animations

protected:
	static bool LoadStaticData();
	static bool LoadAnimationFactors();
	static bool LoadExpressions();

	bool LoadAnimations();
	bool UpdateAnimations(DWORD dwDeltaTime);

public:
	bool PlayAnimation(FACE_ANIMATION id);
	const A3DMATRIX4& GetFaceTM();
	const A3DMATRIX4& GetHeadBoneTM();

protected:	//	Operations

	//	In CECFace, we load each shader from shader man and them set them to 
	//	the skin meshes'. But we need not to release them because A3DSkin
	//	will release then automatically

	//	Init all part shaders
	bool InitPartShaders();
	//	Set part shader
	bool SetPartShader(int iPart, A3DShader * pShader);

	void ChangeHairTextureShader(A3DShader *pShader);
	bool ChangeHairRewuShaderTexture(const char * szTexMap, const char * szTexMap2);
	bool ChangeHairNormalShaderTexture(const char * szBaseTex, const char * szHighTex);
	
	void TickThirdEye(DWORD dwDeltaTime);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////



