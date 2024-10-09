#pragma once

#include "A3DTerrain2LoaderB.h"
class A3DTerrain2Mask;
class A3DTerrain2ForEditor;
class A3DTerrain2MaskForEditor;
class A3DTerrain2LoaderBForEditor;

class A3DTrn2SubLoaderBForEditor : public A3DTrn2SubLoaderB
{
public:
	A3DTrn2SubLoaderBForEditor(A3DTerrain2LoaderB* pLoaderB);

private:
	virtual A3DTerrain2Mask* CreateTerrain2Mask(A3DTerrain2* pTrn, int iMaskArea) const;
};

class A3DTerrain2LoaderBForEditor : public A3DTerrain2LoaderB
{

public:
	A3DTerrain2LoaderBForEditor(A3DTerrain2* pTerrain);
	virtual ~A3DTerrain2LoaderBForEditor();
public:
	//	Create sub loader
	virtual A3DTrn2SubLoaderB* CreateSubLoader();
};