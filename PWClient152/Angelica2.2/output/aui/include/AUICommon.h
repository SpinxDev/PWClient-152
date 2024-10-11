// Filename	: AUIcommon.h
// Creator	: Tom Zhou
// Date		: May 24, 2004

#ifndef _AUICOMMON_H_
#define _AUICOMMON_H_

#include "AUIObject.h"
#include "A3DFTFontMan.h"
#include "vector.h"
#include "hashmap.h"

#define AUICOMMON_ITEM_CODE_START	0xE000
#define AUICOMMON_ITEM_CODE_END		0xE3FF
#define	MAX_EDITBOX_ITEM_NUM		(AUICOMMON_ITEM_CODE_END - AUICOMMON_ITEM_CODE_START + 1)
#define MAX_EI_COLORBOX_WIDTH		3000

class A3DEngine;
class A2DSprite;
class CScriptValue;

#define MAXNUM_CUSTOM_ITEM 255
enum EditboxItemType
{
	enumEIEmotion = 0,
	enumEIIvtrlItem,
	enumEICoord,
	enumEIImage,
	enumEIScriptItem,
	enumEIBox,
	enumEITask,
	enumEICustom,
	enumEINum = enumEICustom + MAXNUM_CUSTOM_ITEM // 自定义类型 custom->num，最多255种，custom类型需要通过继承EditBoxItemBase来实现，子类中必须添加静态对象
};

enum UnderLineMode
{
	UM_NO_UNDERLINE = 0,
	UM_UNDERLINE_SAME_COLOR,
	UM_UNDERLINE_SPECIFY_COLOR,
	UM_USE_AUIOBJECT_OPTION
};

enum AUIObjectPropertyValue
{
	AUIOBJECT_PROPERTY_VALUE_UNKNOWN = 0,
	AUIOBJECT_PROPERTY_VALUE_BOOL,
	AUIOBJECT_PROPERTY_VALUE_INT,
	AUIOBJECT_PROPERTY_VALUE_FLOAT,
	AUIOBJECT_PROPERTY_VALUE_CHAR,
	AUIOBJECT_PROPERTY_VALUE_ACHAR
};

struct FONT_WITH_SHADOW
{
	A3DFTFont* pFont;
	int	nShadow;
};

void ClearAUIObjectPropertyType();
bool SetAUIObjectPropertyType(const char* strPropName, AUIObjectPropertyValue type);
AUIObjectPropertyValue GetAUIObjectPropertyType(const char* strPropName);

inline bool IsEnglishCode(wchar_t ch)
{
	return ch >= 0x21 && ch <= 0x7E || ch >= 0x401 && ch <= 0x451
		|| ch >= 0xC0 && ch <=0xFF && ch != 0xD7 && ch != 0xF7;
}

inline bool IsVerticalRotationNeeded(wchar_t ch)
{
	return ch >= 0x20 && ch <= 0x7E || ch >= 0x401 && ch <= 0x451
		|| ch >= 0xC0 && ch <=0xFF && ch != 0xD7 && ch != 0xF7;
}

inline bool IsEditboxItemCode(wchar_t ch)
{
	return ch >= AUICOMMON_ITEM_CODE_START && ch <= AUICOMMON_ITEM_CODE_END;
}

inline wchar_t EditboxGetNextChar(wchar_t cur)
{
	if (cur >= AUICOMMON_ITEM_CODE_END)
		return AUICOMMON_ITEM_CODE_START;
	else
		return cur + 1;
}

inline wchar_t EditboxGetPrevChar(wchar_t cur)
{
	if (cur <= AUICOMMON_ITEM_CODE_START)
		return AUICOMMON_ITEM_CODE_END;
	else
		return cur - 1;
}

bool AUI_ConvertColor(const ACHAR* pszText, A3DCOLOR& color);

class EditboxScriptItem
{
public:

	EditboxScriptItem()
	{
		cl = 0;
		data = 0;
		sz = 0;
	}

	EditboxScriptItem(const EditboxScriptItem& src) : data(0)
	{
		*this = src;
	}

	~EditboxScriptItem()
	{
		delete[] data;
	}

