#include "Global.h"
#include "A3DTerrain2LoaderForEditor.h"
#include "A3DTerrain2MaskForEditor.h"
#include "A3DTerrain2File.h"
#include "A3DTerrain2Env.h"
#include "A3DTerrain2ForEditor.h"
#include "A3DTexture.h"

using namespace A3DTRN2LOADERB;

A3DTrn2SubLoaderBForEditor::A3DTrn2SubLoaderBForEditor(A3DTerrain2LoaderB* pLoaderB)
: A3DTrn2SubLoaderB(pLoaderB)
{

}

A3DTerrain2Mask* A3DTrn2SubLoaderBForEditor::CreateTerrain2Mask(A3DTerrain2* pTrn, int iMaskArea) const
{
	//	Create new mask
	return new A3DTerrain2MaskForEditor(m_pt, iMaskArea);
}

A3DTerrain2LoaderBForEditor::A3DTerrain2LoaderBForEditor(A3DTerrain2* pTerrain) : 
A3DTerrain2LoaderB(pTerrain)
{

}

A3DTerrain2LoaderBForEditor::~A3DTerrain2LoaderBForEditor()
{

}

//	Create sub loader
A3DTrn2SubLoaderB* A3DTerrain2LoaderBForEditor::CreateSubLoader()
{
	return new A3DTrn2SubLoaderBForEditor(this);
}