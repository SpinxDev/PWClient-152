#include "StdAfx.h"
#include "A3DGFXModelMan.h"
#include "A3DGFXExMan.h"

void A3DGFXModelMan::Release()
{
	// 不能手动删除共享模型, 让上层自己删除
	assert(m_ModelRefMap.size() == 0);
	/*
	Lock();

	for (A3DGFXModelRefMap::iterator it = m_ModelRefMap.begin(); it != m_ModelRefMap.end(); ++it)
	{
		A3DGFXModelRef* pRef = it->second;
		AfxReleaseA3DSkinModel(pRef->GetSkinModel());
		delete pRef;
	}

	m_ModelRefMap.clear();
	Unlock();
	*/
}

A3DGFXModelRef* A3DGFXModelMan::LoadModel(const char* szPath, bool bShared)
{
	if (!bShared)
	{
		A3DSkinModel* pModel = AfxLoadA3DSkinModel(szPath, 0);

		if (pModel == NULL)
			return NULL;

		A3DGFXModelRef* pRef = new A3DGFXModelRef;
		pRef->SetSharedFlag(false);
		pRef->SetSkinModel(pModel);
		pRef->SetFilePath(szPath);
		return pRef;
	}

	Lock();

	A3DGFXModelRef* pRef = NULL;
	A3DGFXModelRefMap::iterator it = m_ModelRefMap.find(szPath);

	if (it == m_ModelRefMap.end())
	{
		A3DSkinModel* pModel = AfxLoadA3DSkinModel(szPath, 0);

		if (pModel)
		{
			pRef = new A3DGFXModelRef;
			pRef->SetSharedFlag(true);
			pRef->SetSkinModel(pModel);
			pRef->SetFilePath(szPath);
			pRef->AddRef();
			m_ModelRefMap[szPath] = pRef;
		}
	}
	else
	{
		pRef = it->second;
		pRef->AddRef();
	}

	Unlock();
	return pRef;
}

void A3DGFXModelMan::ReleaseModel(A3DGFXModelRef* pRef)
{
	if (pRef->IsShared())
	{
		Lock();

		A3DGFXModelRefMap::iterator it = m_ModelRefMap.find(pRef->m_strFilePath);

		if (it == m_ModelRefMap.end())
		{
			assert(false);
			AfxReleaseA3DSkinModel(pRef->GetSkinModel());
			delete pRef;
		}
		else if (pRef->Release() == 0)
		{
			m_ModelRefMap.erase(it);
			AfxReleaseA3DSkinModel(pRef->GetSkinModel());
			delete pRef;
		}

		Unlock();
	}
	else
	{
		AfxReleaseA3DSkinModel(pRef->GetSkinModel());
		delete pRef;
	}
}
