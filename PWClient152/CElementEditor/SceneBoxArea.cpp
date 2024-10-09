// SceneBoxArea.cpp: implementation of the CSceneBoxArea class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "SceneBoxArea.h"
#include "SceneObjectManager.h"
#include "SceneLightObject.h"
#include "Box3D.h"
#include "Render.h"
#include "A3D.h"
#include "AM.h"
#include "MainFrm.h"
#include "TerrainWater.h"
#include "A3DRain.h"
#include "A3DSnow.h"
#include "NatureDataSetDlg.h"
#include "MusicSelDlg.h"
#include "DlgLighting.h"


//#define new A_DEBUG_NEW

void Init_BoxArea_Data(BOX_AREA_PROPERTY &data)
{
	data.m_fLength = 20.0f;
	data.m_fHeight = 20.0f;
	data.m_fWidth  = 20.0f;

	data.m_vLightDir = A3DVECTOR3(0.0f,0.0f,0.0f);

	//fog
	data.m_fFogStart = 50.0f;
	data.m_fFogEnd = 500.0f;
	data.m_fFogDeepness = 0.5f;
	data.m_fFogColor = A3DCOLORRGB(255,255,255);

	data.m_fFogStartNight = 50.0f;
	data.m_fFogEndNight = 500.0f;
	data.m_fFogDeepnessNight = 0.5f;
	data.m_fFogColorNight = A3DCOLORRGB(255,255,255);

	//fog of under water
	data.m_fUnderWaterFogStart = 10.0f;
	data.m_fUnderWaterFogEnd = 100.0f;
	data.m_fUnderWaterFogDeepness = 0.5f;
	data.m_dwUnderWaterClr = A3DCOLORRGB(36,146,255);

	data.m_fUnderWaterFogStartNight = 10.0f;
	data.m_fUnderWaterFogEndNight = 100.0f;
	data.m_fUnderWaterFogDeepnessNight = 0.5f;
	data.m_dwUnderWaterClrNight = A3DCOLORRGB(36,36,72);
	

	//sky box textures
	data.m_strSky0 = "Roof.bmp";
	data.m_strSky1 = "01.bmp";
	data.m_strSky2 = "02.bmp";
	data.m_fCloudSpeedU = 0.002f;
	data.m_fCloudSpeedV = 0.002f;

	data.m_strSkyNight0 = "Roof.bmp";
	data.m_strSkyNight1 = "01.bmp";
	data.m_strSkyNight2 = "02.bmp";

	//light
	data.m_dwDirLightClr = A3DCOLORRGB(255,255,255);//直射光
	data.m_dwAmbient     = A3DCOLORRGB(255,255,255);
	data.m_dwDirLightClrNight = A3DCOLORRGB(64,64,64);//Night light
	data.m_dwAmbientNight = A3DCOLORRGB(64,64,64);
	data.m_fSunPower = 1.0f;
	data.m_fSunPowerNight = 1.0f;
	data.m_nMapSize = 1024;
	data.m_nMapSizeNight = 1024;
	data.m_dwCloudDirClr = A3DCOLORRGB(255,255,255);
	data.m_dwCloudAmbientClr = A3DCOLORRGB(64,64,64);
	data.m_dwCloudNightAmbientClr = A3DCOLORRGB(64,64,64);
	data.m_dwCloudNightDirClr = A3DCOLORRGB(255,255,255);

	//transition time
	data.m_dwTransTime = 5000;
	
	//music
	//data.m_strMusic = "default.mp3";

	//Nature
	data.m_nRainSpeed = 50;
	data.m_fRainSize = 0.12f;
	data.m_dwRainClr = A3DCOLORRGBA(160, 160, 160, 170);
	
	data.m_nSnowSpeed = 8;
	data.m_fSnowSize = 0.6f;
	data.m_dwSnowClr = A3DCOLORRGBA(160, 160, 160, 255);
	data.m_dwFlag = 24;
}

void Clamb255(int &value)
{
	if(value<0) value = 0;
	else if(value>255) value = 255;
}
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneBoxArea::CSceneBoxArea()
{
	m_nObjectType = CSceneObject::SO_TYPE_AREA;
	m_pNatureDataSetFunc = new NATURE_DATA_SET_FUNCS;
	m_pLightingSelFunc   = new LIGHTING_SET_FUNCS;
	
	strText = "...";
	m_pRain = NULL;
	m_pSnow = NULL;
	Init_BoxArea_Data(m_PropertyData);
	m_bTransDone = true;
	m_vPos = A3DVECTOR3(0.0f);
	m_pA3DSky = NULL;
	m_bIsInside = false;
	m_bIsDefault = false;
	BuildAreaProperty();
	ReBuildArea();
	m_pLastArea = NULL;
}


CSceneBoxArea::CSceneBoxArea(bool bDefault)
{
	m_nObjectType = CSceneObject::SO_TYPE_AREA;
	m_pNatureDataSetFunc = new NATURE_DATA_SET_FUNCS;
	m_pLightingSelFunc   = new LIGHTING_SET_FUNCS;
	m_pRain = NULL;
	m_pSnow = NULL;
	Init_BoxArea_Data(m_PropertyData);
	m_vPos = A3DVECTOR3(0.0f);
	m_pA3DSky = NULL;
	m_bIsInside = false;
	m_bIsDefault = bDefault;
	m_bTransDone = true;
	BuildAreaProperty();
	ReBuildArea();
	m_pLastArea = NULL;
}

CSceneBoxArea::~CSceneBoxArea()
{

}

void CSceneBoxArea::Tick(DWORD timeDelta)
{	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		A3DVECTOR3 cPos = g_Render.GetA3DEngine()->GetActiveCamera()->GetPos();
		
		if( m_pRain && (m_PropertyData.m_dwFlag & 0x2))
			m_pRain->TickEmitting();
		if( m_pSnow && (m_PropertyData.m_dwFlag & 0x4))
			m_pSnow->TickEmitting();
		
		if(IsInside(cPos))
		{//摄象机在BOX的内部
			if(!m_bIsInside) 
			{
				m_bIsInside = true;
				OnCamEntry();
			}
		}else
		{
			if(m_bIsInside)
			{
				m_bIsInside = false;
				OnCamLeave();
			}
		}
	}
}