	EditboxScriptItem& operator= (const EditboxScriptItem& src)
	{
		name = src.name;
		cl = src.cl;

		if (src.data)
		{
			data = new char[src.sz];
			memcpy(data, src.data, src.sz);
			sz = src.sz;
		}
		else
		{
			data = 0;
			sz = 0;
		}

		return *this;
	}

	const AWString& GetName() const { return name; }
	void SetName(const wchar_t* n) { name = n; }
	A3DCOLOR GetColor() const { return cl; }
	void SetColor(A3DCOLOR _cl) { cl = _cl; }
	const void* GetData() const { return data; }
	size_t GetDataSize() const { return sz; }

	void SetData(const void* _data, size_t _sz)
	{
		delete[] data;
		data = new char[_sz];
		memcpy(data, _data, _sz);
		sz = _sz;
	}

protected:

	AWString name;
	A3DCOLOR cl;
	void* data;
	size_t sz;
};

typedef abase::vector<EditboxScriptItem*> EditboxScriptItemVec;

inline void CopyEditboxScriptItemVec(EditboxScriptItemVec& dst, const EditboxScriptItem** ppScriptItems, int nScriptItemCount)
{
	for (int i = 0; i < nScriptItemCount; i++)
	{
		dst.push_back(new EditboxScriptItem(*ppScriptItems[i]));
	}
}

class EditBoxItemBase
{
public:

	EditBoxItemBase(EditboxItemType type) : 
		m_type(type), 
		m_dwColor(0xffffffff), 
		m_nMsgIndex(0), 
		m_nImageIndex(0), 
		m_nImageFrame(0), 
		m_fImageScale(1.0f),
		m_pExtraData(NULL), 
		m_uExtraDataSize(0), 
		m_bUnderLine(false), 
		m_bSameColor(true), 
		m_dwUnderLineColor(0), 
		m_nColorboxWidth(0), 
		m_fBoxHeightScale(1.0f)
	{
		RegisterCustomType(type);
	}
	EditBoxItemBase(const EditBoxItemBase& src) : m_pExtraData(NULL) { *this = src; }
	virtual ~EditBoxItemBase() { if (m_pExtraData) delete[] m_pExtraData; }

protected:

	EditboxItemType m_type;

	// common properties
	A3DCOLOR m_dwColor;
	ACString m_strName;
	ACString m_strInfo;
	void* m_pExtraData;
	size_t m_uExtraDataSize;

	int m_nMsgIndex;
	
	// image properties
	int m_nImageIndex;
	int	m_nImageFrame;
	float m_fImageScale;

	// text properties
	bool m_bUnderLine;
	bool m_bSameColor;
	A3DCOLOR m_dwUnderLineColor;

	// colorBox properties
	int m_nColorboxWidth;
	float m_fBoxHeightScale; //0 ~ 1.0

	static EditBoxItemBase* m_mapCustomType[MAXNUM_CUSTOM_ITEM];

	virtual bool UnserializeContent(const ACHAR*& szText);	// should be overwrite
	virtual int GetContentTagNum() const;					// should be overwrite
	virtual int GetRenderTextTagIndex() const;				// should be overwrite, if no text rendered, return -1.
	
	void RegisterCustomType(EditboxItemType type)
	{
		if (type >= enumEICustom && type < enumEINum && m_mapCustomType[type-enumEICustom] == NULL)
			m_mapCustomType[type-enumEICustom] = this;
	}

	virtual EditBoxItemBase* Create()  const // should be overwrite, only called in Unserialize
	{
		return new EditBoxItemBase(m_type);
	}

