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


#define CUSTOMIZE_DATA_VERSION_1		0x10007000
struct PLAYER_CUSTOMIZEDATA_1
{
	DWORD							dwVersion;		// version
	
	FACE_CUSTOMIZEDATA_1			faceData;		// data defined face
	
	unsigned short					bodyID;			// ���޻�����ʹ�õ�����ID
	A3DCOLOR						colorBody;		// �������ɫ
	
	unsigned char					headScale;		// ͷ��С
	unsigned char					upScale;		// ����׳��
	unsigned char					waistScale;		// ����Χ��
	unsigned char					armWidth;		// ��֫Χ��
	unsigned char					legWidth;		// ��֫Χ��
	unsigned char					breastScale;	// �ز��ߵͣ�Ů�Բ��У�
};

#define CUSTOMIZE_DATA_VERSION			0x10007001
struct PLAYER_CUSTOMIZEDATA
{
	DWORD							dwVersion;		// version
	
	FACE_CUSTOMIZEDATA				faceData;		// data defined face
	
	unsigned short					bodyID;			// ���޻�����ʹ�õ�����ID
	A3DCOLOR						colorBody;		// �������ɫ
	
	unsigned char					headScale;		// ͷ��С
	unsigned char					upScale;		// ����׳��
	unsigned char					waistScale;		// ����Χ��
	unsigned char					armWidth;		// ��֫Χ��
	unsigned char					legWidth;		// ��֫Χ��
	unsigned char					breastScale;	// �ز��ߵͣ�Ů�Բ��У�
	
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