void CSceneBoxArea::OnCamEntry(bool bForceUpdate)//摄象机进入区域事件
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		m_dwEscapeTime = 0;

		float fInvTime = 1.0f;
		if( m_PropertyData.m_dwTransTime > 0 )
			fInvTime = 1000.0f / m_PropertyData.m_dwTransTime;

		if( bForceUpdate )
			fInvTime = 1000.0f;

		pMap->m_FogColor.SetTarget(ColorRGBAToColorValue(m_PropertyData.m_fFogColor), fInvTime);
		pMap->m_FogStart.SetTarget(m_PropertyData.m_fFogStart, fInvTime);
		pMap->m_FogEnd.SetTarget(m_PropertyData.m_fFogEnd, fInvTime);
		pMap->m_FogDensity.SetTarget(m_PropertyData.m_fFogDeepness, fInvTime);

		pMap->m_FogColorNight.SetTarget(ColorRGBAToColorValue(m_PropertyData.m_fFogColorNight), fInvTime);
		pMap->m_FogStartNight.SetTarget(m_PropertyData.m_fFogStartNight, fInvTime);
		pMap->m_FogEndNight.SetTarget(m_PropertyData.m_fFogEndNight, fInvTime);
		pMap->m_FogDensityNight.SetTarget(m_PropertyData.m_fFogDeepnessNight, fInvTime);

		pMap->m_FogColorUW.SetTarget(ColorRGBAToColorValue(m_PropertyData.m_dwUnderWaterClr), fInvTime);
		pMap->m_FogStartUW.SetTarget(m_PropertyData.m_fUnderWaterFogStart, fInvTime);
		pMap->m_FogEndUW.SetTarget(m_PropertyData.m_fUnderWaterFogEnd, fInvTime);
		pMap->m_FogDensityUW.SetTarget(m_PropertyData.m_fUnderWaterFogDeepness, fInvTime);
		
		pMap->m_FogColorUWNight.SetTarget(ColorRGBAToColorValue(m_PropertyData.m_dwUnderWaterClrNight), fInvTime);
		pMap->m_FogStartUWNight.SetTarget(m_PropertyData.m_fUnderWaterFogStartNight, fInvTime);
		pMap->m_FogEndUWNight.SetTarget(m_PropertyData.m_fUnderWaterFogEndNight, fInvTime);
		pMap->m_FogDensityUWNight.SetTarget(m_PropertyData.m_fUnderWaterFogDeepnessNight, fInvTime);

		pMap->m_LitDir.SetTarget(ColorRGBAToColorValue(m_PropertyData.m_dwDirLightClr) * m_PropertyData.m_fSunPower, fInvTime);
		pMap->m_LitDirNight.SetTarget(ColorRGBAToColorValue(m_PropertyData.m_dwDirLightClrNight) * m_PropertyData.m_fSunPowerNight, fInvTime);

		pMap->m_LitAmb.SetTarget(ColorRGBAToColorValue(m_PropertyData.m_dwAmbient), fInvTime);
		pMap->m_LitAmbNight.SetTarget(ColorRGBAToColorValue(m_PropertyData.m_dwAmbientNight), fInvTime);

		pMap->m_CloudDir.SetTarget(ColorRGBAToColorValue(m_PropertyData.m_dwCloudDirClr), fInvTime);
		pMap->m_CloudDirNight.SetTarget(ColorRGBAToColorValue(m_PropertyData.m_dwCloudNightDirClr), fInvTime);

		pMap->m_CloudAmb.SetTarget(ColorRGBAToColorValue(m_PropertyData.m_dwCloudAmbientClr), fInvTime);
		pMap->m_CloudAmbNight.SetTarget(ColorRGBAToColorValue(m_PropertyData.m_dwCloudNightAmbientClr), fInvTime);
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		pManager->SetCurrentArea(this);
	
		//改变天空
		A3DSkySphere* pSky = pManager->GetSky();
		pSky->SetFlySpeedU(m_PropertyData.m_fCloudSpeedU);
		pSky->SetFlySpeedV(m_PropertyData.m_fCloudSpeedV);
		
		//这儿如果把转变时间给设上的话，只要盒子区域超过三个，然后较快的来回在三个盒子间
		//切换，天空渲染就会出现异常中断，估计是Engine's bug.
		//if(g_fNightFactor == 1.0f) pSky->TransSky(m_PropertyData.m_strSkyNight0,m_PropertyData.m_strSkyNight1,m_PropertyData.m_strSkyNight2,m_PropertyData.m_dwTransTime);
		//else pSky->TransSky(m_PropertyData.m_strSky0,m_PropertyData.m_strSky1,m_PropertyData.m_strSky2,m_PropertyData.m_dwTransTime);
		//m_bTransDone = false;

		if(g_fNightFactor == 1.0f) pSky->TransSky(m_PropertyData.m_strSkyNight0,m_PropertyData.m_strSkyNight1,m_PropertyData.m_strSkyNight2,0);
		else pSky->TransSky(m_PropertyData.m_strSky0,m_PropertyData.m_strSky1,m_PropertyData.m_strSky2,0);
		m_bTransDone = false;

		if(m_pSnow) m_pSnow->StartSnow();
		if(m_pRain) m_pRain->StartRain();

		CELCloudSceneRender *pCloud = pMap->GetCloudRender();
		
		if(pCloud) pCloud->SetSpriteColor(
			A3DCOLORVALUE(m_PropertyData.m_dwCloudAmbientClr)*m_PropertyData.m_fSunPower,
			A3DCOLORVALUE(m_PropertyData.m_dwCloudDirClr)*m_PropertyData.m_fSunPower);
	}
}

void CSceneBoxArea::OnCamLeave()//摄象机离开区域事件
{
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		if(m_pSnow) m_pSnow->Stop();
		if(m_pRain) m_pRain->Stop();

		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		CSceneBoxArea *pArea = pManager->GetDefaultBoxArea();
		pArea->OnCamEntry();
	}
}

void CSceneBoxArea::Render(A3DViewport* pA3DViewport)
{
	DrawCenter();
	A3DMATRIX4 mat = m_matRotate*m_matScale*m_matTrans;
	
	CBox3D renderBox;
	renderBox.Init(&g_Render,1.0f,false);
	renderBox.SetSize(m_PropertyData.m_fWidth/2.0f,m_PropertyData.m_fHeight/2.0f,m_PropertyData.m_fLength/2.0f);
	renderBox.SetTM(mat);
	renderBox.SetColor(A3DCOLORRGB(255,0,0));
	renderBox.Render();
	if(m_bSelected)
	{
		renderBox.Init(&g_Render,1.0f,true);
		renderBox.SetSize(m_PropertyData.m_fWidth/2.0f,m_PropertyData.m_fHeight/2.0f,m_PropertyData.m_fLength/2.0f);
		renderBox.SetTM(mat);
		renderBox.SetColor(A3DCOLORRGBA(0,255,0,128));
		renderBox.Render();
	}
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	pWireCollector->Flush();

	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->GetCurrentArea() == this)
		{
			g_Render.GetA3DDevice()->SetAlphaBlendEnable(true);
			g_Render.GetA3DDevice()->SetAlphaTestEnable(false);
			if(g_Configs.bShowRain)
			{
				if( m_pRain )
					m_pRain->RenderParticles(pA3DViewport);
			}
			if(g_Configs.bShowSnow)
			{
				if( m_pSnow )
					m_pSnow->RenderParticles(pA3DViewport);
			}
			g_Render.GetA3DDevice()->SetAlphaBlendEnable(false);
			g_Render.GetA3DDevice()->SetAlphaTestEnable(true);
		}
	}

	CSceneObject::Render(pA3DViewport);
}