	static const EditBoxItemBase* GetCustomItemFromType(EditboxItemType type) // only called in Unserialize
	{
		if (type>=enumEICustom && type<enumEINum)
			return m_mapCustomType[type-enumEICustom];
		return NULL;
	}

private:
	// Unserialize
	bool _UnserializeContentCoord(const ACHAR*& szText);
	bool _UnserializeContentImage(const ACHAR*& szText);
	bool _UnserializeContentEmotion(const ACHAR*& szText);
	bool _UnserializeContentBox(const ACHAR*& szText);
	bool _UnserializeContentTask(const ACHAR*& szText);
	bool _UnserialzieContentCommon(const ACHAR*& szText);

	
	static int  _UnserializeItemType(const ACHAR*& szText);
	static int  _GetItemRenderdLength(const ACHAR*& szText);
	static bool _ParseItems(const ACHAR*& szText, int nTag, abase::vector<abase::pair<const ACHAR*, const ACHAR*> >* Tags);
	static const int  s_CoordTagNum;
	static const int  s_ImageTagNum;
	static const int  s_EmotionTagNum;
	static const int  s_BoxTagNum;
	static const int  s_TaskTagNum;
	static const int  s_CommonTagNum;

public:

	A3DCOLOR GetColor() const { return m_dwColor; }
	EditboxItemType GetType() const { return m_type; }
	const ACHAR* GetName() const { return m_strName; }
	const ACHAR* GetInfo() const { return m_strInfo; }
	int GetMsgIndex() const { return m_nMsgIndex; }
	int GetImageIndex() const { return m_nImageIndex; }
	int GetImageFrame() const { return m_nImageFrame; }
	float GetImageScale() const { return m_fImageScale; }
	bool GetUnderLine() const { return m_bUnderLine; }
	bool GetSameColor() const { return m_bSameColor; }
	A3DCOLOR GetUnderLineColor() const { return m_dwUnderLineColor; }
	int GetBoxWidth() const { return m_nColorboxWidth; }
	float GetBoxHeightScale() const { return m_fBoxHeightScale; }

	void SetColor(A3DCOLOR cl) { m_dwColor = cl; }
	void SetName(const ACHAR* szName) { m_strName = szName; }
	void SetInfo(const ACHAR* szInfo) { m_strInfo = szInfo; }
	void SetMsgIndex(int n) { m_nMsgIndex = n; }
	void SetImageIndex(int n) { m_nImageIndex = n; }
	void SetImageFrame(int n) { m_nImageFrame = n; }
	void SetImageScale(float f) { m_fImageScale = f; }
	const void* GetExtraData() const { return m_pExtraData; }
	size_t GetExtraDataSize() const { return m_uExtraDataSize; }
	void SetUnderLine(bool bUnderLine, bool bSameColor = true, A3DCOLOR clUnderLine = 0) { m_bUnderLine = bUnderLine; m_bSameColor = bSameColor; m_dwUnderLineColor = clUnderLine; }
	void SetBoxWidth(int nBoxWidth) { m_nColorboxWidth = nBoxWidth; a_Clamp(m_nColorboxWidth, 0, MAX_EI_COLORBOX_WIDTH);}
	void SetBoxHeightScale(float fBoxHeightScale) { m_fBoxHeightScale = fBoxHeightScale; a_Clamp(m_fBoxHeightScale, 0.0f, 1.0f); }

	void SetExtraData(const void* pData, size_t sz)
	{
		delete[] m_pExtraData;
		m_pExtraData = new char[sz];
		memcpy(m_pExtraData, pData, sz);
		m_uExtraDataSize = sz;
	}

	virtual ACString Serialize() const; // should be overwrite
	static EditBoxItemBase* Unserialize(const ACHAR*& szText);
	static int GetUnserializedLength(const ACHAR* szText); // return unserialized string length
	virtual int GetRenderedLength() const;

	EditBoxItemBase& operator= (const EditBoxItemBase& src)
	{
		m_type			= src.m_type;
		m_dwColor		= src.m_dwColor;
		m_strName		= src.m_strName;
		m_strInfo		= src.m_strInfo;
		m_nMsgIndex		= src.m_nMsgIndex;
		m_nImageIndex	= src.m_nImageIndex;
		m_nImageFrame	= src.m_nImageFrame;
		m_fImageScale	= src.m_fImageScale;

		if (src.m_pExtraData)
			SetExtraData(src.m_pExtraData, src.m_uExtraDataSize);
		else
		{
			m_pExtraData = NULL;
			m_uExtraDataSize = 0;
		}

		RegisterCustomType(m_type);

		return *this;
	}
};

