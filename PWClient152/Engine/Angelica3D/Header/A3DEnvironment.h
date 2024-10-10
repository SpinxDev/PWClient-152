#ifndef _A3D_ENVIRONMENT_H_
#define _A3D_ENVIRONMENT_H_

#include "A3DSkinModel.h"

class A3DViewport;
class A3DCCDBinder;	// Common Const Data Binder


class A3DEnvironment
{
public:
	struct FOG
	{
        bool            bGlobalFogEnable;    // enable fog
        // View depended fog
		A3DCOLOR		crFog1;          // upper part color
        A3DCOLOR		crFog2;          // lower part color
        float           fFogStart;           // start distance (near)
        float           fFogEnd;             // end distance (far)        
        float           fDensity1;           // upper fog density
        float           fDensity2;           // lower fog density
        float           fColorIntensity1;    // upper color intensity
        float           fColorIntensity2;    // lower color intensity
        float           fHorzDensity;        // density at horizontal plane   
        float           fHorzPower;          // exponent of interpolation factor
        // Height depended fog
        float           fHeightStart;        // start height (bottom)
        float           fHeightEnd;          // end height   (top)
        float           fHeightDensity;      // density of height fog
	};
	struct DIRLIGHT
	{
		A3DCOLORVALUE   Diffuse;
		float			DirPower;
		//A3DCOLORVALUE   Specular;
		A3DCOLORVALUE   Ambient0;
		A3DCOLORVALUE   Ambient1;
		A3DVECTOR3      Direction;		// ����Ҫnormalize
	};
	struct POINTLIGHT
	{
		A3DCOLORVALUE   Diffuse;
		A3DVECTOR3      Position;
		FLOAT           Range;
		FLOAT           Attenuation0;
		FLOAT           Attenuation1;
	};
    struct LITMAP
    {
        float MaxLight;
        float MaxLightNight;
        float Gamma;
    };
    
	struct LIGHTMAP_SHADOW_GLOBAL
	{
		bool bShadowMap;
		bool bHWPCF;
		A3DVECTOR4 vLitMapParam;
		float fShadowMapSize;
		int nShadowSampleNum;
		IDirect3DBaseTexture9* LitModelShadowMap;
		IDirect3DBaseTexture9* LitModelShadowMapFloat;
	};
	
	struct LITMODEL_INDIVIDUAL
	{
		bool bLightMap;
		bool bVertLit;
		float fAlpha;
		IDirect3DBaseTexture9* LitModelDiffuseTexture;
		IDirect3DBaseTexture9* LitModelLightMap1;
		IDirect3DBaseTexture9* LitModelLightMap2;
	};

	// ���в����ļ���, �����Ӧ���Shader�ϵĳ����ṹ
	// ���Բ��ܴ��ⲿ�ı�����ṹ�����ֵ!
	struct COMMON : public A3DCOMMCONSTTABLE
	{
		A3DCOLORVALUE   DirDiffuse;			// �������ɫ
		float			DirPower;			// �����ǿ��
		A3DVECTOR3      LightDirection;		// �����ķ���
		A3DVECTOR3      LightDirInView;		// �����ķ���(��View�ռ�)
		//A3DCOLORVALUE   Specular;
		A3DCOLORVALUE   Ambient0;
		A3DCOLORVALUE   Ambient1;

		A3DCOLORVALUE   PtDiffuse;
		A3DVECTOR3      PtPosition;		// ���Դλ��
		A3DVECTOR3      PtPosInView;	// ���Դλ��(View�ռ�)
		A3DVECTOR3		ptAttenRange;	// float3(Attenuation0, Attenuation1, Range)

		A3DVECTOR4		vFogParam1;
		A3DVECTOR4		vFogParam2;
		A3DVECTOR4		vFogColor1;
        A3DVECTOR4		vFogColor2;

		A3DVECTOR4		vTime;			// float4(time, cos(time), sin(time), tan(time))
		A3DVECTOR4		vViewportDim;	// float4(width, height, aspect, InvAspect)
		A3DVECTOR4		vViewPosition;	// ��vEyePos�ظ�
		A3DVECTOR4		vViewDirection;

		// 12�����þ���
		A3DMATRIX4		matViewProj;
		A3DMATRIX4		matViewProjInv;
		A3DMATRIX4		matView;
        A3DMATRIX4		matViewInv;
		A3DMATRIX4		matProj;
		A3DMATRIX4		matProjInv;
		A3DMATRIX4		matWorldViewProj;
		A3DMATRIX4		matWorldViewProjInv;
		A3DMATRIX4		matWorld;
		A3DMATRIX4		matWorldInv;
		A3DMATRIX4		matWorldView;
		A3DMATRIX4		matWorldViewInv;
		//A3DMATRIX4		matViewProjInvTran;
		//A3DMATRIX4		matViewProjTran;
		//A3DMATRIX4		matViewTran;
		//A3DMATRIX4		matViewInvTran;
		//A3DMATRIX4		matProjTran;
		//A3DMATRIX4		matProjInvTran;
		//A3DMATRIX4		matWorldViewProjTran;
		//A3DMATRIX4		matWorldViewProjInvTran;
		//A3DMATRIX4		matWorldTran;
		//A3DMATRIX4		matWorldInvTran;
		//A3DMATRIX4		matWorldViewTran;
		//A3DMATRIX4		matWorldViewInvTran;

