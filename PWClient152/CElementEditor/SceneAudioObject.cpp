// SceneAudioObject.cpp: implementation of the CSceneAudioObject class.
//
//////////////////////////////////////////////////////////////////////

#include "global.h"
#include "elementeditor.h"
#include "SceneObjectManager.h"
#include "SceneAudioObject.h"
#include "SceneLightObject.h"
#include "Box3D.h"
#include "Render.h"
#include "A3D.h"
#include "AM.h"
#include "MainFrm.h"

//#define new A_DEBUG_NEW

#ifndef MAX_NUM_SECT
#define MAX_NUM_SECT  24
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CSceneAudioObject::CSceneAudioObject()
{
	m_auData.m_nGroup = 0;
	m_auData.m_fMinDist = 1.0f;
	m_auData.m_fMaxDist = 50.0f;
	m_auData.m_nAudioTypes = 0;
	m_auData.m_strPathName = AString("default.wav");
	m_fScale = 1.0f;
	m_nObjectType = CSceneObject::SO_TYPE_AUDIO;
	m_pAM3DSoundBuffer = NULL;
	m_vPos = A3DVECTOR3(0.0f);
	m_bEnableAudio = true;
	m_bIsPlaying = true;
	
	audio_type.AddElement("白天", 0);
	audio_type.AddElement("黑夜", 1);
	audio_type.AddElement("白天黑夜", 2);
	BuildProperty();
}

CSceneAudioObject::~CSceneAudioObject()
{
	
}

void CSceneAudioObject::Release()
{
	if(m_pAM3DSoundBuffer)
	{
		m_pAM3DSoundBuffer->Stop();
		g_Render.GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan()->Release3DSound(&m_pAM3DSoundBuffer);
		m_pAM3DSoundBuffer = NULL;
	}
	CSceneObject::Release();
};

void CSceneAudioObject::Render(A3DViewport* pA3DViewport)
{
	if(m_bSelected)
	{
		DrawCenter();
		DrawCircle(A3DCOLORRGB(255,255,0));
	}else
	{
		DrawCenter();
		DrawCircle(A3DCOLORRGB(0,255,255));
	}
	A3DWireCollector *pWireCollector = g_Render.GetA3DEngine()->GetA3DWireCollector();
	if(pWireCollector) pWireCollector->Flush();
	
	CSceneObject::Render(pA3DViewport);
}

void CSceneAudioObject::Tick(DWORD timeDelta)
{
	A3DVECTOR3 cPos;
	cPos = g_Render.GetA3DEngine()->GetActiveCamera()->GetPos();
	cPos = cPos - m_vPos;
	float dis = cPos.Magnitude();
	
	if(m_pAM3DSoundBuffer)
	{
		
		if(dis > m_auData.m_fMaxDist + 10)
		{
			Release();
			return;
		}
		
		if(!g_Configs.bEnableSpeak)
		{//静音
			m_pAM3DSoundBuffer->Stop();
			return;
		}
		
		if(m_auData.m_nAudioTypes == AUDIO_TYPE_DAY )
		{
			if(dis>m_auData.m_fMaxDist || g_fNightFactor > 0.5f)
			{	
				FadeSound(m_pAM3DSoundBuffer,false);
			}else
			{
				FadeSound(m_pAM3DSoundBuffer,true);
			}
		}
		else if(m_auData.m_nAudioTypes == AUDIO_TYPE_NIGHT)
		{
			if(dis>m_auData.m_fMaxDist || g_fNightFactor < 0.5f)
			{	
				FadeSound(m_pAM3DSoundBuffer,false);
			}else
			{
				FadeSound(m_pAM3DSoundBuffer,true);
			}
		}else
		{
			if(dis>m_auData.m_fMaxDist)
			{	
				FadeSound(m_pAM3DSoundBuffer,false);
			}else
			{
				FadeSound(m_pAM3DSoundBuffer,true);
			}
		}
		m_pAM3DSoundBuffer->Tick(timeDelta);
	}else
	{
		if(dis < m_auData.m_fMaxDist + 4)
		ReBuildAudio();
	}
}

//	Fade in/out sound
void CSceneAudioObject::FadeSound(AM3DSoundBuffer * pSound, bool bFadeIn)
{
	if (pSound)
	{
		if( bFadeIn )
		{
			if( !pSound->IsPlaying() )
			{
				pSound->SetVolume(0);
				pSound->Play(true);
				pSound->FadeIn(3000);
			}
			else if( !pSound->IsFadeIn() )
				pSound->FadeIn(3000);
		}
		else
		{
			if( pSound->IsPlaying() && !pSound->IsFadeOut() )
				pSound->FadeOut(3000);
		}
	}
}