void CSceneBoxArea::UpdateProperty(bool bGet)
{
	if(bGet)
	{
		AString temp = AString(m_pProperty->GetPropVal(0));
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
			CSceneObject*pObj = pSManager->FindSceneObject(temp);
			if(pObj != NULL && pObj != this)
			{
				MessageBox(NULL,"该对象的名字已经存在，编辑器将恢复原对象名字！",NULL,MB_OK);
				if(m_PropertyData.m_dwFlag & 0x1) bLight = true;
				else bLight = false;
				if(m_PropertyData.m_dwFlag & 0x2) bRain  = true;
				else bRain = false;
				if(m_PropertyData.m_dwFlag & 0x4) bSnow  = true;
				else bSnow = false;
				if(m_PropertyData.m_dwFlag & 0x8) bShowSun = true;
				else bShowSun = false;
				if(m_PropertyData.m_dwFlag & 0x10) bShowMoon = true;
				else bShowMoon = false;
				m_pProperty->SetPropVal(0,m_strName);
				m_pProperty->SetPropVal(1,m_vPos);
				m_pProperty->SetPropVal(2,m_PropertyData.m_fLength);
				m_pProperty->SetPropVal(3,m_PropertyData.m_fWidth);
				m_pProperty->SetPropVal(4,m_PropertyData.m_fHeight);
				m_pProperty->SetPropVal(6,m_PropertyData.m_fSunPower);
				m_pProperty->SetPropVal(7,m_PropertyData.m_fSunPowerNight);
				m_pProperty->SetPropVal(8,m_PropertyData.m_fFogStart);
				m_pProperty->SetPropVal(9,m_PropertyData.m_fFogEnd);
				m_pProperty->SetPropVal(10,m_PropertyData.m_fFogDeepness);
				m_pProperty->SetPropVal(11,m_PropertyData.m_fFogColor);
				m_pProperty->SetPropVal(12,m_PropertyData.m_fFogStartNight);
				m_pProperty->SetPropVal(13,m_PropertyData.m_fFogEndNight);
				m_pProperty->SetPropVal(14,m_PropertyData.m_fFogDeepnessNight);
				m_pProperty->SetPropVal(15,m_PropertyData.m_fFogColorNight);
				m_pProperty->SetPropVal(16,m_PropertyData.m_strSky0);
				m_pProperty->SetPropVal(17,m_PropertyData.m_strSky1);
				m_pProperty->SetPropVal(18,m_PropertyData.m_strSky2);
				m_pProperty->SetPropVal(19,m_PropertyData.m_dwDirLightClr);
				m_pProperty->SetPropVal(20,m_PropertyData.m_dwAmbient);
				m_pProperty->SetPropVal(21,m_PropertyData.m_strSkyNight0);
				m_pProperty->SetPropVal(22,m_PropertyData.m_strSkyNight1);
				m_pProperty->SetPropVal(23,m_PropertyData.m_strSkyNight2);
				m_pProperty->SetPropVal(24,m_PropertyData.m_dwDirLightClrNight);
				m_pProperty->SetPropVal(25,m_PropertyData.m_dwAmbientNight);
				m_pProperty->SetPropVal(26,m_PropertyData.m_strSound);
				
				m_pProperty->SetPropVal(27,m_PropertyData.m_fUnderWaterFogStart);
				m_pProperty->SetPropVal(28,m_PropertyData.m_fUnderWaterFogEnd);
				m_pProperty->SetPropVal(29,m_PropertyData.m_fUnderWaterFogDeepness);
				m_pProperty->SetPropVal(30,m_PropertyData.m_dwUnderWaterClr);

				m_pProperty->SetPropVal(31,m_PropertyData.m_fUnderWaterFogStartNight);
				m_pProperty->SetPropVal(32,m_PropertyData.m_fUnderWaterFogEndNight);
				m_pProperty->SetPropVal(33,m_PropertyData.m_fUnderWaterFogDeepnessNight);
				m_pProperty->SetPropVal(34,m_PropertyData.m_dwUnderWaterClrNight);

				m_pProperty->SetPropVal(35,m_PropertyData.m_dwTransTime);
				m_pProperty->SetPropVal(36,m_PropertyData.m_fCloudSpeedU);
				m_pProperty->SetPropVal(37,m_PropertyData.m_fCloudSpeedV);
				m_pProperty->SetPropVal(38,m_PropertyData.m_dwCloudDirClr);
				m_pProperty->SetPropVal(39,m_PropertyData.m_dwCloudAmbientClr);
				m_pProperty->SetPropVal(40,m_PropertyData.m_dwCloudNightDirClr);
				m_pProperty->SetPropVal(41,m_PropertyData.m_dwCloudNightAmbientClr);
				m_pProperty->SetPropVal(32,bSnow);
				m_pProperty->SetPropVal(43,bRain);
				m_pProperty->SetPropVal(44,bLight);
				m_pProperty->SetPropVal(46,bShowMoon);
				m_pProperty->SetPropVal(47,bShowSun);
				m_pNatureDataSetFunc->OnSetValue(strText);
				m_pLightingSelFunc->OnSetValue(strText);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		m_strName	= temp;
		m_vPos	= m_pProperty->GetPropVal(1);
		m_PropertyData.m_fLength = m_pProperty->GetPropVal(2);
		m_PropertyData.m_fWidth = m_pProperty->GetPropVal(3);
		m_PropertyData.m_fHeight = m_pProperty->GetPropVal(4);
	
		m_PropertyData.m_fSunPower = m_pProperty->GetPropVal(6);
		m_PropertyData.m_fSunPowerNight = m_pProperty->GetPropVal(7);

		m_PropertyData.m_fFogStart = m_pProperty->GetPropVal(8);
		m_PropertyData.m_fFogEnd = m_pProperty->GetPropVal(9);
		m_PropertyData.m_fFogDeepness = m_pProperty->GetPropVal(10);
		m_PropertyData.m_fFogColor = m_pProperty->GetPropVal(11);
		
		m_PropertyData.m_fFogStartNight = m_pProperty->GetPropVal(12);
		m_PropertyData.m_fFogEndNight = m_pProperty->GetPropVal(13);
		m_PropertyData.m_fFogDeepnessNight = m_pProperty->GetPropVal(14);
		m_PropertyData.m_fFogColorNight = m_pProperty->GetPropVal(15);

		m_PropertyData.m_strSky0   =  AString(m_pProperty->GetPropVal(16));
		m_PropertyData.m_strSky1   =  AString(m_pProperty->GetPropVal(17));
		m_PropertyData.m_strSky2   =  AString(m_pProperty->GetPropVal(18));
		
		m_PropertyData.m_dwDirLightClr = m_pProperty->GetPropVal(19);
		m_PropertyData.m_dwAmbient = m_pProperty->GetPropVal(20);
	
		m_PropertyData.m_strSkyNight0   =  AString(m_pProperty->GetPropVal(21));
		m_PropertyData.m_strSkyNight1   =  AString(m_pProperty->GetPropVal(22));
		m_PropertyData.m_strSkyNight2   =  AString(m_pProperty->GetPropVal(23));
		m_PropertyData.m_dwDirLightClrNight = m_pProperty->GetPropVal(24);
		m_PropertyData.m_dwAmbientNight = m_pProperty->GetPropVal(25);
		
		m_PropertyData.m_strSound   = AString(m_pProperty->GetPropVal(26));
		m_PropertyData.m_fUnderWaterFogStart = m_pProperty->GetPropVal(27);
		m_PropertyData.m_fUnderWaterFogEnd = m_pProperty->GetPropVal(28);
		m_PropertyData.m_fUnderWaterFogDeepness = m_pProperty->GetPropVal(29);
		m_PropertyData.m_dwUnderWaterClr = m_pProperty->GetPropVal(30);

		m_PropertyData.m_fUnderWaterFogStartNight = m_pProperty->GetPropVal(31);
		m_PropertyData.m_fUnderWaterFogEndNight = m_pProperty->GetPropVal(32);
		m_PropertyData.m_fUnderWaterFogDeepnessNight = m_pProperty->GetPropVal(33);
		m_PropertyData.m_dwUnderWaterClrNight = m_pProperty->GetPropVal(34);
		m_PropertyData.m_dwTransTime = m_pProperty->GetPropVal(35);
		if(m_PropertyData.m_dwTransTime<=0)
		{
			MessageBox(NULL,"你设置的<过渡时间>小于1！\n该设置不能小于1，程序将自动把它设置为1",NULL,MB_OK);	
			m_PropertyData.m_dwTransTime = 1;
		}
		m_PropertyData.m_fCloudSpeedU = m_pProperty->GetPropVal(36);
		m_PropertyData.m_fCloudSpeedV = m_pProperty->GetPropVal(37);
		m_PropertyData.m_dwCloudDirClr = m_pProperty->GetPropVal(38);
		m_PropertyData.m_dwCloudAmbientClr = m_pProperty->GetPropVal(39);
		m_PropertyData.m_dwCloudNightDirClr = m_pProperty->GetPropVal(40);
		m_PropertyData.m_dwCloudNightAmbientClr = m_pProperty->GetPropVal(41);
		bSnow = m_pProperty->GetPropVal(42);
		bRain = m_pProperty->GetPropVal(43);
		bLight = m_pProperty->GetPropVal(44);
		bShowMoon =	m_pProperty->GetPropVal(46);
		bShowSun = m_pProperty->GetPropVal(47);
		
		if(bSnow) m_PropertyData.m_dwFlag |=0x4;
		else m_PropertyData.m_dwFlag &=0xffffffb;
		if(bRain) m_PropertyData.m_dwFlag |=0x2;
		else m_PropertyData.m_dwFlag &=0xffffffd;
		if(bLight) m_PropertyData.m_dwFlag |=0x1;
		else m_PropertyData.m_dwFlag &=0xffffffe;

		if(bShowSun) m_PropertyData.m_dwFlag |=0x8;
		else m_PropertyData.m_dwFlag &=0xffffff7;
		if(bShowMoon) m_PropertyData.m_dwFlag |=0x10;
		else m_PropertyData.m_dwFlag &=0xfffffef;
		
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildArea();
		
		if(m_pRain)
		{
			m_pRain->SetUseRateMode(m_PropertyData.m_nRainSpeed, true);//
			m_pRain->SetSize(m_PropertyData.m_fRainSize);//
			m_pRain->SetColor(0, m_PropertyData.m_dwRainClr);//
		}
		if(m_pSnow)
		{
			m_pSnow->SetUseRateMode(m_PropertyData.m_nSnowSpeed, true);//
			m_pSnow->SetSize(m_PropertyData.m_fSnowSize);//
			m_pSnow->SetColor(0, m_PropertyData.m_dwSnowClr);//
		}

		LIGHTINGPARAMS param;
		if(m_bIsDefault)
		{
			param = pMap->GetLightingParams();
			param.dwAmbient = m_PropertyData.m_dwAmbient;
			param.dwSunCol  = m_PropertyData.m_dwDirLightClr;
			param.fSunPower = m_PropertyData.m_fSunPower;
			pMap->SetLightingParams(param);

			param = pMap->GetNightLightingParams();
			param.dwAmbient = m_PropertyData.m_dwAmbientNight;
			param.dwSunCol  = m_PropertyData.m_dwDirLightClrNight;
			param.fSunPower = m_PropertyData.m_fSunPowerNight;
			pMap->SetNightLightingParams(param);
		}
		
		if(m_bIsInside)
			OnCamEntry(true);
	}else
	{
		if(m_PropertyData.m_dwFlag & 0x1) bLight = true;
		else bLight = false;
		if(m_PropertyData.m_dwFlag & 0x2) bRain  = true;
		else bRain = false;
		if(m_PropertyData.m_dwFlag & 0x4) bSnow  = true;
		else bSnow = false;
		if(m_PropertyData.m_dwFlag & 0x8) bShowSun = true;
		else bShowSun = false;
		if(m_PropertyData.m_dwFlag & 0x10) bShowMoon = true;
		else bShowMoon = false;
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_vPos);
		m_pProperty->SetPropVal(2,m_PropertyData.m_fLength);
		m_pProperty->SetPropVal(3,m_PropertyData.m_fWidth);
		m_pProperty->SetPropVal(4,m_PropertyData.m_fHeight);
		m_pProperty->SetPropVal(6,m_PropertyData.m_fSunPower);
		m_pProperty->SetPropVal(7,m_PropertyData.m_fSunPowerNight);
		m_pProperty->SetPropVal(8,m_PropertyData.m_fFogStart);
		m_pProperty->SetPropVal(9,m_PropertyData.m_fFogEnd);
		m_pProperty->SetPropVal(10,m_PropertyData.m_fFogDeepness);
		m_pProperty->SetPropVal(11,m_PropertyData.m_fFogColor);
		m_pProperty->SetPropVal(12,m_PropertyData.m_fFogStartNight);
		m_pProperty->SetPropVal(13,m_PropertyData.m_fFogEndNight);
		m_pProperty->SetPropVal(14,m_PropertyData.m_fFogDeepnessNight);
		m_pProperty->SetPropVal(15,m_PropertyData.m_fFogColorNight);
		m_pProperty->SetPropVal(16,m_PropertyData.m_strSky0);
		m_pProperty->SetPropVal(17,m_PropertyData.m_strSky1);
		m_pProperty->SetPropVal(18,m_PropertyData.m_strSky2);
		m_pProperty->SetPropVal(19,m_PropertyData.m_dwDirLightClr);
		m_pProperty->SetPropVal(20,m_PropertyData.m_dwAmbient);
		m_pProperty->SetPropVal(21,m_PropertyData.m_strSkyNight0);
		m_pProperty->SetPropVal(22,m_PropertyData.m_strSkyNight1);
		m_pProperty->SetPropVal(23,m_PropertyData.m_strSkyNight2);
		m_pProperty->SetPropVal(24,m_PropertyData.m_dwDirLightClrNight);
		m_pProperty->SetPropVal(25,m_PropertyData.m_dwAmbientNight);
		m_pProperty->SetPropVal(26,m_PropertyData.m_strSound);
		m_pProperty->SetPropVal(27,m_PropertyData.m_fUnderWaterFogStart);
		m_pProperty->SetPropVal(28,m_PropertyData.m_fUnderWaterFogEnd);
		m_pProperty->SetPropVal(29,m_PropertyData.m_fUnderWaterFogDeepness);
		m_pProperty->SetPropVal(30,m_PropertyData.m_dwUnderWaterClr);
		m_pProperty->SetPropVal(31,m_PropertyData.m_fUnderWaterFogStartNight);
		m_pProperty->SetPropVal(32,m_PropertyData.m_fUnderWaterFogEndNight);
		m_pProperty->SetPropVal(33,m_PropertyData.m_fUnderWaterFogDeepnessNight);
		m_pProperty->SetPropVal(34,m_PropertyData.m_dwUnderWaterClrNight);
		
		m_pProperty->SetPropVal(35,m_PropertyData.m_dwTransTime);
		m_pProperty->SetPropVal(36,m_PropertyData.m_fCloudSpeedU);
		m_pProperty->SetPropVal(37,m_PropertyData.m_fCloudSpeedV);
		m_pProperty->SetPropVal(38,m_PropertyData.m_dwCloudDirClr);
		m_pProperty->SetPropVal(39,m_PropertyData.m_dwCloudAmbientClr);
		m_pProperty->SetPropVal(40,m_PropertyData.m_dwCloudNightDirClr);
		m_pProperty->SetPropVal(41,m_PropertyData.m_dwCloudNightAmbientClr);
		m_pProperty->SetPropVal(42,bSnow);
		m_pProperty->SetPropVal(43,bRain);
		m_pProperty->SetPropVal(44,bLight);
		m_pProperty->SetPropVal(46,bShowMoon);
		m_pProperty->SetPropVal(47,bShowSun);
		m_pNatureDataSetFunc->OnSetValue(strText);
		m_pLightingSelFunc->OnSetValue(strText);
	}	
}

