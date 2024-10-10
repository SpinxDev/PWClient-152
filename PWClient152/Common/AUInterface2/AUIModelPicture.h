// Filename	: AUIModelPicture.h
// Creator	: Tom Zhou
// Date		: July 27, 2004
// Desc		: AUIModelPicture is the class of model display control.

#ifndef _AUIMODELPICTURE_H_
#define _AUIMODELPICTURE_H_

#include "AUIPicture.h"
#include "A3DAlterableSkinModel.h"

class A3DCamera;

class AUIModelPicture : public AUIPicture
{
public:
	AUIModelPicture();
	virtual ~AUIModelPicture();

	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF);
	virtual bool InitIndirect(const char *pszFileName, int idType);
	virtual bool Release(void);
	virtual bool Save(FILE *file);
	virtual bool Tick(void);
	virtual bool Render(void);
	virtual bool OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	virtual bool GetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const	char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	static bool RegisterPropertyType();

	A3DCamera * GetA3DCamera(void);
	A3DAlterableSkinModel * GetA3DAlterableSkinModel(void);

protected:
	DWORD m_dwTime;
	A3DLight * m_pDirLight;
	A3DCamera * m_pA3DCamera;
	A3DViewport * m_pA3DViewport;
	A3DAlterableSkinModel * m_pA3DModel;
};

typedef AUIModelPicture * PAUIMODELPICTURE;

#endif //_AUIMODELPICTURE_H_