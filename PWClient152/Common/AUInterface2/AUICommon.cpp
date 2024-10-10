// Filename	: AUIcommon.cpp
// Creator	: Tom Zhou
// Date		: May 24, 2004

#include "AUI.h"

#include "hashmap.h"

#include "A3DEngine.h"
#include "A3DFTFontMan.h"
#include "A3DFTFont.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "A2DSpriteTexMan.h"

#include "AUIObject.h"
#include "AUICommon.h"
#include "AUITextArea.h"
#include "AUIManager.h"
#include "AUITag.h"

#include <stdarg.h>

int _tab_char = 8;
static bool _report_error_no_assert		= false;
static bool _aui_enable_log				= true;
static bool _aui_enable_particular_log	= false;

abase::hash_map<AString, AUIObjectPropertyValue> mapAUIObjectPropertyType;

void AUI_ReportErrorSetNoAssert(bool bNoAssert)
{
	_report_error_no_assert = bNoAssert;
}

void AUI_EnableLog(const bool bEnable)
{
	_aui_enable_log = bEnable;
}

bool AUI_IsLogEnabled()
{ 
	return _aui_enable_log;; 
}

void AUI_EnableParticularLog(const bool bEnable)
{
	_aui_enable_particular_log = bEnable;
}

bool AUI_ReportError(int nLine, const char *pszFile)
{
	if(!AUI_IsLogEnabled())
	{
		return false;
	}
	a_LogOutput(1, "Error occurs at line %d in %s.", nLine, pszFile);
	if( !_report_error_no_assert )
		ASSERT(false);
	return false;
}

bool AUI_ReportError(int nLine, int nLevel, const char* szMsg, ...)
{
	if(!AUI_IsLogEnabled())
	{
		return false;
	}
	va_list vaList;
	va_start(vaList, szMsg);

	char msg[1000];
	vsprintf(msg, szMsg, vaList);
	char msg2[1000];
	sprintf(msg2, "%s, at line %d", msg, nLine);
	a_LogOutput(nLevel, msg2);
	
	va_end(vaList);

	if( !_report_error_no_assert )
		ASSERT(!"reached AUI_ReportError");
	return false;
}

bool AUI_ReportError(const char* szMsg, ...)
{
	if(!AUI_IsLogEnabled())
	{
		return false;
	}
	va_list vaList;
	va_start(vaList, szMsg);
	
	char msg[1000];
	vsprintf(msg, szMsg, vaList);
	a_LogOutput(1, msg);
	
	va_end(vaList);
	
	if( !_report_error_no_assert )
		ASSERT(!"reached AUI_ReportError");
	return false;
}

bool AUI_ReportError(int nLine, const char *pszFile, const char* szMsg, vector<CScriptValue>& args)
{
	if(!AUI_IsLogEnabled())
	{
		return false;
	}
    AString strParams;
	if(_aui_enable_particular_log)
	{
		const unsigned int nParams = args.size()>10?10:args.size();
		if(0==nParams)
		{
			strParams = "(no parameters)";
		}else
		{
			strParams = "(parameters: ";
			for (unsigned int i=0; i<nParams; ++i)
			{
				AString strTemp; 
				switch(args[i].m_Type) 
				{
					case CScriptValue::SVT_INVALID:
						strTemp = "invalid";
						break;
					case CScriptValue::SVT_BOOL:
						strTemp = args[i].GetBool()?"true":"false";
						break;
					case CScriptValue::SVT_NUMBER:
						strTemp.Format("%f",args[i].GetDouble());
						break;
					case CScriptValue::SVT_STRING:
					{
						AString str;
						args[i].RetrieveAString(str);
						strTemp.Format("\"%s\"",str);
						break;
					}
					case CScriptValue::SVT_ARRAY:
						strTemp = "array";
						break;
					case CScriptValue::SVT_USERDATA:
						strTemp.Format("%d(userdata)",(int)args[0].GetUserData());
						break;
					default:
						break;
				}
				if(i!=nParams-1)
				{
					strTemp+=", ";
				}else if(nParams<args.size())
				{
					strTemp += " ...";
				}
				strParams+=strTemp;
			}
			strParams += ")";
		}
	}else
	{
		strParams = " ";
	}
	AString strMsg;
	strMsg.Format("%s %s %s %d",szMsg,strParams,pszFile,nLine);
	a_LogOutput(1, strMsg);
	if( !_report_error_no_assert )
		ASSERT(!"reached AUI_ReportError");
	return false;
}

void ClearAUIObjectPropertyType()
{
	mapAUIObjectPropertyType.clear();
}

bool SetAUIObjectPropertyType(const char* strPropName, AUIObjectPropertyValue type)
{
	abase::hash_map<AString, AUIObjectPropertyValue>::iterator it = mapAUIObjectPropertyType.find(strPropName);
	if( it != mapAUIObjectPropertyType.end() )
	{
		if( it->second == type )
			return true;
		else
			return AUI_ReportError("SetAUIObjectPropertyType(), \"%s\" has multiple definition!", strPropName);
	}
	mapAUIObjectPropertyType[strPropName] = type;
	return true;
}

AUIObjectPropertyValue GetAUIObjectPropertyType(const char* strPropName)
{
	abase::hash_map<AString, AUIObjectPropertyValue>::iterator it = mapAUIObjectPropertyType.find(strPropName);
	if( it == mapAUIObjectPropertyType.end() )
		return AUIOBJECT_PROPERTY_VALUE_UNKNOWN;
	else
		return it->second;
}

bool AUI_EnumControlTypes(int &nNumControls, int *a_idControl, int nSizeOfArray)
{
	if( nSizeOfArray < AUIOBJECT_TYPE_NUM )
	{
		return false;
	}

	nNumControls = AUIOBJECT_TYPE_NUM;
	for( int i = 0; i < AUIOBJECT_TYPE_NUM; i++ )
	{
		a_idControl[i] = AUIOBJECT_TYPE_MIN + i;
	}
	return true;
}

void AUI_RetrieveEnumString(int iEnum_Type, abase::vector<AString>& vecString)
{
	vecString.clear();
	switch(iEnum_Type)
	{
	case AUIOBJECT_PROPERTY_ALIGN:
		vecString.push_back("Left");
		vecString.push_back("Center");
		vecString.push_back("Right");
		break;
	case AUIOBJECT_PROPERTY_ALIGNY:
		vecString.push_back("Top");
		vecString.push_back("Center");
		vecString.push_back("Bottom");
		break;
	case AUIOBJECT_PROPERTY_FRAMEMODE:
		vecString.push_back("Auto");
		vecString.push_back("1X1");
		vecString.push_back("3X3");
		vecString.push_back("4X4");
		vecString.push_back("3X1");
		vecString.push_back("1X3");
		break;
	case AUIOBJECT_PROPERTY_UNDERLINEMODE:
		vecString.push_back("No Underline");
		vecString.push_back("Underline use text color");
		vecString.push_back("Underline specify color");
		break;
	}
}