void CSceneBoxArea::BuildAreaProperty()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置");
		
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fLength), "长");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fWidth), "宽");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fHeight), "高");
		
		m_pProperty->DynAddProperty(AVariant(strText),"灯光方向",(ASet*)m_pLightingSelFunc, NULL, WAY_CUSTOM);
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fSunPower),"(白天)亮度");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fSunPowerNight),"(夜晚)亮度");

		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fFogStart), "(白天)雾开始距离");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fFogEnd), "(白天)雾结束距离");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fFogDeepness), "(白天)雾浓度");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fFogColor), "(白天)雾颜色",NULL,NULL,WAY_COLOR);

		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fFogStartNight), "(夜晚)雾开始距离");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fFogEndNight), "(夜晚)雾结束距离");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fFogDeepnessNight), "(夜晚)雾浓度");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fFogColorNight), "(夜晚)雾颜色",NULL,NULL,WAY_COLOR);
	
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_strSky0), "(白天)天空纹理(顶)",NULL,NULL,WAY_FILENAME,"Textures\\sky");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_strSky1), "(白天)天空纹理(侧1)",NULL,NULL,WAY_FILENAME,"Textures\\sky");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_strSky2), "(白天)天空纹理(侧2)",NULL,NULL,WAY_FILENAME,"Textures\\sky");
		
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_dwDirLightClr), "(白天)直射光颜色",NULL,NULL,WAY_COLOR);
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_dwAmbient), "(白天)环境光颜色",NULL,NULL,WAY_COLOR);
		
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_strSky0), "(夜晚)天空纹理(顶)",NULL,NULL,WAY_FILENAME,"Textures\\sky");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_strSky1), "(夜晚)天空纹理(侧1)",NULL,NULL,WAY_FILENAME,"Textures\\sky");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_strSky2), "(夜晚)天空纹理(侧2)",NULL,NULL,WAY_FILENAME,"Textures\\sky");
		
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_dwDirLightClrNight), "(夜晚)直射光颜色",NULL,NULL,WAY_COLOR);
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_dwAmbientNight), "(夜晚)环境光颜色",NULL,NULL,WAY_COLOR);
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_strSound), "背景音效",NULL,NULL,WAY_FILENAME,"Music");
		
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fUnderWaterFogStart), "(白天)水下雾开始距离");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fUnderWaterFogEnd), "(白天)水下雾结束距离");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fUnderWaterFogDeepness), "(白天)水下雾浓度");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_dwUnderWaterClr), "(白天)水下雾颜色",NULL,NULL,WAY_COLOR);
		
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fUnderWaterFogStartNight), "(夜晚)水下雾开始距离");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fUnderWaterFogEndNight), "(夜晚)水下雾结束距离");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fUnderWaterFogDeepnessNight), "(夜晚)水下雾浓度");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_dwUnderWaterClrNight), "(夜晚)水下雾颜色",NULL,NULL,WAY_COLOR);
		
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_dwTransTime), "过渡时间");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fCloudSpeedU), "云层移动速度U");
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_fCloudSpeedV), "云层移动速度V");
		
		//new 05.7.16
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_dwCloudDirClr), "云层白天直射光颜色",NULL,NULL,WAY_COLOR);
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_dwCloudAmbientClr), "云层白天环境光颜色",NULL,NULL,WAY_COLOR);
		//new 05.8.8
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_dwCloudNightDirClr), "云层夜晚直射光颜色",NULL,NULL,WAY_COLOR);
		m_pProperty->DynAddProperty(AVariant(m_PropertyData.m_dwCloudNightAmbientClr), "云层夜晚环境光颜色",NULL,NULL,WAY_COLOR);

		m_pProperty->DynAddProperty(AVariant(bSnow), "下雪有效");
		m_pProperty->DynAddProperty(AVariant(bRain), "下雨有效");
		m_pProperty->DynAddProperty(AVariant(bLight), "灯光有效");
		m_pProperty->DynAddProperty(AVariant(strText),"雨雪天气数据",(ASet*)m_pNatureDataSetFunc, NULL, WAY_CUSTOM);
		m_pProperty->DynAddProperty(AVariant(bShowMoon), "是否显示月亮");
		m_pProperty->DynAddProperty(AVariant(bShowSun), "是否显示太阳");
	}
}

void CSceneBoxArea::ReBuildArea()
{
	//计算点盒子8个点
	float halfLen = m_PropertyData.m_fLength/2.0;
	float halfWidth = m_PropertyData.m_fWidth/2.0f;
	float halfHeight = m_PropertyData.m_fHeight/2.0f;

	Vertices[0] = A3DVECTOR3(-halfWidth,-halfHeight,-halfLen);
	Vertices[1] = A3DVECTOR3(halfWidth,-halfHeight,-halfLen);
	Vertices[2] = A3DVECTOR3(halfWidth,-halfHeight,halfLen);
	Vertices[3] = A3DVECTOR3(-halfWidth,-halfHeight,halfLen);

	Vertices[4] = A3DVECTOR3(-halfWidth,halfHeight,-halfLen);
	Vertices[5] = A3DVECTOR3(halfWidth,halfHeight,-halfLen);
	Vertices[6] = A3DVECTOR3(halfWidth,halfHeight,halfLen);
	Vertices[7] = A3DVECTOR3(-halfWidth,halfHeight,halfLen);

	A3DMATRIX4 mat = m_matScale*m_matRotate*m_matTrans;
	
	m_vMin = A3DVECTOR3(999999.0f,999999.0f,999999.0f);
	m_vMax = A3DVECTOR3(-999999.0f,-999999.0f,-999999.0f);

	for(int i = 0; i<8; i++)
	{
		Vertices[i] = Vertices[i] * mat;
		if(m_vMin.x > Vertices[i].x) m_vMin.x = Vertices[i].x;
		if(m_vMin.y > Vertices[i].y) m_vMin.y = Vertices[i].y;
		if(m_vMin.z > Vertices[i].z) m_vMin.z = Vertices[i].z;

		if(m_vMax.x < Vertices[i].x) m_vMax.x = Vertices[i].x;
		if(m_vMax.y < Vertices[i].y) m_vMax.y = Vertices[i].y;
		if(m_vMax.z < Vertices[i].z) m_vMax.z = Vertices[i].z;
	}
}	