typedef abase::hash_map<wchar_t, EditBoxItemBase*> EditBoxItemMap;

class EditBoxItemsSet
{
public:

	EditBoxItemsSet()
	{
		memset(m_ItemsCount, 0, sizeof(m_ItemsCount));
		m_cNextItemChar = AUICOMMON_ITEM_CODE_START;
	}

	EditBoxItemsSet(const EditBoxItemsSet& itemsset)
	{
		*this = itemsset;
	}

	~EditBoxItemsSet()
	{
		Release();
	}

protected:

	EditBoxItemMap m_Items;
	int m_ItemsCount[enumEINum];
	wchar_t m_cNextItemChar;

public:

	EditBoxItemsSet& operator= (const EditBoxItemsSet& src)
	{
		for (EditBoxItemMap::iterator it = m_Items.begin(); it != m_Items.end(); ++it)
			delete it->second;

		m_Items.clear();

		for (EditBoxItemMap::const_iterator it2 = src.m_Items.begin(); it2 != src.m_Items.end(); ++it2)
			m_Items[it2->first] = new EditBoxItemBase(*it2->second);

		memcpy(m_ItemsCount, src.m_ItemsCount, sizeof(m_ItemsCount));
		m_cNextItemChar = src.m_cNextItemChar;
		return *this;
	}

	void Release()
	{
		for (EditBoxItemMap::iterator it = m_Items.begin(); it != m_Items.end(); ++it)
			delete it->second;

		m_Items.clear();
		memset(m_ItemsCount, 0, sizeof(m_ItemsCount));
		m_cNextItemChar = AUICOMMON_ITEM_CODE_START;
	}

	int GetItemCount() const { return m_Items.size(); }
	EditBoxItemMap::iterator GetItemIterator() { return m_Items.begin(); }

	EditBoxItemBase* GetItemByChar(wchar_t ch) const
	{
		assert(IsEditboxItemCode(ch));
		EditBoxItemMap::const_iterator it = m_Items.find(ch);
		return it == m_Items.end() ? NULL : it->second;
	}

	int GetItemCountByType(EditboxItemType type) const
	{
		return m_ItemsCount[type];
	}

	void DelItemByChar(wchar_t ch)
	{
		EditBoxItemMap::iterator it = m_Items.find(ch);

		if (it != m_Items.end())
		{
			EditBoxItemBase* pItem = it->second;
			m_ItemsCount[pItem->GetType()]--;
			delete pItem;
			m_Items.erase(it);
		}
	}

	wchar_t AppendItem(EditBoxItemBase* pItem)
	{
		if (m_Items.size() >= MAX_EDITBOX_ITEM_NUM)
			return 0;

		wchar_t cur = m_cNextItemChar;

		do
		{
			EditBoxItemMap::iterator it = m_Items.find(m_cNextItemChar);
			
			if (it != m_Items.end())
				m_cNextItemChar = EditboxGetNextChar(m_cNextItemChar);
			else
			{
				m_Items[m_cNextItemChar] = pItem;
				m_ItemsCount[pItem->GetType()]++;
				wchar_t ret = m_cNextItemChar;
				m_cNextItemChar = EditboxGetNextChar(m_cNextItemChar);
				return ret;
			}
		} while(m_cNextItemChar != cur);

		return 0;
	}

	wchar_t AppendItem(EditboxItemType type, A3DCOLOR cl, const ACHAR* szName, const ACHAR* szInfo);

	size_t GetTotalExtraDataSize() const
	{
		size_t sz = 0;

		for (EditBoxItemMap::const_iterator it = m_Items.begin(); it != m_Items.end(); ++it)
			sz += it->second->GetExtraDataSize();

		return sz;
	}

	void GetScriptItemVec(EditboxScriptItemVec& vec) const
	{
		for (EditBoxItemMap::const_iterator it = m_Items.begin(); it != m_Items.end(); ++it)
		{
			EditBoxItemBase* pItem = it->second;

			if (pItem->GetType() == enumEIScriptItem)
			{
				EditboxScriptItem* pScript = new EditboxScriptItem();
				pScript->SetName(pItem->GetName());
				pScript->SetColor(pItem->GetColor());

				if (pItem->GetExtraData())
					pScript->SetData(pItem->GetExtraData(), pItem->GetExtraDataSize());

				vec.push_back(pScript);
			}
		}
	}
};