void CSceneAudioObject::UpdateProperty(bool bGet)
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
				m_pProperty->SetPropVal(0,m_strName);
				m_pProperty->SetPropVal(1,m_vPos);
				m_pProperty->SetPropVal(2,m_auData.m_fMinDist);
				m_pProperty->SetPropVal(3,m_auData.m_fMaxDist);
				m_pProperty->SetPropVal(4,m_auData.m_nGroup);
				m_pProperty->SetPropVal(5,m_auData.m_strPathName);
				m_pProperty->SetPropVal(6,m_auData.m_nAudioTypes);
				AUX_GetMainFrame()->GetToolTabWnd()->UpdatePropertyData(false);
				return;
			}
		}
		m_strName				= temp;
		m_vPos					= m_pProperty->GetPropVal(1);
		m_auData.m_fMinDist		= m_pProperty->GetPropVal(2);
		m_auData.m_fMaxDist		= m_pProperty->GetPropVal(3);
		m_auData.m_nGroup		= m_pProperty->GetPropVal(4);
		m_auData.m_strPathName	= AString(m_pProperty->GetPropVal(5));
		m_auData.m_nAudioTypes  = m_pProperty->GetPropVal(6);	
		m_matTrans.Translate(m_vPos.x,m_vPos.y,m_vPos.z);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildAudio();
	}else
	{
	
		m_pProperty->SetPropVal(0,m_strName);
		m_pProperty->SetPropVal(1,m_vPos);
		m_pProperty->SetPropVal(2,m_auData.m_fMinDist);
		m_pProperty->SetPropVal(3,m_auData.m_fMaxDist);
		m_pProperty->SetPropVal(4,m_auData.m_nGroup);
		m_pProperty->SetPropVal(5,m_auData.m_strPathName);
		m_pProperty->SetPropVal(6,m_auData.m_nAudioTypes);

	}
}

void CSceneAudioObject::BuildProperty()
{
	if(m_pProperty)
	{
		m_pProperty->DynAddProperty(AVariant(m_strName), "名字");
		m_pProperty->DynAddProperty(AVariant(m_vPos), "位置坐标");
		m_pProperty->DynAddProperty(AVariant(m_auData.m_fMinDist), "最小距离");
		m_pProperty->DynAddProperty(AVariant(m_auData.m_fMaxDist), "最大距离");
		m_pProperty->DynAddProperty(AVariant(m_auData.m_nGroup), "分组");
		m_pProperty->DynAddProperty(AVariant(m_auData.m_strPathName), "音效文件",NULL,NULL,WAY_FILENAME,"sfx");
		m_pProperty->DynAddProperty(AVariant(m_auData.m_nAudioTypes), "播放时间", &audio_type);
	}
}

void CSceneAudioObject::ReBuildAudio()
{
	
	if(m_auData.m_strPathName.IsEmpty() || !m_bEnableAudio) return;
	
	if(m_pAM3DSoundBuffer)
	{
		m_pAM3DSoundBuffer->Stop();
		g_Render.GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan()->Release3DSound(&m_pAM3DSoundBuffer);
		m_pAM3DSoundBuffer = NULL;
	}
	AMSoundBufferMan* pSoundBufMan = g_Render.GetA3DEngine()->GetAMSoundEngine()->GetAMSoundBufferMan();
	
	AString name = AString("Sfx\\");
	name = name + m_auData.m_strPathName;
	m_pAM3DSoundBuffer = pSoundBufMan->Load3DSound(name,true);
	if(!m_pAM3DSoundBuffer)
	{
		CString msg;
		msg.Format("CSceneAudioObject::InitAudio, Failed to load 3d sound %s",name);
		g_Log.Log(msg);
		g_LogDlg.Log(msg);
		return;
	}
	m_pAM3DSoundBuffer->SetPosition(m_vPos);
	m_pAM3DSoundBuffer->SetMaxDistance(m_auData.m_fMaxDist);
	m_pAM3DSoundBuffer->SetMinDistance(m_auData.m_fMinDist);
	m_pAM3DSoundBuffer->UpdateChanges();
}

