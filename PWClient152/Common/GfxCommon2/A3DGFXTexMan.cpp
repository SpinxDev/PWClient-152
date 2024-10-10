#include "StdAfx.h"
#include "A3DGFXTexMan.h"

static const int _max_cached_item = 16;

A3DTexture* A3DGFXSharedTexMan::QueryTexture(const AString& strTex, bool bNoDownSample, bool bAbsTexPath)
{
	A3DTextureMan* pMan = m_pDevice->GetA3DEngine()->GetA3DTextureMan();
	A3DTexture* pTexture = NULL;

	if (!pMan->LoadTextureFromFileInFolder(
		strTex,
		bAbsTexPath ? "" : m_pDevice->GetA3DEngine()->GetA3DGFXMan()->GetTextureFolder(),
		&pTexture,
		bNoDownSample ? A3DTF_NODOWNSAMPLE : A3DTF_MIPLEVEL,
		1) || pTexture == NULL)
		return NULL;

#ifdef _ANGELICA21 // Managed by a3d

	return pTexture;

#endif

	LockTex();

	if (m_TexMan.HasItem(strTex))
	{
		m_TexMan.TouchItem(strTex);
		UnlockTex();
		return pTexture;
	}

	if (m_TexMan.GetItemCount() == _max_cached_item)
	{
		A3DTexture* pOldTex = m_TexMan.RemoveLRUItem();
		pMan->ReleaseTexture(pOldTex);
	}

	m_TexMan.AddItem(strTex, pTexture);

	pMan->LoadTextureFromFileInFolder(
		strTex,
		bAbsTexPath ? "" : m_pDevice->GetA3DEngine()->GetA3DGFXMan()->GetTextureFolder(),
		&pTexture,
		bNoDownSample ? A3DTF_NODOWNSAMPLE : A3DTF_MIPLEVEL,
		1);

	UnlockTex();
	return pTexture;
}

void A3DGFXSharedTexMan::Release()
{
	if (m_pDevice == NULL) return;
	A3DTextureMan* pMan = m_pDevice->GetA3DEngine()->GetA3DTextureMan();

	LockTex();

#ifdef _DEBUG
	a_LogOutput(0, "GFX: Cached Texture Count: %d\n", m_TexMan.GetItemCount());
#endif

	while (m_TexMan.GetItemCount())
	{
		A3DTexture* pTex = m_TexMan.RemoveHead();
		pMan->ReleaseTexture(pTex);
	}

	UnlockTex();
	
	m_pDevice = NULL;
	m_bInit = false;
}