ACString MarshalEditBoxText(const ACHAR* szText, const EditBoxItemsSet& ItemsSet);
struct AUI_UNMARSH_UNDERLINE_INFO
{
	bool bUnderLine;
	bool bSameColor;
	A3DCOLOR clUnderLine;
};
struct AUI_UNMARSH_SCRIPTITEM_INFO
{
	const EditboxScriptItem** ppScriptItems;
	int nScriptItemCount;
};
ACString UnmarshalEditBoxText(const ACHAR* szText, EditBoxItemsSet& ItemsSet, int nMsgIndex = 0, const ACHAR* szIvtrItem = L"", A3DCOLOR clIvtrItem = 0xffffffff, int nItemMask = 0xffffffff, 
							  const EditboxScriptItem** ppScriptItems = NULL, int nScriptItemCount = 0,
							  bool bUnderLine = false, bool bSameColor = true, A3DCOLOR clUnderLine = 0xff000000);
ACString UnmarshalEditBoxTextEx(const ACHAR* szText, EditBoxItemsSet& ItemsSet, int nMsgIndex = 0, const ACHAR* szIvtrItem = L"", A3DCOLOR clIvtrItem = 0xffffffff, int nItemMask = 0xffffffff, 
							    AUI_UNMARSH_SCRIPTITEM_INFO* pScriptItemInfo = NULL, 
								AUI_UNMARSH_UNDERLINE_INFO* pUnderLineInfo = NULL);

ACString MarshalEmotionInfo(int nEmotionSet, int nIndex);
void UnmarshalEmotionInfo(const ACHAR* szText, int& nEmotionSet, int& nIndex);
ACString FilterEmotionSet(const ACHAR* szText, int nEmotionSet);
ACString FilterCoordColor(const ACHAR* szText, A3DCOLOR clCoord);
ACString FilterIvtrItemName(const ACHAR* szText, const ACHAR* szName);

struct _AUITEXTAREA_NAME_LINK;
typedef struct _AUITEXTAREA_NAME_LINK AUITEXTAREA_NAME_LINK;
struct _AUITEXTAREA_EMOTION;
typedef struct _AUITEXTAREA_EMOTION AUITEXTAREA_EMOTION;
struct _AUITEXTAREA_CHANNEL_LINK;
typedef struct _AUITEXTAREA_CHANNEL_LINK AUITEXTAREA_CHANNEL_LINK;
class A2DSprite;
class EditBoxItemsSet;
struct AUITEXTAREA_EDITBOX_ITEM;

struct TEXTITEM_RCINFO
{
	abase::vector<AUITEXTAREA_NAME_LINK>* pVecNameLinks;
	abase::vector<AUITEXTAREA_EDITBOX_ITEM>* pVecEBItems;
	abase::vector<AUITEXTAREA_CHANNEL_LINK>* pVecChannelLinks;
};

void AUI_RetrieveEnumString(int iEnum_Type, abase::vector<AString>& vecString);
bool AUI_EnumControlTypes(int &nNumControls, int *a_idControl, int nSizeOfArray);
const char * AUI_RetrieveControlName(int idControl);
int AUI_RetrieveControlPropertyList(int idControl, PAUIOBJECT_PROPERTY a_Property);
bool AUI_GetTextRect(A3DFTFont *pFont, const ACHAR *pszText, int& nWidth,
	int& nHeight, int& nLines, int nSpace = 0, EditBoxItemsSet* pItemsSet = NULL, int nEmotionW = 0, int nEmotionH = 0, abase::vector<A2DSprite*> *pvecImage = NULL, 
	float fWindowScale = 1.0f,
	abase::vector<FONT_WITH_SHADOW>* pvecOtherFonts = NULL,
	bool bAutoIndent = false,
	int nWordWidth = 0);
