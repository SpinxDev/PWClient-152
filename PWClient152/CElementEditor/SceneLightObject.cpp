// SceneLightObject.cpp: implementation of the CSceneLightObject class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "SceneLightObject.h"
#include "Render.h"
#include "MainFrm.h"
#include "SceneObjectManager.h"
#include "Staff.h"
#include "Box3D.h"
#include "A3D.h"

//#define new A_DEBUG_NEW

#define MAX_NUM_SECT  24
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


void Init_Light_Data(LIGHTDATA &data)
{
	
	data.m_nType = 0;
	data.m_fRange     = 5.0f;
	data.m_dwColor    = A3DCOLORRGB(255,255,255);
	data.m_dwAbient   = A3DCOLORRGB(0,0,0);
	data.m_fFalloff   = 1.0f;
    data.m_fAttenuation0 = 0.0f;
    data.m_fAttenuation1 = 0.5f;
    data.m_fAttenuation2 = 0.0f;
    data.m_fTheta = 30.0f;
    data.m_fPhi = 60.0f;	
	data.m_fColorScale = 1.0f;
	data.m_vDir = A3DVECTOR3(0.0f,0.0f,1.0f);
	data.m_nEffectType = 2;
}

CSceneLightObject::CSceneLightObject()
{

	m_nObjectType = SO_TYPE_LIGHT;
	Init_Light_Data(m_Data);

	light_type.AddElement("点光原", 0);
	light_type.AddElement("方向光", 1);
	light_type.AddElement("聚光灯", 2);
	light_effect_type.AddElement("白天", 0);
	light_effect_type.AddElement("黑夜", 1);
	light_effect_type.AddElement("白天黑夜", 2);
	m_bSelected = false;
	
	BuildLightProperty();
}

CSceneLightObject::~CSceneLightObject()
{
}

AString CSceneLightObject::GetObjectName()
{
	return m_strName;
}

void CSceneLightObject::SetObjectName(const AString& name)
{
	m_strName = name;
}

void CSceneLightObject::Render(A3DViewport* pA3DViewport)
{
	Draw();
	CSceneObject::Render(pA3DViewport);
}

void CSceneLightObject::Draw()
{
	if(m_bSelected)
	{
		DrawCenter();
		//绘制灯光范围
		switch(m_Data.m_nType)
		{
			case 0: DrawSphere(A3DCOLORRGB(255,0,0)); break;
			case 1: DrawDirection();break;
			case 2: DrawTaper(true); DrawTaper(false);break;
		}
	}else
	{
		DrawCenter();
		switch(m_Data.m_nType)
		{
			case 0: break;
			case 1: DrawDirection();break;
			case 2: break;
		}
	}
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	pWireCollector->Flush();
}

void CSceneLightObject::DrawSphere(DWORD clr)
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector)
	{
		pWireCollector->AddSphere(m_vPos,m_Data.m_fRange,clr);
	}
}

void CSceneLightObject::DrawCircle()
{
	const float pi_2 = A3D_2PI;
	const float pi_delta = pi_2/MAX_NUM_SECT;
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	if(!pMap->GetTerrain()) return;

	float fRadius = m_Data.m_fRange;
	A3DVECTOR3 vertices[MAX_NUM_SECT];

	A3DVECTOR3 vPos = A3DVECTOR3(0,0,0);
	A3DMATRIX4 matTrans;
	//Calculate vertices array
	if(pMap && pMap->GetTerrain()->GetRender())
	{
		
		float angle = 0;
		//0 - pi2
		for(int i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i].x = vPos.x + fRadius*cos(angle);
			vertices[i].y = vPos.y + fRadius*sin(angle);
			vertices[i].z = vPos.z;
			
			angle += pi_delta;
		}
		
		matTrans = m_matRotate*m_matTrans;
		for( i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i] = vertices[i]*matTrans;
		}
		//Render wire
		DrawLine(vertices,MAX_NUM_SECT,A3DCOLORRGB(255,255,0));

		//0 - pi2
		angle = 0;
		for(i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i].x = vPos.x + fRadius*cos(angle);
			vertices[i].z = vPos.z + fRadius*sin(angle);
			vertices[i].y = vPos.y;
			
			angle += pi_delta;
		}
		matTrans = m_matRotate*m_matTrans;
		for(i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i] = vertices[i]*matTrans;
		}
		//Render wire
		DrawLine(vertices,MAX_NUM_SECT,A3DCOLORRGB(255,255,0));

		angle = 0;
		for(i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i].y = vPos.y + fRadius*cos(angle);
			vertices[i].z = vPos.z + fRadius*sin(angle);
			vertices[i].x = vPos.x;
			
			angle += pi_delta;
		}
		matTrans = m_matRotate*m_matTrans;
		for(i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i] = vertices[i]*matTrans;
		}
		//Render wire
		DrawLine(vertices,MAX_NUM_SECT,A3DCOLORRGB(255,255,0));
	}
}