const char * AUI_RetrieveControlName(int idControl)
{
	static const char *a_szControlName[] =
	{
		"Check Box",			// AUIOBJECT_TYPE_CHECKBOX
		"Combo Box",			// AUIOBJECT_TYPE_COMBOBOX
		"Console",				// AUIOBJECT_TYPE_CONSOLE
		"Customize",			// AUIOBJECT_TYPE_CUSTOMIZE
		"Edit Box",				// AUIOBJECT_TYPE_EDITBOX
		"Image Picture",		// AUIOBJECT_TYPE_IMAGEPICTURE
		"Label",				// AUIOBJECT_TYPE_LABEL
		"List Box",				// AUIOBJECT_TYPE_LISTBOX
		"Model Picture",		// AUIOBJECT_TYPE_MODELPICTURE
		"Progress",				// AUIOBJECT_TYPE_PROGRESS
		"Radio Button",			// AUIOBJECT_TYPE_RADIOBUTTON
		"Scroll",				// AUIOBJECT_TYPE_SCROLL
		"Slider",				// AUIOBJECT_TYPE_SLIDER
		"Still Image Button",	// AUIOBJECT_TYPE_STILLIMAGEBUTTON
		"Sub Dialog",			// AUIOBJECT_TYPE_SUBDIALOG
		"Text Area",			// AUIOBJECT_TYPE_TEXTAREA
		"Tree View",			// AUIOBJECT_TYPE_TREEVIEW
		"Vertical Text",		// AUIOBJECT_TYPE_VERTICALTEXT
		"Window Picture",		// AUIOBJECT_TYPE_WINDOWPICTURE
	};

	if( idControl >= AUIOBJECT_TYPE_MIN && idControl < AUIOBJECT_TYPE_MAX )
		return a_szControlName[idControl - AUIOBJECT_TYPE_MIN];
	else
		return "";
}

#define AUI_pszFilterWav "Sound Wave Files (*.wav)|*.wav|All Files (*.*)|*.*||"
#define AUI_pszFilterGfx "Gfx Files (*.gfx)|*.gfx||"
#define AUI_pszFilterBmp "Image Files (*.bmp; *.tga; *.dds)|*.bmp; *.tga; *.dds|All Files (*.*)|*.*||"
#define AUI_pszFilterSMD "Skin Model Files (*.smd)|*.smd|All Files (*.*)|*.*||"
#define AUI_pszFilterXml "Dialog Files (*.xml)|*.xml;|All Files (*.*)|*.*||"

