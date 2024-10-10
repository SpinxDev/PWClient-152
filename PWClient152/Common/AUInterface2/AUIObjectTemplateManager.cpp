// Filename	: AUIObjectTemplateManager.cpp
// Creator	: Dai Xinyu
// Date		: July 12, 2011
// Desc		: AUITemplateManager is the Manager of UI Themes

#include "AUI.h"
#include "AUIObjectTemplateManager.h"
#include "shlwapi.h"

#define X_OFFSET 0
#define Z_OFFSET 0
#define GAP_LEN  10
//////////////////////////////////////////////////////////////////////////
AUITemplate::AUITemplate(): AUIDialog()
{
	m_pObjRender = NULL;
}

bool AUITemplate::InitFromXML(const char *pszTempName)
{
	if (!AUIDialog::InitFromXML(pszTempName))
		return false;

	for (PAUIOBJECTLISTELEMENT p = m_pListHead; p != NULL; p = p->pNext)
	{
		if (p->pThis)
			p->pThis->Show(false);
	}

	m_x = 0;
	m_y = 0;
	m_nWidth = 0;
	m_nHeight = 0;

	return true;
}

bool AUITemplate::Save(const char *pszFilename)
{
	if (m_pListHead == NULL)
		return true;

	return AUIDialog::Save(pszFilename);
}

bool AUITemplate::Render()
{
	if (!m_pObjRender)
		return true;

	return AUIDialog::Render();
}

void AUITemplate::GetTemplateObjs(const char* szTemplateNames, abase::vector<PAUIOBJECT>& aObjs)
{
	if (szTemplateNames == NULL || szTemplateNames[0] == 0)
		return;

	const char* pStart = szTemplateNames;	
	while (pStart)
	{
		AString strName;

		const char* pTok = strchr(pStart+1, AUI_TEMPLATE_TOK);
		if (pTok)
		{
			for (pStart; pStart<pTok; ++pStart)
				strName += *pStart;
			pStart += 1;
		}
		else
		{
			strName = pStart;
			pStart = NULL;
		}

		AUIDIALOG_AUIOBJECTTABLE::iterator itr = m_AUIObjectTable.find(strName);
		if (itr == m_AUIObjectTable.end() || !(*itr.value()))
			continue;

		AUISubDialog* pSubDlg = dynamic_cast<AUISubDialog*> (*itr.value());
		if (pSubDlg)
		{
			// clear sub dialog in templates
			AUIOBJECT_SETPROPERTY objSet;
			objSet.fn[0] = 0;
			pSubDlg->SetProperty("Dialog File", &objSet);
		}

		aObjs.push_back(*itr.value());
	};
}

bool AUITemplate::GenerateXml(AUIObject* pSelfObj, const char* szTemplateNames, AXMLFile& xmlFile)
{
	xmlFile.Release();

	abase::vector<PAUIOBJECT> aTempObjs;
	GetTemplateObjs(szTemplateNames, aTempObjs);

	if (aTempObjs.empty())
		return false;
	if (pSelfObj)
		aTempObjs.push_back(pSelfObj);

	AXMLFile xmlTemp;
	for (size_t i=0; i<aTempObjs.size(); i++)
	{
		if (!aTempObjs[i]->SaveXML(xmlTemp.GetRootItem()))
			return false;
	}

	AXMLItem* pFirstItem = xmlTemp.GetRootItem()->GetFirstChildItem();
	if (!pFirstItem)
		return false;

	AXMLItem* pItem = pFirstItem->GetNextItem();
	pFirstItem->RemoveItem();
	pFirstItem->InsertItem(xmlFile.GetRootItem());
	while (pItem)
	{
		// copy 's content
		const abase::vector<AXMLItem::PROPERTY_DATA>* paProperty = pItem->GetProperties();
		if (paProperty)
		{
			const abase::vector<AXMLItem::PROPERTY_DATA>& aProperty = *paProperty;
			for (int i=0; i<aProperty.size(); ++i)
				pFirstItem->SetValue(aProperty[i].key, aProperty[i].value);
		}
		
		// copy sub 's item
		AXMLItem* pSubItem = pItem->GetFirstChildItem();
		while (pSubItem)
		{
			AXMLItem* pNext = pSubItem->GetNextItem();
			pSubItem->RemoveItem();
			pSubItem->InsertItem(pFirstItem);
			pSubItem = pNext;	
		}
		pItem = pItem->GetNextItem();
	}

	return true;
}