//光线跟踪程序
bool CSceneBoxArea::RayTrace(A3DVECTOR3 vStart,A3DVECTOR3 vEnd)
{
	//首先投影8个顶点到2D屏幕空间
	A3DVECTOR3 v2DVertices[8];
	A3DAABB aabb;
	
	A3DVECTOR3 vMin,vMax;
	vMax = vMin = Vertices[0];
	for(int i = 1; i<8; i++)
	{
		if(vMin.x > Vertices[i].x) vMin.x = Vertices[i].x;
		if(vMin.y > Vertices[i].y) vMin.y = Vertices[i].y;
		if(vMin.z > Vertices[i].z) vMin.z = Vertices[i].z;

		if(vMax.x < Vertices[i].x) vMax.x = Vertices[i].x;
		if(vMax.y < Vertices[i].y) vMax.y = Vertices[i].y;
		if(vMax.z < Vertices[i].z) vMax.z = Vertices[i].z;
	}
	
	aabb.Mins = vMin; aabb.Maxs = vMax;
	if(g_Render.GetA3DEngine()->GetActiveCamera()->GetWorldFrustum()->AABBInFrustum(aabb))
	{
		
		for(int i = 0; i<8; i++)
			g_Render.GetA3DEngine()->GetActiveViewport()->Transform(Vertices[i], v2DVertices[i]);
		
		//分别测试12条边是否与光线相交，如果相交返回TRUE
		WORD indices[12][2] = { {0,1} , {1,2} , {2,3} , {3,0} ,
		{4,5} , {5,6} , {6,7} , {7, 4}, {0,4} , {2,5} , {3,6} , {4,7}};
		
		for(i = 0; i<8; i++)
		{
			
			int x[4],y[4];
			x[0] = Vertices[indices[i][0]].x;
			y[0] = Vertices[indices[i][0]].z;
			
			x[1] = Vertices[indices[i][1]].x;
			y[1] = Vertices[indices[i][1]].z;
			
			//
			//int dotX,dotY;//鼠标点
			//bool bResult = IntersectionLineWithDot(x[0],y[0],x[1],y[1],dotX,dotY);
			//if(bResult) return true;
		}
	}
	return false;
}

bool CSceneBoxArea::IntersectionLineWithDot(int x1, int y1, int x2, int y2, int dotX, int dotY)
{
	
	return false;
}

bool CSceneBoxArea::IsInside(A3DVECTOR3 vPoint)
{
	
	if(!DotWithPlane(vPoint,Vertices[5],Vertices[1],Vertices[0]))
		return false;
	if(!DotWithPlane(vPoint,Vertices[6],Vertices[2],Vertices[1]))
		return false;
	if(!DotWithPlane(vPoint,Vertices[7],Vertices[3],Vertices[2]))
		return false;
	if(!DotWithPlane(vPoint,Vertices[4],Vertices[0],Vertices[3]))
		return false;
	if(!DotWithPlane(vPoint,Vertices[7],Vertices[6],Vertices[5]))
		return false;
	if(!DotWithPlane(vPoint,Vertices[0],Vertices[1],Vertices[2]))
		return false;
	
	return true;
}

bool CSceneBoxArea::DotWithPlane(A3DVECTOR3 testPoint,A3DVECTOR3 point1, A3DVECTOR3 point2, A3DVECTOR3 point3)
{
	A3DVECTOR3 Normal;
	//计算法线
	Normal = CrossProduct((point2 - point1),(point3 - point1));
	Normal.Normalize();

	float D = -DotProduct(point1,Normal);

	float R = DotProduct(testPoint,Normal) + D;
	if(R < -0.001f) return true;
	else return false;
}

void CSceneBoxArea::Translate(const A3DVECTOR3& vDelta)
{
	A3DMATRIX4 mat;
	if(m_bIsDefault) mat.Translate(0,vDelta.y,0);
	else mat.Translate(vDelta.x,vDelta.y,vDelta.z);
	m_matTrans = m_matTrans*mat;
	m_vPos = m_matTrans.GetRow(3);
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	ReBuildArea();
};

void CSceneBoxArea::RotateX(float fRad)
{
	if(m_bIsDefault) return;
	A3DMATRIX4 mat;
	mat.RotateX(fRad);
	m_matRotate = m_matRotate*mat;
	m_vDirection = A3DVECTOR3(0,0,1.0f)*m_matRotate;
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	ReBuildArea();
};

void CSceneBoxArea::RotateY(float fRad)
{
	if(m_bIsDefault) return;
	A3DMATRIX4 mat;
	mat.RotateY(fRad);
	m_matRotate = m_matRotate*mat;
	m_vDirection = A3DVECTOR3(0,0,1.0f)*m_matRotate;
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	ReBuildArea();
};

void CSceneBoxArea::RotateZ(float fRad)
{
	if(m_bIsDefault) return;
	A3DMATRIX4 mat;
	mat.RotateZ(fRad);
	m_matRotate = m_matRotate*mat;
	m_vDirection = A3DVECTOR3(0,0,1.0f)*m_matRotate;
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	ReBuildArea();
};

void CSceneBoxArea::Rotate(const A3DVECTOR3& axis,float fRad)
{
	if(m_bIsDefault) return;
	A3DMATRIX4 mat;
	mat.RotateAxis(axis,fRad);
	m_matRotate = m_matRotate*mat;
	m_vDirection = A3DVECTOR3(0,0,1.0f)*m_matRotate;
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	ReBuildArea();
};

void CSceneBoxArea::Scale(float fScale)
{
	A3DMATRIX4 mat;
	mat.Scale(fScale,fScale,fScale);
	m_matScale= m_matScale*mat;
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	ReBuildArea();
};

void CSceneBoxArea::SetPos(const A3DVECTOR3& vPos)
{
	if(m_bIsDefault) m_matTrans.Translate(0,vPos.y,0);
	else m_matTrans.Translate(vPos.x,vPos.y,vPos.z);
	m_vPos = vPos;
	m_matAbs = m_matScale * m_matRotate * m_matTrans;
	ReBuildArea();
}

//判断区域是否跟该区域相交
bool CSceneBoxArea::IsIntersectionWithArea(const A3DVECTOR3& vMin, const A3DVECTOR3& vMax)
{
	if(m_vMin.x>vMax.x) return false;
	if(m_vMin.z>vMax.z) return false;
	if(m_vMax.x<vMin.x) return false;
	if(m_vMax.z<vMin.z) return false;
	return true;
}

void CSceneBoxArea::InitMusic()
{
	/*
	if(m_PropertyData.m_strMusic.IsEmpty()) return;
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		AMSoundStream *pBackMusic = pManager->GetMusicInterface();
		pBackMusic->Stop();
		AString strPath = AString("Music\\") + m_PropertyData.m_strMusic;
		if (!pBackMusic->Init(g_Render.GetA3DEngine()->GetAMEngine()->GetAMSoundEngine(), strPath))
		{
			pBackMusic->Release();
			return;
		}
	}
	*/
}

void CSceneBoxArea::PlayMusic()
{
	/*
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		AMSoundStream *pBackMusic = pManager->GetMusicInterface();
		pBackMusic->Play();
	}
	*/
}

void CSceneBoxArea::StopMusic()
{
	/*
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		AMSoundStream *pBackMusic = pManager->GetMusicInterface();
		pBackMusic->Stop();
	}
	*/
}

void CSceneBoxArea::EntryScene()
{
}

void CSceneBoxArea::LeaveScene()
{
	OnCamLeave();
}

CSceneObject* CSceneBoxArea::CopyObject()
{
	if(m_bIsDefault) return NULL;
	CSceneBoxArea *pNewObject = new CSceneBoxArea(*this);
	ASSERT(pNewObject);
	pNewObject->m_pProperty = new ADynPropertyObject();	
	pNewObject->BuildAreaProperty();
	int BoxAreaNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("BoxArea_%d",BoxAreaNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				BoxAreaNum++;
			}else 
			{
				BoxAreaNum++;
				break;
			}
		}
		pNewObject->m_nObjectID = pSManager->GenerateObjectID();
		pSManager->InsertSceneObject(pNewObject);
		pSManager->AddObjectPtrToSelected(pNewObject);
		pNewObject->SetObjectName(name);
		pNewObject->SetPos(m_vPos);
		
		pNewObject->UpdateProperty(false);
		AUX_GetMainFrame()->GetToolTabWnd()->ShowPropertyPannel(pNewObject->m_pProperty);//把灯光的属性给对象属性表
		AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);//输出属性数据
	}
	return pNewObject;
}