void CSceneLightObject::DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum,DWORD clr)
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	
	WORD pIndices[MAX_NUM_SECT*2];
	int n = 0;
	for(int i=0; i<MAX_NUM_SECT; i++)
	{
		pIndices[n] = i;
		pIndices[n+1] = (i+1)%(MAX_NUM_SECT);
		n += 2;
	}
	
	if(pWireCollector)
	{
		pWireCollector->AddRenderData_3D(pVertices,dwNum,pIndices,MAX_NUM_SECT*2,clr);
	}
}

void CSceneLightObject::DrawDirection()
{
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector==NULL) return;
	
	float m_fDelta = 2.0f;
	A3DVECTOR3 vExt = A3DVECTOR3(m_fDelta,m_fDelta,m_fDelta);
	float fTemp = 0.05f;
	float fArrow = 0.75f;
	float fPlane = 0.15f;

	A3DVECTOR3 Vertices[7];
	WORD indices[14];

	A3DMATRIX4 matABS;
	matABS = m_matScale*m_matRotate*m_matTrans;

	indices[0] = 0; indices[1] = 1;
	indices[2] = 1; indices[3] = 2;
	indices[4] = 2; indices[5] = 3;
	indices[6] = 3; indices[7] = 4;
	indices[8] = 4; indices[9] = 5;
	indices[10] = 5; indices[11] = 6;
	indices[12] = 6; indices[13] = 0;

	//z 轴,因为默认情况我们取Z为正方向
	Vertices[0] =  A3DVECTOR3(0,-fPlane,0);
	Vertices[1] =  A3DVECTOR3(0,fPlane,0);
	Vertices[2] =  A3DVECTOR3(0,fPlane,fArrow);
	Vertices[3] =  A3DVECTOR3(0,fPlane+fPlane,fArrow);
	Vertices[4] =  A3DVECTOR3(0,0,m_fDelta);
	Vertices[5] =  A3DVECTOR3(0,-fPlane-fPlane,fArrow);
	Vertices[6] =  A3DVECTOR3(0,-fPlane,fArrow);
	
	Vertices[0] = matABS*Vertices[0];
	Vertices[1] = matABS*Vertices[1];
	Vertices[2] = matABS*Vertices[2];
	Vertices[3] = matABS*Vertices[3];
	Vertices[4] = matABS*Vertices[4];
	Vertices[5] = matABS*Vertices[5];
	Vertices[6] = matABS*Vertices[6];
	pWireCollector->AddRenderData_3D(Vertices,7,indices,14,A3DCOLORRGB(255,255,0));

	//z 轴,因为默认情况我们取Z为正方向
	Vertices[0] =  A3DVECTOR3(-fPlane,0,0);
	Vertices[1] =  A3DVECTOR3(fPlane,0,0);
	Vertices[2] =  A3DVECTOR3(fPlane,0,fArrow);
	Vertices[3] =  A3DVECTOR3(fPlane+fPlane,0,fArrow);
	Vertices[4] =  A3DVECTOR3(0,0,m_fDelta);
	Vertices[5] =  A3DVECTOR3(-fPlane-fPlane,0,fArrow);
	Vertices[6] =  A3DVECTOR3(-fPlane,0,fArrow);
	
	Vertices[0] = matABS*Vertices[0];
	Vertices[1] = matABS*Vertices[1];
	Vertices[2] = matABS*Vertices[2];
	Vertices[3] = matABS*Vertices[3];
	Vertices[4] = matABS*Vertices[4];
	Vertices[5] = matABS*Vertices[5];
	Vertices[6] = matABS*Vertices[6];
	pWireCollector->AddRenderData_3D(Vertices,7,indices,14,A3DCOLORRGB(255,255,0));
	
	Vertices[0] = A3DVECTOR3(0,0,0);
	Vertices[0] = matABS*Vertices[0];
	Vertices[1] = Vertices[4];
	pWireCollector->AddRenderData_3D(Vertices,2,indices,2,A3DCOLORRGB(255,255,0));
}

