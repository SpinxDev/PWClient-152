#pragma once

#include "A3DTerrain2.h"

class A3DTerrain2ForEditor : public A3DTerrain2
{
public:
	friend class A3DTerrain2LoaderBForEditor;
public:
	A3DTerrain2ForEditor();
	virtual ~A3DTerrain2ForEditor();

public:

	virtual A3DTerrain2LoaderB* CreateLoaderB();
	int  AddTexture(const char* szFileName);// Must use loadtexture()
	void ReleaseTextrueRef( int idx );

};