//Load data
bool CSceneBoxArea::Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags) 
{ 
	DWORD temp_clr;
	A3DVECTOR3 vr,vs,vu,vl;
	if(dwVersion == 2)
	{
		// 2.0版本开放
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsDefault,sizeof(bool)))
			return false;

		m_PropertyData.m_strSound = AString(ar.ReadString());
		m_PropertyData.m_strSky0  =  AString(ar.ReadString());
		m_PropertyData.m_strSky1  =  AString(ar.ReadString());
		m_PropertyData.m_strSky2  =  AString(ar.ReadString());
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildArea();
	}

	if(dwVersion > 2 && dwVersion<=0xd)
	{
		// 3.0版本开放
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwTransTime,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedU,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedV,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogStart,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogEnd,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogDeepness,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwUnderWaterClr,sizeof(DWORD)))//
			return false;

		if(!ar.Read(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsDefault,sizeof(bool)))
			return false;

		m_PropertyData.m_strSound = AString(ar.ReadString());
		m_PropertyData.m_strSky0  =  AString(ar.ReadString());
		m_PropertyData.m_strSky1  =  AString(ar.ReadString());
		m_PropertyData.m_strSky2  =  AString(ar.ReadString());
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildArea();
		
	}

	if(dwVersion==0xe)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwTransTime,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedU,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedV,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogStart,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogEnd,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogDeepness,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwUnderWaterClr,sizeof(DWORD)))//
			return false;

		if(!ar.Read(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsDefault,sizeof(bool)))
			return false;

		m_PropertyData.m_strSound = AString(ar.ReadString());
		m_PropertyData.m_strSky0  =  AString(ar.ReadString());
		m_PropertyData.m_strSky1  =  AString(ar.ReadString());
		m_PropertyData.m_strSky2  =  AString(ar.ReadString());
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildArea();	
	}else if(dwVersion>0xe && dwVersion<=0xf)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwTransTime,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedU,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedV,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogStart,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogEnd,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogDeepness,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwUnderWaterClr,sizeof(DWORD)))//
			return false;

		if(!ar.Read(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsDefault,sizeof(bool)))
			return false;

		if(!ar.Read(&m_PropertyData.m_dwAmbientNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClrNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStartNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEndNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepnessNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogColorNight,sizeof(DWORD)))
			return false;

		m_PropertyData.m_strSound = AString(ar.ReadString());
		m_PropertyData.m_strSky0  =  AString(ar.ReadString());
		m_PropertyData.m_strSky1  =  AString(ar.ReadString());
		m_PropertyData.m_strSky2  =  AString(ar.ReadString());

		m_PropertyData.m_strSkyNight0  =  AString(ar.ReadString());
		m_PropertyData.m_strSkyNight1  =  AString(ar.ReadString());
		m_PropertyData.m_strSkyNight2  =  AString(ar.ReadString());
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildArea();	
	}else if(dwVersion>0xf && dwVersion<=0x10)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwTransTime,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedU,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedV,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogStart,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogEnd,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogDeepness,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwUnderWaterClr,sizeof(DWORD)))//
			return false;

		if(!ar.Read(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsDefault,sizeof(bool)))
			return false;

		if(!ar.Read(&m_PropertyData.m_dwAmbientNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClrNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStartNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEndNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepnessNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogColorNight,sizeof(DWORD)))
			return false;

		//Nature
		if(!ar.Read(&m_PropertyData.m_nRainSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(FLOAT)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_nSnowSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(DWORD)))
			return false;

		m_PropertyData.m_strSound =  AString(ar.ReadString());
		m_PropertyData.m_strSky0  =  AString(ar.ReadString());
		m_PropertyData.m_strSky1  =  AString(ar.ReadString());
		m_PropertyData.m_strSky2  =  AString(ar.ReadString());

		m_PropertyData.m_strSkyNight0  =  AString(ar.ReadString());
		m_PropertyData.m_strSkyNight1  =  AString(ar.ReadString());
		m_PropertyData.m_strSkyNight2  =  AString(ar.ReadString());
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildArea();	
	}else if(dwVersion > 0x10 && dwVersion <= 0x1c)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwTransTime,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedU,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedV,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogStart,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogEnd,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogDeepness,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwUnderWaterClr,sizeof(DWORD)))//
			return false;

		if(!ar.Read(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsDefault,sizeof(bool)))
			return false;

		if(!ar.Read(&m_PropertyData.m_dwAmbientNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClrNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStartNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEndNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepnessNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogColorNight,sizeof(DWORD)))
			return false;

		//Nature
		if(!ar.Read(&m_PropertyData.m_nRainSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(FLOAT)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_nSnowSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_vLightDir,sizeof(A3DVECTOR3)))
			return false;

		m_PropertyData.m_strSound = AString(ar.ReadString());
		m_PropertyData.m_strSky0  =  AString(ar.ReadString());
		m_PropertyData.m_strSky1  =  AString(ar.ReadString());
		m_PropertyData.m_strSky2  =  AString(ar.ReadString());

		m_PropertyData.m_strSkyNight0  =  AString(ar.ReadString());
		m_PropertyData.m_strSkyNight1  =  AString(ar.ReadString());
		m_PropertyData.m_strSkyNight2  =  AString(ar.ReadString());
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildArea();	
	} if( dwVersion > 0x1c && dwVersion <= 0x1e)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwTransTime,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedU,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedV,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogStart,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogEnd,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogDeepness,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwUnderWaterClr,sizeof(DWORD)))//
			return false;

		if(!ar.Read(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsDefault,sizeof(bool)))
			return false;

		if(!ar.Read(&m_PropertyData.m_dwAmbientNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClrNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStartNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEndNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepnessNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogColorNight,sizeof(DWORD)))
			return false;

		//Nature
		if(!ar.Read(&m_PropertyData.m_nRainSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(FLOAT)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_nSnowSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_vLightDir,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSunPower,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSunPowerNight,sizeof(float)))
			return false;

		m_PropertyData.m_strSound = AString(ar.ReadString());
		m_PropertyData.m_strSky0  =  AString(ar.ReadString());
		m_PropertyData.m_strSky1  =  AString(ar.ReadString());
		m_PropertyData.m_strSky2  =  AString(ar.ReadString());

		m_PropertyData.m_strSkyNight0  =  AString(ar.ReadString());
		m_PropertyData.m_strSkyNight1  =  AString(ar.ReadString());
		m_PropertyData.m_strSkyNight2  =  AString(ar.ReadString());
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildArea();
	} if( dwVersion > 0x1e && dwVersion <= 0x25)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwTransTime,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedU,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedV,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogStart,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogEnd,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogDeepness,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwUnderWaterClr,sizeof(DWORD)))//
			return false;

		if(!ar.Read(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsDefault,sizeof(bool)))
			return false;

		if(!ar.Read(&m_PropertyData.m_dwAmbientNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClrNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStartNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEndNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepnessNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogColorNight,sizeof(DWORD)))
			return false;

		//Nature
		if(!ar.Read(&m_PropertyData.m_nRainSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(FLOAT)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_nSnowSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_vLightDir,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSunPower,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSunPowerNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwFlag,sizeof(DWORD)))
			return false;
		if(dwVersion != 0x1f)
		{
			m_PropertyData.m_strSound = AString(ar.ReadString());
			m_PropertyData.m_strSky0  =  AString(ar.ReadString());
			m_PropertyData.m_strSky1  =  AString(ar.ReadString());
			m_PropertyData.m_strSky2  =  AString(ar.ReadString());

			m_PropertyData.m_strSkyNight0  =  AString(ar.ReadString());
			m_PropertyData.m_strSkyNight1  =  AString(ar.ReadString());
			m_PropertyData.m_strSkyNight2  =  AString(ar.ReadString());
		}
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildArea();
	}if( dwVersion > 0x25 && dwVersion < 0x28 )
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwTransTime,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedU,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedV,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogStart,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogEnd,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogDeepness,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwUnderWaterClr,sizeof(DWORD)))//
			return false;

		if(!ar.Read(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsDefault,sizeof(bool)))
			return false;

		if(!ar.Read(&m_PropertyData.m_dwAmbientNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClrNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&temp_clr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStartNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEndNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepnessNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogColorNight,sizeof(DWORD)))
			return false;

		//Nature
		if(!ar.Read(&m_PropertyData.m_nRainSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(FLOAT)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_nSnowSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_vLightDir,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSunPower,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSunPowerNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwFlag,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwCloudAmbientClr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwCloudDirClr,sizeof(DWORD)))
			return false;
		if(dwVersion != 0x1f)
		{
			m_PropertyData.m_strSound = AString(ar.ReadString());
			m_PropertyData.m_strSky0  =  AString(ar.ReadString());
			m_PropertyData.m_strSky1  =  AString(ar.ReadString());
			m_PropertyData.m_strSky2  =  AString(ar.ReadString());

			m_PropertyData.m_strSkyNight0  =  AString(ar.ReadString());
			m_PropertyData.m_strSkyNight1  =  AString(ar.ReadString());
			m_PropertyData.m_strSkyNight2  =  AString(ar.ReadString());
		}
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildArea();
	}if( dwVersion == 0x28 )
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwTransTime,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedU,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedV,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogStart,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogEnd,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogDeepness,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwUnderWaterClr,sizeof(DWORD)))//
			return false;

		if(!ar.Read(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsDefault,sizeof(bool)))
			return false;

		if(!ar.Read(&m_PropertyData.m_dwAmbientNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClrNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStartNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEndNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepnessNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogColorNight,sizeof(DWORD)))
			return false;

		//Nature
		if(!ar.Read(&m_PropertyData.m_nRainSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(FLOAT)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_nSnowSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_vLightDir,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSunPower,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSunPowerNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwFlag,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwCloudAmbientClr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwCloudDirClr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwCloudNightAmbientClr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwCloudNightDirClr,sizeof(DWORD)))
			return false;
		if(dwVersion != 0x1f)
		{
			m_PropertyData.m_strSound = AString(ar.ReadString());
			m_PropertyData.m_strSky0  =  AString(ar.ReadString());
			m_PropertyData.m_strSky1  =  AString(ar.ReadString());
			m_PropertyData.m_strSky2  =  AString(ar.ReadString());

			m_PropertyData.m_strSkyNight0  =  AString(ar.ReadString());
			m_PropertyData.m_strSkyNight1  =  AString(ar.ReadString());
			m_PropertyData.m_strSkyNight2  =  AString(ar.ReadString());
		}
		
		m_matTrans.SetRow(3,m_vPos);
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildArea();
	}if( dwVersion > 0x28)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClr,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwTransTime,sizeof(DWORD)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedU,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fCloudSpeedV,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogStart,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogEnd,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogDeepness,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwUnderWaterClr,sizeof(DWORD)))//
			return false;

		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogStartNight,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogEndNight,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_fUnderWaterFogDeepnessNight,sizeof(float)))//
			return false;
		if(!ar.Read(&m_PropertyData.m_dwUnderWaterClrNight,sizeof(DWORD)))//
			return false;

		if(!ar.Read(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Read(&m_bIsDefault,sizeof(bool)))
			return false;

		if(!ar.Read(&m_PropertyData.m_dwAmbientNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwDirLightClrNight,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogStartNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogEndNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogDeepnessNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fFogColorNight,sizeof(DWORD)))
			return false;

		//Nature
		if(!ar.Read(&m_PropertyData.m_nRainSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(FLOAT)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fRainSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_nSnowSpeed,sizeof(int)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSnowSize,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_vLightDir,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSunPower,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_fSunPowerNight,sizeof(float)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwFlag,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwCloudAmbientClr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwCloudDirClr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwCloudNightAmbientClr,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_PropertyData.m_dwCloudNightDirClr,sizeof(DWORD)))
			return false;
		if(dwVersion != 0x1f)
		{
			m_PropertyData.m_strSound = AString(ar.ReadString());
			m_PropertyData.m_strSky0  =  AString(ar.ReadString());
			m_PropertyData.m_strSky1  =  AString(ar.ReadString());
			m_PropertyData.m_strSky2  =  AString(ar.ReadString());

			m_PropertyData.m_strSkyNight0  =  AString(ar.ReadString());
			m_PropertyData.m_strSkyNight1  =  AString(ar.ReadString());
			m_PropertyData.m_strSkyNight2  =  AString(ar.ReadString());
		}
		
		m_matTrans.SetRow(3,m_vPos);
		if(!m_bIsDefault)
		{
			m_matRotate.SetRow(2,vr);
			m_matRotate.SetRow(1,vu);
			m_matRotate.SetRow(0,vl);
		}
		m_matScale._11 = vs.x;
		m_matScale._22 = vs.y;
		m_matScale._33 = vs.z;
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildArea();
	}
	
	if(dwVersion <= 0x1d)
	{
		if(m_bIsDefault)
		{
			m_vPos.x = 0.0f;
			m_vPos.z = 0.0f;
			m_matTrans.SetRow(3,m_vPos);
			m_matAbs = m_matScale * m_matRotate * m_matTrans;
		}
		m_PropertyData.m_fCloudSpeedU = 0.02f;
		m_PropertyData.m_fCloudSpeedV = 0.02F;
	}

	if(dwVersion <= 0x2e) 
	{
		m_PropertyData.m_dwFlag |= 24;
	}

	if(iLoadFlags!= LOAD_EXPLIGHTMAP) InitNatureObjects();
	return true;
}