bool AUITemplate::TemplateObject(A3DEngine* pA3DEngine, A3DDevice* pA3DDevice, AUIObject& object, const char* szTemplateName)
{
	struct OBJ_RETAIN_INFO
	{
		OBJ_RETAIN_INFO(AUIObject& obj) : _obj(obj), _pDlg(NULL)
		{
			_pt = _obj.GetPos();
			_ptDefault = _obj.GetDefaultPos();
			_sz = _obj.GetSize();
			_szDefault = _obj.GetDefaultSize();
			_name = _obj.GetName();
			_content = _obj.GetText();
			_template = _obj.GetTemplateName();

			if (obj.GetType() == AUIOBJECT_TYPE_SUBDIALOG)
			{
				AUISubDialog* pSubDlg = dynamic_cast<AUISubDialog*> (&obj);
				if (pSubDlg)
					_pDlg = pSubDlg->GetSubDialog();
			}
		}
		~OBJ_RETAIN_INFO()
		{
			_obj.SetPos(_pt.x, _pt.y);
			_obj.SetDefaultPos(_ptDefault.x, _ptDefault.y);
			_obj.SetSize(_sz.cx, _sz.cy);
			_obj.SetDefaultSize(_szDefault.cx, _szDefault.cy);
			_obj.SetName(_name);
			_obj.SetText(_content);
			_obj.SetTemplateName(_template);

			if (_obj.GetType() == AUIOBJECT_TYPE_SUBDIALOG)
			{
				AUISubDialog* pSubDlg = dynamic_cast<AUISubDialog*> (&_obj);
				if (pSubDlg)
				{
					if (_pDlg) _pDlg->SetDestroyFlag(false);
					pSubDlg->SetDialog(_pDlg);
				}
			}
		}
		
		AUIObject& _obj;
		POINT _pt, _ptDefault;
		SIZE  _sz, _szDefault;
		AString _name;
		AString _template;
		ACString _content;
		PAUIDIALOG _pDlg;
	};
	OBJ_RETAIN_INFO objInfo(object); // auto recover retain info 
	
	AXMLFile xmlFile;
	if (!GenerateXml(&object, szTemplateName, xmlFile))
		return false;

	object.Release();
	if (!object.InitXML(pA3DEngine, pA3DDevice, xmlFile.GetRootItem()->GetFirstChildItem()))
		return false;

	return true;
}

bool AUITemplate::DeleteControl(PAUIOBJECT pAUIObject)
{
	if (!AUIDialog::DeleteControl(pAUIObject))
		return false;
	if (m_pObjRender == pAUIObject)
		m_pObjRender = NULL;

	return true;
}

void AUITemplate::SetObjectRender(PAUIOBJECT pObj)
{	
	if (m_pObjRender)
		m_pObjRender->Show(false);
	
	if (pObj)
		pObj->Show(true);

	m_pObjRender = pObj;
}

abase::vector<PAUIOBJECT> AUITemplate::GetTypeObjects(int iType)
{
	abase::vector<PAUIOBJECT> vObjs;
	for (PAUIOBJECTLISTELEMENT p = m_pListHead; p != NULL; p = p->pNext)
	{
		if (!p->pThis)
			continue;
		
		if (p->pThis->GetType() == iType)
		{
			 vObjs.push_back(p->pThis);
		}
	}

	return vObjs;
}

//////////////////////////////////////////////////////////////////////////
AUITemplateManager AUITemplateManager::ms_tempMan;

AUITemplateManager& AUITemplateManager::GetSingleton()
{
	return ms_tempMan;
}

AUITemplateManager::AUITemplateManager()
{
	m_bRender = false;
	m_iCurScheme = 0;
}

void AUITemplateManager::EnableRender()
{
	m_bRender = true;
}

PAUIDIALOG AUITemplateManager::CreateDlgInstance(const AString strTemplName)
{
	return new AUITemplate;
}

