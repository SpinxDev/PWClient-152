#pragma once

#include "A3DTerrain2Blk.h"

class A3DTerrain2;

class A3DTerrain2MaskForEditor : public A3DTerrain2Mask
{
public:
	friend class A3DTerrain2;
	friend class A3DTerrain2LoaderA;
	friend class A3DTrn2SubLoaderBForEditor;
public:
	A3DTerrain2MaskForEditor(A3DTerrain2* pTerrain, int iMaskArea);
	virtual ~A3DTerrain2MaskForEditor();

public:
	void ClearLayer();
	bool AddLayer(LAYER& layer, const char* szTexturePath);
	int  GetMaskTextureNum() const { return m_aMaskTextures.GetSize(); }
	int AddMaskTexture( int iMaskWidth );
	void  ClearAllMaskTexture();

};