void CSceneLightObject::DrawTaper(bool bPhi)
{
	A3DVECTOR3 vertices[MAX_NUM_SECT+1];
	const float pi_2 = A3D_2PI;
	const float pi_delta = pi_2/MAX_NUM_SECT;
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	if(!pMap->GetTerrain()) return;
	
	float fRadius;
	DWORD clr;
	if(bPhi)
	{
		fRadius = m_Data.m_fRange * tan(DEG2RAD(m_Data.m_fPhi)/2.0f);
		clr = A3DCOLORRGB(255,255,0);
	}else 
	{
		fRadius = m_Data.m_fRange * tan(DEG2RAD(m_Data.m_fTheta)/2.0f);
		clr = A3DCOLORRGB(255,100,0);
	}
	A3DVECTOR3 vPos = A3DVECTOR3(0,0,0);
	
	//Calculate vertices array
	if(pMap && pMap->GetTerrain()->GetRender())
	{
		
		float angle = 0;
		//0 - pi2
		for(int i=0; i<MAX_NUM_SECT; i++)
		{
			vertices[i].x = fRadius*cos(angle);
			vertices[i].y = fRadius*sin(angle);
			vertices[i].z = m_Data.m_fRange;
			
			angle += pi_delta;
		}

		vertices[MAX_NUM_SECT] = A3DVECTOR3(0,0,0);
		//在这儿做位移以及旋转变化
		A3DMATRIX4 matTrans;
		matTrans = m_matRotate*m_matTrans; 
		for(i=0; i<MAX_NUM_SECT+1; i++)
		{
			vertices[i] = vertices[i]*matTrans;
		}
		//。。。。。。。。。。。
		//
		DrawLine(vertices,MAX_NUM_SECT,clr);


		A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
		WORD pIndices[MAX_NUM_SECT*2];
		int n = 0;
		for(i=0; i<MAX_NUM_SECT; i++)
		{
			pIndices[n] = MAX_NUM_SECT;
			pIndices[n+1] = i;
			n += 2;
		}
		
		if(pWireCollector)
		{
			pWireCollector->AddRenderData_3D(vertices,MAX_NUM_SECT+1,pIndices,MAX_NUM_SECT*2,clr);
		}
	}
}
void CSceneLightObject::BuildLightProperty()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_Data.m_nType), "灯光类型", &light_type);
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置坐标");
		m_pProperty->DynAddProperty(AVariant(m_Data.m_vDir), "方向坐标");
		m_pProperty->DynAddProperty(AVariant(m_Data.m_dwColor), "颜色",NULL,NULL,WAY_COLOR);
		m_pProperty->DynAddProperty(AVariant(m_Data.m_fColorScale),"亮度系数");
		m_pProperty->DynAddProperty(AVariant(m_Data.m_fRange), "光照范围");
		
		m_pProperty->DynAddProperty(AVariant(m_Data.m_dwAbient),"环境颜色",NULL,NULL,WAY_COLOR);
		m_pProperty->DynAddProperty(AVariant(m_Data.m_fFalloff),"发散");
		m_pProperty->DynAddProperty(AVariant(m_Data.m_fAttenuation0),"衰减0");
		m_pProperty->DynAddProperty(AVariant(m_Data.m_fAttenuation1),"衰减1");
		m_pProperty->DynAddProperty(AVariant(m_Data.m_fAttenuation2),"衰减2");
		m_pProperty->DynAddProperty(AVariant(m_Data.m_fTheta),"内圆锥角度");
		m_pProperty->DynAddProperty(AVariant(m_Data.m_fPhi),"外圆锥角度");
		m_pProperty->DynAddProperty(AVariant(m_Data.m_nType), "灯光影响", &light_effect_type);
		
	}
}