//Save data
bool CSceneBoxArea::Save(CELArchive& ar,DWORD dwVersion) 
{ 
	
	if(dwVersion == 2)
	{//2.0版本开放
		ar.WriteString(m_strName);
	
		if(!ar.Write(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		A3DVECTOR3 vr = m_matRotate.GetRow(2);
		A3DVECTOR3 vu = m_matRotate.GetRow(1);
		A3DVECTOR3 vl = m_matRotate.GetRow(0);
		A3DVECTOR3 vs = A3DVECTOR3(m_matScale._11,m_matScale._22,m_matScale._33);
		if(!ar.Write(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Write(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;

		if(!ar.Write(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Write(&m_bIsDefault,sizeof(bool)))
			return false;
		
		ar.WriteString(m_PropertyData.m_strSound);
		ar.WriteString(m_PropertyData.m_strSky0);
		ar.WriteString(m_PropertyData.m_strSky1);
		ar.WriteString(m_PropertyData.m_strSky2);
		
		
		return true; 
	}

	if(dwVersion > 2)
	{//3.0版本开放
		ar.WriteString(m_strName);
		if(!ar.Write(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Write(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		A3DVECTOR3 vr = m_matRotate.GetRow(2);
		A3DVECTOR3 vu = m_matRotate.GetRow(1);
		A3DVECTOR3 vl = m_matRotate.GetRow(0);
		A3DVECTOR3 vs = A3DVECTOR3(m_matScale._11,m_matScale._22,m_matScale._33);
		if(!ar.Write(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vs,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Write(&m_PropertyData.m_dwAmbient,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_PropertyData.m_dwDirLightClr,sizeof(DWORD)))//
			return false;
		//if(!ar.Write(&m_PropertyData.m_dwTerrainLight,sizeof(DWORD)))//
		//	return false;
		if(!ar.Write(&m_PropertyData.m_dwTransTime,sizeof(DWORD)))//
			return false;
		if(!ar.Write(&m_PropertyData.m_fCloudSpeedU,sizeof(float)))//
			return false;
		if(!ar.Write(&m_PropertyData.m_fCloudSpeedV,sizeof(float)))//
			return false;
		if(!ar.Write(&m_PropertyData.m_fUnderWaterFogStart,sizeof(float)))//
			return false;
		if(!ar.Write(&m_PropertyData.m_fUnderWaterFogEnd,sizeof(float)))//
			return false;
		if(!ar.Write(&m_PropertyData.m_fUnderWaterFogDeepness,sizeof(float)))//
			return false;
		if(!ar.Write(&m_PropertyData.m_dwUnderWaterClr,sizeof(DWORD)))//
			return false;
		
		if(!ar.Write(&m_PropertyData.m_fUnderWaterFogStartNight,sizeof(float)))//
			return false;
		if(!ar.Write(&m_PropertyData.m_fUnderWaterFogEndNight,sizeof(float)))//
			return false;
		if(!ar.Write(&m_PropertyData.m_fUnderWaterFogDeepnessNight,sizeof(float)))//
			return false;
		if(!ar.Write(&m_PropertyData.m_dwUnderWaterClrNight,sizeof(DWORD)))//
			return false;


		if(!ar.Write(&m_PropertyData.m_fFogColor,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fFogDeepness,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fFogEnd,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fFogStart,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fHeight,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fLength,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fWidth,sizeof(float)))
			return false;
		if(!ar.Write(&m_bIsDefault,sizeof(bool)))
			return false;

		if(!ar.Write(&m_PropertyData.m_dwAmbientNight,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_PropertyData.m_dwDirLightClrNight,sizeof(DWORD)))
			return false;
		//if(!ar.Write(&m_PropertyData.m_dwTerrainLightNight,sizeof(DWORD)))
		//	return false;
		if(!ar.Write(&m_PropertyData.m_fFogStartNight,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fFogEndNight,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fFogDeepnessNight,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fFogColorNight,sizeof(DWORD)))
			return false;
		//Nature
		if(!ar.Write(&m_PropertyData.m_nRainSpeed,sizeof(int)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fRainSize,sizeof(FLOAT)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fRainSize,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_PropertyData.m_nSnowSpeed,sizeof(int)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fSnowSize,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fSnowSize,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_PropertyData.m_vLightDir,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fSunPower,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_fSunPowerNight,sizeof(float)))
			return false;
		if(!ar.Write(&m_PropertyData.m_dwFlag,sizeof(DWORD)))
			return false;
		
		if(!ar.Write(&m_PropertyData.m_dwCloudAmbientClr,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_PropertyData.m_dwCloudDirClr,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_PropertyData.m_dwCloudNightAmbientClr,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_PropertyData.m_dwCloudNightDirClr,sizeof(DWORD)))
			return false;
		ar.WriteString(m_PropertyData.m_strSound);
		
		ar.WriteString(m_PropertyData.m_strSky0);
		
		ar.WriteString(m_PropertyData.m_strSky1);
		
		ar.WriteString(m_PropertyData.m_strSky2);

		ar.WriteString(m_PropertyData.m_strSkyNight0);
		
		ar.WriteString(m_PropertyData.m_strSkyNight1);
		
		ar.WriteString(m_PropertyData.m_strSkyNight2);
		
		
		return true; 
	}
	return true;
}

void CSceneBoxArea::Release()
{
	if( m_pRain )
	{
		m_pRain->Release();
		delete m_pRain;
		m_pRain = NULL;
	}

	if( m_pSnow )
	{
		m_pSnow->Release();
		delete m_pSnow;
		m_pSnow = NULL;
	}

	if(m_pNatureDataSetFunc) delete m_pNatureDataSetFunc;
	if(m_pLightingSelFunc) delete m_pLightingSelFunc;
	CSceneObject::Release();
}

bool CSceneBoxArea::InitNatureObjects()
{
	if( m_pRain )
	{
		m_pRain->Release();
		delete m_pRain;
	}

	if( m_pSnow )
	{
		m_pSnow->Release();
		delete m_pSnow;
	}
	
	m_pRain = new A3DRain();
	if(m_pRain==NULL)
	{
		g_Log.Log("CSceneBoxArea::InitNatureObjects,No enough memory!");
		return false;
	}
	if( !m_pRain->Init(g_Render.GetA3DDevice(), 1.0f) )
	{
		g_Log.Log("CSceneBoxArea::InitNatureObjects");
		return false;
	}
	m_pRain->SetHostCamera(g_Render.GetA3DEngine()->GetActiveCamera());
	m_pRain->SetEmitterWidth(71.0f);
	m_pRain->SetEmitterLength(71.0f);
	m_pRain->SetUseRateMode(m_PropertyData.m_nRainSpeed, true);//
	m_pRain->SetSize(m_PropertyData.m_fRainSize);//
	m_pRain->SetSizeVariation(30.0f);
	m_pRain->SetSpeed(2.2f);
	m_pRain->SetSpeedVariation(50.0f);
	m_pRain->SetLife(30);
	m_pRain->SetTextureMap("rain.tga");
	m_pRain->SetSrcBlend(A3DBLEND_SRCALPHA);
	m_pRain->SetDestBlend(A3DBLEND_INVSRCALPHA);
	m_pRain->CreateRain();
	m_pRain->SetColorNum(1);
	m_pRain->SetColor(0, m_PropertyData.m_dwRainClr);//
	
	m_pSnow = new A3DSnow();
	if(m_pSnow==NULL)
	{
		g_Log.Log("CSceneBoxArea::InitNatureObjects,No enough memory!");
		return false;
	}
	if( !m_pSnow->Init(g_Render.GetA3DDevice(), 1.8f) )
	{
		g_Log.Log("CSceneBoxArea::InitNatureObjects, failed to init snow interface!");
		return false;
	}
	
	m_pSnow->SetHostCamera((A3DCamera*)(g_Render.GetA3DEngine()->GetActiveCamera()));
	m_pSnow->SetEmitterWidth(150.0f);
	m_pSnow->SetEmitterLength(150.0f);
	m_pSnow->SetEmitterHeight(70.0f);
	m_pSnow->SetUseRateMode(m_PropertyData.m_nSnowSpeed, true);
	m_pSnow->SetSize(m_PropertyData.m_fSnowSize);
	m_pSnow->SetSizeVariation(20.0f);
	m_pSnow->SetSpeed(0.9f);
	m_pSnow->SetSpeedVariation(50.0f);
	m_pSnow->SetLife(200);
	m_pSnow->SetTextureMap("Snow.bmp");
	m_pSnow->SetSrcBlend(A3DBLEND_ONE);
	m_pSnow->SetDestBlend(A3DBLEND_INVSRCCOLOR);
	m_pSnow->SetSpinTime(100);
	m_pSnow->SetSpinTimeVariation(50);
	m_pSnow->CreateSnow();
	m_pSnow->SetColorNum(1);
	m_pSnow->SetColor(0, m_PropertyData.m_dwSnowClr);
	return true;
}

BOOL CALLBACK NATURE_DATA_SET_FUNCS::OnActivate(void)
{
	m_strText = "...";
	CSceneBoxArea* pObj = NULL;
	CString strID;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			pObj = (CSceneBoxArea*)pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_AREA)
			{
				CNatureDataSetDlg dlg;
				dlg.m_NatureData[TYPE_RAIN].nSpeed = pObj->m_PropertyData.m_nRainSpeed;
				dlg.m_NatureData[TYPE_RAIN].fSize  = pObj->m_PropertyData.m_fRainSize;
				dlg.m_NatureData[TYPE_RAIN].dwColor = pObj->m_PropertyData.m_dwRainClr;
				dlg.m_NatureData[TYPE_SNOW].nSpeed = pObj->m_PropertyData.m_nSnowSpeed;
				dlg.m_NatureData[TYPE_SNOW].fSize  = pObj->m_PropertyData.m_fSnowSize;
				dlg.m_NatureData[TYPE_SNOW].dwColor =pObj->m_PropertyData.m_dwSnowClr;
				if(IDOK==dlg.DoModal())
				{
					pObj->m_PropertyData.m_nRainSpeed = dlg.m_NatureData[TYPE_RAIN].nSpeed; 
					pObj->m_PropertyData.m_fRainSize = dlg.m_NatureData[TYPE_RAIN].fSize;  
					pObj->m_PropertyData.m_dwRainClr = dlg.m_NatureData[TYPE_RAIN].dwColor; 
					pObj->m_PropertyData.m_nSnowSpeed = dlg.m_NatureData[TYPE_SNOW].nSpeed; 
					pObj->m_PropertyData.m_fSnowSize = dlg.m_NatureData[TYPE_SNOW].fSize;  
					pObj->m_PropertyData.m_dwSnowClr = dlg.m_NatureData[TYPE_SNOW].dwColor;
				}
			}
		}
	}
	
	return TRUE;
}

LPCTSTR CALLBACK NATURE_DATA_SET_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK NATURE_DATA_SET_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK NATURE_DATA_SET_FUNCS::OnSetValue(const AVariant& var)
{
	m_strText = "...";
}

BOOL CALLBACK LIGHTING_SET_FUNCS::OnActivate(void)
{
	m_strText = "...";
	CSceneBoxArea* pObj = NULL;
	CString strID;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		CSceneObjectManager *pManager = pMap->GetSceneObjectMan();
		if(pManager->m_listSelectedObject.GetCount()==1)
		{
			pObj = (CSceneBoxArea*)pManager->m_listSelectedObject.GetHead();
			if(pObj->GetObjectType()==CSceneObject::SO_TYPE_AREA)
			{
				BOX_AREA_PROPERTY pro;
				pObj->GetProperty(pro);
				CDlgLighting::INITDATA data;
				data.bVecDir = true;
				data.iSunDir = 0;
				data.iSunHei = 0;
				data.vSunDir = pro.m_vLightDir;
				data.dwAmbient = pro.m_dwAmbient;
				data.dwSunCol = pro.m_dwDirLightClr;
				data.iMapSize = 1024;
				data.fSunPower = 1.0f;
				CDlgLighting dlg(data);
				dlg.SetModifyDirOnly(true);
				if(IDOK==dlg.DoModal())
				{
					LIGHTINGPARAMS param = dlg.GetParams();
					pro.m_vLightDir = param.vSunDir;
					pObj->SetProperty(pro);
					if(pObj->IsDefaultArea())
					{
						param = pMap->GetLightingParams();
						param.vSunDir = pro.m_vLightDir;
						pMap->SetLightingParams(param);
					}
				}
			}
		}
	}
	
	return TRUE;
}

LPCTSTR CALLBACK LIGHTING_SET_FUNCS::OnGetShowString(void) const
{
	return m_strText;
}

AVariant CALLBACK LIGHTING_SET_FUNCS::OnGetValue(void) const
{
	return m_var;
}

void CALLBACK LIGHTING_SET_FUNCS::OnSetValue(const AVariant& var)
{
	m_strText = "...";
}




