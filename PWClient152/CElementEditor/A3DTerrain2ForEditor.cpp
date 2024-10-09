#include "A3DTerrain2ForEditor.h"
#include "AFileImage.h"
#include "A3DPI.h"
#include "A3DTerrain2Env.h"
#include "A3DTerrain2LoaderA.h"
#include "A3DTerrain2LoaderForEditor.h"
#include "A3DTextureMan.h"
#include "A3DEngine.h"

A3DTerrain2ForEditor::A3DTerrain2ForEditor():
A3DTerrain2()
{

}

A3DTerrain2ForEditor::~A3DTerrain2ForEditor()
{

}

A3DTerrain2LoaderB* A3DTerrain2ForEditor::CreateLoaderB()
{
	return new A3DTerrain2LoaderBForEditor(this);
}

int  A3DTerrain2ForEditor::AddTexture(const char* szFileName)
{
	int i;
	for (i=0; i < m_aTextures.GetSize(); i++)
	{
		TEXTURE* pSlot = m_aTextures[i];
		if(pSlot->strFileName == szFileName) return i;
	}
	
	TEXTURE* pTex = new TEXTURE;
	pTex->iRefCount = 0;
	pTex->strFileName = szFileName;
	pTex->pA3DTexture = NULL;
	
	m_aTextures.Add(pTex);
	return m_aTextures.GetSize() - 1;
}

void A3DTerrain2ForEditor::ReleaseTextrueRef( int idx )
{
	TEXTURE* pSlot = m_aTextures[idx];
	if (!pSlot->pA3DTexture)
		return;

	if(--pSlot->iRefCount < 0) pSlot->iRefCount = 0;
}