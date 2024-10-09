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
// ��ͼ���÷������õĲ�λ
//
///////////////////////////////////////////////////////////////////////////
enum FACE_TEX_PART_ID
{
	FACE_TEX_PART_FACE = 0,			// ��
	FACE_TEX_PART_EYE = 1,			// ��Ƥ
	FACE_TEX_PART_EYESHADOW = 2,	// ��Ӱ
	FACE_TEX_PART_BROW = 3,			// ü
	FACE_TEX_PART_MOUTH = 4,		// ��
	FACE_TEX_PART_NOSE = 5,			// ��
	FACE_TEX_PART_EYEBALL = 6,		// ����
	FACE_TEX_PART_MOUSTACHE = 7,	// С����
	FACE_TEX_PART_HAIR = 8,			// ͷ��
	FACE_TEX_PART_GOATEE = 9,		// �����
	FACE_TEX_PART_FALING = 10,		// ����
};

///////////////////////////////////////////////////////////////////////////
// 
// eFace���÷������õĲ�λ
//
///////////////////////////////////////////////////////////////////////////
enum FACE_SHAPE_PART_ID
{
	FACE_SHAPE_PART_FACE = 0,				// ����
	FACE_SHAPE_PART_EYE = 1,				// ����
	FACE_SHAPE_PART_BROW = 2,				// ü��
	FACE_SHAPE_PART_NOSETIP = 3,			// ��ͷ
	FACE_SHAPE_PART_NOSEBRIDGE = 4,			// ����
	FACE_SHAPE_PART_UPLIPLINE = 5,			// �ϴ���
	FACE_SHAPE_PART_MIDLIPLINE = 6,			// ����
	FACE_SHAPE_PART_DOWNLIPLINE = 7,		// �´���
	FACE_SHAPE_PART_EAR = 8,				// ����
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

	// �������Ի����ݶ���82
	struct FACE_CUSTOMIZEDATA_1
	{
		// ͷ�������������
		unsigned char			scaleUp;			// ��ͥ����
		unsigned char			scaleMiddle;		// ��ͥ����
		unsigned char			scaleDown;			// ��ͥ����

		// ���͵�������Ĳ���
		unsigned short			idFaceShape1;		// ����1
		unsigned short			idFaceShape2;		// ����2
		unsigned char			blendFaceShape;		// �����ں�ϵ��, 0.0~1.0, Ϊ0.0ʱ��������1,Ϊ1.0ʱ��������2
		unsigned char			scaleFaceH;			// �������ұ�������(����Ϊ��01)
		unsigned char			scaleFaceV;			// �������±�������(����Ϊ��01)
		unsigned short			idFaceTex;			// ������ͼ