bool AUITemplateManager::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, const char *pszFilename /* = NULL */, 
							  int nDefaultWidth /* = AUIMANAGER_DEFAULTWIDTH */, int nDefaultHeight /* = AUIMANAGER_DEFAULTHEIGHT */)
{
	if (!AUIManager::Init(pA3DEngine, pA3DDevice, pszFilename, nDefaultWidth, nDefaultHeight))
		return false;

	m_aThemes.clear();
	m_iCurScheme = 0;
	for (size_t i=0; i<m_aDialog.size(); ++i)
	{
		m_aDialog[i]->Show(false);
		m_aDialog[i]->SetPosEx(0, 0);
		if (m_aDialog[i]->GetParentDlgControl())
			continue;

		if (dynamic_cast<AUITemplate*> (m_aDialog[i]))
			m_aThemes.push_back(dynamic_cast<AUITemplate*> (m_aDialog[i]));
	}

	return true;
}

int AUITemplateManager::AddEmptyScheme(const char* pszSchemeName)
{
	int id = CreateEmptyDlg(m_pA3DEngine, m_pA3DDevice, pszSchemeName);
	if (id == AUIMANAGER_WRONGDIALOGID)
		return AUIMANAGER_WRONGDIALOGID;

	AUITemplate* pTemplate = (dynamic_cast<AUITemplate*>(m_aDialog[id]));
	if (pTemplate)
	{
		pTemplate->m_szFilename = pszSchemeName;
		pTemplate->m_szFilename += ".xml";
	}

	return AppendScheme(id);
}

int AUITemplateManager::AddScheme(const char* pszFileName)
{
	int id = CreateDlg(m_pA3DEngine, m_pA3DDevice, pszFileName);
	if (id == AUIMANAGER_WRONGDIALOGID)
		return AUIMANAGER_WRONGDIALOGID;

	return AppendScheme(id);
}

int AUITemplateManager::AppendScheme(int id)
{
	if (!dynamic_cast<AUITemplate*> (m_aDialog[id]))
		return AUIMANAGER_WRONGDIALOGID;

	m_aThemes.push_back(dynamic_cast<AUITemplate*> (m_aDialog[id]));
	return (int)m_aThemes.size() - 1;
}

bool AUITemplateManager::SetCurScheme(const char* szSchemeName)
{
	for (size_t i=0; i<m_aThemes.size(); i++)
	{
		if (stricmp(m_aThemes[i]->GetName(), szSchemeName) == 0)
		{
			return SetCurScheme((int)i);
		}
	}

	return false;
}

AUITemplate* AUITemplateManager::GetScheme(int idScheme)
{
	if (idScheme < 0 || idScheme >= m_aThemes.size())
		return NULL;

	return m_aThemes[idScheme];
}

bool AUITemplateManager::SetCurScheme(int idScheme)
{
	if (idScheme < 0 || idScheme >= m_aThemes.size())
		return false;

	if (m_iCurScheme >= 0 && m_iCurScheme < m_aThemes.size())
		m_aThemes[m_iCurScheme]->Show(false);
	
	m_iCurScheme = idScheme;
	m_aThemes[idScheme]->Show(m_bRender);

	return true;
}


AUITemplate* AUITemplateManager::GetCurScheme()
{
	if (m_iCurScheme >=0 && m_iCurScheme < m_aThemes.size())
		return m_aThemes[m_iCurScheme];
	
	return NULL;
}

abase::vector<AUITemplate*> AUITemplateManager::GetSchemeList()
{
	return m_aThemes;
}

abase::vector<AString> AUITemplateManager::GetSchemeNames()
{
	abase::vector<AString> aNames;
	for (size_t i=0; i<m_aThemes.size(); i++)
		aNames.push_back(m_aThemes[i]->GetName());

	return aNames;
}

bool AUITemplateManager::Save(const char *pszFilename)
{
	FILE *file = fopen(pszFilename, "wt");
	if( !file ) return false;

	for( int i = 0; i < (int)m_aThemes.size(); i++ )
	{
		fprintf(file, "%s %d %d %d\n", PathFindFileNameA(m_aThemes[i]->GetFilename()),
			0, 0, 0);
	}

	fclose(file);

	return true;
}

int AUITemplateManager::GetSchemeNum() const
{
	return (int)m_aThemes.size();
}