// EleDataManDoc.cpp : implementation of the CEleDataManDoc class
//

#include "stdafx.h"
#include "EleDataMan.h"

#include "EleDataManDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEleDataManDoc

IMPLEMENT_DYNCREATE(CEleDataManDoc, CDocument)

BEGIN_MESSAGE_MAP(CEleDataManDoc, CDocument)
	//{{AFX_MSG_MAP(CEleDataManDoc)
	ON_COMMAND(ID_FILE_SAVE_AS, OnFileSaveAs)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEleDataManDoc construction/destruction

CEleDataManDoc::CEleDataManDoc()
{
	// TODO: add one-time construction code here
}

CEleDataManDoc::~CEleDataManDoc()
{
}

BOOL CEleDataManDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}


void CEleDataManDoc::Initialize(elementdataman& dataman)
{
/*
	unsigned int i;
	m_nNumElementData = 1000;
	for(i=0; i<m_nNumElementData; i++)
	{
		int type = rand()%(DT_MAX-1)+1;
//		int type = DT_EQUIPMENT_ADDON;

		switch(type) 
		{
		case DT_EQUIPMENT_ADDON:
			{
				EQUIPMENT_ADDON data;
				data.id = i;
				dataman.add_structure(i, data);			
				break;
			}
		case DT_WEAPON_MAJOR_TYPE:
			{
				WEAPON_MAJOR_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);			
				break;
			}
		case DT_WEAPON_SUB_TYPE:
			{
				WEAPON_SUB_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);			
				break;
			}
		case DT_WEAPON_ESSENCE:
			{
				WEAPON_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);			
				break;
			}
		case DT_ARMOR_MAJOR_TYPE:
			{
				ARMOR_MAJOR_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);			
				break;
			}
		case DT_ARMOR_SUB_TYPE:
			{
				ARMOR_SUB_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);			
				break;
			}
		case DT_ARMOR_ESSENCE:
			{
				ARMOR_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);			
				break;
			}
		case DT_DECORATION_MAJOR_TYPE:
			{
				DECORATION_MAJOR_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);			
				break;
			}
		case DT_DECORATION_SUB_TYPE:
			{
				DECORATION_SUB_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);			
				break;
			}
		case DT_DECORATION_ESSENCE:
			{
				DECORATION_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);			
				break;
			}
		case DT_MEDICINE_MAJOR_TYPE:
			{
				MEDICINE_MAJOR_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_MEDICINE_SUB_TYPE:
			{
				MEDICINE_SUB_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_MEDICINE_ESSENCE:
			{
				MEDICINE_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_MATERIAL_MAJOR_TYPE:
			{
				MATERIAL_MAJOR_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_MATERIAL_SUB_TYPE:
			{
				MATERIAL_SUB_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_MATERIAL_ESSENCE:
			{
				MATERIAL_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_DAMAGERUNE_SUB_TYPE:
			{
				DAMAGERUNE_SUB_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_DAMAGERUNE_ESSENCE:
			{
				DAMAGERUNE_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_ARMORRUNE_SUB_TYPE:
			{
				ARMORRUNE_SUB_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_ARMORRUNE_ESSENCE:
			{
				ARMORRUNE_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_SKILLTOME_SUB_TYPE:
			{
				SKILLTOME_SUB_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_SKILLTOME_ESSENCE:
			{
				SKILLTOME_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_FLYSWORD_ESSENCE:
			{
				FLYSWORD_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_TOWNSCROLL_ESSENCE:
			{
				TOWNSCROLL_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_UNIONSCROLL_ESSENCE:
			{
				UNIONSCROLL_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_REVIVESCROLL_ESSENCE:
			{
				REVIVESCROLL_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_ELEMENT_ESSENCE:
			{
				ELEMENT_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_TASKMATTER_ESSENCE:
			{
				TASKMATTER_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_TOSSMATTER_ESSENCE:
			{
				TOSSMATTER_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_PROJECTILE_TYPE:
			{
				PROJECTILE_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_PROJECTILE_ESSENCE:
			{
				PROJECTILE_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_MONSTER_ADDON:
			{
				MONSTER_ADDON data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_MONSTER_TYPE:
			{
				MONSTER_TYPE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_MONSTER_ESSENCE:
			{
				MONSTER_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_TALK_SERVICE:
			{
				NPC_TALK_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_SELL_SERVICE:
			{
				NPC_SELL_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_BUY_SERVICE:
			{
				NPC_BUY_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_REPAIR_SERVICE:
			{
				NPC_REPAIR_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_INSTALL_SERVICE:
			{
				NPC_INSTALL_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_UNINSTALL_SERVICE:
			{
				NPC_UNINSTALL_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_TASK_IN_SERVICE:
			{
				NPC_TASK_IN_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_TASK_OUT_SERVICE:
			{
				NPC_TASK_OUT_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_TASK_MATTER_SERVICE:
			{
				NPC_TASK_MATTER_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_SKILL_SERVICE:
			{
				NPC_SKILL_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_HEAL_SERVICE:
			{
				NPC_HEAL_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_TRANSMIT_SERVICE:
			{
				NPC_TRANSMIT_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_TRANSPORT_SERVICE:
			{
				NPC_TRANSPORT_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_PROXY_SERVICE:
			{
				NPC_PROXY_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_STORAGE_SERVICE:
			{
				NPC_STORAGE_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		case DT_NPC_MAKE_SERVICE:
			{
				NPC_MAKE_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}	
		case DT_NPC_DECOMPOSE_SERVICE:
			{
				NPC_DECOMPOSE_SERVICE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}	
		case DT_NPC_ESSENCE:
			{
				NPC_ESSENCE data;
				data.id = i;
				dataman.add_structure(i, data);		
				break;
			}
		default:
			break;
		}
	}
*/
}