void CSceneLightObject::UpdateProperty(bool bGet)
{
	if(bGet)
	{
		AString temp = AString(m_pProperty->GetPropVal(0));
		float oldTheta,oldPhi;
		oldTheta = m_Data.m_fTheta;
		oldPhi = m_Data.m_fPhi;
		m_Data.m_fTheta = m_pProperty->GetPropVal(11);
		m_Data.m_fPhi = m_pProperty->GetPropVal(12);
		if(m_Data.m_fPhi<m_Data.m_fTheta || m_Data.m_fTheta<0.0f || m_Data.m_fPhi>180.0f)
		{
			MessageBox(NULL,"外圆锥的角度一定要大于内圆锥的角度，同时内圆锥的角度必须大于0\n，外圆锥的角度必须小于3.141592",NULL,MB_OK);
			m_Data.m_fTheta = oldTheta;
			m_Data.m_fPhi = oldPhi;
			return;
		}
		CElementMap *pMap = AUX_GetMainFrame()->GetMap();
		if(pMap)
		{
			CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
			CSceneObject*pObj = pSManager->FindSceneObject(temp);
			if(pObj != NULL && pObj != this)
			{
				MessageBox(NULL,"该对象的名字已经存在，编辑器将恢复原对象名字！",NULL,MB_OK);
				m_pProperty->SetPropVal(0,m_strName);
				m_pProperty->SetPropVal(1,m_Data.m_nType);
				m_pProperty->SetPropVal(2,m_vPos);
				m_pProperty->SetPropVal(3,m_Data.m_vDir);
				m_pProperty->SetPropVal(4,m_Data.m_dwColor);
				m_pProperty->SetPropVal(5,m_Data.m_fColorScale);
				m_pProperty->SetPropVal(6,m_Data.m_fRange);
				m_pProperty->SetPropVal(7,m_Data.m_dwAbient);
				m_pProperty->SetPropVal(8,m_Data.m_fFalloff);
				m_pProperty->SetPropVal(9,m_Data.m_fAttenuation0);
				m_pProperty->SetPropVal(10,m_Data.m_fAttenuation1);
				m_pProperty->SetPropVal(11,m_Data.m_fAttenuation2);
				m_pProperty->SetPropVal(12,m_Data.m_fTheta);
				m_pProperty->SetPropVal(13,m_Data.m_fPhi);
				m_pProperty->SetPropVal(14,m_Data.m_nEffectType);
				
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		m_strName	= temp;
		m_Data.m_nType	= m_pProperty->GetPropVal(1);
		m_vPos	= m_pProperty->GetPropVal(2);
		m_Data.m_vDir  = m_pProperty->GetPropVal(3);
		
		m_Data.m_dwColor = m_pProperty->GetPropVal(4);
		m_Data.m_fColorScale = m_pProperty->GetPropVal(5);
		m_Data.m_fRange = m_pProperty->GetPropVal(6);
	
		m_Data.m_dwAbient = m_pProperty->GetPropVal(7);
		m_Data.m_fFalloff = m_pProperty->GetPropVal(8);
		m_Data.m_fAttenuation0 = m_pProperty->GetPropVal(9);
		m_Data.m_fAttenuation1 = m_pProperty->GetPropVal(10);
		m_Data.m_fAttenuation2 = m_pProperty->GetPropVal(11);
		m_Data.m_fTheta = m_pProperty->GetPropVal(12);
		m_Data.m_fPhi = m_pProperty->GetPropVal(13);
		m_Data.m_nEffectType = m_pProperty->GetPropVal(14);
		m_matRotate.SetRow(2,m_Data.m_vDir);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
	}else
	{
		m_Data.m_vDir = m_matRotate.GetRow(2);
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_Data.m_nType);
		m_pProperty->SetPropVal(2,m_vPos);
		m_pProperty->SetPropVal(3,m_Data.m_vDir);
		m_pProperty->SetPropVal(4,m_Data.m_dwColor);
		m_pProperty->SetPropVal(5,m_Data.m_fColorScale);
		m_pProperty->SetPropVal(6,m_Data.m_fRange);
		m_pProperty->SetPropVal(7,m_Data.m_dwAbient);
		m_pProperty->SetPropVal(8,m_Data.m_fFalloff);
		m_pProperty->SetPropVal(9,m_Data.m_fAttenuation0);
		m_pProperty->SetPropVal(10,m_Data.m_fAttenuation1);
		m_pProperty->SetPropVal(11,m_Data.m_fAttenuation2);
		m_pProperty->SetPropVal(12,m_Data.m_fTheta);
		m_pProperty->SetPropVal(13,m_Data.m_fPhi);
		m_pProperty->SetPropVal(14,m_Data.m_nEffectType);
	}
}

//把灯光数据取出来，主要是用于属性表更改UNDO
void CSceneLightObject::GetLightData(LIGHTDATA &data)
{
	data = m_Data;
	data.m_vDir = m_matRotate.GetRow(2);
	data.m_strName = m_strName;
}

void CSceneLightObject::SetLightData(const LIGHTDATA &data)
{
	m_Data = data;
	m_strName = data.m_strName;
	m_matRotate.SetRow(2,m_Data.m_vDir);
}


CSceneObject* CSceneLightObject::CopyObject()
{
	CSceneLightObject *pNewObject = new CSceneLightObject(*this);
	ASSERT(pNewObject);
	pNewObject->m_pProperty = new ADynPropertyObject();	
	pNewObject->BuildLightProperty();
	int lightNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Light_%d",lightNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				lightNum++;
			}else 
			{
				lightNum++;
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

//	Load data
bool CSceneLightObject::Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags)
{
	A3DVECTOR3 vDir;
	float fDistance;
	if(dwVersion<2)
	{//2以前版本
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_Data.m_nType,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vDir,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.m_fRange,sizeof(float)))
			return false;
		m_matRotate.SetRow(2,vDir);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
	}
	if(dwVersion==2)
	{
		
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_Data.m_nType,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vDir,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.m_fRange,sizeof(float)))
			return false;
		if(!ar.Read(&fDistance,sizeof(float)))
			return false;
		
		m_matRotate.SetRow(2,vDir);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
	}

	if(dwVersion>2 && dwVersion<5)
	{
		A3DVECTOR3 vr,vu,vl;
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_Data.m_nType,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.m_fRange,sizeof(float)))
			return false;
		if(!ar.Read(&fDistance,sizeof(float)))
			return false;

		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
	}

	if(dwVersion > 4 && dwVersion<=0xd)
	{
		A3DVECTOR3 vr,vu,vl;
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_Data.m_nType,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.m_fRange,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_dwAbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.m_fFalloff,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fAttenuation0,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fAttenuation1,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fAttenuation2,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fTheta,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fPhi,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fColorScale,sizeof(float)))
			return false;
		
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
	}else if(dwVersion>0xd && dwVersion <= 0x10)
	{
		A3DVECTOR3 vr,vu,vl;
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_Data.m_nType,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.m_fRange,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_dwAbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.m_fFalloff,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fAttenuation0,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fAttenuation1,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fAttenuation2,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fTheta,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fPhi,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fColorScale,sizeof(float)))
			return false;
		
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
	}else if(dwVersion > 0x10)
	{
		A3DVECTOR3 vr,vu,vl;
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_Data.m_nType,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Read(&m_Data.m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.m_fRange,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_dwAbient,sizeof(DWORD)))
			return false;
		if(!ar.Read(&m_Data.m_fFalloff,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fAttenuation0,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fAttenuation1,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fAttenuation2,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fTheta,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fPhi,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_fColorScale,sizeof(float)))
			return false;
		if(!ar.Read(&m_Data.m_nEffectType,sizeof(int)))
			return false;
		
		m_matRotate.SetRow(2,vr);
		m_matRotate.SetRow(1,vu);
		m_matRotate.SetRow(0,vl);
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
	}

	return true;
}