bool AUI_TextOutFormat(A3DFTFont *pFont, int x, int y, const ACHAR *pszText,
	A3DRECT *pRect = NULL, int nStartLine = 0, int *pnLines = NULL,
	int nSpace = 0, bool bAutoIndent = false,
	int *pnHeight = NULL,
	A2DSprite *pA2DSpriteEmotion = NULL,
	abase::vector<AUITEXTAREA_EMOTION> *pvecEmotion = NULL,
	int *pnLinesThisPage = NULL, bool bDrawToScreen = true,	int nShadow = 0, int nAlpha = 255,
	bool bSmallEmotion = false,
	EditBoxItemsSet* pItemsSet = NULL,
	abase::vector<A2DSprite*> *pvecImage = NULL,
	float fWindowScale = 1.0f,
	bool bAutoReturn = true,
	abase::vector<FONT_WITH_SHADOW>* pvecOtherFonts = NULL,
	int nWordWidth = 0,
	TEXTITEM_RCINFO* pItemRcInfo = NULL,
	bool bVertical = false);

struct TAG_OP_IN_PARAMS;
struct TAG_OP_OUT_PARAMS;
bool AUI_TextOutFormat(TAG_OP_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams);

extern void InitTagParams(TAG_OP_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams,
	A3DFTFont *pFont, int x, int y, const ACHAR *pszText,
	A3DRECT *pRect = NULL, int nStartLine = 0, int *pnLines = NULL,
	int nSpace = 0, bool bAutoIndent = false,
	int *pnHeight = NULL,
	A2DSprite *pA2DSpriteEmotion = NULL,
	abase::vector<AUITEXTAREA_EMOTION> *pvecEmotion = NULL,
	int *pnLinesThisPage = NULL, bool bDrawToScreen = true,	int nShadow = 0, int nAlpha = 255,
	bool bSmallEmotion = false,
	EditBoxItemsSet* pItemsSet = NULL,
	abase::vector<A2DSprite*> *pvecImage = NULL,
	float fWindowScale = 1.0f,
	bool bAutoReturn = true,
	abase::vector<FONT_WITH_SHADOW>* pvecOtherFonts = NULL,
	int nWordWidth = 0,
	TEXTITEM_RCINFO* pItemRcInfo = NULL,
	bool bVertical = false);
void AUI_ConvertChatString(const ACHAR *pszChat, ACHAR *pszConv, bool bName = true);
void AUI_ConvertChatString_S(const ACHAR *pszChat, int iChatLen, ACHAR *pszConv, int iConvLen, bool bName = true);
void AUI_ConvertChatString(const ACString& strChat, ACString& strConv, bool bName = true);
DWORD AUI_A2DSpriteGetPixel(A2DSprite* pA2DSprite, int x, int y);





#define SAFE_DELETE(p) {delete (p); (p) = NULL;}
#define SAFE_DELETE_ARRAY(p) {delete [] (p); (p) = NULL;}

#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; } }

#define CHECK_BREAK(p) {if (!(p)) break;}

#define BEGIN_FAKE_WHILE	\
	bool bSucceed = false;  \
do {

#define END_FAKE_WHILE		\
	bSucceed = true;	\
} while (false);

#define RETURN_FAKE_WHILE_RESULT \
return bSucceed;

#define BEGIN_ON_FAIL_FAKE_WHILE	\
if (!bSucceed) { 

#define END_ON_FAIL_FAKE_WHILE	\
}


#define CHECK_BREAK2(p) {if (!(p)) { nLine = __LINE__; break; } }

#define BEGIN_FAKE_WHILE2	\
	bool bSucceed = false;  \
	int nLine = -1; \
do {

#define END_FAKE_WHILE2		\
	bSucceed = true;	\
} while (false);

#define RETURN_FAKE_WHILE_RESULT2 \
return bSucceed;

#define BEGIN_ON_FAIL_FAKE_WHILE2	\
if (!bSucceed) { 

#define END_ON_FAIL_FAKE_WHILE2	\
}

#define ERR_LINE2 (nLine)


#define XMLSET_INT_VALUE(item, key, value, defaultvalue) \
	if( value != defaultvalue )	\
	{\
		ACString strText;\
		strText.Format(_AL("%d"), value);\
		item->SetValue(key, strText);\
	}
