// Filename	: AUIObjectTemplateManager.h
// Creator	: Dai Xinyu
// Date		: July 12, 2011
// Desc		: AUITemplateManager is the Manager of UI Themes

#ifndef _AUIOBJECT_TEMPLATE_MANAGER
#define _AUIOBJECT_TEMPLATE_MANAGER

#include "AUIObject.h"
#include "AUIDialog.h"
#include "AUIManager.h"

class AUITemplate : public AUIDialog
{
	friend class AUITemplateManager;
public:
	AUITemplate();

	virtual bool Save(const char *pszFilename);

	// virtual functions
	virtual bool DeleteControl(PAUIOBJECT pAUIObject);

	// template operations
	bool TemplateObject(A3DEngine* pA3DEngine, A3DDevice* pA3DDevice, AUIObject& object, const char* szTemplateName);

	abase::vector<PAUIOBJECT> GetTypeObjects(int iType);

	// for only editor
	void SetObjectRender(PAUIOBJECT pObj);

protected:
	virtual bool InitFromXML(const char *pszTempName);

	virtual bool Render();

	void GetTemplateObjs(const char* szTemplateNames,  abase::vector<PAUIOBJECT>& aObjs);

	bool GenerateXml(AUIObject* pSelfObj, const char* szTemplateNames, AXMLFile& xmlFile);

protected:
	PAUIOBJECT m_pObjRender;
};

class AUITemplateManager : public AUIManager
{
public:
	static AUITemplateManager& GetSingleton();

	virtual ~AUITemplateManager(){};
	
	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, 
		const char *pszFilename = NULL, int nDefaultWidth = AUIMANAGER_DEFAULTWIDTH, int nDefaultHeight = AUIMANAGER_DEFAULTHEIGHT);

	virtual bool Save(const char *pszFilename);

	int  AddEmptyScheme(const char* pszSchemeName);

	int  AddScheme(const char* pszFileName);

	bool SetCurScheme(const char* szSchemeName);

	bool SetCurScheme(int idScheme);

	AUITemplate* GetCurScheme();

	AUITemplate* GetScheme(int idScheme);

	abase::vector<AString> GetSchemeNames();

	abase::vector<AUITemplate*> GetSchemeList();

	int GetSchemeNum() const;

	// for only auieditor
	void EnableRender();

protected:
	AUITemplateManager();
	
	virtual PAUIDIALOG CreateDlgInstance(const AString strTemplName);

	int AppendScheme(int id);

protected:
	static AUITemplateManager ms_tempMan;

	int m_iCurScheme;
	abase::vector<AUITemplate*> m_aThemes;
	bool m_bRender;
};

#endif