		// ���͸߼��趨��������Ĳ���
		unsigned char			offsetForeheadH;	// ��ͷ�ĺ���
		unsigned char			offsetForeheadV;	// ��ͷ������
		unsigned char			offsetForeheadZ;	// ��ͷ��ǰ���ƶ�
		unsigned char			rotateForehead;		// ��ͷ����ת
		unsigned char			scaleForehead;		// ��ͷ�ı�������(���Լ�Ϊ��������)
		unsigned char			offsetYokeBoneH;	// ȧ�ǵĺ���
		unsigned char			offsetYokeBoneV;	// ȧ�ǵ�����
		unsigned char			offsetYokeBoneZ;	// ȧ�ǵ�ǰ���ƶ�
		unsigned char			rotateYokeBone;		// ȧ�ǵ���ת(z��)
		unsigned char			scaleYokeBone;		// ȧ�ǵı�������
		unsigned char			offsetCheekH;		// ���յĺ���
		unsigned char			offsetCheekV;		// ���յ�����
		unsigned char			offsetCheekZ;		// ���յ�ǰ���ƶ�
		unsigned char			scaleCheek;			// ���յı�������(�Ը���Ϊ����)
		unsigned char			offsetChainV;		// �°͵�����
		unsigned char			offsetChainZ;		// �°͵�ǰ���ƶ�
		unsigned char			rotateChain;		// �°͵���ת(��x����ת)
		unsigned char			scaleChainH;		// �°͵����ұ�������(������120Ϊ��)
		unsigned char			offsetJawH;			// �ǵĺ���
		unsigned char			offsetJawV;			// �ǵ�����
		unsigned char			offsetJawZ;			// �ǵ�ǰ���ƶ�
		unsigned char			scaleJawSpecial;	// �ǵ���������ϵ��
		unsigned char			scaleJawH;			// �ǵ����ұ�������
		unsigned char			scaleJawV;			// �ǵ����±�������

		
		// ���͵�������Ҫ�Ĳ���
		unsigned short			idEyeShape;			// ����id
		unsigned short			idEyeBaseTex;		// ��Ƥ����id
		unsigned short			idEyeHighTex;		// ��Ӱ����id
		unsigned short			idEyeBallTex;		// ��������id
		unsigned char			scaleEyeH;			// ���۲�ˮƽ��������(λ������,��������)
		unsigned char			scaleEyeV;			// ���۲���ֱ��������
		unsigned char			rotateEye;			// ���۲���ת����, ֻ��z����ת
		unsigned char			offsetEyeH;			// ���۲�ˮƽλ��, ���ͷ���ڵ���������
		unsigned char			offsetEyeV;			// ���۲���ֱλ��, ���ͷ���ڵ���������
		unsigned char			offsetEyeZ;			// ���۲���ֱλ��, ���ͷ���ڵ���������
		unsigned char			scaleEyeBall;		// ����������
		unsigned char			scaleEyeH2;			// ���۲�ˮƽ��������(λ������,��������)
		unsigned char			scaleEyeV2;			// ���۲���ֱ��������
		unsigned char			rotateEye2;			// ���۲���ת����, ֻ��z����ת
		unsigned char			offsetEyeH2;		// ���۲�ˮƽλ��, ���ͷ���ڵ���������
		unsigned char			offsetEyeV2;		// ���۲���ֱλ��, ���ͷ���ڵ���������
		unsigned char			offsetEyeZ2;		// ���۲���ֱλ��, ���ͷ���ڵ���������
		unsigned char			scaleEyeBall2;		// ����������
		

		// ü�͵�������Ҫ�Ĳ���
		unsigned short			idBrowTex;			// ü����ͼ
		unsigned short			idBrowShape;		// ü��id
		unsigned char			scaleBrowH;			// ��ü��ˮƽ��������(λ������,��������)
		unsigned char			scaleBrowV;			// ��ü�����±�������(��������,���С)
		unsigned char			rotateBrow;			// ��ü����ת����, ֻ��z����ת
		unsigned char			offsetBrowH;		// ��ü��ˮƽλ��, ���ͷ���ڵ���������
		unsigned char			offsetBrowV;		// ��ü����ֱλ��, ���ͷ���ڵ���������
		unsigned char			offsetBrowZ;		// ��ü��ǰ��λ��, ���ͷ���ڵ���������
		unsigned char			scaleBrowH2;		// ��ü��ˮƽ��������(λ������,��������)
		unsigned char			scaleBrowV2;		// ��ü�����±�������(��������,���С)
		unsigned char			rotateBrow2;		// ��ü����ת����, ֻ��z����ת
		unsigned char			offsetBrowH2;		// ��ü��ˮƽλ��, ���ͷ���ڵ���������
		unsigned char			offsetBrowV2;		// ��ü����ֱλ��, ���ͷ���ڵ���������
		unsigned char			offsetBrowZ2;		// ��ü��ǰ��λ��, ���ͷ���ڵ���������
		