#define XMLSET_BOOL_VALUE(item, key, value, defaultvalue) \
	if( value != defaultvalue )	\
	{\
		if( value )\
			item->SetValue(key, _AL("true"));\
		else\
			item->SetValue(key, _AL("false"));\
	}
#define XMLSET_FLOAT_VALUE(item, key, value, defaultvalue) \
	if( value != defaultvalue )	\
	{\
		ACString strText;\
		strText.Format(_AL("%g"), value);\
		item->SetValue(key, strText);\
	}
#define XMLSET_STRING_VALUE(item, key, value, defaultvalue) \
	if( a_stricmp(value, defaultvalue) != 0 )	\
	{\
		item->SetValue(key, value);\
	}

#define XMLSET_COLOR_VALUE(item, key, value, defaultvalue) \
{\
	ACString strColor;\
	strColor.Format(_AL("%d,%d,%d,%d"), A3DCOLOR_GETRED(value), A3DCOLOR_GETGREEN(value),\
		A3DCOLOR_GETBLUE(value), A3DCOLOR_GETALPHA(value));\
	XMLSET_STRING_VALUE(item, key, strColor, defaultvalue);\
}


#define XMLGET_INT_VALUE(item, key, variable)\
	{\
		const ACHAR* pszText = item->GetValue(key);\
		if( pszText )\
			variable = a_atoi(pszText);\
	}

#define XMLGET_BOOL_VALUE(item, key, variable)\
	{\
		const ACHAR* pszText = item->GetValue(key);\
		if( pszText )\
		{\
			if( a_stricmp(pszText, _AL("true")) == 0 )\
				variable = true;\
			else if( a_stricmp(pszText, _AL("false")) == 0 )\
				variable = false;\
		}\
	}
#define XMLGET_FLOAT_VALUE(item, key, variable)\
	{\
		const ACHAR* pszText = item->GetValue(key);\
		if( pszText )\
			variable = a_atof(pszText);\
	}
#define XMLGET_STRING_VALUE(item, key, variable)\
	{\
		const ACHAR* pszText = item->GetValue(key);\
		if( pszText )\
			variable = AC2AS(pszText);\
	}
#define XMLGET_WSTRING_VALUE(item, key, variable)\
	{\
		const ACHAR* pszText = item->GetValue(key);\
		if( pszText )\
			variable = pszText;\
	}

#define XMLGET_COLOR_VALUE(item, key, variable)\
	{\
			ACString strColor = _AL("");\
			XMLGET_WSTRING_VALUE(item, key, strColor)\
			if( strColor != _AL("") )\
			{\
				CSplit s(strColor);\
				CSPLIT_STRING_VECTOR vec = s.Split(_AL(","));\
				if( vec.size() >= 3 )\
				{\
					int r = a_atoi(vec[0]);\
					int g = a_atoi(vec[1]);\
					int b = a_atoi(vec[2]);\
					int a = 255;\
					if( vec.size() >= 4 )\
						a = a_atoi(vec[3]);\
					variable = A3DCOLORRGBA(r, g, b, a);\
				}\
			}\
	}


void AUI_EnableLog(const bool bEnable);
bool AUI_IsLogEnabled();
void AUI_ReportErrorSetNoAssert(bool bNoAssert);
void AUI_EnableParticularLog(const bool bEnable);

bool AUI_ReportError(int nLine, const char *pszFile);
bool AUI_ReportError(int nLine, int nLevel, const char* szMsg, ...);
#define DEFAULT_2_PARAM ERR_LINE2, 1
bool AUI_ReportError(const char* szMsg, ...);
bool AUI_ReportError(int nLine, const char *pszFile, const char* szMsg, abase::vector<CScriptValue>& args);
#define AUI_REPORT_ERR_LUA_FUNC(ERR_MSG) AUI_ReportError(__LINE__,__FILE__,ERR_MSG,args);

void glb_OptimizeSpriteResource(A3DEngine* pA3DEngine, A2DSprite* pA2DSprite, bool bLoad);

#endif //_AUICOMMON_H_