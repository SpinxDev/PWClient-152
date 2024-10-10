/*
 * FILE: A3DGFXMan.h
 *
 * DESCRIPTION: Graphics FX Managing Class
 *
 * CREATED BY: Hedi, 2001/12/8
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DGFXMAN_H_
#define _A3DGFXMAN_H_

#include "A3DTypes.h"
#include "AList.h"

class A3DDevice;
class A3DGraphicsFX;
class A3DGFXCollector;
class AFileImage;
class A3DViewport;
class A3DModel;

class A3DGFXMan
{
private:
	char			m_szFolderName[MAX_PATH];
	char			m_szTextureFolder[MAX_PATH];

	A3DDevice		* m_pA3DDevice;
	AList			m_GFXList;

	AList			m_PreloadedGFXFilesList;

	AFileImage * 	FindFileImage(const char * szFileName);

	int				m_nGFXCache;
	A3DGFXCollector * m_pGFXCollector;

public:
	A3DGFXMan();
	~A3DGFXMan();
	
	bool Render(A3DViewport * pCurrentViewport, int nCategoryMask=0xffffffff);
	bool TickAnimation();

	bool Init(A3DDevice * pA3DDevice);
	bool Reset();
	bool Release();

	// Create an A3DGraphicsFX class object from a file; but this graphicsFX is invisible at beginning, 
	// call A3DGraphicsFX::Start if you want the effect showing;
	bool LoadGFXFromFile(const char * szGFXFile, A3DModel * pModel, const char * szParentFrameName, bool bLinked, A3DGraphicsFX ** ppGFX, 
		A3DVECTOR3 vecPos=A3DVECTOR3(0.0f), A3DVECTOR3 vecDir=A3DVECTOR3(0.0f, 0.0f, 1.0f), A3DVECTOR3 vecUp=A3DVECTOR3(0.0f, 1.0f, 0.0), bool bForceFile=false);

	// Called directly from A3DGraphicsFX::Start, add a reference into m_GFXList
	bool AddGFX(A3DGraphicsFX * pGFX);

	// Check if the gfx is still alive;
	bool IsGFXAlive(A3DGraphicsFX * pGFX);

	// Release an A3DGraphicsFX class object;
	bool ReleaseGFX(A3DGraphicsFX*& pGFX);

	// Remove the reference in m_GFXList, but not release it;
	bool DeleteGFX(A3DGraphicsFX*& pGFX);

	inline void SetFolderName(const char * szFolderName) 
	{ 
		strcpy_s(m_szFolderName, szFolderName); 
		sprintf(m_szTextureFolder, "%s\\Textures", m_szFolderName);
	}
	inline char * GetFolderName() { return m_szFolderName; }
	inline char * GetTextureFolder() { return m_szTextureFolder; }
	inline int GetGFXCount() { return m_GFXList.GetSize(); }
};

typedef A3DGFXMan * PA3DGFXMan;

#endif//_A3DGRAPHICSFX_H_