void CSceneAudioObject::DrawCircle(DWORD clr)
{
	const float pi_2 = A3D_2PI;
	const float pi_delta = pi_2/MAX_NUM_SECT;
	
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap==NULL) return;
	if(!pMap->GetTerrain()) return;

	float fRadius = m_auData.m_fMaxDist;
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
		DrawLine(vertices,MAX_NUM_SECT,clr);

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
		DrawLine(vertices,MAX_NUM_SECT,clr);

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
		DrawLine(vertices,MAX_NUM_SECT,clr);
	}
}

void CSceneAudioObject::DrawLine(A3DVECTOR3 *pVertices,DWORD dwNum,DWORD clr)
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

void CSceneAudioObject::EntryScene()
{
	if(m_pAM3DSoundBuffer) FadeSound(m_pAM3DSoundBuffer,true);
}

void CSceneAudioObject::LeaveScene()
{
	if(m_pAM3DSoundBuffer)
		FadeSound(m_pAM3DSoundBuffer,false);
}

CSceneObject* CSceneAudioObject::CopyObject()
{
	CSceneAudioObject *pNewObject = new CSceneAudioObject(*this);
	ASSERT(pNewObject);
	pNewObject->m_pProperty = new ADynPropertyObject();	
	pNewObject->BuildProperty();
	pNewObject->ResetSoundBuffer();
	pNewObject->ReBuildAudio();
	int AudioNum = 0;
	CElementMap *pMap = AUX_GetMainFrame()->GetMap();
	if(pMap)
	{
		AString name;
		CSceneObjectManager *pSManager = pMap->GetSceneObjectMan();
		pSManager->ClearSelectedList();
		while(1)
		{
			name.Format("Audio_%d",AudioNum);
			if(pSManager->FindSceneObject(name)!=NULL)
			{
				AudioNum++;
			}else 
			{
				AudioNum++;
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
bool CSceneAudioObject::Load(CELArchive& ar,DWORD dwVersion,int iLoadFlags)
{
	if(dwVersion>1 && dwVersion<=0xd)
	{
		// 2.0版本开放
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_auData.m_fMaxDist,sizeof(float)))
			return false;
		if(!ar.Read(&m_auData.m_fMinDist,sizeof(float)))
			return false;
		if(!ar.Read(&m_auData.m_nGroup,sizeof(int)))
			return false;
		
		m_auData.m_strPathName = AString(ar.ReadString());
		m_matTrans.SetRow(3,m_vPos);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildAudio();
		return true;
	}else if(dwVersion > 0xd && dwVersion < 0x28)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_auData.m_fMaxDist,sizeof(float)))
			return false;
		if(!ar.Read(&m_auData.m_fMinDist,sizeof(float)))
			return false;
		if(!ar.Read(&m_auData.m_nGroup,sizeof(int)))
			return false;
		
		m_auData.m_strPathName = AString(ar.ReadString());
		m_matTrans.SetRow(3,m_vPos);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildAudio();
		return true;
	}else if(dwVersion >= 0x28)
	{
		m_strName = AString(ar.ReadString());
		if(!ar.Read(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Read(&m_vPos,sizeof(A3DVECTOR3)))
			return false;
		
		if(!ar.Read(&m_auData.m_fMaxDist,sizeof(float)))
			return false;
		if(!ar.Read(&m_auData.m_fMinDist,sizeof(float)))
			return false;
		if(!ar.Read(&m_auData.m_nGroup,sizeof(int)))
			return false;
		if(!ar.Read(&m_auData.m_nAudioTypes,sizeof(int)))
			return false;
		
		m_auData.m_strPathName = AString(ar.ReadString());
		m_matTrans.SetRow(3,m_vPos);
		m_matAbs = m_matScale * m_matRotate * m_matTrans;
		ReBuildAudio();
		return true;
	}
	return true;
}

//	Save data
bool CSceneAudioObject::Save(CELArchive& ar,DWORD dwVersion)
{
	if(dwVersion > 1)
	{//2.0版本开放
		ar.WriteString(m_strName);
		if(!ar.Write(&m_nObjectID,sizeof(int)))
			return false;
		if(!ar.Write(&m_vPos,sizeof(A3DVECTOR3)))
			return false;

		if(!ar.Write(&m_auData.m_fMaxDist,sizeof(float)))
			return false;
		if(!ar.Write(&m_auData.m_fMinDist,sizeof(float)))
			return false;
		if(!ar.Write(&m_auData.m_nGroup,sizeof(int)))
			return false;
		if(!ar.Write(&m_auData.m_nAudioTypes,sizeof(int)))
			return false;
		
		ar.WriteString(m_auData.m_strPathName);
		
		return true;
	}

	return true;	
}
