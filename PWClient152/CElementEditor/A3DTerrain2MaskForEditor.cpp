#include "A3DTerrain2Blk.h"
#include "A3DTerrain2MaskForEditor.h"
#include "A3DTerrain2ForEditor.h"
#include "A3DTexture.h"


A3DTerrain2MaskForEditor::A3DTerrain2MaskForEditor(A3DTerrain2* pTerrain, int iMaskArea) : 
A3DTerrain2Mask(pTerrain,iMaskArea)
{
	
}
A3DTerrain2MaskForEditor::~A3DTerrain2MaskForEditor()
{

}

void A3DTerrain2MaskForEditor::ClearLayer()
{
	m_aTextures.RemoveAll();

	int n = m_aLayers.GetSize();
	for( int i = 0; i < n; ++i)
		((A3DTerrain2ForEditor*)m_pTerrain)->ReleaseTextrueRef(m_aLayers[i].iTextureIdx);
	m_aLayers.RemoveAll();
}

bool A3DTerrain2MaskForEditor::AddLayer(LAYER& layer, const char* szTexturePath)
{
	layer.iTextureIdx = ((A3DTerrain2ForEditor*)m_pTerrain)->AddTexture(szTexturePath);
	A3DTexture* pTexture = ((A3DTerrain2ForEditor*)m_pTerrain)->LoadTexture(layer.iTextureIdx);
	if(pTexture == NULL) return false;

	m_aTextures.Add(pTexture);
	layer.iSpecMapIdx = -1;
	m_aLayers.Add(layer);

	return true;
}

int A3DTerrain2MaskForEditor::AddMaskTexture( int iMaskWidth )
{
	A3DTexture* pTexture = new A3DTexture;
	if (!pTexture)
		return -1;

	//	Create 32-bit mask texture
	if (!pTexture->Create(m_pTerrain->GetA3DDevice(), iMaskWidth, iMaskWidth, A3DFMT_A8R8G8B8, 1))
	{
		delete pTexture;
		return -1;
	}

	m_aMaskTextures.Add(pTexture);

	return m_aMaskTextures.GetSize() - 1;
}

void A3DTerrain2MaskForEditor::ClearAllMaskTexture()
{
	int n = m_aMaskTextures.GetSize();
	for( int i = 0; i < n; ++i)
	{
		m_aMaskTextures[i]->Release();
		delete m_aMaskTextures[i];
	}

	m_aMaskTextures.RemoveAll();
}