//	Save data
bool CSceneLightObject::Save(CELArchive& ar,DWORD dwVersion)
{
	if(dwVersion>4)
	{
		A3DVECTOR3 vr,vu,vl;
		vr = m_matRotate.GetRow(2);
		vu = m_matRotate.GetRow(1);
		vl = m_matRotate.GetRow(0);
		ar.WriteString(m_strName);
		if(!ar.Write(&m_nObjectType,sizeof(int)))
			return false;
		if(!ar.Write(&m_Data.m_nType,sizeof(int)))
			return false;
		if(!ar.Write(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vr,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vu,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&vl,sizeof(A3DVECTOR3)))
			return false;
		if(!ar.Write(&m_Data.m_dwColor,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_Data.m_fRange,sizeof(float)))
			return false;
		if(!ar.Write(&m_Data.m_dwAbient,sizeof(DWORD)))
			return false;
		if(!ar.Write(&m_Data.m_fFalloff,sizeof(float)))
			return false;
		if(!ar.Write(&m_Data.m_fAttenuation0,sizeof(float)))
			return false;
		if(!ar.Write(&m_Data.m_fAttenuation1,sizeof(float)))
			return false;
		if(!ar.Write(&m_Data.m_fAttenuation2,sizeof(float)))
			return false;
		if(!ar.Write(&m_Data.m_fTheta,sizeof(float)))
			return false;
		if(!ar.Write(&m_Data.m_fPhi,sizeof(float)))
			return false;
		if(!ar.Write(&m_Data.m_fColorScale,sizeof(float)))
			return false;
		if(dwVersion>0x10)
		{
			if(!ar.Write(&m_Data.m_nEffectType,sizeof(int)))
			return false;
		}
		
		
		return true;
	}
	return true;
}
	