		//���͵�������Ҫ�Ĳ���
		unsigned short			idNoseTex;			// �ǵ���ͼ
		unsigned short			idNoseTipShape;		// ��ͷ��״id
		unsigned char			scaleNoseTipH;		// ��ͷ���ұ�������(����Ϊ53)
		unsigned char			scaleNoseTipV;		// ��ͷ���±�������(�Ը���Ϊ����)
		unsigned char			offsetNoseTipV;		// ��ͷ��ֱλ��, ���ͷ���ڵ���������
		unsigned char			scaleNoseTipZ;		// ��ͷ��ֱλ������,(������070)
		unsigned short			idNoseBridgeShape;	// ������״id
		unsigned char			scaleBridgeTipH;	// �������ұ�������
		unsigned char			offsetBridgeTipZ;	// ����ǰ��λ�Ƶ���
		
		// ���͵�������Ҫ�Ĳ���
		unsigned short			idMouthUpLipLine;	// �ϴ���id
		unsigned short			idMouthMidLipLine;	// ����id
		unsigned short			idMouthDownLipLine;	// �´���id
		unsigned char			thickUpLip;			// ���촽���(����)(λ������,������051)
		unsigned char			thickDownLip;		// ���촽���(����)(λ������,������051)
		unsigned char			offsetMouthV;		// �첿���崹ֱλ��, ���ͷ���ڵ���������
		unsigned char			offsetMouthZ;		// �첿������Z���λ��, ���ͷ���ڵ���������
		unsigned short			idMouthTex;			// �����ͼ	
		unsigned char			scaleMouthH;		// ���첿ˮƽ��������
		unsigned char			scaleMouthH2;		// ���첿ˮƽ��������
		unsigned char			offsetCornerOfMouthSpecial;		//����ǵ�����λ��
		unsigned char			offsetCornerOfMouthSpecial2;	//����ǵ�����λ��

		// ���͵�������Ҫ�Ĳ���
		unsigned short			idEarShape;			// ����id
		unsigned char			scaleEar;			// ����������С(λ�üҴ�С����,����061)
		unsigned char			offsetEarV;			// ����������ƶ�

		// ���͵�������Ҫ�Ĳ���
		unsigned short			idHairModel;		// ͷ��ģ��id
		unsigned short			idHairTex;			// ͷ����ͼid

		// ���ӵ�������Ҫ�Ĳ��� 
		unsigned short			idMoustacheTex;		// С������ͼid
		unsigned short			idMoustacheSkin;	// �����ģ��id
		unsigned short			idGoateeTex;		// �������ͼid
		
		// �����������Ҫ�Ĳ���
		unsigned short			idFalingSkin;		// ����ģ��id
		unsigned short			idFalingTex;		// �������ͼid

		A3DCOLOR				colorFace;			// ������ɫ
		A3DCOLOR				colorEye;			// ��Ӱ����ɫ
		A3DCOLOR				colorBrow;			// üë����ɫ
		A3DCOLOR				colorMouth;			// �촽����ɫ
		A3DCOLOR				colorHair;			// ͷ������ɫ
		A3DCOLOR				colorEyeBall;		// ͫ�׵���ɫ
		A3DCOLOR				colorMoustache;		// ���ӵ���ɫ
	};
	
	struct FACE_CUSTOMIZEDATA : public FACE_CUSTOMIZEDATA_1
	{
		unsigned short idThirdEye;					//	����ģ��id

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

	//	�޸ĵ���ֻ�۹�Ч�ļ�
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
	
	//---------�����͵������---------new----------------
protected:
	// ��ʼ�����͹���
	bool CreateFaceController();
	void ReleaseFaceController();
	void InitEmptyFaceData();

	void InitFaceCusFactor();

	//�ѱ�����int ת����float
	float TransformScaleFromIntToFloat(int nScale, float fScaleFactor,float fMax);
	
	//��λ�ƴ�int ת����float
	float TransformOffsetFromIntToFloat(int nOffset, float fOffsetFactor, float fMax);

	//����ת��int ת����float
	float TransformRotateFromIntToFloat(int nRotate, float fRotateFactor, float fMax);



public:

	// ��ȡ����������
	CFaceBoneController* GetBoneController(void)	{ return m_pBoneCtrler; }
	A3DSkinModel* GetSkinModel(void)				{ return m_pSkinModel; }