int CEleDataManDoc::SearchTest(unsigned int times)
{
	unsigned int i=0;
	int id;
	DWORD tickCount0 = GetTickCount();
	DATA_TYPE datatype;
	for(i=0; i<times; i++)
	{
		id = rand()%m_nNumElementData;
		if( g_ElementDataMan.get_data_ptr(id, ID_SPACE_ESSENCE, datatype) == NULL )
			return -1;
	}
	DWORD tickCount1 = GetTickCount();
	return tickCount1 - tickCount0;
}

float CEleDataManDoc::GenerateTest(unsigned int times)
{
	unsigned int i=0;
	int id;
	unsigned int valid_item_count = 0;
	DWORD tickCount0 = GetTickCount();
	item_data ** itemlist;
	typedef item_data * p_item_data;
	itemlist = new p_item_data[times];
	for(i=0; i<times; i++)
	{
		itemlist[i] = NULL;
	}

	size_t s;
	for(i=0; i<times; i++)
	{
		id = rand()%1000;
		char tag = 0;
		if( g_ItemDataMan.generate_item(id, &(itemlist[i]), s, element_data::NORMAL(0),element_data::ADDON_LIST_SHOP,&tag,sizeof(tag)) == 0 )
			valid_item_count ++;
	}

	DWORD tickCount1 = GetTickCount();
	int deltatime = tickCount1 - tickCount0;
	
	for(i=0; i<times; i++)
	{
		if(itemlist[i])
			abase::fastfree(itemlist[i], 0);
	}
	delete [] itemlist;
	if(valid_item_count == 0)
		return -1;
	else 
		return (float)deltatime/(float)valid_item_count;
}
/////////////////////////////////////////////////////////////////////////////
// CEleDataManDoc serialization

void CEleDataManDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CEleDataManDoc diagnostics

#ifdef _DEBUG
void CEleDataManDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CEleDataManDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CEleDataManDoc commands

void CEleDataManDoc::OnFileSaveAs() 
{
	// TODO: Add your command handler code here
	CFileDialog dlgFile(FALSE);

	if(dlgFile.DoModal() == IDOK)
	{
		CString strPathname = dlgFile.GetPathName();
//		if(g_ItemDataMan.save_data((LPCTSTR)strPathname) != 0)
//			AfxMessageBox("Fail to save the element data!");			
	}
}

void CEleDataManDoc::OnFileOpen() 
{
	// TODO: Add your command handler code here
	CFileDialog dlgFile(TRUE);

	if(dlgFile.DoModal() == IDOK)
	{
		CString strPathname = dlgFile.GetPathName();
		if(g_ItemDataMan.load_data((LPCTSTR)strPathname) != 0)
			AfxMessageBox("Fail to open the element data!");			
	}
}