int AUI_RetrieveControlPropertyList(int idControl, PAUIOBJECT_PROPERTY a_Property)
{
	ASSERT(a_Property);
	AUIOBJECT_PROPERTY p_base[] =
	{
		{ "Name", AUIOBJECT_PROPERTY_CHAR, "" },
		{ "X", AUIOBJECT_PROPERTY_INT, "" },
		{ "Y", AUIOBJECT_PROPERTY_INT, "" },
		{ "Width", AUIOBJECT_PROPERTY_INT, "" },
		{ "Height", AUIOBJECT_PROPERTY_INT, "" },
		{ "Hint", AUIOBJECT_PROPERTY_STRING, "" },
		{ "Template", AUIOBJECT_PROPERTY_CHAR, ""}
	};
	memcpy(a_Property, p_base, sizeof(p_base));
	int nNumProperties = sizeof(p_base) / sizeof(AUIOBJECT_PROPERTY);

	switch( idControl )
	{
		case AUIOBJECT_TYPE_CHECKBOX:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Anomaly", AUIOBJECT_PROPERTY_BOOL, "" },
#ifdef _ANGELICA_AUDIO
				{ "Sound Effect", AUIOBJECT_PROPERTY_AUDIOEVENT, "" },
				{ "Hover Sound Effect", AUIOBJECT_PROPERTY_AUDIOEVENT, "" },
#else
				{ "Sound Effect", AUIOBJECT_PROPERTY_SFX_FILENAME, AUI_pszFilterWav },
				{ "Hover Sound Effect", AUIOBJECT_PROPERTY_SFX_FILENAME, AUI_pszFilterWav },
#endif
				{ "Text Content", AUIOBJECT_PROPERTY_STRING, "" },
				{ "Vertical Text", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Align", AUIOBJECT_PROPERTY_ALIGN, "" },
				{ "Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Font", AUIOBJECT_PROPERTY_FONT, "" },
				{ "Shadow", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "FreeType", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Outline", AUIOBJECT_PROPERTY_INT, "" },
				{ "Outline Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Inner Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Fade Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Upper Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Text Lower Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Check Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Check Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Normal Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Checked Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "OnHover Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "CheckedOnHover Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Text Offset X", AUIOBJECT_PROPERTY_INT, "" },
				{ "Text Offset Y", AUIOBJECT_PROPERTY_INT, "" },
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_COMBOBOX:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Align", AUIOBJECT_PROPERTY_ALIGN, "" },
#ifdef _ANGELICA_AUDIO
				{ "Sound Effect", AUIOBJECT_PROPERTY_AUDIOEVENT, "" },
#else
				{ "Sound Effect", AUIOBJECT_PROPERTY_SFX_FILENAME, AUI_pszFilterWav },
#endif
				{ "Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Font", AUIOBJECT_PROPERTY_FONT, "" },
				{ "Shadow", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "FreeType", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Outline", AUIOBJECT_PROPERTY_INT, "" },
				{ "Outline Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Inner Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Fade Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Upper Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Text Lower Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Frame Mode", AUIOBJECT_PROPERTY_FRAMEMODE, "" },
				{ "Frame Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Frame Pushed Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Frame Expand Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Hilight Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "List Box Name", AUIOBJECT_PROPERTY_CHAR, "" },
				{ "Line Space", AUIOBJECT_PROPERTY_INT, "" },
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_CONSOLE:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Font", AUIOBJECT_PROPERTY_FONT, "" }
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_CUSTOMIZE:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Object Type", AUIOBJECT_PROPERTY_CHAR, "" }
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_EDITBOX:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Anomaly", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Is Password", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Is ReadOnly", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Is Number Only", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Is Multiple Line", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Is Auto Return", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Is Want Return", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Frame Mode", AUIOBJECT_PROPERTY_FRAMEMODE, "" },
				{ "Frame Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Up Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Down Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Scroll Area Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Scroll Bar Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Font", AUIOBJECT_PROPERTY_FONT, "" },
				{ "Shadow", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "FreeType", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Outline", AUIOBJECT_PROPERTY_INT, "" },
				{ "Outline Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Inner Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Fade Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Upper Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Text Lower Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Max Text Length", AUIOBJECT_PROPERTY_INT, "" },
				{ "Line Space", AUIOBJECT_PROPERTY_INT, ""},
				{ "Enable Selection", AUIOBJECT_PROPERTY_BOOL, ""},
				{ "Selection Background Color",	AUIOBJECT_PROPERTY_COLOR, ""},
				{ "Text Align", AUIOBJECT_PROPERTY_ALIGN, ""}
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_IMAGEPICTURE:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Image Degree", AUIOBJECT_PROPERTY_INT, "" },
				{ "Gfx File", AUIOBJECT_PROPERTY_GFX_FILENAME, AUI_pszFilterGfx },
				{ "Clock File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Frames Number", AUIOBJECT_PROPERTY_INT, "" },
				{ "Frame Interval", AUIOBJECT_PROPERTY_INT, "" },
				{ "Render Top", AUIOBJECT_PROPERTY_BOOL, "" }
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_LABEL:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Text Content", AUIOBJECT_PROPERTY_STRING, "" },
				{ "Vertical Text", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Align", AUIOBJECT_PROPERTY_ALIGN, "" },
				{ "Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Font", AUIOBJECT_PROPERTY_FONT, "" },
				{ "Shadow", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "FreeType", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Outline", AUIOBJECT_PROPERTY_INT, "" },
				{ "Outline Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Inner Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Fade Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Upper Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Text Lower Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Line Space", AUIOBJECT_PROPERTY_INT, "" },
				{ "Force Dynamic Render", AUIOBJECT_PROPERTY_BOOL, "" },
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_LISTBOX:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
#ifdef _ANGELICA_AUDIO
				{ "Sound Effect", AUIOBJECT_PROPERTY_AUDIOEVENT, "" },
#else
				{ "Sound Effect", AUIOBJECT_PROPERTY_SFX_FILENAME, AUI_pszFilterWav },
#endif
				{ "Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Font", AUIOBJECT_PROPERTY_FONT, "" },
				{ "Shadow", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "FreeType", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Outline", AUIOBJECT_PROPERTY_INT, "" },
				{ "Outline Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Inner Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Fade Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Upper Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Text Lower Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Auto Wrap", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Horizontal Scroll", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Align", AUIOBJECT_PROPERTY_ALIGNY, "" },
				{ "Hover Hilight", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Frame Mode", AUIOBJECT_PROPERTY_FRAMEMODE, "" },
				{ "Frame Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Hilight Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Item Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp},
				{ "Up Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Down Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Scroll Area Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Scroll Bar Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Multiple Selection", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Multiple Column Info", AUIOBJECT_PROPERTY_CHAR, "" },
				{ "Line Space", AUIOBJECT_PROPERTY_INT, "" },				
				{ "Row Fixed Height", AUIOBJECT_PROPERTY_INT, "" },				
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_MODELPICTURE:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Model File", AUIOBJECT_PROPERTY_FILENAME, AUI_pszFilterSMD },
				{ "Background Color", AUIOBJECT_PROPERTY_COLOR, "" }
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_PROGRESS:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Frame Mode", AUIOBJECT_PROPERTY_FRAMEMODE, "" },
				{ "Frame Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Fill Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Pos Image File", AUIOBJECT_PROPERTY_FILENAME, AUI_pszFilterBmp },
				{ "Pos Bg Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Inc Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Dec Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Vertical Progress", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Reverse Direction", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Force Dynamic Render", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Fill Margin Left", AUIOBJECT_PROPERTY_INT, "" },
				{ "Fill Margin Top", AUIOBJECT_PROPERTY_INT, "" },
				{ "Fill Margin Right", AUIOBJECT_PROPERTY_INT, "" },
				{ "Fill Margin Bottom", AUIOBJECT_PROPERTY_INT, "" }
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_RADIOBUTTON:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Anomaly", AUIOBJECT_PROPERTY_BOOL, "" },
#ifdef _ANGELICA_AUDIO
				{ "Sound Effect", AUIOBJECT_PROPERTY_AUDIOEVENT, "" },
				{ "Hover Sound Effect", AUIOBJECT_PROPERTY_AUDIOEVENT, "" },
#else
				{ "Sound Effect", AUIOBJECT_PROPERTY_SFX_FILENAME, AUI_pszFilterWav },
				{ "Hover Sound Effect", AUIOBJECT_PROPERTY_SFX_FILENAME, AUI_pszFilterWav },
#endif
				{ "Text Content", AUIOBJECT_PROPERTY_STRING, "" },
				{ "Vertical Text", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Align", AUIOBJECT_PROPERTY_ALIGN, "" },
				{ "Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Font", AUIOBJECT_PROPERTY_FONT, "" },
				{ "Shadow", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "FreeType", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Outline", AUIOBJECT_PROPERTY_INT, "" },
				{ "Outline Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Inner Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Fade Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Upper Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Text Lower Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Check Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Check Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Normal Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Checked Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "OnHover Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "CheckedOnHover Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Group ID", AUIOBJECT_PROPERTY_INT, "" },
				{ "Button ID", AUIOBJECT_PROPERTY_INT, "" },
				{ "Text Offset X", AUIOBJECT_PROPERTY_INT, "" },
				{ "Text Offset Y", AUIOBJECT_PROPERTY_INT, "" },
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_SCROLL:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Up Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Down Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Scroll Area Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Frame Mode", AUIOBJECT_PROPERTY_FRAMEMODE, "" },
				{ "Scroll Bar Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp }
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_SLIDER:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Frame Mode", AUIOBJECT_PROPERTY_FRAMEMODE, "" },
				{ "Frame Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Bar Image File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Total Levels", AUIOBJECT_PROPERTY_INT, "" },
				{ "Vertical Slider", AUIOBJECT_PROPERTY_BOOL, ""}
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_STILLIMAGEBUTTON:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Anomaly", AUIOBJECT_PROPERTY_BOOL, "" },
#ifdef _ANGELICA_AUDIO
				{ "Sound Effect", AUIOBJECT_PROPERTY_AUDIOEVENT, "" },
				{ "Hover Sound Effect", AUIOBJECT_PROPERTY_AUDIOEVENT, "" },
#else
				{ "Sound Effect", AUIOBJECT_PROPERTY_SFX_FILENAME, AUI_pszFilterWav },
				{ "Hover Sound Effect", AUIOBJECT_PROPERTY_SFX_FILENAME, AUI_pszFilterWav },
#endif
				{ "Text Content", AUIOBJECT_PROPERTY_STRING, "" },
				{ "Vertical Text", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Text Offset X", AUIOBJECT_PROPERTY_INT, "" },
				{ "Text Offset Y", AUIOBJECT_PROPERTY_INT, "" },
				{ "Font", AUIOBJECT_PROPERTY_FONT, "" },
				{ "Shadow", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "FreeType", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Outline", AUIOBJECT_PROPERTY_INT, "" },
				{ "Outline Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Inner Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Fade Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Upper Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Text Lower Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Hover Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Hover Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Frame Mode", AUIOBJECT_PROPERTY_FRAMEMODE, "" },
				{ "Up Frame File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Down Frame File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "OnHover Frame File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Disabled Frame File", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Image Degree", AUIOBJECT_PROPERTY_INT, "" },
				{ "Down Gfx File", AUIOBJECT_PROPERTY_GFX_FILENAME, AUI_pszFilterGfx },
				{ "OnHover Gfx File", AUIOBJECT_PROPERTY_GFX_FILENAME, AUI_pszFilterGfx },
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_SUBDIALOG:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Dialog File", AUIOBJECT_PROPERTY_INTERFACE_FILENAME, AUI_pszFilterXml },
				{ "Enable Dialog Frame", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Frame Mode", AUIOBJECT_PROPERTY_FRAMEMODE, "" },
				{ "Frame Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Up Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Down Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "VScroll Area Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "VScroll Bar Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Left Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Right Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "HScroll Area Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "HScroll Bar Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_TEXTAREA:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Text Content", AUIOBJECT_PROPERTY_STRING, ""},
				{ "Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Font", AUIOBJECT_PROPERTY_FONT, "" },
				{ "Shadow", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "FreeType", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Outline", AUIOBJECT_PROPERTY_INT, "" },
				{ "Outline Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Inner Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Fade Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Upper Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Text Lower Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Frame Mode", AUIOBJECT_PROPERTY_FRAMEMODE, "" },
				{ "Frame Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Up Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Down Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Scroll Area Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Scroll Bar Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Left Scroll Bar", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Transparent", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Trans Scroll Wheel", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Line Space", AUIOBJECT_PROPERTY_INT, "" },
				{ "Link Underline Mode", AUIOBJECT_PROPERTY_UNDERLINEMODE, "" },
				{ "Link Underline Color", AUIOBJECT_PROPERTY_COLOR, "" },
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_TREEVIEW:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Auto Wrap", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Transparent", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Trans Scroll Wheel", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Long Hilight Bar", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Font", AUIOBJECT_PROPERTY_FONT, "" },
				{ "Shadow", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "FreeType", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Outline", AUIOBJECT_PROPERTY_INT, "" },
				{ "Outline Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Inner Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Fade Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Upper Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Text Lower Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Frame Mode", AUIOBJECT_PROPERTY_FRAMEMODE, "" },
				{ "Frame Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Hilight Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Up Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Down Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Scroll Area Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Scroll Bar Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Plus Symbol Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Minus Symbol Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Leaf Symbol Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Leaf Check Symbol Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Text Indent", AUIOBJECT_PROPERTY_INT, "" },
				{ "Line Space", AUIOBJECT_PROPERTY_INT, "" },
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_VERTICALTEXT:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Text Content", AUIOBJECT_PROPERTY_STRING, "" },
				{ "Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Font", AUIOBJECT_PROPERTY_FONT, "" },
				{ "Shadow", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "FreeType", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Outline", AUIOBJECT_PROPERTY_INT, "" },
				{ "Outline Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Inner Text Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Fade Color", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Text Upper Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Text Lower Color", AUIOBJECT_PROPERTY_COLOR, "" },
				{ "Frame Mode", AUIOBJECT_PROPERTY_FRAMEMODE, "" },
				{ "Frame Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Left Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Right Button Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Scroll Area Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Scroll Bar Image", AUIOBJECT_PROPERTY_SURFACE_FILENAME, AUI_pszFilterBmp },
				{ "Left To Right", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Up Scroll Bar", AUIOBJECT_PROPERTY_BOOL, "" },
				{ "Line Space", AUIOBJECT_PROPERTY_INT, "" },
			};
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
		case AUIOBJECT_TYPE_WINDOWPICTURE:
		{
			AUIOBJECT_PROPERTY p_add[] =
			{
				{ "Draw External", AUIOBJECT_PROPERTY_BOOL, "" },
			};
				
			memcpy(a_Property + nNumProperties, p_add, sizeof(p_add));
			nNumProperties += sizeof(p_add) / sizeof(AUIOBJECT_PROPERTY);
			break;
		}
	}

	return nNumProperties;
}

bool AUI_GetTextRect(A3DFTFont *pFont, const ACHAR *pszText,
	int& nWidth, int& nHeight, int& nLines, int nSpace, EditBoxItemsSet* pItemsSet, int nEmotionW, int nEmotionH, abase::vector<A2DSprite*> *pvecImage, 
	float fWindowScale, abase::vector<FONT_WITH_SHADOW>* pvecOtherFonts, bool bAutoIndent, int nWordWidth)
{
	nWidth = 0;
	nHeight = 0;
	nLines = 1;

	A3DPOINT2 ptSize;
	EditBoxItemBase* pItem;
	int nTextW = 0, nTextH = 0;
	int nFontIndex = 0;
	
	A3DFTFont* pCurFont = pFont;
	ptSize = pCurFont->GetTextExtent(_AL("W"));
	nTextW = ptSize.x;
	nTextH = ptSize.y;
	int tab_width = _tab_char * nTextW;

	if( nEmotionW == 0 || nEmotionH == 0 )
	{
		nEmotionW = nTextW;
		nEmotionH = nTextH;
	}

	ACHAR szThis[2] = { 0, 0 };
	int i, nWidthThis = 0, nHeightThis = nTextH, nNumQuote = 0;

	nHeight = 0;
	int nLen = (int)a_strlen(pszText);
	for( i = 0; i < nLen; i++ )
	{
		if( pszText[i] == '\r' )
		{
			nLines++;
			nWidth = max(nWidth, nWidthThis);
			nHeight += nHeightThis + nSpace;
			if (bAutoIndent)
				nWidthThis = 2 * nSpace;
			else
				nWidthThis = 0;
			nHeightThis = pCurFont->GetFontHeight();
			continue;
		}
		else if (IsEditboxItemCode(pszText[i]))
		{
			if (pItemsSet)
			{
				pItem = pItemsSet->GetItemByChar(pszText[i]);
				if( pItem->GetType() == enumEIEmotion )
				{
					nTextW = nEmotionW;
					if( nEmotionH > nHeightThis )
						nHeightThis = nEmotionH;
				}
				else if (pItem->GetType() == enumEIImage)
				{
					if(pvecImage && pItem->GetImageIndex() < pvecImage->size())
					{
						A2DSprite* pSprite = (*pvecImage)[pItem->GetImageIndex()];
						if( pSprite )
						{
							A2DSpriteItem *pSpriteItem = pSprite->GetItem(pItem->GetImageFrame());
							if( pSpriteItem )
							{
								nTextW = pSpriteItem->GetRect().Width() * fWindowScale * pItem->GetImageScale();
								nTextH = pSpriteItem->GetRect().Height() * fWindowScale * pItem->GetImageScale();
								if( nTextH > nHeightThis )
									nHeightThis = nTextH;
							}
						}
					}
				}
				else
				{
					ptSize = pCurFont->GetTextExtent(pItem->GetName());
					nTextW = ptSize.x;
					nTextH = ptSize.y;
					if( nTextH > nHeightThis )
						nHeightThis = nTextH;
				}
			}
		}
		else if( pszText[i] == '^' )
		{
			if( pszText[i + 1] == '^' || pszText[i + 1] == '&' )
				i++;
			else if( pszText[i + 1] == 'U' )
			{
				i += 7;
				continue;
			}
			else if( pszText[i + 1] == 'u' )
			{
				i++;
				continue;
			}
			else if( pszText[i + 1] == 'o' )
			{
				i++;
				nWidthThis += nWordWidth;
				continue;
			}
			else if( pszText[i + 1] == 'N')
			{
				i++;
				continue;
			}
			else if( i + 4 < nLen && pszText[i + 1] == 'O' )
			{
				int nIndex = ((pszText[i + 2] - '0') * 10 + pszText[i + 3] - '0') * 10 + pszText[i + 4] - '0';
				if( nIndex > 0 && pvecOtherFonts && nIndex <= pvecOtherFonts->size() && (*pvecOtherFonts)[nIndex - 1].pFont )
					pCurFont = (*pvecOtherFonts)[nIndex - 1].pFont;
				else
					pCurFont = pFont;
				i += 4;
				continue;
			}
			else
			{
				i += 6;
				continue;
			}
		}
		else if( pszText[i] == '&' )
		{
			nNumQuote++;
			continue;
		}
		else if( pszText[i] == '\n' )
			continue;
		else if( pszText[i] == '\v' )
		{
			nWidthThis = (nWidthThis / tab_width + 1) * tab_width;
			continue;
		}
		else
		{
			szThis[0] = pszText[i];
			ptSize = pCurFont->GetTextExtent(szThis);
			nTextW = ptSize.x;
			nTextH = ptSize.y;
			if( nTextH > nHeightThis )
				nHeightThis = nTextH;
		}
		nWidthThis += nTextW;
	}

	nWidth = max(nWidth, nWidthThis);
	nHeight += nHeightThis;

	return true;
}

#define DEAL_ENGLISHWORD {\
	if( bAutoReturn && pRect )\
	{\
		if( !bRotateLayout )\
		{\
			nEnglishStart = i - 1;\
			nEnglishStartX = nCurX;\
			clEnglish = color;\
			bRotateLayout = true;\
		}\
	}\
}

#define FINISH_ENGLISHWORD {\
	if( bAutoReturn && pRect && bRotateLayout )\
	{\
		if( !bDrawEnglishWord )\
		{\
			bDrawEnglishWord = true;\
			i = nEnglishStart;\
			nCurX = nEnglishStartX;\
			color = clEnglish;\
			continue;\
		}\
		else\
			bDrawEnglishWord = false;\
		bRotateLayout = false;\
	}\
}

bool AUI_TextOutFormat(A3DFTFont *pFontX, int xx, int yy, const ACHAR *pszTextX,
	A3DRECT *pRectX, int nStartLineX, int *pnLinesX, int nSpaceX, bool bAutoIndentX,
	int *pnHeightX, A2DSprite *pA2DSpriteEmotionX,
	abase::vector<AUITEXTAREA_EMOTION> *pvecEmotionX,
	int *pnLinesThisPageX,
	bool bDrawToScreenX, int nShadowX, int nAlphaX, bool bSmallEmotionX,
	EditBoxItemsSet* pItemsSetX,
	abase::vector<A2DSprite*> *pvecImageX,
	float fWindowScaleX,
	bool bAutoReturnX,
	abase::vector<FONT_WITH_SHADOW>* pvecOtherFontsX,
	int nWordWidthX,
	TEXTITEM_RCINFO* pItemRcInfo,
	bool bVertical)
{
	TAG_OP_IN_PARAMS inParams;
	TAG_OP_OUT_PARAMS outParams;
	InitTagParams(inParams, outParams,
		pFontX, xx, yy, pszTextX, 
		pRectX, nStartLineX, pnLinesX, nSpaceX, bAutoIndentX, 
		pnHeightX, pA2DSpriteEmotionX,
		pvecEmotionX,
		pnLinesThisPageX,
		bDrawToScreenX, nShadowX, nAlphaX, bSmallEmotionX,
		pItemsSetX,
		pvecImageX,
		fWindowScaleX,
		bAutoReturnX,
		pvecOtherFontsX,
		nWordWidthX,
		pItemRcInfo,
		bVertical);

	TAG_RESOLVER_CONTEXT context;
	if (!AUIRichTextPrinter::InitContext(inParams, outParams, context))
		return false;

	AUIRichTextPrinter& textPrint = AUIRichTextPrinter::GetSingleton();
	return textPrint.Output(inParams, context, outParams);
}

bool AUI_TextOutFormat(TAG_OP_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams)
{	
	TAG_RESOLVER_CONTEXT context;
	outParams._pCacheMan = NULL;
	if (!AUIRichTextPrinter::InitContext(inParams, outParams, context))
		return false;

	return AUIRichTextPrinter::GetSingleton().Output(inParams, context, outParams);
}

void AUI_ConvertChatString(const ACHAR *pszChat, ACHAR *pszConv, bool bName)
{
	int i, nLen = 0;
	if (!pszChat || !pszConv)
		return;

	pszConv[0] = 0;
	for( i = 0; i < (int)a_strlen(pszChat); i++ )
	{
		if( pszChat[i] == '^' )
		{
			pszConv[nLen] = '^';
			pszConv[nLen + 1] = '^';
			nLen += 2;
		}
		else if( pszChat[i] == '&' )
		{
			pszConv[nLen] = '^';
			pszConv[nLen + 1] = '&';
			nLen += 2;
		}
		else
		{
			pszConv[nLen] = pszChat[i];
			nLen++;
		}
	}
	pszConv[nLen] = 0;
}

void AUI_ConvertChatString_S(const ACHAR *pszChat, int iChatLen, ACHAR *pszConv, int iConvLen, bool bName)
{
	int i, nLen = 0;
	if (!pszChat || !pszConv)
		return;

	int iTemp = a_strlen(pszChat);
	iChatLen = a_Min(iChatLen, iTemp);
	pszConv[0] = 0;
	for( i = 0; i < iChatLen && nLen < iConvLen-2; i++ )
	{
		if( pszChat[i] == '^' )
		{
			pszConv[nLen] = '^';
			pszConv[nLen + 1] = '^';
			nLen += 2;
		}
		else if( pszChat[i] == '&' )
		{
			pszConv[nLen] = '^';
			pszConv[nLen + 1] = '&';
			nLen += 2;
		}
		else
		{
			pszConv[nLen] = pszChat[i];
			nLen++;
		}
	}
	pszConv[nLen] = 0;
}

void AUI_ConvertChatString(const ACString& strChat, ACString& strConv, bool bName)
{
	strConv.Empty();

	if (strChat.IsEmpty())
		return;

	for( int i = 0; i < strChat.GetLength(); i++ )
	{
		if( strChat[i] == '^' )
		{
			strConv += '^';
			strConv += '^';
		}
		else if( strChat[i] == '&' )
		{
			strConv += '^';
			strConv += '&';
		}
		else
		{
			strConv += strChat[i];
		}
	}
	strConv += (ACHAR)'\0';
}

DWORD AUI_A2DSpriteGetPixel(A2DSprite* pA2DSprite, int x, int y)
{
	D3DLOCKED_RECT rectLocked;
	HRESULT hval;
	for(int i = 0; i < pA2DSprite->GetTextureNum(); i++)
	{
		A2DSpriteTextures::TEXTURE_RECT *pTextureRect = pA2DSprite->GetTextures()->GetTextureRect(i);
		if( pTextureRect && pTextureRect->rect.PtInRect(x, y) )
		{
			x -= pTextureRect->rect.left;
			y -= pTextureRect->rect.top;
			RECT rc = {x, y, x+1, y+1};
			hval = pA2DSprite->GetTexture(i)->GetD3DTexture()->LockRect(0, &rectLocked, &rc, D3DLOCK_READONLY);
			if( hval != D3D_OK )
				return true;

			DWORD ret = *((DWORD *)rectLocked.pBits);
			pA2DSprite->GetTexture(i)->GetD3DTexture()->UnlockRect(0);
			return ret;
		}
	}

	return 0;
}

wchar_t EditBoxItemsSet::AppendItem(EditboxItemType type, A3DCOLOR cl, const ACHAR* szName, const ACHAR* szInfo)
{
	EditBoxItemBase* pItem = new EditBoxItemBase(type);
	pItem->SetColor(cl);
	pItem->SetName(szName);
	pItem->SetInfo(szInfo);

	wchar_t ret = AppendItem(pItem);

	if (ret)
		return ret;

	delete pItem;
	return 0;
}

EditBoxItemBase* EditBoxItemBase::m_mapCustomType[MAXNUM_CUSTOM_ITEM] = {NULL};

const int	EditBoxItemBase::s_CoordTagNum		= 5;
const int	EditBoxItemBase::s_ImageTagNum		= 5;
const int	EditBoxItemBase::s_EmotionTagNum	= 1;
const int	EditBoxItemBase::s_BoxTagNum		= 4;
const int	EditBoxItemBase::s_TaskTagNum		= 2;
const int	EditBoxItemBase::s_CommonTagNum		= 1;

ACString EditBoxItemBase::Serialize() const
{
	ACString str;
	ACHAR buf[64] = {0};

	switch (m_type)
	{
	case enumEICoord:
		_snwprintf(buf, 64, L"<%d><%d><%d><%d><", m_type, m_dwColor, m_bUnderLine, m_bSameColor ? m_dwColor : m_dwUnderLineColor);
		str = buf + m_strName;
		str += L"><";
		str += m_strInfo;
		str += L">";
		break;

	case enumEIImage:
		_snwprintf(buf, 64, L"<%d><%d><%d><%d><%f><", m_type, m_dwColor, m_nImageIndex, m_nImageFrame, m_fImageScale);
		str = buf + m_strInfo;
		str += L">";
		break;

	case enumEIBox:
		_snwprintf(buf, 64, L"<%d><%d><%d><%f><", m_type, m_dwColor, m_nColorboxWidth, m_fBoxHeightScale);
		str = buf + m_strInfo;
		str += L">";
		break;

	case enumEITask:
		_snwprintf(buf, 64, L"<%d><", m_type);
		str = buf + m_strName;
		str += L"><";
		str += m_strInfo;
		str += L">";
		break;

	default:
		swprintf(buf, L"<%d><", m_type);
		str = buf + m_strInfo;
		str += L">";
		break;
	}
	
	return str;
}

int EditBoxItemBase::_GetItemRenderdLength(const ACHAR*& szText)
{
	int type = _UnserializeItemType(szText);
	if (type == -1)
	{
		szText++;
		return 1;
	}

	const EditBoxItemBase* pItem = EditBoxItemBase::GetCustomItemFromType((EditboxItemType)type);
	int nTag = s_CommonTagNum;
	int nTagNameIndex = -1;
	if (pItem)
	{
		nTag = pItem->GetContentTagNum();
		nTagNameIndex = pItem->GetRenderTextTagIndex();
	}
	else
	{
		switch (type)
		{
		case enumEICoord:
			nTag = s_CoordTagNum;
			nTagNameIndex = 3;
			break;
			
		case enumEIEmotion:
			nTag = s_EmotionTagNum;
			break;

		case enumEIImage:
			nTag = s_ImageTagNum;
			break;

		case enumEIBox:
			nTag = s_BoxTagNum;
			break;

		case enumEITask:
			nTag = s_TaskTagNum;
			nTagNameIndex = 0;
			break;

		default:
			nTag = s_CommonTagNum;
			break;
		}
	}
	
	int nWord = 1;
	if (nTagNameIndex != -1)
	{
		abase::vector<abase::pair<const ACHAR*, const ACHAR*> > Tags;
		if (_ParseItems(szText, nTag, &Tags) && nTagNameIndex < Tags.size() && nTagNameIndex >= 0)
			nWord = (Tags[nTagNameIndex].second - Tags[nTagNameIndex].first);
	}
	else
	{
		_ParseItems(szText, nTag, NULL);
	}

	return nWord;
}

int EditBoxItemBase::_UnserializeItemType(const ACHAR*& szText)
{
	const ACHAR* szNext = wcschr(szText, _AL('<'));
	
	if (szNext == NULL)
		return -1;
	
	szNext++;
	int type = _wtoi(szNext);
	
	if (type < 0 || type >= enumEINum)
		return -1;
	
	szNext = wcschr(szNext, _AL('>'));
	if (szNext == NULL)
		return -1;
	szText = szNext+1;
	
	return type;
}

EditBoxItemBase* EditBoxItemBase::Unserialize(const ACHAR*& szText)
{
	int type = _UnserializeItemType(szText);
	if (type == -1)
		return NULL;

	const EditBoxItemBase* pItem = EditBoxItemBase::GetCustomItemFromType((EditboxItemType)type);
	EditBoxItemBase* pItemNew;
	if (!pItem)
	{
		if (type >= enumEIEmotion && type < enumEICustom)
			pItemNew = new EditBoxItemBase((EditboxItemType)type);
		else
			return NULL;
	}
	else
	{
		pItemNew = pItem->Create();
	}

	if (!pItemNew->UnserializeContent(szText))
	{
		delete pItemNew;
		return NULL;
	}
	return pItemNew;
}

// <><><><>...
bool EditBoxItemBase::_ParseItems(const ACHAR*& szText, int nTag, abase::vector<abase::pair<const ACHAR*, const ACHAR*> >* Tags)
{
	if (!szText || nTag < 0)
		return false;	

	if (Tags)
		Tags->clear();

	const ACHAR* szTemp = szText;
	for (int i=0; i<nTag; ++i)
	{
		szTemp = wcsstr(szTemp, _AL("<"));
		if (!szTemp)
			return false;

		const ACHAR* szEnd = wcsstr(szTemp, _AL(">"));
		if (!szEnd)
			return false;

		if (Tags)
			Tags->push_back(abase::pair<const ACHAR*, const ACHAR*> (szTemp+1, szEnd));  //[first, second)
		
		szTemp = szEnd + 1;
	}
	szText = szTemp;

	return true;
}

//<2>, szText = <bUnderLine><cl_underline><name><info>
bool EditBoxItemBase::_UnserializeContentCoord(const ACHAR*& szText)
{
	abase::vector<abase::pair<const ACHAR*, const ACHAR*> > Tags;
	if (!_ParseItems(szText, s_CoordTagNum, &Tags))
		return false;

	SetColor(_wtoi(Tags[0].first));

	m_bUnderLine = _wtoi(Tags[1].first)?true:false;

	m_dwUnderLineColor = _wtoi(Tags[2].first);

	SetName(ACString(Tags[3].first, Tags[3].second-Tags[3].first));

	SetInfo(ACString(Tags[4].first, Tags[4].second-Tags[4].first));

	return true;
}

// <name><info>
bool EditBoxItemBase::_UnserializeContentTask(const ACHAR*& szText)
{
	abase::vector<abase::pair<const ACHAR*, const ACHAR*> > Tags;
	if (!_ParseItems(szText, s_TaskTagNum, &Tags))
		return false;

	SetName(ACString(Tags[0].first, Tags[0].second-Tags[0].first));

	SetInfo(ACString(Tags[1].first, Tags[1].second-Tags[1].first));

	return true;
}

//<color><index><frame><scale><info>
bool EditBoxItemBase::_UnserializeContentImage(const ACHAR*& szText)
{
	abase::vector<abase::pair<const ACHAR*, const ACHAR*> > Tags;
	if (!_ParseItems(szText, s_ImageTagNum, &Tags))
		return false;

	SetColor(_wtoi(Tags[0].first));

	SetImageIndex(_wtoi(Tags[1].first));

	SetImageFrame(_wtoi(Tags[2].first));

	float f = 1.0f;
	swscanf(Tags[3].first, _AL("%f"), &f);
	SetImageScale(f);

	SetInfo(ACString(Tags[4].first, Tags[4].second-Tags[4].first));

	return true;
}

//<color><box_width><box_height_scale><info>
bool EditBoxItemBase::_UnserializeContentBox(const ACHAR*& szText)
{
	abase::vector<abase::pair<const ACHAR*, const ACHAR*> > Tags;
	if (!_ParseItems(szText, s_BoxTagNum, &Tags))
		return false;

	SetColor(_wtoi(Tags[0].first));
	SetBoxWidth(_wtoi(Tags[1].first));
	float f = 1.0f;
	swscanf(Tags[2].first, _AL("%f"), &f);
	SetBoxHeightScale(f);
	SetInfo(ACString(Tags[3].first, Tags[3].second-Tags[3].first));

	return true;
}

//<info>
bool EditBoxItemBase::_UnserialzieContentCommon(const ACHAR*& szText)
{
	abase::vector<abase::pair<const ACHAR*, const ACHAR*> > Tags;
	if (!_ParseItems(szText, s_CommonTagNum, &Tags))
		return false;
	SetInfo(ACString(Tags[0].first, Tags[0].second-Tags[0].first));
	return true;
}

//<info><name>
bool EditBoxItemBase::_UnserializeContentEmotion(const ACHAR*& szText)
{
	if (!szText)
		return false;

	abase::vector<abase::pair<const ACHAR*, const ACHAR*> > Tags;
	if (!_ParseItems(szText, s_EmotionTagNum, &Tags))
		return false;

	SetInfo(ACString(Tags[0].first, Tags[0].second-Tags[0].first));
	SetName(_AL("W"));
	return true;
}

int EditBoxItemBase::GetContentTagNum() const
{
	switch (m_type)
	{
	case enumEICoord:
		return s_CoordTagNum;
		
	case enumEIEmotion:
		return s_EmotionTagNum;
		
	case enumEIImage:
		return s_ImageTagNum;
		
	case enumEIBox:
		return s_BoxTagNum;
		
	case enumEITask:
		return s_TaskTagNum;
		
	default:
		return s_CommonTagNum;
	}
}

int EditBoxItemBase::GetRenderTextTagIndex() const
{
	switch (m_type)
	{
	case enumEICoord:
		return 0;
		
	case enumEIEmotion:
		return -1;
		
	case enumEIImage:
		return -1;
		
	case enumEIBox:
		return -1;
		
	case enumEITask:
		return 0;
	
	default:
		return -1;
	}
}

bool EditBoxItemBase::UnserializeContent(const ACHAR*& szText)
{
	if (!szText)
		return false;

	switch (m_type)
	{
	case enumEICoord:
		if (!_UnserializeContentCoord(szText))
			return false;
		break;

	case enumEIEmotion:
		if (!_UnserializeContentEmotion(szText))
			return false;
		break;

	case enumEIImage:
		if (!_UnserializeContentImage(szText))
			return false;
		break;

	case enumEIBox:
		if (!_UnserializeContentBox(szText))
			return false;
		break;

	case enumEITask:
		if (!_UnserializeContentTask(szText))
			return false;
		break;

	default:
		if (!_UnserialzieContentCommon(szText))
			return false;
		break;
	}

	return true;
}

int EditBoxItemBase::GetUnserializedLength(const ACHAR* szText)
{
	if (!szText)
		return 0;
	
	int nWord = 0;
	for(; *szText; )
	{
		wchar_t ch = *szText;
		
		if (IsEditboxItemCode(ch))
		{
			nWord += _GetItemRenderdLength(szText);
		}		
		else
		{
			if (ch != '\n')
				nWord++;
			++szText;
		}
	}
	
	return nWord;
}

int EditBoxItemBase::GetRenderedLength() const
{
	switch (m_type)
	{
	case enumEICoord:
	case enumEITask:
		return m_strName.GetLength();
		
	default:
		return 1;
	}
}

ACString MarshalEditBoxText(const ACHAR* szText, const EditBoxItemsSet& ItemsSet)
{
	int i = 0;
	int iStart = 0;
	ACString str;

	while (szText[i])
	{
		if (IsEditboxItemCode(szText[i]))
		{
			str += ACString(&szText[iStart], i - iStart + 1);
			iStart = i + 1;

			EditBoxItemBase* pItem = ItemsSet.GetItemByChar(szText[i]);

			if (pItem)
				str += pItem->Serialize();
		}

		i++;
	}

	if (i > iStart)
		str += ACString(&szText[iStart], i - iStart);

	return str;
}

ACString UnmarshalEditBoxText(const ACHAR* szText, EditBoxItemsSet& ItemsSet, int nMsgIndex, const ACHAR* szIvtrItem,
	A3DCOLOR clIvtrItem, int nItemMask, const EditboxScriptItem** ppScriptItems, int nScriptItemCount,
	bool bUnderLine, bool bSameColor, A3DCOLOR clUnderLine)
{
	if( !szText )
		return _AL("");

	AUI_UNMARSH_SCRIPTITEM_INFO scriptInfo = {ppScriptItems, nScriptItemCount};
	AUI_UNMARSH_UNDERLINE_INFO underlineInfo = {bUnderLine, bSameColor, clUnderLine};
	return UnmarshalEditBoxTextEx(szText, ItemsSet, nMsgIndex, szIvtrItem, clIvtrItem, nItemMask, &scriptInfo, &underlineInfo);
}
ACString UnmarshalEditBoxTextEx(const ACHAR* szText, EditBoxItemsSet& ItemsSet, int nMsgIndex, const ACHAR* szIvtrItem,
							    A3DCOLOR clIvtrItem, int nItemMask, 
								AUI_UNMARSH_SCRIPTITEM_INFO* pScriptItemInfo, 
								AUI_UNMARSH_UNDERLINE_INFO* pUnderLineInfo)
{
	if( !szText )
		return _AL("");
	const ACHAR* szStart = szText;
	ACString str;
	int nCurScriptIndex = 0;

	while (*szText)
	{
		wchar_t ch = *szText;
		
		if (IsEditboxItemCode(ch))
		{
			if (szText > szStart)
				str += ACString(szStart, szText - szStart);

			szText++;
			EditBoxItemBase* pItem = EditBoxItemBase::Unserialize(szText);
			szStart = szText;

			if (!pItem)
				continue;
			
			if ((pItem->GetType() >= enumEICustom && (nItemMask & (1 << enumEICustom))) || (nItemMask & (1 << pItem->GetType())))
			{
				ch = ItemsSet.AppendItem(pItem);
				
				if (ch == 0)
					delete pItem;
				else
				{
					str += ch;
					pItem->SetMsgIndex(nMsgIndex);
					if (pUnderLineInfo)
						pItem->SetUnderLine(pUnderLineInfo->bUnderLine, pUnderLineInfo->bSameColor, pUnderLineInfo->clUnderLine);
					
					switch (pItem->GetType())
					{
					case enumEIIvtrlItem:
						
						pItem->SetName(szIvtrItem);
						pItem->SetColor(clIvtrItem);
						break;
						
					case enumEIScriptItem:
						
						if (pScriptItemInfo && nCurScriptIndex < pScriptItemInfo->nScriptItemCount)
						{
							const EditboxScriptItem& item = *(pScriptItemInfo->ppScriptItems[nCurScriptIndex]);
							pItem->SetName(item.GetName());
							pItem->SetColor(item.GetColor());
							
							if (item.GetData())
								pItem->SetExtraData(item.GetData(), item.GetDataSize());
							
							nCurScriptIndex++;
						}
						
						break;
					}
				}
			}
			else
				delete pItem;
			
		}
		else
			szText++;
	}

	if (szText > szStart)
		str += ACString(szStart, szText - szStart);

	return str;
}

ACString MarshalEmotionInfo(int nEmotionSet, int nIndex)
{
	ACString str;
	str.Format(L"%d:%d", nEmotionSet, nIndex);
	return str;
}

void UnmarshalEmotionInfo(const ACHAR* szText, int& nEmotionSet, int& nIndex)
{
	swscanf(szText, L"%d:%d", &nEmotionSet, &nIndex);
	a_ClampFloor(nEmotionSet, 0);
	a_ClampFloor(nIndex, 0);
	if( nEmotionSet >= AUIMANAGER_MAX_EMOTIONGROUPS )
		nEmotionSet = 0;
}

ACString FilterEmotionSet(const ACHAR* szText, int nEmotionSet)
{
	EditBoxItemsSet ItemsSet;
	ACString strOrgText = UnmarshalEditBoxText(szText, ItemsSet);
	int nCount = ItemsSet.GetItemCount();

	if (nCount == 0)
		return ACString(szText);

	EditBoxItemMap::iterator it = ItemsSet.GetItemIterator();

	for (int i = 0; i < nCount; i++)
	{
		EditBoxItemBase* pItem = it->second;

		if (pItem->GetType() == enumEIEmotion)
		{
			int nSet = 0;
			int nIndex = 0;

			UnmarshalEmotionInfo(pItem->GetInfo(), nSet, nIndex);
			pItem->SetInfo(MarshalEmotionInfo(nEmotionSet, nIndex));
		}

		++it;
	}

	return MarshalEditBoxText(strOrgText, ItemsSet);
}

ACString FilterCoordColor(const ACHAR* szText, A3DCOLOR clCoord)
{
	EditBoxItemsSet ItemsSet;
	ACString strOrgText = UnmarshalEditBoxText(szText, ItemsSet);
	int nCount = ItemsSet.GetItemCount();

	if (nCount == 0)
		return ACString(szText);

	EditBoxItemMap::iterator it = ItemsSet.GetItemIterator();

	for (int i = 0; i < nCount; i++)
	{
		EditBoxItemBase* pItem = it->second;

		if (pItem->GetType() == enumEICoord)
			pItem->SetColor(clCoord);

		++it;
	}

	return MarshalEditBoxText(strOrgText, ItemsSet);
}

ACString FilterIvtrItemName(const ACHAR* szText, const ACHAR* szName)
{
	EditBoxItemsSet ItemsSet;
	ACString strOrgText = UnmarshalEditBoxText(szText, ItemsSet);
	int nCount = ItemsSet.GetItemCount();

	if (nCount == 0)
		return ACString(szText);

	EditBoxItemMap::iterator it = ItemsSet.GetItemIterator();

	for (int i = 0; i < nCount; i++)
	{
		EditBoxItemBase* pItem = it->second;

		if (pItem->GetType() == enumEIIvtrlItem)
		{
			if( szName )
				pItem->SetName(szName);
		}

		++it;
	}

	return MarshalEditBoxText(strOrgText, ItemsSet);
}

void glb_OptimizeSpriteResource(A3DEngine* pA3DEngine, A2DSprite* pA2DSprite, bool bLoad)
{
	if(!pA2DSprite || !pA2DSprite->GetTextures())
	{
		return;
	}
	A2DSpriteTextures* pTex = pA2DSprite->GetTextures();

	if (bLoad)
	{
		pTex->IncUserRef();

		if (pTex->GetSpriteBuffer() == NULL)
		{
			assert(pTex->GetUserRef() == 1);
			pTex->Init(pA3DEngine->GetA3DDevice(), pA2DSprite->GetName(), 0, 0, AUI_COLORKEY);
		}
	}
	else
	{
		if (pTex->GetUserRef() > 0)
		{
			pTex->DecUserRef();

			if (pTex->GetUserRef() == 0)
			{
				if (pTex->GetSpriteBuffer())
					pTex->UnloadTextures();
				else
				{
					assert(false);
				}
			}
		}
		else
		{
			assert(false);
		}
	}
}


bool AUI_ConvertColor(const ACHAR* pszText, A3DCOLOR& color)
{
	if (a_strlen(pszText) >= 7 && pszText[0] == '^')
	{
		A3DCOLOR cl = 0;
		for(int i = 0; i < 6; i++ )
		{
			int nValue = pszText[i + 1];
			if( nValue >= '0' && nValue <= '9' )
				nValue -= '0';
			else if( nValue >= 'a' && nValue <= 'f' )
				nValue = nValue - 'a' + 0xA;
			else if( nValue >= 'A' && nValue <= 'F' )
				nValue = nValue - 'A' + 0xA;
			else
				return false;
			
			cl = cl * 0x10 + nValue;
		}
		color = cl;
	}
	else
		return false;

	return true;
}