	// ����FaceData
	bool SetFaceData(const FACE_CUSTOMIZEDATA& faceData);

	// ����ǰ�������ݱ����������Ա������ָ�
	bool StoreFaceData();

	// �ָ����ݵ���������
	bool RestoreFaceData();

	// ��ȡ��������
	FACE_CUSTOMIZEDATA* GetFaceData(void)	{ return &m_FaceData; }

	// ����FaceData����ȫ�������͸���(��״, ������ɫ, ģ��,��ɫ)
	bool UpdateAll(void);

	//���������Զ������
	bool LoadFaceCusFactorFromIni(void);

	//------------��״����13--------begin------------

	// ����ȫ��������״
	bool CalculateAllFaceShape(void);

	// ��������
	bool CalculateEye(void);

	// ����ü��
	bool CalculateBrow(void);

	// �������
	bool CalculateNose(void);

	// ��������
	bool CalculateMouth(void);

	// �������
	bool CalculateEar(void);

	// ���������ں�
	bool CalculateBlendFace(void);

	// ��������
	bool CalculateFace(void);

	// �����ͷ
	bool CalculateForehead(void);

	// ����ȧ��
	bool CalculateYokeBone(void);

	// ��������
	bool CalculateCheek(void);

	// �����°�
	bool CalculateChain(void);

	// ������
	bool CalculateJaw(void);

	// ����3ͥ
	bool Calculate3Parts(void);
	//-----------��״����-------end------------

	//-----------�����������9----begin---------

	// ����ȫ��������ص�����
	bool UpdateAllFaceTexture(void);

	// ������������
	bool UpdateFaceTexture(void);

	// �����۾�����(��Ƥ,��Ӱ)
	bool UpdateEyeTexture(void);

	// ����üë����
	bool UpdateBrowTexture(void);

	// �����촽����
	bool UpdateMouthTexture(void);

	// ���±�������
	bool UpdateNoseTexture(void);

	// ������������
	bool UpdateEyeBallTexture(void);
	
	// ����С��������
	bool UpdateMoustacheTexture(void);

	// ����ͷ������
	bool UpdateHairTexture(void);

	//���´��������
	bool UpdateGoateeTexture(void);

	//���·�������
	bool UpdateFalingTexture(void);

	//-----------�����������----end-----------
	
	//-----------ģ�͵�������2----begin---------

	// ����ͷ���ͺ���ģ��
	bool UpdateHairAndGoateeModel(void);

	// ����ͷ��
	bool UpdateHairModel(void);
	
	// ���º���ģ��
	bool UpdateGoateeModel(void);

	// ���·���ģ��
	bool UpdateFalingModel(void);
	

	//-----------ģ�͵�������----end-------------

	//-----------��ɫ��������7----begin-----------

	// ��������������ɫ
	bool UpdateAllFaceColor(void);

	// ��������ɫ
	bool UpdateFaceColor(void);

	// ������Ӱ��ɫ
	bool UpdateEyeColor(void);

	// ����üë��ɫ
	bool UpdateBrowColor(void);

	// �����촽��ɫ
	bool UpdateMouthColor(void);

	// ����ͷ����ɫ
	bool UpdateHairColor(void);

	// ����������ɫ
	bool UpdateEyeBallColor(void);

	// ���º�����ɫ
	bool UpdateGoateeColor(void);

	//-----------��ɫ��������----end-------------

	bool UpdateThirdEye();

protected:
	FACE_CUSTOMIZEDATA			m_FaceData;				// ��������
	FACE_CUSTOMIZEDATA			m_FaceDataStored;		// ������������������
	FACE_CUSTOMIZE_FACTOR		m_FaceCusFactor;		// ���Ի�����
	

	CFaceBoneController*		m_pBoneCtrler;			// face bone controller

	int							m_nCharacter;			// ����ְҵ
	int							m_nGender;				// �����Ա�
	int							m_nFaceID;				// ʹ�õ�����ID��������ʹ��

	//----------�����͵������---------new----------------
	
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
	int		m_iThirdEyeHH;	//	���� hook

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