        A3DVECTOR3      vEyePos;	// ��vViewPosition�ظ�
        A3DVECTOR4      vProjectionRatio;
        float           fPosExtendToNormal; // λ���ط��߷���ƫ��
		
		
		//����ͼ����Ӱȫ�ֲ���(Ӱ��LitModel��Terrain)
		float			fGlobalUseShadowMap;
		float			fGlobalUseHWPCF;
		float			fGlobalShadowMapSize;	
		float			fGlobalShadowSampleNum;
		IDirect3DBaseTexture9* pGlobalShadowMap;
		IDirect3DBaseTexture9* pGlobalShadowMapFloat;
		A3DVECTOR4		vGlobalLightMapParam;




		//LitModel������Ⱦ����
		float			LitModelAlpha;
		float			LitModelUseLightMap;
		float			LitModelUseVerlit;
		IDirect3DBaseTexture9* LitModelDiffuseTexture;
		IDirect3DBaseTexture9* LitModelLightMap1;
		IDirect3DBaseTexture9* LitModelLightMap2;

	};
protected:
	FOG				m_FogParam;
    FOG				m_FogParamUnderWater;
    FOG				m_FogParamAboveWater;
	DIRLIGHT		m_DirLight;
	POINTLIGHT		m_PointLight;
    LITMAP          m_LightMap;
	LIGHTMAP_SHADOW_GLOBAL	m_LitModelGlobal;
	LITMODEL_INDIVIDUAL m_LitModelIndividual;
	COMMON			m_Common;
	A3DCCDBinder*	m_pCCDBinder;
    bool            m_bIsUnderWater;
    float           m_fDNFactor;

public:
	A3DEnvironment();
	virtual ~A3DEnvironment();

	bool Init		();
	bool Release	();
	bool Tick		();

	// SetXXX �����ڲ�������ݽ��д���, ��Щ���ݻ�ƴ�ϵ�һ��
	bool				UpdateFogParam		();
	const FOG*			GetFogParam			();
    const FOG*			GetFogParamUnderWater();
    const FOG*			GetFogParamAboveWater();
    bool				SetFogParamUnderWater(const FOG* pFog);
    bool				SetFogParamAboveWater(const FOG* pFog);

    bool				SetDirLightParam	(const DIRLIGHT* pDirLight);
	const DIRLIGHT*		GetDirLightParam	();
	bool				SetPointLightParam	(const POINTLIGHT* pPointLight);
	const POINTLIGHT*	GetPointLightParam	();
    bool				SetLightMapParam	(const LITMAP* pLitMap);
    const LITMAP*		GetLightMapParam	();
	bool				SetViewport			(A3DViewport* pA3DViewport);
	bool				SetViewport			(A3DViewport* pA3DViewport, const A3DMATRIX4* pmatWorld);
	bool				SetWorldMatrix		(const A3DMATRIX4& matWorld);
	bool				SetLightInfo		(const A3DSkinModel::LIGHTINFO& LightInfo);	// ֻ���µ��Դ����

	bool								SetLightMapShadowGlobalParam(const LIGHTMAP_SHADOW_GLOBAL* pLitModelGlobal);
	const LIGHTMAP_SHADOW_GLOBAL*		GetLightMapShadowGlobalParam();
	bool								SetLitModelIndividualParam(const LITMODEL_INDIVIDUAL* pLitModel);
	const LITMODEL_INDIVIDUAL*			GetLitModelIndividualParam();

	inline const COMMON*	GetCommonConstTable	() const;
	inline A3DCCDBinder*	GetCCDBinder		() const;

    inline bool         GetIsUnderWater     () const;
    void                SetIsUnderWater     (bool bUnder);
    inline bool         GetGlobalFogEnable  () const;
    bool                SetGlobalFogEnable  (bool bEnable);
    inline float        GetDNFactor         () const;
    void                SetDNFactor         (float fFactor);
    inline float        GetPosExtendToNormal() const { return m_Common.fPosExtendToNormal; }
    void                SetPosExtendToNormal(float fExtend) { m_Common.fPosExtendToNormal = fExtend; }

};

bool A3DEnvironment::GetIsUnderWater() const 
{ 
	return m_bIsUnderWater; 
}

A3DCCDBinder* A3DEnvironment::GetCCDBinder() const 
{ 
	return m_pCCDBinder; 
}

const A3DEnvironment::COMMON* A3DEnvironment::GetCommonConstTable() const
{
	return &m_Common;
}

bool A3DEnvironment::GetGlobalFogEnable() const
{
	return m_FogParam.bGlobalFogEnable;
}

float A3DEnvironment::GetDNFactor() const
{
	return m_fDNFactor;
}

class A3DColorTone
{
private:
	A3DMATRIX4 m_preHueMatrix;
	A3DMATRIX4 m_postHueMatrix;
public:
	void ShearColor(A3DMATRIX4& matrix,int whichColor,int color1,int color2);
	void RotateColor(A3DMATRIX4& matrix,float phi,int axiIndex);
	void InitHue();
	void ComputeColorMatrix(A3DMATRIX4& matrix, const A3DCOLORVALUE& crFrom, const A3DCOLORVALUE& crTo);
};

#endif // _A3D_ENVIRONMENT_H_