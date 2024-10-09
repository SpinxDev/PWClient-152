/*
 * FILE: EC_Face.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Duyuxin, 2004/12/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "EC_Global.h"
#include "EC_Face.h"
#include "EC_Resource.h"
#include "EC_Game.h"
#include "EC_GameRun.h"
#include "EC_Viewport.h"
#include "EC_World.h"
#include "EC_SceneLights.h"
#include "EC_Player.h"
#include "EC_RewuShader.h"
#include "EC_ProfConfigs.h"

#include "Expression.h"
#include "FaceBone.h"
#include "FaceBoneController.h"
#include "FaceAnimation.h"

#include "elementdataman.h"

#include <A3DDevice.h>
#include <A3DEngine.h>
#include <A3DSkinModel.h>
#include <A3DSkin.h>
#include <A3DSkinMesh.h>
#include <A3DMacros.h>
#include <A3DMaterial.h>
#include <A3DShader.h>
#include <A3DShaderMan.h>
#include <A3DSkeleton.h>
#include <A3DFuncs.h>
#include <A3DCamera.h>
#include <A3DLight.h>
#include <AIniFile.h>
#include <AFI.h>
#include <A3DGFXEx.h>
#include <A3DGFXElement.h>
#include <A3DGFXExMan.h>

#define new A_DEBUG_NEW

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////

bool						CECFace::m_bStaticDataInited = false;				// static data init flag
CECFace::ANIMATION_FACTOR	CECFace::m_AnimationFactors[FACE_ANIMATION_MAX];	// all animation definiton data
CExpression 				CECFace::m_Expressions[NUM_PROFESSION][2][FACE_ANIMATION_MAX];	// all expression for all races

///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////
//	Skin sort index
enum
{
	FACE_SKIN_SORT_FACE		= 0,	//	脸部
	FACE_SKIN_SORT_HAIR		= 1,	//	头发
};

static const char* _face_skin[NUM_PROFESSION*NUM_GENDER] = 
{
	"models\\face\\武侠男标准脸\\武侠男标准脸.%s",
	"models\\face\\法师女标准脸\\法师女标准脸.%s",
	
	"models\\face\\武侠男标准脸\\武侠男标准脸.%s",
	"models\\face\\法师女标准脸\\法师女标准脸.%s",
	
	"models\\face\\巫师男标准脸\\巫师男标准脸%d.%s",
	"models\\face\\巫师女标准脸\\巫师女标准脸%d.%s",
	
	NULL,
	"models\\face\\妖精标准脸\\妖精标准脸%d.%s",
	
	"models\\face\\妖兽男标准脸\\妖兽男标准脸%d.%s",
	NULL,
	
	"models\\face\\刺客男标准脸\\刺客男标准脸%d.%s",
	"models\\face\\刺客女标准脸\\刺客女标准脸%d.%s",
	
	"models\\face\\羽芒男标准脸\\羽芒男标准脸.%s",
	"models\\face\\羽芒女标准脸\\羽芒女标准脸.%s",
	
	"models\\face\\羽灵男标准脸\\羽灵男标准脸.%s",
	"models\\face\\羽灵女标准脸\\羽灵女标准脸.%s",

	"models\\face\\剑灵男标准脸\\剑灵男标准脸%d.%s",
	"models\\face\\剑灵女标准脸\\剑灵女标准脸%d.%s",
	
	"models\\face\\魅灵男标准脸\\魅灵男标准脸%d.%s",
	"models\\face\\魅灵女标准脸\\魅灵女标准脸%d.%s",

	"models\\face\\夜影男标准脸\\夜影男标准脸%d.%s",
	"models\\face\\夜影女标准脸\\夜影女标准脸%d.%s",
	
	"models\\face\\月仙男标准脸\\月仙男标准脸%d.%s",
	"models\\face\\月仙女标准脸\\月仙女标准脸%d.%s",
};

static const char* _face_animation[FACE_ANIMATION_MAX] = 
{
	"眨眼",
	"大笑",
	"生气",
	"微笑",
	"悲伤",
};

const char* CECFace::m_aMeshName[CECFace::NUM_PART] = 
{
	"model人脸_0",
	"model人脸_1",
	"model人脸_2",
	"model人脸_3",
	"model人脸_4",
	"model人脸_5",
	"model人脸_6",
};

static const char* l_aVSFiles[CECFace::NUM_PART+2] = 
{
	SHADER_DIR"vs\\facerender\\face_skin_mesh.txt",
	SHADER_DIR"vs\\facerender\\eye_skin_mesh.txt",
	SHADER_DIR"vs\\facerender\\brow_skin_mesh.txt",
	SHADER_DIR"vs\\facerender\\mouth_skin_mesh.txt",
	SHADER_DIR"vs\\facerender\\nose_skin_mesh.txt",
	SHADER_DIR"vs\\facerender\\eyeball_skin_mesh.txt",
	SHADER_DIR"vs\\facerender\\moustache_skin_mesh.txt",

	SHADER_DIR"vs\\facerender\\hair_skin_mesh.txt",		//	Hair
	SHADER_DIR"vs\\facerender\\goatee_skin_mesh.txt",	//	Goatee
};

const char* CECFace::m_szNeckHH		= "HH_Neck";
const char* CECFace::m_szNeckCC		= "CC_Neck";
const char* CECFace::m_szHeadBone	= "Bip01 Head";
const char* CECFace::m_szNeckBone	= "脖子对齐";
const char* CECFace::m_sz3rdEyeHH	= "HH_额饰";

///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////

static A3DShader * LoadNormalHairTextureShader(){
	A3DShader *result = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile(res_ShaderFile(RES_SHD_HAIR));
	if (!result){
		a_LogOutput(1, "Failed to load hair shader %s", res_ShaderFile(RES_SHD_HAIR));
	}
	return result;
}


///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECSkinModelWrapper
//	
///////////////////////////////////////////////////////////////////////////

//	Bind skeleton file
bool CECSkinModelWrapper::BindSkeletonFile(const char* szFile)
{
	return A3DSkinModel::BindSkeletonFile(szFile) ? true : false;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECModelWrapper
//	
///////////////////////////////////////////////////////////////////////////

//	Bind skin model
void CECModelWrapper::BindSkinModel(A3DSkinModel* pSkinModel)
{
	m_pA3DSkinModel = pSkinModel;
}

///////////////////////////////////////////////////////////////////////////
//	
//	Implement CECFace
//	
///////////////////////////////////////////////////////////////////////////


bool CECFace::LoadStaticData()
{
	m_bStaticDataInited = true;

	// first load animation factors.
	if( !LoadAnimationFactors() )
		return false;

	if( !LoadExpressions() )
		return false;

	return true;
}

//载入表情参数
bool CECFace::LoadAnimationFactors()
{
	AIniFile IniFile;

	const char * pszFile = "Configs\\ExpressionFactor.ini";
	if (!IniFile.Open(pszFile))
	{
		a_LogOutput(1, "CECFace::LoadExpressionFactorFromIniFile, Failed to open file %s", pszFile);
		return false;
	}

	AString strSection;

	strSection = "blink";
	m_AnimationFactors[FACE_ANIMATION_BLINK ].nAppearTime = IniFile.GetValueAsInt(strSection, "ApperTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_BLINK ].nKeepTime	= IniFile.GetValueAsInt(strSection, "KeepTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_BLINK ].nDisappearTime = IniFile.GetValueAsInt(strSection, "DisppearTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_BLINK].nRestTime = IniFile.GetValueAsInt(strSection, "RestTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_BLINK ].nNumRepeat = IniFile.GetValueAsInt(strSection, "NumRepeat", 1000);

	strSection = "laugh";
	m_AnimationFactors[FACE_ANIMATION_LAUGH].nAppearTime = IniFile.GetValueAsInt(strSection, "ApperTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_LAUGH].nKeepTime	= IniFile.GetValueAsInt(strSection, "KeepTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_LAUGH].nDisappearTime = IniFile.GetValueAsInt(strSection, "DisppearTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_LAUGH].nRestTime = IniFile.GetValueAsInt(strSection, "RestTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_LAUGH].nNumRepeat = IniFile.GetValueAsInt(strSection, "NumRepeat", 1000);

	strSection = "angry";
	m_AnimationFactors[FACE_ANIMATION_ANGRY].nAppearTime = IniFile.GetValueAsInt(strSection, "ApperTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_ANGRY].nKeepTime	= IniFile.GetValueAsInt(strSection, "KeepTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_ANGRY].nDisappearTime = IniFile.GetValueAsInt(strSection, "DisppearTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_ANGRY].nRestTime = IniFile.GetValueAsInt(strSection, "RestTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_ANGRY].nNumRepeat = IniFile.GetValueAsInt(strSection, "NumRepeat", 1000);

	strSection = "smile";
	m_AnimationFactors[FACE_ANIMATION_SMILE].nAppearTime = IniFile.GetValueAsInt(strSection, "ApperTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_SMILE].nKeepTime	= IniFile.GetValueAsInt(strSection, "KeepTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_SMILE].nDisappearTime = IniFile.GetValueAsInt(strSection, "DisppearTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_SMILE].nRestTime = IniFile.GetValueAsInt(strSection, "RestTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_SMILE].nNumRepeat = IniFile.GetValueAsInt(strSection, "NumRepeat", 1000);

	strSection= "sad";
	m_AnimationFactors[FACE_ANIMATION_SAD].nAppearTime = IniFile.GetValueAsInt(strSection, "ApperTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_SAD].nKeepTime	= IniFile.GetValueAsInt(strSection, "KeepTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_SAD].nDisappearTime = IniFile.GetValueAsInt(strSection, "DisppearTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_SAD].nRestTime = IniFile.GetValueAsInt(strSection, "RestTime", 1000);
	m_AnimationFactors[FACE_ANIMATION_SAD].nNumRepeat = IniFile.GetValueAsInt(strSection, "NumRepeat", 1000);

	return true;
}

bool CECFace::LoadExpressions()
{
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan();

	//数据空间ID
	ID_SPACE eIDSpace = ID_SPACE_FACE;		

	//获取脸型相关数据数量
	int nNumFaceData;
	nNumFaceData = pElementDataMan->get_data_num( eIDSpace);

	//脸型相关的数据类型
	DATA_TYPE dtFaceData;

	int i;
	for(i=0; i<nNumFaceData; i++)
	{
		//获取脸型数据id
		unsigned int uFaceDataID = pElementDataMan->get_data_id(eIDSpace, i, dtFaceData);

		if(dtFaceData == DT_FACE_EXPRESSION_ESSENCE)
		{
			FACE_EXPRESSION_ESSENCE* pFaceExpressionEssence = (FACE_EXPRESSION_ESSENCE*) pElementDataMan->get_data_ptr(uFaceDataID, eIDSpace,dtFaceData);
			if( !pFaceExpressionEssence ||dtFaceData != DT_FACE_EXPRESSION_ESSENCE)
				return true;

			//当前人物id和性别id
			int uCharacterComboID = pFaceExpressionEssence->character_combo_id ;
			int uGenderID = pFaceExpressionEssence->gender_id ;
			
			//if 当前人物的表情
			int id = pFaceExpressionEssence->id;

			char* pszExpressionFile = pFaceExpressionEssence->file_expression;
			AString strFile = pszExpressionFile;
			int nBgine = strFile.ReverseFind('\\');
			int nEnd = strFile.ReverseFind('.');
			char* pszBegin= &strFile[nBgine] + 1;
			char* pszEnd = &strFile[nEnd];
			*pszEnd = '\0';

			for(int cid=0; cid<NUM_PROFESSION; cid++)
			{
				if( (1 << cid) & uCharacterComboID )
				{
					for(int ani=0;ani<FACE_ANIMATION_MAX;ani++)
					{
						// load expression from file
						if( strcmp(pszBegin, _face_animation[ani]) == 0 )
						{
							m_Expressions[cid][uGenderID][ani].LoadExpression(pszExpressionFile);
							break;
						}
					}
				}
			}
		}
	}

	return true;
}

CECFace::CECFace()
{
	m_bHasTicked		= false;
	
	m_pSkinModel		= NULL;
	m_pECModel			= NULL;
	m_pParent			= NULL;

	m_pFaceSkin			= NULL;

	m_pHairSkin			= NULL;
	m_pGoateeSkin		= NULL;
	m_pFalingSkin		= NULL;

	m_nHairSkinIndex	= -1;
	m_nGoateeSkinIndex	= -1;
	m_nFalingSkinIndex	= -1;

	m_pFaceShader		= NULL;
	m_pEyeShader		= NULL;	
	m_pBrowShader		= NULL;
	m_pMouthShader		= NULL;
	m_pNoseShader		= NULL;
	m_pEyeBallShader	= NULL;
	m_pMoustacheShader	= NULL;
	m_pHairShader		= NULL;
	m_pGoateeShader		= NULL;

	m_pBoneCtrler		= NULL;

	m_iNeckCC			= -1;
	m_iNeckHH			= -1;
	m_iHeadBone			= -1;
	m_iNeckBone			= -1;
	m_iThirdEyeHH			= -1;

	m_pThirdEyeGfx		= NULL;

	m_colorHair			= 0xffffffff;
	m_colorFace			= 0xffffffff;
	m_colorEye			= 0xffffffff;
	m_colorBrow			= 0xffffffff;
	m_colorMouth		= 0xffffffff;
	m_colorNose			= 0xffffffff;
	m_colorEyeBall		= 0xffffffff;
	m_colorMoustache	= 0xffffffff;

	m_bHairTrans		= true;

	memset(m_aVS, 0, sizeof (m_aVS));

	//Initialize Face Customize Factor
	InitFaceCusFactor();
	
	if( !m_bStaticDataInited )
		LoadStaticData();
}

CECFace::~CECFace()
{
	Release();
}

//	Initialize object
bool CECFace::Init(int nCharacter, int nGender, int nFaceID)
{
	m_bHasTicked		= false;

	m_nCharacter	= nCharacter;	// 人物
	m_nGender		= nGender;		// 性别
	m_nFaceID		= nFaceID;		// 脸号

	// load different model's according to the character and gender
	AString strBone;
	AString strSkin;
	
	const char * pszSkin = _face_skin[m_nCharacter * NUM_GENDER + m_nGender];
	if(pszSkin)
	{
		const int nMaxBodyID = CECProfConfig::Instance().GetMaxBodyID(m_nCharacter);
		if (nMaxBodyID > 0)
		{
			// use the max value instead of invalid value
			if(nFaceID < 0 || nFaceID > nMaxBodyID)
			{
#ifndef	_PROFILE_MEMORY
				ASSERT(false);
#endif
				nFaceID = nMaxBodyID;
			}

			strBone.Format(pszSkin, nFaceID + 1, "bon");
			strSkin.Format(pszSkin, nFaceID + 1, "ski");
		}
		else
		{
			strBone.Format(pszSkin, "bon");
			strSkin.Format(pszSkin, "ski");
		}
	}

	if( strBone.IsEmpty() || strSkin.IsEmpty() )
	{
		a_LogOutput(1, "CECFacer::Init(), Unknown character or gender.");
		return false;
	}

	if (m_pSkinModel)
	{
		ASSERT(0);
		return false;
	}

	if (!(m_pSkinModel = new CECSkinModelWrapper))
	{
		glb_ErrorOutput(ECERR_NOTENOUGHMEMORY, "CECFace::Init", __LINE__);
		return false;
	}

	if (!m_pSkinModel->Init(g_pGame->GetA3DEngine()))
	{
		glb_ErrorOutput(ECERR_FAILEDTOCALL, "CECFace::Init", __LINE__);
		return false;
	}

	if (!m_pSkinModel->BindSkeletonFile(strBone))
	{
		delete m_pSkinModel;
		a_LogOutput(1, "CECFace::Init, failed to bind bone file %s", strBone);
		return false;
	}

	m_pSkinModel->EnableSpecular(true);

	m_pECModel = new CECModelWrapper;
	m_pECModel->BindSkinModel(m_pSkinModel);

	//	Face skin will be customized later, so they counld't be sharedly loaded by
	//	A3DSkinMan
	if (!(m_pFaceSkin = g_pGame->LoadA3DSkin(strSkin, true)))
	{
		a_LogOutput(1, "CECFace::Init, Falied to load skin %s", strSkin);
		return false;
	}
	m_pFaceSkin->SetAlphaSortWeight(FACE_SKIN_SORT_FACE);

	if (m_pSkinModel->AddSkin(m_pFaceSkin, false) < 0)
	{
		g_pGame->ReleaseA3DSkin(m_pFaceSkin);
		A3DRELEASE(m_pSkinModel);
		a_LogOutput(1, "CECFace::Init, Falied to add skin");
		return false;
	}

	// find the connector object
	if (!m_pSkinModel->GetSkeleton()->GetHook(m_szNeckCC, &m_iNeckCC))
		m_iNeckCC = -1;

	if (!m_pSkinModel->GetSkeleton()->GetBone(m_szNeckBone, &m_iNeckBone))
		m_iNeckBone = -1;

	if (!m_pSkinModel->GetSkeleton()->GetHook(m_sz3rdEyeHH, &m_iThirdEyeHH))
		m_iThirdEyeHH = -1;

	m_pSkinModel->SetMaterialMethod(A3DSkinModel::MTL_SCALED);

	m_pSkinModel->Update(0);//更新skinModel

	//	Load shaders of various part
	if (!InitPartShaders())
	{
		A3DRELEASE(m_pSkinModel);
		a_LogOutput(1, "CECFace::Init, Falied to initialize part shaders");
		return false;
	}

	//创建骨骼控制器
	if( !CreateFaceController() )
	{
		a_LogOutput(1, "CECFace::Init, Failed to create face controller!");
		return false;
	}

	InitEmptyFaceData();

	// 载入表情
	if( !LoadAnimations() )
		return false;

	PlayAnimation(FACE_ANIMATION_BLINK);
	return true;
}

void CECFace::ReleaseFaceController()
{
	if( m_pBoneCtrler != NULL)
	{
		m_pBoneCtrler->Release();
		delete m_pBoneCtrler;
		m_pBoneCtrler = NULL;
	}
}

//	Release object
void CECFace::Release()
{	
	//释放骨骼控制器
	ReleaseFaceController();

	ChangeThirdEyeGfx(NULL);

	A3DRELEASE(m_pSkinModel);
	A3DRELEASE(m_pECModel);

	if( m_pFaceSkin )
	{
		g_pGame->ReleaseA3DSkin(m_pFaceSkin, true);
		m_pFaceSkin = NULL;
	}

	if( m_pHairSkin )
	{
		g_pGame->ReleaseA3DSkin(m_pHairSkin, true);
		m_pHairSkin = NULL;

		m_pHairShader = NULL;
		m_nHairSkinIndex = -1;
	}

	if( m_pGoateeSkin )
	{
		g_pGame->ReleaseA3DSkin(m_pGoateeSkin, true);
		m_pGoateeSkin = NULL;

		m_pGoateeShader = NULL;
		m_nGoateeSkinIndex = -1;
	}

	if( m_pFalingSkin )
	{
		g_pGame->ReleaseA3DSkin(m_pFalingSkin, true);
		m_pFalingSkin = NULL;
		m_nFalingSkinIndex = -1;
	}

	A3DShaderMan* pShaderMan = g_pGame->GetA3DEngine()->GetA3DShaderMan();

	//	Load vertex shaders...
	for (int i=0; i < NUM_PART+2; i++)
	{
		if (m_aVS[i])
		{
			pShaderMan->ReleaseVertexShader(&m_aVS[i]);
			m_aVS[i] = NULL;
		}
	}

	m_bHasTicked		= false;
}

//	Initialize shaders of all parts
bool CECFace::InitPartShaders()
{
	//	We will customize this shader later, so use A3DShaderMan to load
	//	it directly rather than use A3DTextureMan which will load a shared
	//	shader.
	A3DShaderMan* pShaderMan = g_pGame->GetA3DEngine()->GetA3DShaderMan();

	//	Load vertex shaders...
	for (int i=0; i < NUM_PART+2; i++)
	{
		if (!(m_aVS[i] = pShaderMan->LoadVertexShader(l_aVSFiles[i], false, A3DVT_BVERTEX3)))
			a_LogOutput(1, "CECFace::InitPartShaders, Failed to load vertex shader %s!", l_aVSFiles[i]);
	}

	if( m_nCharacter == PROF_ORC && m_nGender == GENDER_MALE )
	{
		// 妖兽的头无个性化
	}
	else
	{
		//	Face shader will be set in ChangeFaceTexture
	}

	//	Eye shader
	m_pEyeShader = pShaderMan->LoadShaderFile(res_ShaderFile(RES_SHD_EYE));
	if (!m_pEyeShader)
		a_LogOutput(1, "CECFace::InitPartShaders, Failed to load eye shader");
	m_pEyeShader->GetGeneralProps().dwMask |= SGMF_TTEMP;
	if( !SetPartShader(PART_EYE, m_pEyeShader) )
		pShaderMan->ReleaseShader(&m_pEyeShader);

	//	Brow shader
	m_pBrowShader = pShaderMan->LoadShaderFile(res_ShaderFile(RES_SHD_BROW));
	if (!m_pBrowShader)
		a_LogOutput(1, "CECFace::InitPartShaders, Failed to load brow shader");
	m_pBrowShader->GetGeneralProps().dwMask |= SGMF_TTEMP;
	if( !SetPartShader(PART_BROW, m_pBrowShader) )
		pShaderMan->ReleaseShader(&m_pBrowShader);

	//	Mouth shader
	m_pMouthShader = pShaderMan->LoadShaderFile(res_ShaderFile(RES_SHD_MOUTH));
	if (!m_pMouthShader)
		a_LogOutput(1, "CECFace::InitPartShaders, Failed to load mouth shader");
	m_pMouthShader->GetGeneralProps().dwMask |= SGMF_TTEMP;
	if( !SetPartShader(PART_MOUTH, m_pMouthShader) )
		pShaderMan->ReleaseShader(&m_pMouthShader);
	
	//	Nose shader
	m_pNoseShader = pShaderMan->LoadShaderFile(res_ShaderFile(RES_SHD_NOSE));
	if (!m_pNoseShader)
		a_LogOutput(1, "CECFace::InitPartShaders, Failed to load nose shader");
	m_pNoseShader->GetGeneralProps().dwMask |= SGMF_TTEMP;
	if( !SetPartShader(PART_NOSE, m_pNoseShader) )
		pShaderMan->ReleaseShader(&m_pNoseShader);

	//	Eye ball shader
	m_pEyeBallShader = pShaderMan->LoadShaderFile(res_ShaderFile(RES_SHD_EYEBALL));
	if (!m_pEyeBallShader)
		a_LogOutput(1, "CECFace::InitPartShaders, Failed to load eye ball shader");
	m_pEyeBallShader->GetGeneralProps().dwMask |= SGMF_TTEMP;
	if( !SetPartShader(PART_EYEBALL, m_pEyeBallShader) )
		pShaderMan->ReleaseShader(&m_pEyeBallShader);

	//	Moustache shader
	//	only male has moustache
	if( m_nGender == GENDER_MALE )
	{
		m_pMoustacheShader = pShaderMan->LoadShaderFile(res_ShaderFile(RES_SHD_MOUSTACHE));
		if (!m_pMoustacheShader)
			a_LogOutput(1, "CECFace::InitPartShaders, Failed to load moustache shader");
		m_pMoustacheShader->GetGeneralProps().dwMask |= SGMF_TTEMP;
		if( !SetPartShader(PART_MOUSTACHE, m_pMoustacheShader) )
			pShaderMan->ReleaseShader(&m_pMoustacheShader);
	}

	// now add an empty hair skin here;
	m_nHairSkinIndex = m_pSkinModel->AddSkin(NULL, false);
	m_pHairSkin = m_pSkinModel->GetA3DSkin(m_nHairSkinIndex);

	// now add an empty moustache skin here;
	m_nGoateeSkinIndex = m_pSkinModel->AddSkin(NULL, false);
	m_pGoateeSkin = m_pSkinModel->GetA3DSkin(m_nGoateeSkinIndex);

	m_nFalingSkinIndex = m_pSkinModel->AddSkin(NULL, false);
	m_pFalingSkin = m_pSkinModel->GetA3DSkin(m_nFalingSkinIndex);

	// we can use gloss textures if using pixel shaders.
	if( g_pGame->GetA3DDevice()->TestPixelShaderVersion(1, 1) )
	{
		SHADERSTAGE stage2;
		if( m_pMouthShader && !m_pMouthShader->InsertStage(-1, stage2) )
			return false;
		if( m_pMouthShader && !m_pMouthShader->ChangeStageTexture(2, A3DSDTEX_NORMAL, (DWORD)"textures\\face\\lipreflect.tga") )
			return false;
		
		if( m_pEyeBallShader && !m_pEyeBallShader->InsertStage(-1, stage2) )
			return false;
		if( m_pEyeBallShader && !m_pEyeBallShader->ChangeStageTexture(2, A3DSDTEX_NORMAL, (DWORD)"textures\\face\\eyereflect.tga") )
			return false;
	}
	return true;
}

//	Set parent model
void CECFace::SetParent(A3DSkinModel* pParent)
{
	m_pParent = pParent;

	if (m_pParent)
	{
		if (m_pSkinModel)
			m_pSkinModel->SetLightInfo(m_pParent->GetLightInfo());
	}

	if (!m_pParent || !m_pParent->GetSkeleton()->GetHook(m_szNeckHH, &m_iNeckHH))
		m_iNeckHH = -1;

	// now try to find neck bone
	if (!m_pParent || !m_pParent->GetSkeleton()->GetBone(m_szHeadBone, &m_iHeadBone))
		m_iHeadBone = -1;
}

//	Update facial animations
bool CECFace::UpdateAnimations(DWORD dwDeltaTime)
{
	for(int i=0; i<FACE_ANIMATION_MAX; i++)
	{
		if( m_Animations[i].HasInited() )
		{
			m_Animations[i].Tick(dwDeltaTime);
		}
	}

	return true;
}

const A3DMATRIX4& CECFace::GetHeadBoneTM()
{
	//	Get head bone's absolute TM
	A3DBone* pHeadBone = m_pParent->GetSkeleton()->GetBone(m_iHeadBone);
	ASSERT(pHeadBone);
	return pHeadBone->GetAbsoluteTM();
}

void CECFace::TickThirdEye(DWORD dwDeltaTime)
{
	if (m_iThirdEyeHH >= 0 && m_pThirdEyeGfx)
	{
		A3DSkeletonHook *pThirdEyeHook = m_pSkinModel->GetSkeleton()->GetHook(m_iThirdEyeHH);
		ASSERT(pThirdEyeHook);
		m_pThirdEyeGfx->SetParentTM(pThirdEyeHook->GetAbsoluteTM());
		m_pThirdEyeGfx->TickAnimation(dwDeltaTime);
	}
}

//	Tick routinue
bool CECFace::Tick(DWORD dwDeltaTime)
{
	if (!m_pSkinModel)
		return false;

	if (!m_pParent)
	{
		m_pSkinModel->Update(dwDeltaTime);
		TickThirdEye(dwDeltaTime);
		return true;
	}

	if (m_iHeadBone >= 0 && m_iNeckBone >= 0)
	{
		//	Get head bone's absolute TM
		A3DBone* pHeadBone = m_pParent->GetSkeleton()->GetBone(m_iHeadBone);
		ASSERT(pHeadBone);
		A3DMATRIX4 matHeadBone = pHeadBone->GetAbsoluteTM();

		//	Get neck bone's relative TM at original
		A3DBone* pNeckBone = m_pSkinModel->GetSkeleton()->GetBone(m_iNeckBone);
		ASSERT(pNeckBone);
		A3DMATRIX4 matNeckBone = pNeckBone->GetOriginalMatrix();

		// temp code
		if( matHeadBone.GetRow(3).IsZero() )
			return false;
		
		//	Re-update model's absolute TM
		m_pSkinModel->SetAbsoluteTM(InverseTM(matNeckBone) * matHeadBone);
		m_pSkinModel->Update(dwDeltaTime, false); // face bone has bone controller, bone controller will not be updated unless with anim update flag
		TickThirdEye(dwDeltaTime);

		m_bHasTicked = true;
	}
	else
	{
		m_pSkinModel->Update(dwDeltaTime);
		TickThirdEye(dwDeltaTime);
		return true;
	}

	UpdateAnimations(dwDeltaTime);

	if (m_bHairTrans)
	{
		// now test if the face is too close too the water, we just turn off the hair's alpha property
		A3DVECTOR3 vecHeadPos = m_pSkinModel->GetPos();
		A3DVECTOR3 vecCamPos = g_pGame->GetViewport()->GetA3DCamera()->GetPos();
		float waterHeight = g_pGame->GetGameRun()->GetWorld()->GetWaterHeight(vecHeadPos);
		if( fabs(vecHeadPos.y - waterHeight) < 0.1f || (vecHeadPos.y - waterHeight) * (vecCamPos.y - waterHeight) < 0.0f )
		{
			SetHairMaterial(false);
			SetGoateeMaterial(false);
			SetFalingMaterial(false);
		}
		else
		{
			SetHairMaterial(true);
			SetGoateeMaterial(true);
			SetFalingMaterial(true);
		}
	}

	return true;
}

//	Render routine
bool CECFace::Render(CECViewport* pViewport, bool bPortrait, bool bHighLight, const A3DSkinModel::LIGHTINFO *pPortraitLight)
{
	if( !m_pParent || m_iNeckBone == -1 || m_iHeadBone == -1 || !m_bHasTicked )
		return true;

	// first of all adjust light info;
	if( m_pSkinModel )
	{
		A3DSkinModel::LIGHTINFO LightInfo;
		if (bPortrait){
			LightInfo = *pPortraitLight;
		}else{
			memset(&LightInfo, 0, sizeof(LightInfo));
			
			const A3DLIGHTPARAM& lp = g_pGame->GetDirLight()->GetLightparam();
			LightInfo.colDirDiff	= lp.Diffuse;
			LightInfo.colDirSpec	= lp.Specular;
			LightInfo.vLightDir		= lp.Direction;
			LightInfo.colAmbient	= g_pGame->GetA3DDevice()->GetAmbientValue();
			
			// we use dynamic light info taken from the parent model
			A3DSkinModel::LIGHTINFO parentInfo = m_pParent->GetLightInfo();
			if( !bPortrait && parentInfo.bPtLight )
			{
				LightInfo.colPtDiff = parentInfo.colPtDiff;
				LightInfo.colPtAmb =  parentInfo.colPtAmb;
				LightInfo.fPtRange = parentInfo.fPtRange;
				LightInfo.vPtAtten = parentInfo.vPtAtten;
				LightInfo.vPtLightPos = parentInfo.vPtLightPos;
			}
			else
			{
				LightInfo.colPtDiff = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
				LightInfo.colPtAmb = A3DCOLORVALUE(0.0f, 0.0f, 0.0f, 1.0f);
				LightInfo.fPtRange = 0.01f;
				LightInfo.vPtAtten = A3DVECTOR3(0.0f);
				LightInfo.vPtLightPos = A3DVECTOR3(0.0f);
			}
			LightInfo.bPtLight = true;
		}
		m_pSkinModel->SetLightInfo(LightInfo);
	}

	A3DMATRIX4 matScale = IdentityMatrix();
	matScale._11 = 0.5f;
	matScale._22 = -0.5f;
	matScale._41 = 0.5f;
	matScale._42 = 0.5f;

	A3DMATRIX4 matFace = IdentityMatrix();
	A3DCamera * pCamera = pViewport->GetA3DCamera();
	A3DMATRIX4 matProjectedView;
	matProjectedView = InverseTM(pCamera->GetViewTM()) * InverseTM(matFace) * m_tmTattoo * matScale;
	
	//  first of all, see if we need change face skin's material to make it colorful
	if( !g_pGame->GetA3DDevice()->TestPixelShaderVersion(1, 1) && m_pFaceSkin )
	{
		int i, nNumMaterial = m_pFaceSkin->GetMaterialNum();
		for(i=0; i<nNumMaterial; i++)
		{
			A3DMATERIALPARAM param = m_pFaceSkin->GetMaterial(i)->GetMaterialParam();
			param.Diffuse = m_colorFace;
			param.Ambient = m_colorFace;
			m_pFaceSkin->GetMaterial(i)->SetMaterialParam(param);
		}
	}

	//	Apply all parts colors
	if (m_pHairShader)
	{
		m_pHairShader->GetGeneralProps().dwTFactor = m_colorHair;
	}

	if (m_pGoateeShader)
	{
		m_pGoateeShader->GetGeneralProps().dwTFactor = m_colorMoustache;
	}

	if (m_pFaceShader){
		if (CECWanmeiShader().MatchShader(m_pFaceShader)){
			SHADERGENERAL & shaderParameter = m_pFaceShader->GetGeneralProps();
			shaderParameter.dwTFactor = m_colorFace;
			shaderParameter.dwTTemp = m_colorEye;
			shaderParameter.aPSConsts[0] = A3DCOLORVALUE(m_colorBrow);
			shaderParameter.aPSConsts[1] = A3DCOLORVALUE(m_colorMouth);
		}else{
			m_pFaceShader->GetGeneralProps().dwTFactor = m_colorFace;
			memcpy(m_pFaceShader->GetStage(1).matTexTrans, &matProjectedView, sizeof(float) * 16);
		}
	}

	if (m_pEyeShader)
	{
		m_pEyeShader->GetGeneralProps().dwTFactor = m_colorEye;
		m_pEyeShader->GetGeneralProps().dwTTemp = m_colorFace;
		memcpy(m_pEyeShader->GetStage(2).matTexTrans, &matProjectedView, sizeof(float) * 16);
	}

	if (m_pBrowShader)
	{
		m_pBrowShader->GetGeneralProps().dwTFactor = m_colorBrow;
		m_pBrowShader->GetGeneralProps().dwTTemp = m_colorFace;
		memcpy(m_pBrowShader->GetStage(2).matTexTrans, &matProjectedView, sizeof(float) * 16);
	}

	if (m_pMouthShader)
	{
		m_pMouthShader->GetGeneralProps().dwTFactor = m_colorMouth;
		m_pMouthShader->GetGeneralProps().dwTTemp = m_colorFace;
		// no tattoo
	}

	if (m_pNoseShader)
	{
		m_pNoseShader->GetGeneralProps().dwTTemp = m_colorFace;
		memcpy(m_pNoseShader->GetStage(1).matTexTrans, &matProjectedView, sizeof(float) * 16);
	}

	if (m_pEyeBallShader)
	{
		m_pEyeBallShader->GetGeneralProps().dwTFactor = m_colorEyeBall;
		m_pEyeBallShader->GetGeneralProps().dwTTemp = m_colorFace;
		// no tattoo
	}

	if (m_pMoustacheShader)
	{
		m_pMoustacheShader->GetGeneralProps().dwTFactor = m_colorMoustache;
		m_pMoustacheShader->GetGeneralProps().dwTTemp = m_colorFace;
		memcpy(m_pMoustacheShader->GetStage(2).matTexTrans, &matProjectedView, sizeof(float) * 16);
	}

	if (m_pSkinModel)
	{
		if (bPortrait)
			m_pSkinModel->RenderAtOnce(pViewport->GetA3DViewport(), 0, false);
		else if (bHighLight && g_pGame->GetA3DDevice()->TestPixelShaderVersion(1, 1))
			g_pGame->RenderHighLightModel(pViewport, m_pSkinModel);
		else
			m_pSkinModel->Render(pViewport->GetA3DViewport());
	}

	if (m_pThirdEyeGfx)
	{
		A3DGFXExMan *pGFXExMan = g_pGame->GetA3DGFXExMan();
		pGFXExMan->RegisterGfx(m_pThirdEyeGfx);
	}

	return true;
}

//	Set part shader
bool CECFace::SetPartShader(int iPart, A3DShader * pShader)
{
	if (!m_pSkinModel)
		return false;

	A3DSkinModel::SKIN* pSkinItem = m_pSkinModel->GetSkinItem(0);
	if (!pSkinItem)
		return false;

	const char* szMeshName = m_aMeshName[iPart];
	A3DSkinMeshRef* pRef = pSkinItem->pA3DSkin->GetSkinMeshRefByName(szMeshName);
	if (!pRef || !pRef->pMesh)
	{
		//a_LogOutput(1, "CECFace::SetPartShader, Can't find mesh %s", szMeshName);
		return false;
	}

	//	Set vertex shader
	pRef->pvs = m_aVS[iPart];

	int iTexIndex = pRef->pMesh->GetTextureIndex();
	if (!pSkinItem->pA3DSkin->ChangeSkinTexturePtr(iTexIndex, pShader))
	{
		a_LogOutput(1, "CECFace::SetPartShader, Failed to change skin texture");
		return false;
	}

	// adjust skin's material to make it a little speculable
	int nNumMaterial = pSkinItem->pA3DSkin->GetMaterialNum();
	for(int i=0; i<nNumMaterial; i++)
	{
		A3DMATERIALPARAM param = pSkinItem->pA3DSkin->GetMaterial(i)->GetMaterialParam();
		param.Specular = 0xff323232;
		param.Power = 10.0f;
		pSkinItem->pA3DSkin->GetMaterial(i)->SetMaterialParam(param);
	}

	return true;
}

bool CECFace::SetHairMaterial(bool bUseAlpha)
{
	if( !m_pHairSkin )
		return true;

	A3DCOLOR	specularColor = 0xffffffff;
	float		specularPower = 10.0f;

	if( !g_pGame->GetA3DDevice()->TestPixelShaderVersion(1, 1) )
		specularColor = 0xff606060;

	int i, nNumMaterial = m_pHairSkin->GetMaterialNum();
	for(i=0; i<nNumMaterial; i++)
	{
		A3DMATERIALPARAM param = m_pHairSkin->GetMaterial(i)->GetMaterialParam();
		param.Specular = specularColor;
		param.Power = specularPower;
		m_pHairSkin->GetMaterial(i)->SetMaterialParam(param);
	}

	for(i=0; i < m_pHairSkin->GetSkinMeshNum(); i++)
	{
		A3DSkinMeshRef& Ref = m_pHairSkin->GetSkinMeshRef(i);
		Ref.pvs = m_aVS[NUM_PART];	// hair vs

		if (Ref.pMesh->GetTextureIndex() == 0) // if it is use the hair texture, just set to use alpha
		{
			if( !bUseAlpha )
				Ref.fTrans = -1.0f;
		//	else
		//		Ref.fTrans = 0.0f;
		}
	}

	return true;
}

bool CECFace::SetGoateeMaterial(bool bUseAlpha)
{
	if( !m_pGoateeSkin )
		return true;

	A3DCOLOR	specularColor = 0xffffffff;
	float		specularPower = 10.0f;

	if( !g_pGame->GetA3DDevice()->TestPixelShaderVersion(1, 1) )
		specularColor = 0xff606060;

	int nNumMaterial = m_pGoateeSkin->GetMaterialNum();
	int i(0);
	for(i=0; i<nNumMaterial; i++)
	{
		A3DMATERIALPARAM param = m_pGoateeSkin->GetMaterial(i)->GetMaterialParam();
		param.Specular = specularColor;
		param.Power = specularPower;
		m_pGoateeSkin->GetMaterial(i)->SetMaterialParam(param);
	}

	for(i=0; i<m_pGoateeSkin->GetSkinMeshNum(); i++)
	{
		A3DSkinMeshRef& Ref = m_pGoateeSkin->GetSkinMeshRef(i);
		Ref.pvs = m_aVS[NUM_PART+1];

		if (Ref.pMesh->GetTextureIndex() == 0)
		{
			if( !bUseAlpha )
				Ref.fTrans = -1.0f;
		//	else
		//		Ref.fTrans = 0.0f;
		}
	}

	return true;
}

bool CECFace::SetFalingMaterial(bool bUseAlpha)
{
	if( !m_pFalingSkin )
		return true;

	// no specular for faling
	int nNumMaterial = m_pFalingSkin->GetMaterialNum();
	int i(0);
	for(i=0; i<nNumMaterial; i++)
	{
		A3DMATERIALPARAM param = m_pFalingSkin->GetMaterial(i)->GetMaterialParam();
		param.Specular = 0xff000000;
		param.Power = 10.0f;
		m_pFalingSkin->GetMaterial(i)->SetMaterialParam(param);
	}

	for(i=0; i < m_pFalingSkin->GetSkinMeshNum(); i++)
	{
		A3DSkinMeshRef& Ref = m_pFalingSkin->GetSkinMeshRef(i);
		Ref.pvs = NULL;

		if (Ref.pMesh->GetTextureIndex() == 0)
		{
			if( !bUseAlpha )
				Ref.fTrans = -1.0f;
		//	else
		//		Ref.fTrans = 0.0f;
		}
	}

	return true;
}

//	Change hair model
bool CECFace::ChangeHairModel(const char* szModelFile, const char * szSkinFile)
{
	if (!m_pSkinModel)
		return false;

	assert(m_nHairSkinIndex >= 0);

	//	Hair skin will be customized later, so they counld't be sharedly loaded by
	//	A3DSkinMan
	A3DSkin* pSkin = g_pGame->LoadA3DSkin(szSkinFile, true);
	if (!pSkin)
	{
		a_LogOutput(1, "CECFace::ChangeHairModel, Failed to load skin %s", szSkinFile);
		return false;
	}

	// replace it with a new hair skin;
	m_pSkinModel->ReplaceSkin(m_nHairSkinIndex, pSkin, false);

	// release old hair skin.
	if( m_pHairSkin )
	{
		g_pGame->ReleaseA3DSkin(m_pHairSkin, true);
		m_pHairSkin = NULL;

		m_pHairShader = NULL;
	}

	// record to the new skin;
	m_pHairSkin = m_pSkinModel->GetA3DSkin(m_nHairSkinIndex);
	m_pHairSkin->SetAlphaSortWeight(FACE_SKIN_SORT_HAIR);

	SetHairMaterial(true);

	// change the texture to shader;
	if( m_pHairSkin->GetTextureNum() > 0 )
	{
		// we only change the hair skin texture 
		//	Hair shader
		ChangeHairTextureShader(LoadNormalHairTextureShader());
	}

	return true;
}

//	Change moustache model
bool CECFace::ChangeGoateeModel(const char * szSkinFile)
{
	if (!m_pSkinModel)
		return false;

	assert(m_nGoateeSkinIndex >= 0);

	if( NULL == szSkinFile || szSkinFile[0] == '\0' )
	{
		// Change to a empty goatee
		// release old goatee skin.
		if( m_pGoateeSkin )
		{
			g_pGame->ReleaseA3DSkin(m_pGoateeSkin, true);
			m_pGoateeSkin = NULL;

			m_pGoateeShader = NULL;
		}

		// replace it with a new goatee skin;
		m_pSkinModel->ReplaceSkin(m_nGoateeSkinIndex, NULL, false);
		return true;
	}

	//	Moustache skin will be customized later, so they counld't be sharedly loaded by
	//	A3DSkinMan
	A3DSkin* pSkin = g_pGame->LoadA3DSkin(szSkinFile, true);
	if (!pSkin)
	{
		a_LogOutput(1, "CECFace::ChangeMoustacheModel, Falied to load skin %s", szSkinFile);
		return false;
	}

	// replace it with a new moustache skin;
	m_pSkinModel->ReplaceSkin(m_nGoateeSkinIndex, pSkin, false);

	// release old moustache skin.
	if( m_pGoateeSkin )
	{
		g_pGame->ReleaseA3DSkin(m_pGoateeSkin, true);
		m_pGoateeSkin = NULL;

		m_pGoateeShader = NULL;
	}

	// record to the new skin;
	m_pGoateeSkin = m_pSkinModel->GetA3DSkin(m_nGoateeSkinIndex);
	
	SetGoateeMaterial(true);

	// change the texture to shader;
	if( m_pGoateeSkin->GetTextureNum() > 0 )
	{
		//	Goatee shader
		m_pGoateeShader = g_pGame->GetA3DEngine()->GetA3DShaderMan()->LoadShaderFile(res_ShaderFile(RES_SHD_GOATEE));
		if (!m_pGoateeShader)
			a_LogOutput(1, "CECFace::ChangeGoateeModel(), Failed to load goatee shader");
		m_pGoateeSkin->ChangeSkinTexturePtr(0, m_pGoateeShader);
	}

	return true;
}

//	Change faling model
bool CECFace::ChangeFalingModel(const char * szSkinFile)
{
	if (!m_pSkinModel)
		return false;

	assert(m_nFalingSkinIndex >= 0);

	if( m_nGender == GENDER_FEMALE || m_nCharacter == PROF_ORC )
		return true;

	if( NULL == szSkinFile || szSkinFile[0] == '\0' )
	{
		// Change to a emopty faling
		// release old faling skin.
		if( m_pFalingSkin )
		{
			g_pGame->ReleaseA3DSkin(m_pFalingSkin, true);
			m_pFalingSkin = NULL;
		}

		// replace it with a new faling skin;
		m_pSkinModel->ReplaceSkin(m_nFalingSkinIndex, NULL, false);
		return true;
	}

	//	Faling skin will be customized later, so they counld't be sharedly loaded by
	//	A3DSkinMan
	A3DSkin* pSkin = g_pGame->LoadA3DSkin(szSkinFile, true);
	if (!pSkin)
	{
		a_LogOutput(1, "CECFace::ChangeFalingModel, Falied to load skin %s", szSkinFile);
		return false;
	}
	
	// replace it with a new faling skin;
	m_pSkinModel->ReplaceSkin(m_nFalingSkinIndex, pSkin, false);

	// release old faling skin.
	if( m_pFalingSkin )
	{
		g_pGame->ReleaseA3DSkin(m_pFalingSkin, true);
		m_pFalingSkin = NULL;
	}

	// record to the new skin;
	m_pFalingSkin = m_pSkinModel->GetA3DSkin(m_nFalingSkinIndex);
	
	SetFalingMaterial(true);

	return true;
}

//	change textures
bool CECFace::ChangeFaceTexture(const char * szTexMap)
{
	if((m_nCharacter == PROF_ORC && m_nGender == GENDER_MALE ) )
		return true;

	if (!szTexMap || !szTexMap[0])
	{
		ASSERT(szTexMap);
		return NULL;
	}

	char szFileTitle[MAX_PATH];
	af_GetFileTitle(szTexMap, szFileTitle, MAX_PATH);
	strlwr(szFileTitle);

	char temp[MAX_PATH];

	if( m_nCharacter == PROF_MONK ||
		m_nCharacter ==	PROF_GHOST ||
		m_nCharacter == PROF_JIANLING ||
		m_nCharacter == PROF_MEILING )
	{
		strcpy(temp, "rw_");
		strcat(temp, szFileTitle);
		
		strcpy(szFileTitle, temp);

		af_GetFilePath(szTexMap, temp, MAX_PATH);
		strcat(temp, "\\");
		strcat(temp, szFileTitle);

		szTexMap = temp;
	}

	A3DShaderMan* pShaderMan = g_pGame->GetA3DEngine()->GetA3DShaderMan();

	if (CECRewuShader().MatchTexture(szTexMap, NULL)){
		m_pFaceShader = CECRewuShader().Load();
		CECRewuShader().SetTexture(m_pFaceShader, szTexMap, NULL);
	}else if (CECWanmeiShader().MatchTexture(szTexMap, NULL)){
		m_pFaceShader = CECWanmeiShader().Load();
		CECWanmeiShader().SetTexture(m_pFaceShader, szTexMap, NULL);
	}else{
		m_pFaceShader = pShaderMan->LoadShaderFile(res_ShaderFile(RES_SHD_FACE));
		m_pFaceShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szTexMap);
		m_pFaceShader->GetGeneralProps().dwMask |= SGMF_TTEMP;
	}
	if (!SetPartShader(PART_FACE, m_pFaceShader)){
		pShaderMan->ReleaseShader(&m_pFaceShader);
	}

	return true;
}

bool CECFace::ChangeEyeTexture(const char * szBaseTex, const char * szHighTex)
{
	if (m_pEyeShader && !m_pEyeShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szBaseTex))
		return false;
	if (m_pEyeShader && !m_pEyeShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szHighTex))
		return false;

	return true;
}

bool CECFace::ChangeBrowTexture(const char * szBaseTex, const char * szHighTex)
{
	if (m_pBrowShader && !m_pBrowShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szBaseTex))
		return false;

	if (m_pBrowShader && !m_pBrowShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szHighTex))
		return false;
	
	return true;
}

bool CECFace::ChangeMouthTexture(const char * szBaseTex, const char * szHighTex)
{
	if (m_pMouthShader && !m_pMouthShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szBaseTex))
		return false;
	if (m_pMouthShader && !m_pMouthShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szHighTex))
		return false;

	return true;
}

bool CECFace::ChangeNoseTexture(const char * szBaseTex)
{
	if (m_pNoseShader && !m_pNoseShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szBaseTex))
		return false;

	return true;
}

bool CECFace::ChangeEyeBallTexture(const char * szBaseTex, const char * szHighTex)
{
	if (m_pEyeBallShader && !m_pEyeBallShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szBaseTex))
		return false;
	if (m_pEyeBallShader && !m_pEyeBallShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szHighTex))
		return false;

	return true;
}

bool CECFace::ChangeMoustacheTexture(const char * szBaseTex, const char * szHighTex)
{
	if (m_pMoustacheShader && !m_pMoustacheShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szBaseTex))
		return false;
	if (m_pMoustacheShader && !m_pMoustacheShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szHighTex))
		return false;

	return true;
}

bool CECFace::ChangeTattooTexture(const char * szTattooTex)
{
	if( szTattooTex && szTattooTex[0] )
	{
		if( m_pFaceShader )
		{
			//if (!m_pFaceShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szTattooTex))
			//	return false;
			//m_pFaceShader->ShowStage(1, true);
		}

		if( m_pEyeShader )
		{
			if (!m_pEyeShader->ChangeStageTexture(2, A3DSDTEX_NORMAL, (DWORD)szTattooTex))
				return false;
			m_pEyeShader->ShowStage(2, true);
		}

		if( m_pBrowShader )
		{
			if (!m_pBrowShader->ChangeStageTexture(2, A3DSDTEX_NORMAL, (DWORD)szTattooTex))
				return false;
			m_pBrowShader->ShowStage(2, true);
		}

		if( m_pNoseShader )
		{
			if (!m_pNoseShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szTattooTex))
				return false;
			m_pNoseShader->ShowStage(1, true);
		}

		if( m_pMoustacheShader )
		{
			if (!m_pMoustacheShader->ChangeStageTexture(2, A3DSDTEX_NORMAL, (DWORD)szTattooTex))
				return false;
			m_pMoustacheShader->ShowStage(2, true);
		}
	}
	else
	{
		if( m_pFaceShader )
		{
			//if (!m_pFaceShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, NULL))
			//	return false;
			//m_pFaceShader->ShowStage(1, false);
		}

		if( m_pEyeShader )
		{
			if (!m_pEyeShader->ChangeStageTexture(2, A3DSDTEX_NORMAL, NULL))
				return false;
			m_pEyeShader->ShowStage(2, false);
		}

		if( m_pBrowShader )
		{
			if (!m_pBrowShader->ChangeStageTexture(2, A3DSDTEX_NORMAL, NULL))
				return false;
			m_pBrowShader->ShowStage(2, false);
		}

		if( m_pNoseShader )
		{
			if (!m_pNoseShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, NULL))
				return false;
			m_pNoseShader->ShowStage(1, false);
		}

		if( m_pMoustacheShader )
		{
			if (!m_pMoustacheShader->ChangeStageTexture(2, A3DSDTEX_NORMAL, NULL))
				return false;
			m_pMoustacheShader->ShowStage(2, false);
		}
	}
	return true;
}

void CECFace::ChangeHairTextureShader(A3DShader *p){
	m_pHairShader = p;
	m_pHairSkin->ChangeSkinTexturePtr(0, m_pHairShader);
}

bool CECFace::ChangeHairRewuShaderTexture(const char * szTexMap, const char * szTexMap2){
	if (!m_pHairShader){
		return false;
	}
	if (!CECRewuShader().MatchShader(m_pHairShader)){
		ChangeHairTextureShader(CECRewuShader().Load());
	}
	CECRewuShader().SetTexture(m_pHairShader, szTexMap, szTexMap2);
	m_pHairShader->SetAlphaTextureFlag(false);	//	2014-9-17 徐文彬：	非alpha的DXT1格式、alpha值为0的区域无法设置黑色以外的颜色，
												//						导致实际显示有黑边（贴图插值）。因此，使用DXT3、DXT5等格式
												//						可避免黑边，但因透明度排序容易出现问题，此处强行设置为 false，
												//						既利用了 DXT3、DXT5可以设置非黑的边缘颜色，又没有透明度相关问题
	return true;
}

bool CECFace::ChangeHairNormalShaderTexture(const char * szBaseTex, const char * szHighTex){
	if (!m_pHairShader){
		return false;
	}
	if (CECRewuShader().MatchShader(m_pHairShader)){
		ChangeHairTextureShader(LoadNormalHairTextureShader());
	}
	return m_pHairShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szBaseTex)
		&& m_pHairShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szHighTex);
}

bool CECFace::ChangeHairTexture(const char * szBaseTex, const char * szHighTex)
{
	if (!m_pHairShader){
		return false;
	}
	if (CECRewuShader().MatchTexture(szBaseTex, szHighTex)){
		return ChangeHairRewuShaderTexture(szBaseTex, szHighTex);
	}else{
		return ChangeHairNormalShaderTexture(szBaseTex, szHighTex);
	}
}

bool CECFace::ChangeGoateeTexture(const char * szBaseTex, const char * szHighTex)
{
	// now change the shader's current texture
	if (m_pGoateeShader && !m_pGoateeShader->ChangeStageTexture(0, A3DSDTEX_NORMAL, (DWORD)szBaseTex))
		return false;
	if (m_pGoateeShader && !m_pGoateeShader->ChangeStageTexture(1, A3DSDTEX_NORMAL, (DWORD)szHighTex))
		return false;

	return true;
}

bool CECFace::ChangeFalingTexture(const char * szHighTex)
{
	if( m_nGender == GENDER_FEMALE || m_nCharacter == PROF_ORC )
		return true;
	
	if( m_pFalingSkin &&
		m_pFalingSkin->GetTextureNum() > 0)
	{
		m_pFalingSkin->ChangeSkinTexture(0, szHighTex);
	}

	return true;
}

bool CECFace::ChangeThirdEyeGfx(const char *szGfx)
{
	bool bRet(false);
	
	A3DGFXExMan *pGFXExMan = g_pGame->GetA3DGFXExMan();
	if (m_pThirdEyeGfx)
	{
		pGFXExMan->CacheReleasedGfx(m_pThirdEyeGfx);
		m_pThirdEyeGfx = NULL;
	}
	if (!szGfx || !szGfx[0])
	{
		bRet = true;
	}
	else if (m_iThirdEyeHH >= 0)
	{
		szGfx += strlen("gfx\\");
		m_pThirdEyeGfx = pGFXExMan->LoadGfx(g_pGame->GetA3DDevice(), szGfx);
		if (m_pThirdEyeGfx)
		{
			TickThirdEye(0);
			m_pThirdEyeGfx->SetScale(1.0f);
			m_pThirdEyeGfx->Start(true);
			bRet = true;
		}
	}
	return bRet;
}

// 初始化脸型数据为合法的空白数据
void CECFace::InitEmptyFaceData()
{
	WORD	idFaceTex[NUM_PROFESSION][2]	= {{44, 0},	 {0, 5},   {855, 860}, {0, 0}, {0, 0}, {845, 850}, {0, 0}, {0, 142}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
	WORD	idEyeBaseTex[NUM_PROFESSION][2]	= {{50, 0},  {0, 257}, {0, 0}, {0, 0}, {0, 0}, {0, 257}, {0, 0}, {0, 257}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
	WORD	idEyeHighTex[NUM_PROFESSION][2]	= {{349, 0}, {0, 256}, {0, 0}, {0, 0}, {0, 0}, {0, 256}, {0, 0}, {0, 256}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
	WORD	idEyeBallTex[NUM_PROFESSION][2]	= {{48, 0},  {0, 9},   {0, 0}, {0, 0}, {104, 0}, {0, 9}, {0, 0}, {0, 9}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
	WORD	idBrowTex[NUM_PROFESSION][2]	= {{45, 0},  {0, 6},   {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 6}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
	WORD	idNoseTex[NUM_PROFESSION][2]	= {{42, 0},  {0, 3},   {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 3}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
	WORD	idMouthTex[NUM_PROFESSION][2]	= {{52, 0},  {0, 14},  {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 14}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
	WORD	idHairModel[NUM_PROFESSION][2]	= {{265, 0}, {0, 189}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 189}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
	WORD	idHairTex[NUM_PROFESSION][2]	= {{313, 0}, {0, 311}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 311}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};
	WORD	idMoustacheTex[NUM_PROFESSION][2] = {{207, 0}, {0, 0},   {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};

	memset(&m_FaceData, 0, sizeof(m_FaceData));

	//3庭3
	m_FaceData.scaleUp				= 128;
	m_FaceData.scaleMiddle			= 128;
	m_FaceData.scaleDown			= 128;

	//脸型融合3
	m_FaceData.idFaceShape1			= 0;
	m_FaceData.idFaceShape2			= 0;
	m_FaceData.blendFaceShape		= 50;

	//脸盘3
	m_FaceData.scaleFaceH			= 128;
	m_FaceData.scaleFaceV			= 128;
	m_FaceData.idFaceTex			= idFaceTex[m_nCharacter][m_nGender];

	//额头5
	m_FaceData.offsetForeheadH		= 128;
	m_FaceData.offsetForeheadV		= 128;
	m_FaceData.offsetForeheadZ		= 128;
	m_FaceData.rotateForehead		= 128;
	m_FaceData.scaleForehead		= 128;

	//颧骨5
	m_FaceData.offsetYokeBoneH		= 128;
	m_FaceData.offsetYokeBoneV		= 128;
	m_FaceData.offsetYokeBoneZ		= 128;
	m_FaceData.rotateYokeBone		= 128;
	m_FaceData.scaleYokeBone		= 128;

	//脸颊4
	m_FaceData.offsetCheekH			= 128;
	m_FaceData.offsetCheekV			= 128;
	m_FaceData.offsetCheekZ			= 128;
	m_FaceData.scaleCheek			= 128;

	//下巴4
	m_FaceData.offsetChainV			= 128;
	m_FaceData.offsetChainZ			= 128;
	m_FaceData.rotateChain			= 128;
	m_FaceData.scaleChainH			= 128;

	//颌骨6
	m_FaceData.offsetJawH			= 128;
	m_FaceData.offsetJawV			= 128;
	m_FaceData.offsetJawZ			= 128;
	m_FaceData.scaleJawSpecial		= 128;
	m_FaceData.scaleJawH			= 128;
	m_FaceData.scaleJawV			= 128;
	
	//眼睛11 + 7
	m_FaceData.idEyeBaseTex			= idEyeBaseTex[m_nCharacter][m_nGender];
	m_FaceData.idEyeHighTex			= idEyeHighTex[m_nCharacter][m_nGender];
	m_FaceData.idEyeBallTex			= idEyeBallTex[m_nCharacter][m_nGender];
	m_FaceData.idEyeShape			= 0;
	m_FaceData.scaleEyeH			= 128;
	m_FaceData.scaleEyeV			= 128;
	m_FaceData.rotateEye			= 128;
	m_FaceData.offsetEyeH			= 128;
	m_FaceData.offsetEyeV			= 128;
	m_FaceData.offsetEyeZ			= 128;
	m_FaceData.scaleEyeBall			= 128;

	m_FaceData.scaleEyeH2			= 128;
	m_FaceData.scaleEyeV2			= 128;	
	m_FaceData.rotateEye2			= 128;	
	m_FaceData.offsetEyeH2			= 128;
	m_FaceData.offsetEyeV2			= 128;
	m_FaceData.offsetEyeZ2			= 128;
	m_FaceData.scaleEyeBall2		= 128;	

	//眉毛8 + 6
	m_FaceData.idBrowTex			= idBrowTex[m_nCharacter][m_nGender];
	m_FaceData.idBrowShape			= 0;
	m_FaceData.scaleBrowH			= 128;
	m_FaceData.scaleBrowV			= 128;
	m_FaceData.rotateBrow			= 128;
	m_FaceData.offsetBrowH			= 128;
	m_FaceData.offsetBrowV			= 128;
	m_FaceData.offsetBrowZ			= 128;

	m_FaceData.scaleBrowH2			= 128;
	m_FaceData.scaleBrowV2			= 128;
	m_FaceData.rotateBrow2			= 128;
	m_FaceData.offsetBrowH2			= 128;
	m_FaceData.offsetBrowV2			= 128;
	m_FaceData.offsetBrowZ2			= 128;

	//鼻子8
	m_FaceData.idNoseTex			= idNoseTex[m_nCharacter][m_nGender];
	m_FaceData.idNoseTipShape		= 0;
	m_FaceData.scaleNoseTipH		= 128;
	m_FaceData.scaleNoseTipV		= 128;
	m_FaceData.scaleNoseTipZ		= 128;
	m_FaceData.offsetNoseTipV		= 128;
	m_FaceData.idNoseBridgeShape	= 128;
	m_FaceData.scaleBridgeTipH		= 128;

	//嘴12
	m_FaceData.idMouthUpLipLine		= 0;
	m_FaceData.idMouthMidLipLine	= 0;
	m_FaceData.idMouthDownLipLine	= 0;
	m_FaceData.thickUpLip			= 128;
	m_FaceData.thickDownLip			= 128;
	m_FaceData.scaleMouthH			= 128;
	m_FaceData.scaleMouthH2			= 128;
	m_FaceData.offsetMouthV			= 128;
	m_FaceData.offsetMouthZ			= 128;
	m_FaceData.idMouthTex			= idMouthTex[m_nCharacter][m_nGender];

	m_FaceData.offsetCornerOfMouthSpecial = 128;
	m_FaceData.offsetCornerOfMouthSpecial2 = 128;

	//耳朵3
	m_FaceData.idEarShape			= 0;
	m_FaceData.scaleEar				= 128;
	m_FaceData.offsetEarV			= 128;

	//头发1
	m_FaceData.idHairModel			= idHairModel[m_nCharacter][m_nGender];
	m_FaceData.idHairTex			= idHairTex[m_nCharacter][m_nGender];

	//胡子2
	m_FaceData.idMoustacheTex		= idMoustacheTex[m_nCharacter][m_nGender];
	m_FaceData.idMoustacheSkin		= 0;
	
	//各部位颜色7
	m_FaceData.colorHair			= 0xffffffff;
	m_FaceData.colorFace			= 0xffffffff;
	m_FaceData.colorEye				= 0xffffffff;
	m_FaceData.colorBrow			= 0xffffffff;
	m_FaceData.colorMouth			= 0xffffffff;
	m_FaceData.colorEyeBall			= 0xffffffff;
	m_FaceData.colorMoustache		= 0xffffffff;

	// set initial stored face data to empty data too.
	m_FaceDataStored = m_FaceData;
}


// 将当前脸的数据备份起来，以备将来恢复
bool CECFace::StoreFaceData()
{
	m_FaceDataStored = m_FaceData;
	return true;
}

// 恢复备份的脸型数据
bool CECFace::RestoreFaceData()
{
	m_FaceData = m_FaceDataStored;
	UpdateAll();
	return true;
}

//计算眼型
bool CECFace::CalculateEye(void)
{
	if( m_FaceData.idEyeShape <= 0 )
		return true;

	elementdataman* pElementDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE dtFaceData;
	FACE_SHAPE_ESSENCE* pShapeEssence = (FACE_SHAPE_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idEyeShape, ID_SPACE_FACE, dtFaceData);
	if (!pShapeEssence || dtFaceData != DT_FACE_SHAPE_ESSENCE)
		return true;
	char* pszFileName = pShapeEssence->file_shape;
	
	CExpression expEye;
	expEye.LoadExpression(pszFileName);

	//左眼---------------------------------------------------------------------
	
	float fScaleEyeH = TransformScaleFromIntToFloat(m_FaceData.scaleEyeH, m_FaceCusFactor.fScaleEyeHFactor, SCALE_EYEH_FACTOR);
	float fScaleEyeV = TransformScaleFromIntToFloat(m_FaceData.scaleEyeV, m_FaceCusFactor.fScaleEyeVFactor, SCALE_EYEV_FACTOR);
	
	//设置左眼缩放向量
	A3DVECTOR3 vScaleVector(fScaleEyeH , fScaleEyeV, 1.0f);
	
	float fRotateEye = TransformRotateFromIntToFloat(m_FaceData.rotateEye, m_FaceCusFactor.fRotateEyeFactor, ROTATE_EYE_FACTOR);

	//设置左眼旋转向量
	A3DVECTOR3 vRotateVector( 0.0f, 0.0f, fRotateEye);

	float fOffsetEyeH = TransformOffsetFromIntToFloat( m_FaceData.offsetEyeH, m_FaceCusFactor.fOffsetEyeHFactor, OFFSET_EYEH_FACTOR);
	float fOffsetEyeV = TransformOffsetFromIntToFloat( m_FaceData.offsetEyeV, m_FaceCusFactor.fOffsetEyeVFactor, OFFSET_EYEV_FACTOR);
	float fOffsetEyeZ = TransformOffsetFromIntToFloat( m_FaceData.offsetEyeZ, m_FaceCusFactor.fOffsetEyeZFactor, OFFSET_EYEZ_FACTOR);

	//设置左眼平移向量
	A3DVECTOR3 vOffsetVector(  fOffsetEyeH, fOffsetEyeV, fOffsetEyeZ);

	float fScaleEyeBall = TransformScaleFromIntToFloat(m_FaceData.scaleEyeBall, m_FaceCusFactor.fScaleEyeBallFactor, SCALE_EYEBALL_FACTOR);
	//设置左眼的缩放向量
	A3DVECTOR3 vScaleEyeBallVector(fScaleEyeBall, fScaleEyeBall, fScaleEyeBall);

	//右眼-----------------------------------------------------------------------

	float fScaleEyeH2 = TransformScaleFromIntToFloat(m_FaceData.scaleEyeH2, m_FaceCusFactor.fScaleEyeHFactor2, SCALE_EYEH_FACTOR2);
	float fScaleEyeV2 = TransformScaleFromIntToFloat(m_FaceData.scaleEyeV2, m_FaceCusFactor.fScaleEyeVFactor2, SCALE_EYEV_FACTOR2);

	//设置右眼缩放向量
	A3DVECTOR3 vScaleVector2( fScaleEyeH2, fScaleEyeV2, 1.0f);

	float fRotateEye2 = TransformRotateFromIntToFloat(m_FaceData.rotateEye2,m_FaceCusFactor.fRotateEyeFactor2, ROTATE_EYE_FACTOR2);

	//设置右眼旋转向量
	A3DVECTOR3 vRotateVector2( 0.0f, 0.0f, fRotateEye2);

	float fOffsetEyeH2 = TransformOffsetFromIntToFloat( m_FaceData.offsetEyeH2, m_FaceCusFactor.fOffsetEyeHFactor2, OFFSET_EYEH_FACTOR2);
	float fOffsetEyeV2 = TransformOffsetFromIntToFloat( m_FaceData.offsetEyeV2, m_FaceCusFactor.fOffsetEyeVFactor2, OFFSET_EYEV_FACTOR2);
	float fOffsetEyeZ2 = TransformOffsetFromIntToFloat( m_FaceData.offsetEyeZ2, m_FaceCusFactor.fOffsetEyeZFactor2, OFFSET_EYEZ_FACTOR2);
	
	//设置右眼平移向量
	A3DVECTOR3 vOffsetVector2(  -fOffsetEyeH2,fOffsetEyeV2, fOffsetEyeZ2);

	float fScaleEyeBall2 = TransformScaleFromIntToFloat(m_FaceData.scaleEyeBall2, m_FaceCusFactor.fScaleEyeBallFactor2, SCALE_EYEBALL_FACTOR2);

	//设置右眼的缩放向量
	A3DVECTOR3 vScaleEyeBallVector2(fScaleEyeBall2, fScaleEyeBall2, fScaleEyeBall2);

	
	float fWeight = 1.0f;//权值

	//眼型调整
	if(m_pBoneCtrler->GetReady())
	{
		//初始化原始眼睛
		AddShape(m_pBoneCtrler, 1, &expEye, &fWeight,0);

		//左眼---------------------------------------------------------------
		
		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组
		
		m_pBoneCtrler->SelectBoneGroup (7);//7号骨骼组是眼睛
		m_pBoneCtrler->MirrorSelectFromRightToLeft();
		//缩放
		m_pBoneCtrler->ScaleBone (vScaleVector, CT_ALL,true, true);

		//旋转
		m_pBoneCtrler->RotateBone( vRotateVector, CT_ALL,true, true);		
		
		//平移眼型和眼球
		m_pBoneCtrler->TranslateBone ( vOffsetVector, true, true);

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组
		
		m_pBoneCtrler->SelectBoneGroup (8);//8号骨骼组是眼球

		m_pBoneCtrler->MirrorSelectFromRightToLeft();

		m_pBoneCtrler->ResetSelectedBones(true, false);
		
		m_pBoneCtrler->TranslateBone ( vOffsetVector, true, true);
			
		//缩放眼球
		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组

		m_pBoneCtrler->ScaleEyeball(vScaleEyeBallVector, true, CFaceBoneController::ESP_LEFT);

		//右眼---------------------------------------------------------------

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组
		
		m_pBoneCtrler->SelectBoneGroup (7);//7号骨骼组是眼睛

		//缩放
		m_pBoneCtrler->ScaleBone (vScaleVector2, CT_ALL,true, false);

		//旋转
		m_pBoneCtrler->RotateBone( -vRotateVector2, CT_ALL,true, false);		
		
		//平移眼型和眼球
		m_pBoneCtrler->TranslateBone (vOffsetVector2, true, false);

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组
		
		m_pBoneCtrler->SelectBoneGroup (8);//8号骨骼组是眼球

		m_pBoneCtrler->ResetSelectedBones(true, false);
		
		m_pBoneCtrler->TranslateBone ( vOffsetVector2, true, false);
			
		//缩放眼球
		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组

		m_pBoneCtrler->ScaleEyeball(vScaleEyeBallVector2, true, CFaceBoneController::ESP_RIGHT);

		return true;
	}
	return false;
}

//计算眉型
bool CECFace::CalculateBrow(void)
{
	if( m_FaceData.idBrowShape <= 0 )		
		return true;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	DATA_TYPE dtFaceData;
	FACE_SHAPE_ESSENCE* pShapeEssence = (FACE_SHAPE_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idBrowShape, ID_SPACE_FACE, dtFaceData);
	if (!pShapeEssence ||  dtFaceData != DT_FACE_SHAPE_ESSENCE)
		return true;
	char* pszFileName = pShapeEssence->file_shape;
	
	CExpression expBrow;
	expBrow.LoadExpression ( pszFileName);//载入表情

	//左眉--------------------------------------------------------------------------------

	float fScaleBrowH = TransformScaleFromIntToFloat(m_FaceData.scaleBrowH, m_FaceCusFactor.fScaleBrowHFactor, SCALE_BROWH_FACTOR);
	float fScaleBrowV = TransformScaleFromIntToFloat(m_FaceData.scaleBrowV, m_FaceCusFactor.fScaleBrowVFactor, SCALE_BROWV_FACTOR);

	//缩放向量
	A3DVECTOR3 vScaleVectorH (fScaleBrowH, 1.0f, 1.0f);//水平缩放,几何中心
	A3DVECTOR3 vScaleVectorV (1.0f, fScaleBrowV, 1.0f);//竖直缩放,各自中心

	float fRotateBrow = TransformRotateFromIntToFloat(m_FaceData.rotateBrow, m_FaceCusFactor.fRotateBrowFactor,  ROTATE_BROW_FACTOR);
	//旋转向量
	A3DVECTOR3 vRotateVector(0.0f, 0.0f, fRotateBrow);

	float fOffsetBrowH = TransformOffsetFromIntToFloat(m_FaceData.offsetBrowH, m_FaceCusFactor.fOffsetBrowHFactor, OFFSET_BROWH_FACTOR);
	float fOffsetBrowV = TransformOffsetFromIntToFloat(m_FaceData.offsetBrowV, m_FaceCusFactor.fOffsetBrowVFactor, OFFSET_BROWV_FACTOR);
	float fOffsetBrowZ = TransformOffsetFromIntToFloat(m_FaceData.offsetBrowZ, m_FaceCusFactor.fOffsetBrowZFactor, OFFSET_BROWZ_FACTOR);
	//设置平移向量
	A3DVECTOR3 vOffsetVector( fOffsetBrowH, fOffsetBrowV, fOffsetBrowZ);

	//右眉------------------------------------------------------------------------------------

	float fScaleBrowH2 = TransformScaleFromIntToFloat(m_FaceData.scaleBrowH2, m_FaceCusFactor.fScaleBrowHFactor2, SCALE_BROWH_FACTOR2);
	float fScaleBrowV2 = TransformScaleFromIntToFloat(m_FaceData.scaleBrowV2, m_FaceCusFactor.fScaleBrowVFactor2, SCALE_BROWV_FACTOR2);

	//缩放向量
	A3DVECTOR3 vScaleVectorH2 (fScaleBrowH2, 1.0f, 1.0f);//水平缩放,几何中心
	A3DVECTOR3 vScaleVectorV2 (1.0f, fScaleBrowV2, 1.0f);//竖直缩放,各自中心

	float fRotateBrow2 = TransformRotateFromIntToFloat(m_FaceData.rotateBrow2, m_FaceCusFactor.fRotateBrowFactor2, ROTATE_BROW_FACTOR2);
	//旋转向量
	A3DVECTOR3 vRotateVector2(0.0f, 0.0f, fRotateBrow2);
	
	float fOffsetBrowH2 = TransformOffsetFromIntToFloat(m_FaceData.offsetBrowH2, m_FaceCusFactor.fOffsetBrowHFactor2, OFFSET_BROWH_FACTOR2);
	float fOffsetBrowV2 = TransformOffsetFromIntToFloat(m_FaceData.offsetBrowV2, m_FaceCusFactor.fOffsetBrowVFactor2, OFFSET_BROWV_FACTOR2);
	float fOffsetBrowZ2 = TransformOffsetFromIntToFloat(m_FaceData.offsetBrowZ2, m_FaceCusFactor.fOffsetBrowZFactor2, OFFSET_BROWZ_FACTOR2);
	//设置平移向量
	A3DVECTOR3 vOffsetVector2( -fOffsetBrowH2, fOffsetBrowV2, fOffsetBrowZ2);


	float fWeight = 1.0f;//权值

	//眉型调整
	if(m_pBoneCtrler->GetReady())
	{
		//初始化原始眉型
		AddShape(m_pBoneCtrler, 1, &expBrow, &fWeight, 0);
	
		//左眉---------------------------------------------------

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组

		m_pBoneCtrler->SelectBoneGroup (9);//9为眉型

		m_pBoneCtrler->MirrorSelectFromRightToLeft();

		//水平缩放
		m_pBoneCtrler->ScaleBone (vScaleVectorH, CT_ALL,true, true);

		//竖直缩放
		m_pBoneCtrler->ScaleBone (vScaleVectorV, CT_SELF, true, true);

		//旋转
		m_pBoneCtrler->RotateBone (vRotateVector, CT_ALL, true, true);

		//平移
		m_pBoneCtrler->TranslateBone ( vOffsetVector, true, true);

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组

		//右眉-------------------------------------------------------

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组

		m_pBoneCtrler->SelectBoneGroup (9);//9为眉型
		
		//水平缩放
		m_pBoneCtrler->ScaleBone (vScaleVectorH2, CT_ALL,true, false);

		//竖直缩放
		m_pBoneCtrler->ScaleBone (vScaleVectorV2, CT_SELF, true, false);

		//旋转
		m_pBoneCtrler->RotateBone (-vRotateVector2, CT_ALL, true, false);

		//平移
		m_pBoneCtrler->TranslateBone ( vOffsetVector2, true, false);

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组


		return true;
	}

	return false;
}

//计算鼻型
bool CECFace::CalculateNose(void)
{
	if( m_FaceData.idNoseTipShape <= 0 || m_FaceData.idNoseBridgeShape <= 0 )
		return true;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	DATA_TYPE dtFaceData;

	//获取当前鼻型
	FACE_SHAPE_ESSENCE* pShapeEssence;
	pShapeEssence = (FACE_SHAPE_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idNoseTipShape, ID_SPACE_FACE, dtFaceData);
	if (!pShapeEssence ||  dtFaceData != DT_FACE_SHAPE_ESSENCE)
		return true;
	char* pszFileName1 = pShapeEssence->file_shape;

	pShapeEssence = (FACE_SHAPE_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idNoseBridgeShape, ID_SPACE_FACE, dtFaceData);
	if (!pShapeEssence ||  dtFaceData != DT_FACE_SHAPE_ESSENCE)
		return true;
	char* pszFileName2 = pShapeEssence->file_shape;

	CExpression expNoseTip;//鼻头表情
	CExpression expNoseBridge;//鼻梁表情
	expNoseTip.LoadExpression ( pszFileName1);
	expNoseBridge.LoadExpression(pszFileName2);

	//-------------------------------------------------------------------------
	//鼻头
	float fScaleNoseTipH = TransformScaleFromIntToFloat(m_FaceData.scaleNoseTipH, m_FaceCusFactor.fScaleNoseTipHFactor, SCALE_NOSETIPH_FACTOR);
	
	//鼻头水平缩放向量
	A3DVECTOR3 vNoseTipScaleHVector(fScaleNoseTipH, 1.0f, 1.0f);

	float fScaleNoseTipV = TransformScaleFromIntToFloat(m_FaceData.scaleNoseTipV, m_FaceCusFactor.fScaleNoseTipVFactor, SCALE_NOSETIPV_FACTOR);

	//鼻头垂直缩放向量
	A3DVECTOR3 vNoseTipScaleVVector(fScaleNoseTipV, fScaleNoseTipV, fScaleNoseTipV);
	
	float fScaleNoseTipZ = TransformScaleFromIntToFloat(m_FaceData.scaleNoseTipZ,m_FaceCusFactor.fScaleNoseTipZFactor, SCALE_NOSETIPZ_FACTOR);
	//鼻头前后缩放向量
	A3DVECTOR3 vNoseTipScaleZVector(1.0f, 1.0f, fScaleNoseTipZ);

	float fOffsetNoseTipV = TransformOffsetFromIntToFloat(m_FaceData.offsetNoseTipV,m_FaceCusFactor.fOffsetNoseTipVFactor, OFFSET_NOSETIPV_FACTOR);

	//鼻头垂直位移向量
	A3DVECTOR3 vNoseTipOffsetVVector(0.0f, fOffsetNoseTipV, 0.0f);

	float fScaleBridgeTipH = TransformScaleFromIntToFloat(m_FaceData.scaleBridgeTipH, m_FaceCusFactor.fScaleBridgeTipHFactor, SCALE_BRIDGETIPH_FACTOR);
	//鼻梁垂直比例向量
	A3DVECTOR3 vBridgeTipScaleHVector (fScaleBridgeTipH, 1.0f, 1.0f);

	//鼻梁前后位移向量
	float fOffsetBridgeTipZ = TransformOffsetFromIntToFloat(m_FaceData.offsetBridgeTipZ,m_FaceCusFactor.fOffsetBridgeTipZFactor, OFFSET_BRIDGETIPZ_FACTOR);

	float fWeight = 1.0f;//权值
	
	if(m_pBoneCtrler->GetReady())
	{
		AddShape(m_pBoneCtrler, 1, &expNoseTip, &fWeight, 0);
		AddShape(m_pBoneCtrler, 1, &expNoseBridge, &fWeight, 0);

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组

		m_pBoneCtrler->SelectBoneGroup (11);//11为鼻头

		m_pBoneCtrler->SetSpecialCenter (53);//指定鼻头水平缩放的缩放中心为"鼻头40号"骨骼
		
		//水平缩放鼻头
		m_pBoneCtrler->ScaleBone (vNoseTipScaleHVector, CT_SPECIAL, true, true);

		//垂直缩放鼻头
		m_pBoneCtrler->ScaleBone (vNoseTipScaleVVector, CT_SELF, true, true);

		m_pBoneCtrler->SetSpecialCenter (56);//指定鼻头前后缩放的缩放中心为"鼻头70号"骨骼

		//前后缩放鼻头
		m_pBoneCtrler->ScaleBone( vNoseTipScaleZVector, CT_SPECIAL, true, true);

		//鼻头的垂直位移
		m_pBoneCtrler->TranslateBone ( vNoseTipOffsetVVector, true, true);
		
		m_pBoneCtrler->ClearSelection ();
		
		//设置鼻梁骨骼组
		m_pBoneCtrler->SelectBoneGroup (10);//10为鼻梁
		
		m_pBoneCtrler->SetSpecialCenter (50);//指定鼻梁水平缩放的缩放中心为"鼻型010号"骨骼

		//设置鼻梁骨骼的缩放
		m_pBoneCtrler->ScaleBone(vBridgeTipScaleHVector, CT_SPECIAL, true, true);

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组

		//设置鼻梁骨胳前后位移
		
		m_pBoneCtrler->MoveNoseBridge(fOffsetBridgeTipZ, true, true);

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组

		return true;
	}

	return false;
}

//计算嘴型
bool CECFace::CalculateMouth(void)
{
	if( m_FaceData.idMouthUpLipLine <= 0 ||
		m_FaceData.idMouthMidLipLine <= 0 ||
		m_FaceData.idMouthDownLipLine <= 0 )
		return true;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	DATA_TYPE dtFaceData;

	//获取当前嘴型
	FACE_SHAPE_ESSENCE* pShapeEssence;
	pShapeEssence = (FACE_SHAPE_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idMouthUpLipLine, ID_SPACE_FACE, dtFaceData);
	if (!pShapeEssence ||  dtFaceData != DT_FACE_SHAPE_ESSENCE)
		return true;
	char* pszFileName1 = pShapeEssence->file_shape;

	pShapeEssence = (FACE_SHAPE_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idMouthMidLipLine, ID_SPACE_FACE, dtFaceData);
	if (!pShapeEssence ||  dtFaceData != DT_FACE_SHAPE_ESSENCE)
		return true;
	char* pszFileName2 = pShapeEssence->file_shape;

	pShapeEssence = (FACE_SHAPE_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idMouthDownLipLine, ID_SPACE_FACE, dtFaceData);
	if (!pShapeEssence ||  dtFaceData != DT_FACE_SHAPE_ESSENCE)
		return true;
	char* pszFileName3 = pShapeEssence->file_shape;

	CExpression expMouthUpLipLine;
	CExpression expMouthMidLipLine;
	CExpression expMouthDownLipLine;

	expMouthUpLipLine.LoadExpression(pszFileName1);
	expMouthMidLipLine.LoadExpression(pszFileName2);
	expMouthDownLipLine.LoadExpression (pszFileName3);

	//------------------------------------------------------------------
	//嘴
	
	float fThickUpLip = TransformScaleFromIntToFloat( m_FaceData.thickUpLip, m_FaceCusFactor.fThickUpLipFactor, THICK_UPLIP_FACTOR);

	//上嘴唇厚度缩放向量
	A3DVECTOR3 vThickUpLipVector(1.0f, fThickUpLip, 1.0f);

	float fThickDownLip = TransformScaleFromIntToFloat(m_FaceData.thickDownLip, m_FaceCusFactor.fThickDownLipFactor, THICK_DOWNLIP_FACTOR);

	//下嘴唇厚度缩放向量
	A3DVECTOR3 vThickDownLipVector(1.0f, fThickDownLip, 1.0f);

	float fScaleMouthH = TransformScaleFromIntToFloat(m_FaceData.scaleMouthH, m_FaceCusFactor.fScaleMouthHFactor, SCALE_MOUTHH_FACTOR);
	float fScaleMouthH2 = TransformScaleFromIntToFloat(m_FaceData.scaleMouthH2,m_FaceCusFactor.fScaleMouthHFactor2, SCALE_MOUTHH_FACTOR2);

	//嘴部整体水平比例向量
	A3DVECTOR3 vScaleMouthHVector(fScaleMouthH, 1.0f, 1.0f);
	A3DVECTOR3 vScaleMouthHVector2(fScaleMouthH2, 1.0f, 1.0f);

	float fOffsetMouthV = TransformOffsetFromIntToFloat(m_FaceData.offsetMouthV, m_FaceCusFactor.fOffsetMouthVFactor, OFFSET_MOUTHV_FACTOR);
	float fOffsetMouthZ = TransformOffsetFromIntToFloat(m_FaceData.offsetMouthZ, m_FaceCusFactor.fOffsetMouthZFactor, OFFSET_MOUTHZ_FACTOR);

	//嘴部位移向量
	A3DVECTOR3 vOffsetMouthVector(0.0f, fOffsetMouthV, fOffsetMouthZ );

	float fOffsetCornorOfMouthSpecila = TransformOffsetFromIntToFloat(m_FaceData.offsetCornerOfMouthSpecial, m_FaceCusFactor.fOffsetCornerOfMouthSpecialFactor,  OFFSET_CORNEROFMOUTHSPECIAL_FACTOR);
	float fOffsetCornorOfMouthSpecila2 = TransformOffsetFromIntToFloat(m_FaceData.offsetCornerOfMouthSpecial2, m_FaceCusFactor.fOffsetCornerOfMouthSpecialFactor2, OFFSET_CORNEROFMOUTHSPECIAL_FACTOR2);

	float fWeight = 1.0f;//权值
	
	if(m_pBoneCtrler->GetReady())
	{
	
		//初始化上嘴唇
		AddShape(m_pBoneCtrler, 1, &expMouthUpLipLine, &fWeight, 0);
		//初始化唇线
		AddShape(m_pBoneCtrler, 1, &expMouthMidLipLine, &fWeight, 0);
		//初始化下嘴唇
		AddShape(m_pBoneCtrler, 1, &expMouthDownLipLine, &fWeight, 0);

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组

		m_pBoneCtrler->SelectBoneGroup (12);//12为上嘴唇
		m_pBoneCtrler->SetSpecialCenter (32);//指定上嘴唇厚度缩放的缩放中心为"嘴型051号"骨骼
		m_pBoneCtrler->ScaleBone (vThickUpLipVector, CT_SPECIAL, true, true);

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组
		m_pBoneCtrler->SelectBoneGroup (13);//13为上嘴唇
		m_pBoneCtrler->SetSpecialCenter (32);//指定下嘴唇厚度缩放的缩放中心为"嘴型051号"骨骼

		m_pBoneCtrler->ScaleBone (vThickDownLipVector, CT_SPECIAL, true, true);

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组
		m_pBoneCtrler->SelectBoneGroup (12);//12为上嘴唇
		m_pBoneCtrler->SelectBoneGroup (13);//13为下嘴唇
		m_pBoneCtrler->SelectBoneGroup (14);//14为上唇沟
		m_pBoneCtrler->SelectBoneGroup (15);//15为下唇沟
		m_pBoneCtrler->SetSpecialCenter (57);//指定嘴唇水平缩放的缩放中心为"嘴型010号"骨骼

		m_pBoneCtrler->MirrorSelectFromRightToLeft();
		m_pBoneCtrler->ScaleBone (vScaleMouthHVector, CT_SPECIAL, true, true);

		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组
		m_pBoneCtrler->SelectBoneGroup (12);//12为上嘴唇
		m_pBoneCtrler->SelectBoneGroup (13);//13为下嘴唇
		m_pBoneCtrler->SelectBoneGroup (14);//14为上唇沟
		m_pBoneCtrler->SelectBoneGroup (15);//15为下唇沟
		m_pBoneCtrler->SetSpecialCenter (57);//指定嘴唇水平缩放的缩放中心为"嘴型010号"骨骼
		m_pBoneCtrler->ScaleBone (vScaleMouthHVector2, CT_SPECIAL, true, false);
		
//		if( m_FaceData.offsetMouthZ < 0)
//		{
			m_pBoneCtrler->SelectBoneGroup (16);//16号骨骼为牙齿
			m_pBoneCtrler->AddSelection(62);
//		}

		m_pBoneCtrler->TranslateBone (vOffsetMouthVector, true, true);
		
		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组

		//嘴角的特殊移动
		m_pBoneCtrler->MoveMouthCorner(fOffsetCornorOfMouthSpecila, true, CFaceBoneController::ESP_LEFT );
		m_pBoneCtrler->MoveMouthCorner(fOffsetCornorOfMouthSpecila2, true, CFaceBoneController::ESP_RIGHT );
		
		m_pBoneCtrler->ClearSelection();

		return true;
	}	
	return false;
}
//计算耳型
bool CECFace::CalculateEar(void)
{
	if( m_FaceData.idEarShape <= 0 )
		return true;
	
	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	DATA_TYPE dtFaceData;
	FACE_SHAPE_ESSENCE* pShapeEssence = (FACE_SHAPE_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idEarShape, ID_SPACE_FACE, dtFaceData);
	if (!pShapeEssence ||  dtFaceData != DT_FACE_SHAPE_ESSENCE)
		return true;
	char* pszFileName1 = pShapeEssence->file_shape;

	CExpression expEar;
	expEar.LoadExpression (pszFileName1);//载入表情

	//-------------------------------------------------------------------------
	//耳朵
	
	float fScaleEar = TransformScaleFromIntToFloat(m_FaceData.scaleEar, m_FaceCusFactor.fScaleEarFactor, SCALE_EAR_FACTOR);

	//耳型缩放向量
	A3DVECTOR3 vScaleEarVector(fScaleEar, fScaleEar, 1.0f);
	
	float fOffsetEarV = TransformOffsetFromIntToFloat(m_FaceData.offsetEarV, m_FaceCusFactor.fOffsetEarVFactor, OFFSET_EARV_FACTOR);

	//耳型位移向量
	A3DVECTOR3 vOffsetEarV(0.0f, fOffsetEarV, 0.0f);

	float fWeight = 1.0f;//权值
	if(m_pBoneCtrler->GetReady ())
	{
		//初始化基本耳型
		AddShape(m_pBoneCtrler, 1, &expEar, &fWeight, 0);

		m_pBoneCtrler->ClearSelection ();

		m_pBoneCtrler->SelectBoneGroup (18);//18为耳型
		m_pBoneCtrler->SetSpecialCenter(4);//脸盘061
		m_pBoneCtrler->ScaleBone ( vScaleEarVector, CT_SPECIAL,true, true);

		m_pBoneCtrler->ClearSelection ();
		m_pBoneCtrler->SelectBoneGroup (18);//18为耳型
		m_pBoneCtrler->TranslateBone ( vOffsetEarV,true, true);

		m_pBoneCtrler->ClearSelection ();

		return true;
	}
	return false;
}

//计算脸盘
bool CECFace::CalculateFace(void)
{
	//---------------------------------------------------------
	//脸盘
	float fScaleFaceH = TransformScaleFromIntToFloat(m_FaceData.scaleFaceH, m_FaceCusFactor.fScaleFaceHFactor, SCALE_FACEH_FACTOR);
	float fScaleFaceV = TransformScaleFromIntToFloat(m_FaceData.scaleFaceV, m_FaceCusFactor.fScaleFaceVFactor, SCALE_FACEV_FACTOR);

	//脸盘缩放比例向量
	A3DVECTOR3 vScaleFaceVector( fScaleFaceH, fScaleFaceV, 1.0f);


	if(m_pBoneCtrler->GetReady ())
	{

		m_pBoneCtrler->ClearSelection();

		m_pBoneCtrler->SelectBoneGroup (1);//1号骨骼组为脸盘
		
		m_pBoneCtrler->SetSpecialCenter(50);//50号骨头为脸盘缩放中心

		m_pBoneCtrler->ScaleBone (vScaleFaceVector, CT_SPECIAL,true, true);//缩放
		
		m_pBoneCtrler->ClearSelection ();
		
		return true;
	}
	return false;
}

//计算3庭
bool CECFace::Calculate3Parts(void)
{
	float fScaleUp = TransformScaleFromIntToFloat(m_FaceData.scaleUp, m_FaceCusFactor.fScaleUpPartFactor, SCALE_UPPART_FACTOR);
	float fScaleMiddle = TransformScaleFromIntToFloat(m_FaceData.scaleMiddle, m_FaceCusFactor.fScaleMiddlePartFactor, SCALE_MIDDLEPART_FACTOR);
	float fScaleDown = TransformScaleFromIntToFloat(m_FaceData.scaleDown, m_FaceCusFactor.fScaleDownPartFactor, SCALE_DOWNPART_FACTOR);

	if(m_pBoneCtrler->GetReady ())
	{
		m_pBoneCtrler->ScaleParts(fScaleUp, fScaleMiddle ,fScaleDown);
		
		return true;
	}
	return false;
}

//计算额头
bool CECFace::CalculateForehead(void)
{
	
	float fOffsetForeheadH = TransformOffsetFromIntToFloat(m_FaceData.offsetForeheadH, m_FaceCusFactor.fOffsetForeheadHFactor, OFFSET_FOREHEADH_FACTOR);
	float fOffsetForeheadV = TransformOffsetFromIntToFloat(m_FaceData.offsetForeheadV, m_FaceCusFactor.fOffsetForeheadVFactor, OFFSET_FOREHEADV_FACTOR);
	float fOffsetForeheadZ = TransformOffsetFromIntToFloat(m_FaceData.offsetForeheadZ, m_FaceCusFactor.fOffsetForeheadZFactor, OFFSET_FOREHEADZ_FACTOR);

	//平移向量
	A3DVECTOR3 vOffsetForehead(fOffsetForeheadH , fOffsetForeheadV, fOffsetForeheadZ);

	float fRotateForehead = TransformRotateFromIntToFloat(m_FaceData.rotateForehead, m_FaceCusFactor.fRotateForeheadFactor, ROTATE_FOREHEAD_FACTOR);
	//旋转向量
	A3DVECTOR3 vRotateForeheadVector( fRotateForehead, 0.0f, 0.0f);

	float fScaleForehead = TransformScaleFromIntToFloat(m_FaceData.scaleForehead, m_FaceCusFactor.fScaleForeheadFactor, SCALE_FOREHEAD_FACTOR);
	//缩放向量
	A3DVECTOR3 vScaleForeheadVector( fScaleForehead, fScaleForehead, fScaleForehead);

	//开始调节
	if( m_pBoneCtrler->GetReady())
	{
		m_pBoneCtrler->ClearSelection();//清除选中的骨骼组
		m_pBoneCtrler->SelectBoneGroup ( 3); //3为额头
		
		m_pBoneCtrler->TranslateBone ( vOffsetForehead,true, true);//平移

		m_pBoneCtrler->RotateBone ( vRotateForeheadVector, CT_ALL,true, true);//旋转

		m_pBoneCtrler->ScaleBone ( vScaleForeheadVector, CT_SELF,true, true);//比例缩放

		m_pBoneCtrler->ClearSelection ();

		return true;
	}
	return false;
}

//计算颧骨
bool CECFace::CalculateYokeBone(void)
{

	float fOffsetYokeBoneH = TransformOffsetFromIntToFloat(m_FaceData.offsetYokeBoneH, m_FaceCusFactor.fOffsetYokeBoneHFactor, OFFSET_YOKEBONEH_FACTOR);
	float fOffsetYokeBoneV = TransformOffsetFromIntToFloat(m_FaceData.offsetYokeBoneV, m_FaceCusFactor.fOffsetYokeBoneVFactor, OFFSET_YOKEBONEV_FACTOR);
	float fOffsetYokeBoneZ = TransformOffsetFromIntToFloat(m_FaceData.offsetYokeBoneZ, m_FaceCusFactor.fOffsetYokeBoneZFactor, OFFSET_YOKEBONEZ_FACTOR);

	//颧骨平移向量
	A3DVECTOR3 vOffsetYokeBoneVector( fOffsetYokeBoneH, fOffsetYokeBoneV, fOffsetYokeBoneZ);

	float fRotateYokeBone = TransformRotateFromIntToFloat(m_FaceData.rotateYokeBone, m_FaceCusFactor.fRotateYokeBoneFactor, ROTATE_YOKEBONE_FACTOR);
	//颧骨的旋转向量
	A3DVECTOR3 vRotateYokeBone( 0.0f, 0.0f, fRotateYokeBone);
	
	float fScaleYokeBone = TransformScaleFromIntToFloat(m_FaceData.scaleYokeBone,m_FaceCusFactor.fScaleYokeBoneFactor, SCALE_YOKEBONE_FACTOR);

	//颧骨的缩放向量
	A3DVECTOR3 vScaleYokeBone( fScaleYokeBone, fScaleYokeBone, fScaleYokeBone);
	
 	if( m_pBoneCtrler->GetReady ())
	{
		m_pBoneCtrler->ClearSelection ();

		m_pBoneCtrler->SelectBoneGroup ( 4);//4号骨骼组为颧骨

		m_pBoneCtrler->TranslateBone (vOffsetYokeBoneVector,true, true);//移动

		m_pBoneCtrler->RotateBone ( vRotateYokeBone, CT_ALL,true, true);

		m_pBoneCtrler->ScaleBone( vScaleYokeBone, CT_SELF,true, true);

		m_pBoneCtrler->ClearSelection ();

		return true;
	}

	return false;
}

//计算脸颊
bool CECFace::CalculateCheek(void)
{

	float fOffsetCheekH = TransformOffsetFromIntToFloat(m_FaceData.offsetCheekH, m_FaceCusFactor.fOffsetCheekHFactor, OFFSET_CHEEKH_FACTOR);
	float fOffsetCheekV = TransformOffsetFromIntToFloat(m_FaceData.offsetCheekV, m_FaceCusFactor.fOffsetCheekVFactor, OFFSET_CHEEKV_FACTOR);
	float fOffsetCheekZ = TransformOffsetFromIntToFloat(m_FaceData.offsetCheekZ, m_FaceCusFactor.fOffsetCheekZFactor, OFFSET_CHEEKZ_FACTOR);

	//脸颊平移向量
	A3DVECTOR3 vOffsetCheekVector( fOffsetCheekH, fOffsetCheekV, fOffsetCheekZ);

	float fScaleCheek = TransformScaleFromIntToFloat(m_FaceData.scaleCheek, m_FaceCusFactor.fScaleCheekFactor, SCALE_CHEEK_FACTOR);

	//脸颊缩放向量
	A3DVECTOR3 vScaleCheekVector( fScaleCheek , fScaleCheek , fScaleCheek );

	if( m_pBoneCtrler->GetReady ())
	{
		m_pBoneCtrler->ClearSelection ();
		m_pBoneCtrler->SelectBoneGroup ( 5);//5号骨骼组为脸颊

		m_pBoneCtrler->TranslateBone ( vOffsetCheekVector,true, true);//平移

		m_pBoneCtrler->ScaleBone ( vScaleCheekVector, CT_SELF,true, true);//缩放
		
		m_pBoneCtrler->ClearSelection ();

		return true;
	}

	return false;
}


//计算下巴
bool CECFace::CalculateChain(void)
{
	float fOffsetChainV = TransformOffsetFromIntToFloat(m_FaceData.offsetChainV, m_FaceCusFactor.fOffsetChainVFactor, OFFSET_CHAINV_FACTOR);
	float fOffsetChainZ = TransformOffsetFromIntToFloat(m_FaceData.offsetChainZ, m_FaceCusFactor.fOffsetChainZFactor, OFFSET_CHAINZ_FACTOR);

	//下巴平移向量
	A3DVECTOR3 vOffsetChainVector(0.0f, fOffsetChainV, fOffsetChainZ);
	
	float fRotateChain = TransformRotateFromIntToFloat(m_FaceData.rotateChain, m_FaceCusFactor.fRotateChainFactor, ROTATE_CHAIN_FACTOR);

	//下巴的旋转向量
	A3DVECTOR3 vRotateChainVector(fRotateChain, 0.0f, 0.0f);

	float fScaleChainH = TransformScaleFromIntToFloat(m_FaceData.scaleChainH,m_FaceCusFactor.fScaleChainHFactor, SCALE_CHAINH_FACTOR);

	//下巴的缩放向量
	A3DVECTOR3 vScaleChainVector(fScaleChainH, 1.0f , 1.0f);

	if( m_pBoneCtrler->GetReady())
	{
		m_pBoneCtrler->ClearSelection ();

		m_pBoneCtrler->SelectBoneGroup ( 6);//6号骨骼组为下巴

		m_pBoneCtrler->TranslateBone (vOffsetChainVector,true, true);//平移
		m_pBoneCtrler->RotateBone ( vRotateChainVector, CT_ALL,true, true);//旋转

		m_pBoneCtrler->SetSpecialCenter ( 48);//48号骨骼
		
		m_pBoneCtrler->ScaleBone ( vScaleChainVector, CT_SPECIAL,true, true);//缩放

		m_pBoneCtrler->ClearSelection ();

		return true;
	}
	
	return false;
}
//计算颌骨
bool CECFace::CalculateJaw(void)
{

	float fOffsetJawH = TransformOffsetFromIntToFloat(m_FaceData.offsetJawH, m_FaceCusFactor.fOffsetJawHFactor, OFFSET_JAWH_FACTOR);
	float fOffsetJawV = TransformOffsetFromIntToFloat(m_FaceData.offsetJawV, m_FaceCusFactor.fOffsetJawVFactor, OFFSET_JAWV_FACTOR);
	float fOffsetJawZ = TransformOffsetFromIntToFloat(m_FaceData.offsetJawZ, m_FaceCusFactor.fOffsetJawZFactor, OFFSET_JAWZ_FACTOR);

	A3DVECTOR3 vOffsetJawVector(fOffsetJawH, fOffsetJawV, fOffsetJawZ);//颌骨的移动向量

	float fScaleJawH = TransformScaleFromIntToFloat(m_FaceData.scaleJawH, m_FaceCusFactor.fScaleJawHFactor, SCALE_JAWH_FACTOR);
	float fScaleJawV = TransformScaleFromIntToFloat(m_FaceData.scaleJawV, m_FaceCusFactor.fScaleJawVFactor, SCALE_JAWV_FACTOR);

	A3DVECTOR3 vScaleJawVector(fScaleJawH, fScaleJawV, 1.0f);//颌骨比例缩放向量

	float fScaleJawSpecial = TransformOffsetFromIntToFloat(m_FaceData.scaleJawSpecial, m_FaceCusFactor.fScaleJawSpecialFactor, SCALE_JAWSPECIAL_FACTOR);

	const int CHEEK_BONE_INDEX1		= 5;				// Cheek bone index 1
	const int CHEEK_BONE_INDEX2		= 9;				// Cheek bone index 2

	if( m_pBoneCtrler->GetReady ())
	{
		m_pBoneCtrler->ClearSelection ();

		m_pBoneCtrler->SelectBoneGroup ( 2);//2号骨骼组为颌骨

		m_pBoneCtrler->TranslateBone ( vOffsetJawVector,true, true);//移动

		m_pBoneCtrler->ScaleBone(vScaleJawVector, CT_ALL,true, true);//缩放
		
		m_pBoneCtrler->ClearSelection ();

		//选择颌骨特殊缩放的骨骼(比正常情况的颌骨少一个骨骼)
		for (int i=CHEEK_BONE_INDEX1+1; i<CHEEK_BONE_INDEX2; i++)
		{
			m_pBoneCtrler->AddSelection(i);
		}
		
		m_pBoneCtrler->MoveCheek(fScaleJawSpecial,true, true);//特殊缩放

		m_pBoneCtrler->ClearSelection ();

		return true;
	}
	
	return false;
}

//计算脸型融合
bool CECFace::CalculateBlendFace(void)
{
	if( m_FaceData.idFaceShape1 <= 0 || m_FaceData.idFaceShape2 <= 0 )
		return false;

	elementdataman* pElementDataMan = g_pGame->GetElementDataMan();
	DATA_TYPE dtFaceData;
	FACE_SHAPE_ESSENCE* pShapeEssence = (FACE_SHAPE_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idFaceShape1, ID_SPACE_FACE, dtFaceData);
	if (!pShapeEssence ||  dtFaceData != DT_FACE_SHAPE_ESSENCE)
		return true;
	char* pszFileName1 = pShapeEssence->file_shape;

	pShapeEssence = (FACE_SHAPE_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idFaceShape2, ID_SPACE_FACE, dtFaceData);
	if (!pShapeEssence ||  dtFaceData != DT_FACE_SHAPE_ESSENCE)
		return true;
	char* pszFileName2 = pShapeEssence->file_shape;
	
	CExpression expCombineFaces[2];
	expCombineFaces[0].LoadExpression(pszFileName1);
	expCombineFaces[1].LoadExpression(pszFileName2);


	//融合脸型
	float fFaceWeight[2];//脸型权值
	float fBlendFaceShape = TransformScaleFromIntToFloat(m_FaceData.blendFaceShape,1.0f, 1.0f);

	fFaceWeight[0] = 1.0f - fBlendFaceShape;
	fFaceWeight[1] = fBlendFaceShape;

	
	if( m_pBoneCtrler->GetReady())
	{
		//合并脸型
		AddShape(m_pBoneCtrler, 2, expCombineFaces, fFaceWeight, 0);
		return true;
	}

	return false;
}

//更新脸部纹理
bool CECFace::UpdateEyeTexture(void)
{	
	if( m_FaceData.idEyeBaseTex <= 0 ||
		m_FaceData.idEyeHighTex <= 0 )
		return false;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;
	
	FACE_TEXTURE_ESSENCE* pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(m_FaceData.idEyeBaseTex, ID_SPACE_FACE,dtFaceData); 
	if (!pFaceTextureEssence ||  dtFaceData != DT_FACE_TEXTURE_ESSENCE)
		return true;
	char* pszBaseTexFile = pFaceTextureEssence->file_base_tex;//眼睛纹理

	pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(m_FaceData.idEyeHighTex, ID_SPACE_FACE,dtFaceData); 
	if (!pFaceTextureEssence ||  dtFaceData != DT_FACE_TEXTURE_ESSENCE)
		return true;
	char* pszHighTexFile = pFaceTextureEssence->file_high_tex; //眼影文理

	ChangeEyeTexture ( pszBaseTexFile, pszHighTexFile);//改变眼睛纹理

	return true;
}

//更新眉毛纹理
bool CECFace::UpdateBrowTexture(void)
{
	if( m_FaceData.idBrowTex <= 0 )
		return false;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;
	
	FACE_TEXTURE_ESSENCE* pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(m_FaceData.idBrowTex, ID_SPACE_FACE,dtFaceData); 
	if (!pFaceTextureEssence ||  dtFaceData != DT_FACE_TEXTURE_ESSENCE)
		return true;

	char* pszBaseTexFile = pFaceTextureEssence->file_base_tex;
	char* pszHighTexFile = pFaceTextureEssence->file_high_tex;

	ChangeBrowTexture(pszBaseTexFile, pszHighTexFile);//改变眉毛纹理

	return true;
}

//更新鼻子纹理
bool CECFace::UpdateNoseTexture(void)
{
	if( m_FaceData.idNoseTex <= 0 )
		return false;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;
	
	FACE_TEXTURE_ESSENCE* pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(m_FaceData.idNoseTex, ID_SPACE_FACE,dtFaceData); 
	if (!pFaceTextureEssence ||  dtFaceData != DT_FACE_TEXTURE_ESSENCE)
		return true;
	char* pszBaseTexFile = pFaceTextureEssence->file_base_tex;
	ChangeNoseTexture ( pszBaseTexFile);
	return true;
}

//更新小胡子纹理
bool CECFace::UpdateMoustacheTexture(void)
{
	if( m_FaceData.idMoustacheTex <= 0 )
		return false;

	//女
	if( m_nGender == GENDER_FEMALE)
		return true;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;
	
	FACE_TEXTURE_ESSENCE* pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(m_FaceData.idMoustacheTex, ID_SPACE_FACE,dtFaceData); 
	if (!pFaceTextureEssence ||  dtFaceData != DT_FACE_TEXTURE_ESSENCE)
		return true;
	
	char* pszBaseTexFile = pFaceTextureEssence->file_base_tex;
	char* pszHighTexFile = pFaceTextureEssence->file_high_tex;

	//改变小胡子纹理
	ChangeMoustacheTexture( pszBaseTexFile, pszHighTexFile);

	return true;
}

//更新脸纹理
bool CECFace::UpdateFaceTexture(void)
{
	if( m_FaceData.idFaceTex <= 0 )
		return false;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;
	
	FACE_TEXTURE_ESSENCE* pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(m_FaceData.idFaceTex, ID_SPACE_FACE,dtFaceData); 
	if (!pFaceTextureEssence ||  dtFaceData != DT_FACE_TEXTURE_ESSENCE)
		return true;
	char* pszBaseTexFile = pFaceTextureEssence->file_base_tex;
	char* pszHighTexFile = pFaceTextureEssence->file_high_tex;
	//改变脸纹理
	ChangeFaceTexture( pszBaseTexFile );

	return true;
}

//更新附眼
bool CECFace::UpdateThirdEye(void)
{
	if( m_FaceData.idThirdEye <= 0 )
		return false;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;
	
	FACE_THIRDEYE_ESSENCE* pFaceThirdEye = (FACE_THIRDEYE_ESSENCE*)pElementDataMan->get_data_ptr(m_FaceData.idThirdEye, ID_SPACE_FACE,dtFaceData); 
	if (!pFaceThirdEye ||  dtFaceData != DT_FACE_THIRDEYE_ESSENCE)
		return true;

	const char *pszGfxFile = pFaceThirdEye->file_gfx;

	ChangeThirdEyeGfx(pszGfxFile);

	return true;
}

//更新嘴唇纹理
bool CECFace::UpdateMouthTexture(void)
{
	if( m_FaceData.idMouthTex <= 0 )
		return false;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;
	
	FACE_TEXTURE_ESSENCE* pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(m_FaceData.idMouthTex, ID_SPACE_FACE,dtFaceData); 
	if (!pFaceTextureEssence ||  dtFaceData != DT_FACE_TEXTURE_ESSENCE)
		return true;
	char* pszBaseTexFile = pFaceTextureEssence->file_base_tex;
	char* pszHighTexFile = pFaceTextureEssence->file_high_tex;

	//改变嘴唇纹理
	ChangeMouthTexture( pszBaseTexFile, pszHighTexFile);

	return true;
}

//更新眼球纹理
bool CECFace::UpdateEyeBallTexture(void)
{
	if( m_FaceData.idEyeBallTex <= 0 )
		return false;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;
	
	FACE_TEXTURE_ESSENCE* pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(m_FaceData.idEyeBallTex, ID_SPACE_FACE,dtFaceData); 
	if (!pFaceTextureEssence ||  dtFaceData != DT_FACE_TEXTURE_ESSENCE)
		return true;

	char* pszBaseTexFile = pFaceTextureEssence->file_base_tex;
	char* pszHighTexFile = pFaceTextureEssence->file_high_tex;

	//改变眼球纹理
	ChangeEyeBallTexture( pszBaseTexFile, pszHighTexFile);

	return true;
}

// 更新头发纹理
bool CECFace::UpdateHairTexture(void)
{
	if( m_FaceData.idHairTex <= 0)
		return false;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;
	
	FACE_TEXTURE_ESSENCE* pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(m_FaceData.idHairTex, ID_SPACE_FACE,dtFaceData); 
	if (!pFaceTextureEssence ||  dtFaceData != DT_FACE_TEXTURE_ESSENCE)
		return true;
	char* pszBaseTexFile = pFaceTextureEssence->file_base_tex;
	char* pszHighTexFile = pFaceTextureEssence->file_high_tex;
	
	//改变头发纹理
	ChangeHairTexture(pszBaseTexFile, pszHighTexFile);

	return true;
}

//更新大胡子纹理
bool CECFace::UpdateGoateeTexture(void)
{
	if( m_FaceData.idGoateeTex <= 0)
		return false;
	
	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;
	
	FACE_TEXTURE_ESSENCE* pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*)pElementDataMan->get_data_ptr(m_FaceData.idGoateeTex, ID_SPACE_FACE,dtFaceData); 
	if (!pFaceTextureEssence||  dtFaceData != DT_FACE_TEXTURE_ESSENCE)
		return true;
	char* pszBaseTexFile = pFaceTextureEssence->file_base_tex;
	char* pszHighTexFile = pFaceTextureEssence->file_high_tex;

	//改变胡子纹理
	ChangeGoateeTexture(pszBaseTexFile, pszHighTexFile);

	return true;
}

//更新法令纹理
bool CECFace::UpdateFalingTexture()
{
	if( m_FaceData.idFalingTex <= 0 )
		return false;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;

	FACE_TEXTURE_ESSENCE* pFaceTextureEssence = (FACE_TEXTURE_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idFalingTex, ID_SPACE_FACE,dtFaceData);
	if (!pFaceTextureEssence ||  dtFaceData != DT_FACE_TEXTURE_ESSENCE)
		return true;
	
	char* pszTexFile = pFaceTextureEssence->file_base_tex;
	

	//改变法令纹理
	ChangeFalingTexture(pszTexFile);

	return true;
}

//更新头发
bool CECFace::UpdateHairModel(void)
{
	if( m_FaceData.idHairModel <= 0 )
	{
		if( m_pHairSkin && m_nHairSkinIndex >= 0)
		{
			m_pSkinModel->ReplaceSkin(m_nHairSkinIndex, NULL, false);
			g_pGame->ReleaseA3DSkin(m_pHairSkin, true);
			m_pHairSkin = NULL;
			m_pHairShader = NULL;
		}
		return false;
	}

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan();
	
	DATA_TYPE dtFaceData;

	FACE_HAIR_ESSENCE* pFaceHairEssence = (FACE_HAIR_ESSENCE*)pElementDataMan->get_data_ptr(m_FaceData.idHairModel, ID_SPACE_FACE,dtFaceData);
	if (!pFaceHairEssence ||  dtFaceData != DT_FACE_HAIR_ESSENCE)
		return true;
	char* pszHairModelFile = pFaceHairEssence->file_hair_model;
	char* pszHairSkinFile = pFaceHairEssence->file_hair_skin;
	
	ChangeHairModel(pszHairModelFile, pszHairSkinFile);

	UpdateHairTexture();

	return true;
}
	
//更新胡子模型
bool CECFace::UpdateGoateeModel(void)
{
	if( m_FaceData.idMoustacheSkin <= 0 )
		return false;

	//女
	if( m_nGender == GENDER_FEMALE)
		return true;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;

	FACE_MOUSTACHE_ESSENCE* pFaceMoustancheEssence = (FACE_MOUSTACHE_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idMoustacheSkin, ID_SPACE_FACE,dtFaceData);
	if (!pFaceMoustancheEssence|| dtFaceData != DT_FACE_MOUSTACHE_ESSENCE)
		return true;	
	char* pszGoateeSkinFile = pFaceMoustancheEssence->file_moustache_skin;
		
	ChangeGoateeModel(pszGoateeSkinFile);
	
	UpdateGoateeTexture();
	return true;
}
// 更新法令模型
bool CECFace::UpdateFalingModel(void)
{
	if( m_FaceData.idFalingSkin <= 0 )
		return false;

	//获取元素数据管理器
	elementdataman* pElementDataMan;
	pElementDataMan= g_pGame->GetElementDataMan ();
	
	DATA_TYPE dtFaceData;

	FACE_FALING_ESSENCE* pFaceFalingEssence = (FACE_FALING_ESSENCE*) pElementDataMan->get_data_ptr(m_FaceData.idFalingSkin, ID_SPACE_FACE,dtFaceData);
	if (!pFaceFalingEssence || dtFaceData != DT_FACE_FALING_ESSENCE)
		return true;
	
	char* pszFalingSkinFile = pFaceFalingEssence->file_faling_skin;
		
	ChangeFalingModel(pszFalingSkinFile);
	
	UpdateFalingTexture();

	return true;
}
//更新脸颜色
bool CECFace::UpdateFaceColor(void)
{
	SetFaceColor(m_FaceData.colorFace);
	return true;

}

//更新眼影颜色
bool CECFace::UpdateEyeColor(void)
{
	SetEyeColor(m_FaceData.colorEye);
	return true;

}

//更新眉毛颜色
bool CECFace::UpdateBrowColor(void)
{
	SetBrowColor(m_FaceData.colorBrow);
	return true;
}

//更新嘴唇颜色
bool CECFace::UpdateMouthColor(void)
{
	SetMouthColor(m_FaceData.colorMouth);
	return true;

}

//更新头发颜色
bool CECFace::UpdateHairColor(void)
{
	SetHairColor(m_FaceData.colorHair);
	return true;
}

//更新眼球颜色
bool CECFace::UpdateEyeBallColor(void)
{
	SetEyeBallColor( m_FaceData.colorEyeBall);
	return true;
}

//更新胡子颜色
bool CECFace::UpdateGoateeColor(void)
{
	//女
	if( m_nGender == GENDER_FEMALE)
		return true;

	SetMoustacheColor(m_FaceData.colorMoustache);
	return true;

}

//更新所有脸部颜色
bool CECFace::UpdateAllFaceColor(void)
{
	//更新脸颜色
	UpdateFaceColor();

	//更新眼影颜色
	UpdateEyeColor();

	//更新眉毛颜色
	UpdateBrowColor();

	//更新嘴唇颜色
	UpdateMouthColor();

	//更新头发颜色
	UpdateHairColor();

	//更新眼球颜色
	UpdateEyeBallColor();

	//更新胡子颜色
	UpdateGoateeColor();


	return true;
}
//更新头发和胡子模型
bool CECFace::UpdateHairAndGoateeModel(void)
{
	//更新头发
	UpdateHairModel();
	
	//更新胡子模型
	UpdateGoateeModel();

	UpdateFalingModel();

	return true;
}




//更新全部脸部相关的纹理
bool CECFace::UpdateAllFaceTexture(void)
{
	//更新脸型纹理
	UpdateFaceTexture();

	//更新眼睛纹理(眼皮,眼影)
	UpdateEyeTexture();

	//更新眉毛纹理
	UpdateBrowTexture();

	//更新嘴唇纹理
	UpdateMouthTexture();

	//更新鼻子纹理
	UpdateNoseTexture();

	//更新眼球纹理
	UpdateEyeBallTexture();
	
	//更新胡子纹理
	UpdateMoustacheTexture();

	// 更新头发纹理
	UpdateHairTexture();

	//更新大胡子纹理
	UpdateGoateeTexture();

	//更新法令纹理
	UpdateFalingTexture();

	//设置纹身为空
	ChangeTattooTexture(NULL);


	return true;
}

//初始化脸型骨骼
bool CECFace::CreateFaceController()
{
	A3DSkeleton* pSkeleton = NULL;
	pSkeleton = m_pSkinModel->GetSkeleton();

	m_pBoneCtrler = new CFaceBoneController;

	//给骨骼控制器设置骨骼
	m_pBoneCtrler->SetSkeleton(pSkeleton);
	m_pBoneCtrler->InitBone();
	m_pBoneCtrler->StoreBoneTM();
	
	return true;
}

//计算全部脸部形状
bool CECFace::CalculateAllFaceShape(void)
{
	// 初始化骨骼
	m_pBoneCtrler->RestoreBoneTM();

	// 计算眼型
	CalculateEye();

	// 计算眉型
	CalculateBrow();

	// 计算鼻型
	CalculateNose();

	// 计算嘴型
	CalculateMouth();

	// 计算耳型
	CalculateEar();

	// 计算脸型融合
	CalculateBlendFace();

	// 计算脸盘
	CalculateFace();

	// 计算额头
	CalculateForehead();

	// 计算颧骨
	CalculateYokeBone();

	// 计算脸颊
	CalculateCheek();

	// 计算下巴
	CalculateChain();

	// 计算颌骨
	CalculateJaw();

	// 计算3庭
	Calculate3Parts();

	return true;
}

//脸型更新(形状, 纹理颜色, 模型,颜色)
bool CECFace::UpdateAll(void)
{
	//计算全部脸部形状
	CalculateAllFaceShape();

	//更新全部脸部相关的纹理
	UpdateAllFaceTexture();

	//更新头发和胡子模型
	UpdateHairAndGoateeModel();

	//更新所有脸部颜色
	UpdateAllFaceColor();

	//更新附眼
	UpdateThirdEye();

	return true;
}

//设置FaceData
bool CECFace::SetFaceData(const FACE_CUSTOMIZEDATA& faceData)
{
	m_FaceData = faceData;
	UpdateAll();

	return true;
}

bool CECFace::LoadAnimations()
{
	int		i;

	// connect animations with face bone controller
	for(i=0; i<FACE_ANIMATION_MAX; i++)
	{
		m_Animations[i].SetFaceBoneController(m_pBoneCtrler);

		if( !m_Expressions[m_nCharacter][m_nGender][i].IsEmpty() )
		{
			float weight = 1.0f;
			m_Animations[i].Init(1, &m_Expressions[m_nCharacter][m_nGender][i], &weight);
		}
	}
	
	return true;
}

bool CECFace::PlayAnimation(FACE_ANIMATION id)
{
	if( m_Animations[id].HasInited() )
	{
		m_Animations[id].Play(m_AnimationFactors[id].nAppearTime, 
				m_AnimationFactors[id].nKeepTime, 
				m_AnimationFactors[id].nDisappearTime, 
				m_AnimationFactors[id].nRestTime, 
				m_AnimationFactors[id].nNumRepeat);
	}

	return true;
}

const A3DMATRIX4& CECFace::GetFaceTM()
{
	return m_pSkinModel->GetAbsoluteTM();
}

//把比例从int 转换到float
float CECFace::TransformScaleFromIntToFloat(int nScale, float fScaleFactor, float fMax)
{


	if(fScaleFactor >=  fMax)
		fScaleFactor = fMax;

	if(fScaleFactor >=  1.0)
		fScaleFactor = 0.99f;

	else if ( fScaleFactor < 0)
		fScaleFactor = 0;

	float fScale  = (nScale - 128.0f)/128.0f;

	fScale = fScale * fScaleFactor + 1.0f;

	return  fScale;
}

//把位移从int 转换到float
float CECFace::TransformOffsetFromIntToFloat(int nOffset, float fOffsetFactor, float fMax)
{
	if(fOffsetFactor >= fMax)
		fOffsetFactor = fMax;

	float fOffset = (nOffset - 128.0f)/128.f;
	fOffset = fOffset * fOffsetFactor;
	return fOffset;
}

//把旋转从int 转换到float
float CECFace::TransformRotateFromIntToFloat(int nRotate, float fRotateFactor, float fMax)
{
	if( fRotateFactor >= fMax)
		fRotateFactor = fMax;

	float fRotate = (nRotate - 128.0f)/128.0f;
	fRotate = fRotate * fRotateFactor;
	return fRotate;
}

void CECFace::InitFaceCusFactor()
{
	m_FaceCusFactor.nVision = 1;

	m_FaceCusFactor.fScaleFaceHFactor = SCALE_FACEH_FACTOR;
	m_FaceCusFactor.fScaleFaceVFactor = SCALE_FACEV_FACTOR;
		
	m_FaceCusFactor.fScaleUpPartFactor = SCALE_UPPART_FACTOR;
	m_FaceCusFactor.fScaleMiddlePartFactor = SCALE_MIDDLEPART_FACTOR;
	m_FaceCusFactor.fScaleDownPartFactor = SCALE_DOWNPART_FACTOR;


	m_FaceCusFactor.fOffsetForeheadHFactor = OFFSET_FOREHEADH_FACTOR;
	m_FaceCusFactor.fOffsetForeheadVFactor = OFFSET_FOREHEADV_FACTOR;
	m_FaceCusFactor.fOffsetForeheadZFactor = OFFSET_FOREHEADZ_FACTOR;
	m_FaceCusFactor.fRotateForeheadFactor = ROTATE_FOREHEAD_FACTOR;
	m_FaceCusFactor.fScaleForeheadFactor = SCALE_FOREHEAD_FACTOR;

	m_FaceCusFactor.fOffsetYokeBoneHFactor = OFFSET_YOKEBONEH_FACTOR;
	m_FaceCusFactor.fOffsetYokeBoneVFactor = OFFSET_YOKEBONEV_FACTOR;
	m_FaceCusFactor.fOffsetYokeBoneZFactor = OFFSET_YOKEBONEZ_FACTOR;
	m_FaceCusFactor.fRotateYokeBoneFactor = ROTATE_YOKEBONE_FACTOR;
	m_FaceCusFactor.fScaleYokeBoneFactor = SCALE_YOKEBONE_FACTOR;

	m_FaceCusFactor.fOffsetCheekHFactor = OFFSET_CHEEKH_FACTOR;
	m_FaceCusFactor.fOffsetCheekVFactor = OFFSET_CHEEKV_FACTOR;
	m_FaceCusFactor.fOffsetCheekZFactor = OFFSET_CHEEKZ_FACTOR;
	m_FaceCusFactor.fScaleCheekFactor = SCALE_CHEEK_FACTOR;
		
	m_FaceCusFactor.fOffsetChainVFactor = OFFSET_CHAINV_FACTOR;
	m_FaceCusFactor.fOffsetChainZFactor = OFFSET_CHAINZ_FACTOR;
	m_FaceCusFactor.fRotateChainFactor = ROTATE_CHAIN_FACTOR;
	m_FaceCusFactor.fScaleChainHFactor = SCALE_CHAINH_FACTOR;

	m_FaceCusFactor.fOffsetJawHFactor = OFFSET_JAWH_FACTOR;
	m_FaceCusFactor.fOffsetJawVFactor = OFFSET_JAWV_FACTOR;
	m_FaceCusFactor.fOffsetJawZFactor = OFFSET_JAWZ_FACTOR;
	m_FaceCusFactor.fScaleJawSpecialFactor = SCALE_JAWSPECIAL_FACTOR;
	m_FaceCusFactor.fScaleJawHFactor = SCALE_JAWH_FACTOR;
	m_FaceCusFactor.fScaleJawVFactor = SCALE_JAWV_FACTOR;

	m_FaceCusFactor.fScaleEyeHFactor = SCALE_EYEH_FACTOR;
	m_FaceCusFactor.fScaleEyeVFactor = SCALE_EYEV_FACTOR;
	m_FaceCusFactor.fRotateEyeFactor = ROTATE_EYE_FACTOR;
	m_FaceCusFactor.fOffsetEyeHFactor = OFFSET_EYEH_FACTOR;
	m_FaceCusFactor.fOffsetEyeVFactor = OFFSET_EYEV_FACTOR;
	m_FaceCusFactor.fOffsetEyeZFactor = OFFSET_EYEZ_FACTOR;
	m_FaceCusFactor.fScaleEyeBallFactor = SCALE_EYEBALL_FACTOR;
	m_FaceCusFactor.fScaleEyeHFactor2 = SCALE_EYEH_FACTOR2;
	m_FaceCusFactor.fScaleEyeVFactor2 = SCALE_EYEV_FACTOR2;
	m_FaceCusFactor.fRotateEyeFactor2 = ROTATE_EYE_FACTOR2;
	m_FaceCusFactor.fOffsetEyeHFactor2 = OFFSET_EYEH_FACTOR2;
	m_FaceCusFactor.fOffsetEyeVFactor2 = OFFSET_EYEV_FACTOR2;
	m_FaceCusFactor.fOffsetEyeZFactor2 = OFFSET_EYEZ_FACTOR2;
	m_FaceCusFactor.fScaleEyeBallFactor2 = SCALE_EYEBALL_FACTOR2; 

	m_FaceCusFactor.fScaleBrowHFactor = SCALE_BROWH_FACTOR;
	m_FaceCusFactor.fScaleBrowVFactor = SCALE_BROWV_FACTOR;
	m_FaceCusFactor.fRotateBrowFactor = ROTATE_BROW_FACTOR;
	m_FaceCusFactor.fOffsetBrowHFactor = OFFSET_BROWH_FACTOR;
	m_FaceCusFactor.fOffsetBrowVFactor = OFFSET_BROWV_FACTOR;
	m_FaceCusFactor.fOffsetBrowZFactor = OFFSET_BROWZ_FACTOR;
		
	m_FaceCusFactor.fScaleBrowHFactor2 = SCALE_BROWH_FACTOR2;
	m_FaceCusFactor.fScaleBrowVFactor2 = SCALE_BROWV_FACTOR2;
	m_FaceCusFactor.fRotateBrowFactor2 = ROTATE_BROW_FACTOR2;
	m_FaceCusFactor.fOffsetBrowHFactor2 = OFFSET_BROWH_FACTOR2;
	m_FaceCusFactor.fOffsetBrowVFactor2 = OFFSET_BROWV_FACTOR2;
	m_FaceCusFactor.fOffsetBrowZFactor2 = OFFSET_BROWZ_FACTOR2;

	m_FaceCusFactor.fScaleNoseTipHFactor = SCALE_NOSETIPH_FACTOR;
	m_FaceCusFactor.fScaleNoseTipVFactor = SCALE_NOSETIPV_FACTOR;
	m_FaceCusFactor.fOffsetNoseTipVFactor = OFFSET_NOSETIPV_FACTOR;
	m_FaceCusFactor.fScaleNoseTipZFactor = SCALE_NOSETIPZ_FACTOR;
	m_FaceCusFactor.fScaleBridgeTipHFactor = SCALE_BRIDGETIPH_FACTOR;
	m_FaceCusFactor.fOffsetBridgeTipZFactor = OFFSET_BRIDGETIPZ_FACTOR;

	m_FaceCusFactor.fThickUpLipFactor = THICK_UPLIP_FACTOR;
	m_FaceCusFactor.fThickDownLipFactor = THICK_DOWNLIP_FACTOR;
	m_FaceCusFactor.fScaleMouthHFactor = SCALE_MOUTHH_FACTOR;
	m_FaceCusFactor.fOffsetMouthVFactor = OFFSET_MOUTHV_FACTOR;
	m_FaceCusFactor.fOffsetMouthZFactor = OFFSET_MOUTHZ_FACTOR;
	m_FaceCusFactor.fOffsetCornerOfMouthSpecialFactor = OFFSET_CORNEROFMOUTHSPECIAL_FACTOR;
	m_FaceCusFactor.fScaleMouthHFactor2 = SCALE_MOUTHH_FACTOR2;
	m_FaceCusFactor.fOffsetCornerOfMouthSpecialFactor2 = OFFSET_CORNEROFMOUTHSPECIAL_FACTOR2;
		
	m_FaceCusFactor.fScaleEarFactor = SCALE_EAR_FACTOR;
	m_FaceCusFactor.fOffsetEarVFactor =OFFSET_EARV_FACTOR;	
}

//载入脸型自定义参数
bool CECFace::LoadFaceCusFactorFromIni(void)
{
	AIniFile IniFile;

	if (!IniFile.Open("Configs\\CustomizeFactor.ini"))
	{
		a_LogOutput(1, "CECFace::LoadFaceShapeFactorFromINIFile, Failed to open file Configs\\CustomizeFactor.ini");
		return false;
	}

	AString strSection = "Version";
	m_FaceCusFactor.nVision = IniFile.GetValueAsInt(strSection, "Version", m_FaceCusFactor.nVision);
	
	strSection= "Face";
	m_FaceCusFactor.fScaleFaceHFactor	= IniFile.GetValueAsFloat(strSection, "ScaleFaceHFactor", m_FaceCusFactor.fScaleFaceHFactor);
	m_FaceCusFactor.fScaleFaceVFactor	= IniFile.GetValueAsFloat(strSection, "ScaleFaceVFactor", m_FaceCusFactor.fScaleFaceVFactor);
	
	strSection = "3Parts";
	m_FaceCusFactor.fScaleUpPartFactor = IniFile.GetValueAsFloat(strSection, "ScaleUpPartFactor", m_FaceCusFactor.fScaleUpPartFactor);
	m_FaceCusFactor.fScaleMiddlePartFactor = IniFile.GetValueAsFloat(strSection, "ScaleMiddlePartFactor", m_FaceCusFactor.fScaleMiddlePartFactor);
	m_FaceCusFactor.fScaleDownPartFactor = IniFile.GetValueAsFloat(strSection, "ScaleDownPartFactor", m_FaceCusFactor.fScaleDownPartFactor);

	strSection = "Forehead";

	m_FaceCusFactor.fOffsetForeheadHFactor = IniFile.GetValueAsFloat(strSection, "OffsetForeheadHFactor", m_FaceCusFactor.fOffsetForeheadHFactor);
	m_FaceCusFactor.fOffsetForeheadVFactor = IniFile.GetValueAsFloat(strSection, "OffsetForeheadVFactor", m_FaceCusFactor.fOffsetForeheadVFactor);
	m_FaceCusFactor.fOffsetForeheadZFactor = IniFile.GetValueAsFloat(strSection, "OffsetForeheadZFactor", m_FaceCusFactor.fOffsetForeheadZFactor);
	m_FaceCusFactor.fRotateForeheadFactor = IniFile.GetValueAsFloat(strSection, "RotateForeheadFactor", m_FaceCusFactor.fRotateForeheadFactor);
	m_FaceCusFactor.fScaleForeheadFactor = IniFile.GetValueAsFloat(strSection, "ScaleForeheadFactor", m_FaceCusFactor.fScaleForeheadFactor);

	strSection = "YokeBone";
	
	m_FaceCusFactor.fOffsetYokeBoneHFactor = IniFile.GetValueAsFloat(strSection, "OffsetYokeBoneHFactor", m_FaceCusFactor.fOffsetYokeBoneHFactor);
	m_FaceCusFactor.fOffsetYokeBoneVFactor = IniFile.GetValueAsFloat(strSection, "OffsetYokeBoneVFactor", m_FaceCusFactor.fOffsetYokeBoneVFactor);
	m_FaceCusFactor.fOffsetYokeBoneZFactor = IniFile.GetValueAsFloat(strSection, "OffsetYokeBoneZFactor", m_FaceCusFactor.fOffsetYokeBoneZFactor);
	m_FaceCusFactor.fRotateYokeBoneFactor = IniFile.GetValueAsFloat(strSection, "RotateYokeBoneFactor", m_FaceCusFactor.fRotateYokeBoneFactor);
	m_FaceCusFactor.fScaleYokeBoneFactor = IniFile.GetValueAsFloat(strSection, "ScaleYokeBoneFactor", m_FaceCusFactor.fScaleYokeBoneFactor);

	strSection = "Cheek";

	m_FaceCusFactor.fOffsetCheekHFactor = IniFile.GetValueAsFloat(strSection, "OffsetCheekHFactor", m_FaceCusFactor.fOffsetCheekHFactor);
	m_FaceCusFactor.fOffsetCheekVFactor = IniFile.GetValueAsFloat(strSection, "OffsetCheekVFactor", m_FaceCusFactor.fOffsetCheekVFactor);
	m_FaceCusFactor.fOffsetCheekZFactor = IniFile.GetValueAsFloat(strSection, "OffsetCheekZFactor", m_FaceCusFactor.fOffsetCheekZFactor);
	m_FaceCusFactor.fScaleCheekFactor = IniFile.GetValueAsFloat(strSection, "ScaleCheekFactor", m_FaceCusFactor.fScaleCheekFactor);

	strSection = "Chain";

	m_FaceCusFactor.fOffsetChainVFactor = IniFile.GetValueAsFloat(strSection, "OffsetChainVFactor", m_FaceCusFactor.fOffsetChainVFactor);
	m_FaceCusFactor.fOffsetChainZFactor = IniFile.GetValueAsFloat(strSection, "OffsetChainZFactor", m_FaceCusFactor.fOffsetChainZFactor);
	m_FaceCusFactor.fRotateChainFactor = IniFile.GetValueAsFloat(strSection, "RotateChainFactor", m_FaceCusFactor.fRotateChainFactor);
	m_FaceCusFactor.fScaleChainHFactor = IniFile.GetValueAsFloat(strSection, "ScaleChainHFactor", m_FaceCusFactor.fScaleChainHFactor);

	strSection = "Jaw";
	m_FaceCusFactor.fOffsetJawHFactor = IniFile.GetValueAsFloat(strSection, "OffsetJawHFactor", m_FaceCusFactor.fOffsetJawHFactor);
	m_FaceCusFactor.fOffsetJawVFactor = IniFile.GetValueAsFloat(strSection, "OffsetJawVFactor", m_FaceCusFactor.fOffsetJawVFactor);
	m_FaceCusFactor.fOffsetJawZFactor = IniFile.GetValueAsFloat(strSection, "OffsetJawZFactor", m_FaceCusFactor.fOffsetJawZFactor);
	m_FaceCusFactor.fScaleJawSpecialFactor = IniFile.GetValueAsFloat(strSection, "ScaleJawSpecialFactor", m_FaceCusFactor.fScaleJawSpecialFactor);
	m_FaceCusFactor.fScaleJawHFactor = IniFile.GetValueAsFloat(strSection, "ScaleJawHFactor", m_FaceCusFactor.fScaleJawHFactor);
	m_FaceCusFactor.fScaleJawVFactor = IniFile.GetValueAsFloat(strSection, "ScaleJawVFactor", m_FaceCusFactor.fScaleJawVFactor);

	strSection ="Eye";
	m_FaceCusFactor.fScaleEyeHFactor = IniFile.GetValueAsFloat(strSection, "ScaleEyeHFactor", m_FaceCusFactor.fScaleEyeHFactor);
	m_FaceCusFactor.fScaleEyeVFactor = IniFile.GetValueAsFloat(strSection, "ScaleEyeVFactor", m_FaceCusFactor.fScaleEyeVFactor);
	m_FaceCusFactor.fRotateEyeFactor = IniFile.GetValueAsFloat(strSection, "RotateEyeFactor", m_FaceCusFactor.fRotateEyeFactor);
	m_FaceCusFactor.fOffsetEyeHFactor = IniFile.GetValueAsFloat(strSection, "OffsetEyeHFactor", m_FaceCusFactor.fOffsetEyeHFactor);
	m_FaceCusFactor.fOffsetEyeVFactor = IniFile.GetValueAsFloat(strSection, "OffsetEyeVFactor", m_FaceCusFactor.fOffsetEyeVFactor);
	m_FaceCusFactor.fOffsetEyeZFactor = IniFile.GetValueAsFloat(strSection, "OffsetEyeZFactor", m_FaceCusFactor.fOffsetEyeZFactor);
	m_FaceCusFactor.fScaleEyeBallFactor = IniFile.GetValueAsFloat(strSection, "ScaleEyeBallFactor", m_FaceCusFactor.fScaleEyeBallFactor);

	m_FaceCusFactor.fScaleEyeHFactor2 = IniFile.GetValueAsFloat(strSection, "ScaleEyeHFactor2", m_FaceCusFactor.fScaleEyeHFactor2);
	m_FaceCusFactor.fScaleEyeVFactor2 = IniFile.GetValueAsFloat(strSection, "ScaleEyeVFactor2", m_FaceCusFactor.fScaleEyeVFactor2);
	m_FaceCusFactor.fRotateEyeFactor2 = IniFile.GetValueAsFloat(strSection, "RotateEyeFactor2", m_FaceCusFactor.fRotateEyeFactor2);
	m_FaceCusFactor.fOffsetEyeHFactor2 = IniFile.GetValueAsFloat(strSection, "OffsetEyeHFactor2", m_FaceCusFactor.fOffsetEyeHFactor2);
	m_FaceCusFactor.fOffsetEyeVFactor2 = IniFile.GetValueAsFloat(strSection, "OffsetEyeVFactor2", m_FaceCusFactor.fOffsetEyeVFactor2);
	m_FaceCusFactor.fOffsetEyeZFactor2 = IniFile.GetValueAsFloat(strSection, "OffsetEyeZFactor2", m_FaceCusFactor.fOffsetEyeZFactor2);
	m_FaceCusFactor.fScaleEyeBallFactor2 = IniFile.GetValueAsFloat(strSection, "ScaleEyeBallFactor2", m_FaceCusFactor.fScaleEyeBallFactor2);

	strSection = "Brow";
	m_FaceCusFactor.fScaleBrowHFactor = IniFile.GetValueAsFloat(strSection, "ScaleBrowHFactor", m_FaceCusFactor.fScaleBrowHFactor);
	m_FaceCusFactor.fScaleBrowVFactor = IniFile.GetValueAsFloat(strSection, "ScaleBrowVFactor", m_FaceCusFactor.fScaleBrowVFactor);
	m_FaceCusFactor.fRotateBrowFactor = IniFile.GetValueAsFloat(strSection, "RotateBrowFactor", m_FaceCusFactor.fRotateBrowFactor);
	m_FaceCusFactor.fOffsetBrowHFactor = IniFile.GetValueAsFloat(strSection, "OffsetBrowHFactor", m_FaceCusFactor.fOffsetBrowHFactor);
	m_FaceCusFactor.fOffsetBrowVFactor = IniFile.GetValueAsFloat(strSection, "OffsetBrowVFactor", m_FaceCusFactor.fOffsetBrowVFactor);
	m_FaceCusFactor.fOffsetBrowZFactor = IniFile.GetValueAsFloat(strSection, "OffsetBrowZFactor", m_FaceCusFactor.fOffsetBrowZFactor);

	m_FaceCusFactor.fScaleBrowHFactor2 = IniFile.GetValueAsFloat(strSection, "ScaleBrowHFactor2", m_FaceCusFactor.fScaleBrowHFactor2);
	m_FaceCusFactor.fScaleBrowVFactor2 = IniFile.GetValueAsFloat(strSection, "ScaleBrowVFactor2", m_FaceCusFactor.fScaleBrowVFactor2);
	m_FaceCusFactor.fRotateBrowFactor2 = IniFile.GetValueAsFloat(strSection, "RotateBrowFactor2", m_FaceCusFactor.fRotateBrowFactor2);
	m_FaceCusFactor.fOffsetBrowHFactor2 = IniFile.GetValueAsFloat(strSection, "OffsetBrowHFactor2", m_FaceCusFactor.fOffsetBrowHFactor2);
	m_FaceCusFactor.fOffsetBrowVFactor2 = IniFile.GetValueAsFloat(strSection, "OffsetBrowVFactor2", m_FaceCusFactor.fOffsetBrowVFactor2);
	m_FaceCusFactor.fOffsetBrowZFactor2 = IniFile.GetValueAsFloat(strSection, "OffsetBrowZFactor2", m_FaceCusFactor.fOffsetBrowZFactor2);

	strSection = "Nose";
	m_FaceCusFactor.fScaleNoseTipHFactor = IniFile.GetValueAsFloat(strSection, "ScaleNoseTipHFactor", m_FaceCusFactor.fScaleNoseTipHFactor);
	m_FaceCusFactor.fScaleNoseTipVFactor = IniFile.GetValueAsFloat(strSection, "ScaleNoseTipVFactor", m_FaceCusFactor.fScaleNoseTipVFactor);
	m_FaceCusFactor.fOffsetNoseTipVFactor = IniFile.GetValueAsFloat(strSection, "OffsetNoseTipVFactor", m_FaceCusFactor.fOffsetNoseTipVFactor);
	m_FaceCusFactor.fScaleNoseTipZFactor = IniFile.GetValueAsFloat(strSection, "ScaleNoseTipZFactor", m_FaceCusFactor.fScaleNoseTipZFactor);
	m_FaceCusFactor.fScaleBridgeTipHFactor = IniFile.GetValueAsFloat(strSection, "ScaleBridgeTipHFactor", m_FaceCusFactor.fScaleBridgeTipHFactor);
	m_FaceCusFactor.fOffsetBridgeTipZFactor = IniFile.GetValueAsFloat(strSection, "OffsetBridgeTipZFactor", m_FaceCusFactor.fOffsetBridgeTipZFactor);
	
	strSection = "Mouth";
	m_FaceCusFactor.fThickUpLipFactor = IniFile.GetValueAsFloat(strSection, "ThickUpLipFactor", m_FaceCusFactor.fThickUpLipFactor);
	m_FaceCusFactor.fThickDownLipFactor = IniFile.GetValueAsFloat(strSection, "ThickDownLipFactor", m_FaceCusFactor.fThickDownLipFactor);
	m_FaceCusFactor.fScaleMouthHFactor = IniFile.GetValueAsFloat(strSection, "ScaleMouthHFactor", m_FaceCusFactor.fScaleMouthHFactor);
	m_FaceCusFactor.fOffsetMouthVFactor = IniFile.GetValueAsFloat(strSection, "OffsetMouthVFactor", m_FaceCusFactor.fOffsetMouthVFactor);
	m_FaceCusFactor.fOffsetMouthZFactor = IniFile.GetValueAsFloat(strSection, "OffsetMouthZFactor", m_FaceCusFactor.fOffsetMouthZFactor);
	m_FaceCusFactor.fOffsetCornerOfMouthSpecialFactor = IniFile.GetValueAsFloat(strSection, "OffsetCornerOfMouthSpecialFactor", m_FaceCusFactor.fOffsetCornerOfMouthSpecialFactor); 
	m_FaceCusFactor.fScaleMouthHFactor2 = IniFile.GetValueAsFloat(strSection, "ScaleMouthHFactor2", m_FaceCusFactor.fScaleMouthHFactor2);
	m_FaceCusFactor.fOffsetCornerOfMouthSpecialFactor2 = IniFile.GetValueAsFloat(strSection, "OffsetCornerOfMouthSpecialFactor2", m_FaceCusFactor.fOffsetCornerOfMouthSpecialFactor2); 

	strSection = "Ear";
	m_FaceCusFactor.fScaleEarFactor = IniFile.GetValueAsFloat(strSection, "ScaleEarFactor", m_FaceCusFactor.fScaleEarFactor);
	m_FaceCusFactor.fOffsetEarVFactor = IniFile.GetValueAsFloat(strSection, "OffsetEarVFactor", m_FaceCusFactor.fOffsetEarVFactor);
																												
	IniFile.Close();

	return true;
}

void CECFace::SetTransparent(float fTrans)
{
	//	设置脸部模型及特效的透明度
	//	脸部模型的透明度在父亲模型设置透明度的时候已设置过，这里只是临时改变透明度（比如左上角人物面板中头像的显示，不受透明度影响）
	//
	if (m_pSkinModel)
		m_pSkinModel->SetTransparent(fTrans);
	if (m_pThirdEyeGfx)
	{
		fTrans = 1.0f - fTrans;
		a_Clamp(fTrans, 0.0f, 1.0f);
		m_pThirdEyeGfx->SetAlphaTransition(fTrans, 1);
		m_pThirdEyeGfx->TickAnimation(1);
	}
}

float CECFace::GetTransparent()const
{
	//	查询当前透明度（可用于临时改变透明度时恢复）
	//
	float fTrans = 1.0f;
	if (m_pSkinModel)
		fTrans = m_pSkinModel->GetTransparent();
	return fTrans;
}