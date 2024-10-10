// AUITag.h: interface for the AUITag class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AUITAG_H__F5D79DDA_CEE1_4AFA_9BD0_DC83840670E1__INCLUDED_)
#define AFX_AUITAG_H__F5D79DDA_CEE1_4AFA_9BD0_DC83840670E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "AUIObject.h"
#include "A3DFTFontMan.h"
#include "AUICommon.h"
#include "A2DSprite.h"
#include "A2DSpriteItem.h"
#include "A2DSpriteTexMan.h"

class A3DFlatCollector;
class A3DWireCollector;
struct TAG_OP_IN_PARAMS
{
	//////////////////////////////////////////////////////////////////////////
	// in params
	int			_x;
	int			_y;
	const ACHAR* _pszText;
	A3DRECT*	_pRect;
	int			_nStartLine;
	int			_nWordWidth;
	int			_nShadow;
	int			_nAlpha;
	int			_nSpace;
	float		_fWindowScale;
	A3DCOLOR	_clDefault;

	// flags
	bool		_bAutoIndent;
	bool		_bAutoReturn;
	bool		_bSmallEmotion;
	bool		_bSmallImage;
	bool		_bDrawToScreen;
	bool		_bMultiEmotionSet;
	bool		_bVertical;

	// resources
	A3DFTFont*	_pFont;
	abase::vector<FONT_WITH_SHADOW>* _pVecOtherFonts;
	union
	{
		struct
		{
			A2DSprite*	_pA2DSpriteEmotion;
			abase::vector<AUITEXTAREA_EMOTION>* _pVecEmotion;
		} stSingleEmotion;
		struct
		{
			A2DSprite**	_ppA2DSpriteEmotion;
			abase::vector<AUITEXTAREA_EMOTION>** _ppVecEmotion;
		} stMultiEmotion;
	} _unEmotionInfo;
	abase::vector<A2DSprite*>* _pVecImage;
	EditBoxItemsSet* _pItemsSet;

	TAG_OP_IN_PARAMS(A3DFTFont *pFont = NULL, 
		int x = 0, 
		int y = 0, 
		const ACHAR *pszText = NULL,
		A3DRECT *pRect = NULL, 
		int nStartLine = 0, 
		int nSpace = 0, 
		bool bAutoIndent = false,

		A2DSprite *pA2DSpriteEmotion = NULL,
		abase::vector<AUITEXTAREA_EMOTION> *pvecEmotion = NULL,

		bool bDrawToScreen = true,	
		int nShadow = 0, 
		int nAlpha = 255,
		bool bSmallEmotion = false,
		EditBoxItemsSet* pItemsSet = NULL,
		abase::vector<A2DSprite*> *pvecImage = NULL,
		float fWindowScale = 1.0f,
		bool bAutoReturn = true,
		abase::vector<FONT_WITH_SHADOW>* pvecOtherFonts = NULL,
		int nWordWidth = 0,
		A3DCOLOR clDefalut = A3DCOLORRGB(255, 255, 255),
		bool bSmallImage = false,
		bool bVertical = false)
		:	_pFont(pFont),
		_x(x),
		_y(y),
		_pszText(pszText),
		_pRect(pRect),
		_nStartLine(nStartLine),
		_nSpace(nSpace),
		_bAutoIndent(bAutoIndent),
		_bDrawToScreen(bDrawToScreen),
		_nShadow(nShadow),
		_nAlpha(nAlpha),
		_bSmallEmotion(bSmallEmotion),
		_pItemsSet(pItemsSet),
		_pVecImage(pvecImage),
		_fWindowScale(fWindowScale),
		_bAutoReturn(bAutoReturn),
		_pVecOtherFonts(pvecOtherFonts),
		_nWordWidth(nWordWidth),
		_clDefault(clDefalut),
		_bMultiEmotionSet(false),
		_bSmallImage(bSmallImage),
		_bVertical(bVertical)
	{		
		_unEmotionInfo.stSingleEmotion._pA2DSpriteEmotion = pA2DSpriteEmotion;
		_unEmotionInfo.stSingleEmotion._pVecEmotion = pvecEmotion;
	}

	inline void SetMultiEmotionSet(A2DSprite**	_ppA2DSpriteEmotion, abase::vector<AUITEXTAREA_EMOTION>** _ppVecEmotion)
	{
		_unEmotionInfo.stMultiEmotion._ppA2DSpriteEmotion = _ppA2DSpriteEmotion;
		_unEmotionInfo.stMultiEmotion._ppVecEmotion = _ppVecEmotion;
		_bMultiEmotionSet = true;
	}
};

class AUITextTagCacheManager;
struct TAG_OP_OUT_PARAMS
{
	//////////////////////////////////////////////////////////////////////////
	// out params
	int* _pnLines;
	int* _pnWidth;
	int* _pnHeight;
	int* _pnLinesThisPage;
	TEXTITEM_RCINFO* _pItemRcInfo;
	AUITextTagCacheManager* _pCacheMan;

	TAG_OP_OUT_PARAMS(int *pnLines = NULL,
		int *pnHeight = NULL,
		int *pnLinesThisPage = NULL,
		TEXTITEM_RCINFO* pRcInfo = NULL,
		AUITextTagCacheManager* pCache = NULL,
		int *pnWidth = NULL)
		:	_pnLines(pnLines),
		_pnHeight(pnHeight),
		_pnLinesThisPage(pnLinesThisPage),
		_pItemRcInfo(pRcInfo),
		_pCacheMan(pCache),
		_pnWidth(pnWidth)
	{}
};

typedef struct _TAG_RESOLVER_CONTEXT
{
	A3DFTFont*	pCurFont;
	int			index;
	int			nCurX;
	int			nCurY;
	bool		bExceedBottom;
	bool		bRotateLayout; // 用于垂直显示。如果是英文，则旋转显示

	// quote context
	bool		bInQuote;
	int			iQuoteBeginPos;
	A3DCOLOR	color_quote;

	// channel context
	bool		bInChannelQuote;
	int			iChannelQuoteBeginPos;
	A3DCOLOR	color_clQuote;

	int			nCurShadow;

	bool		bFinish;

	A3DCOLOR	color;

	A3DCOLOR	color_underline;
	bool		bUnderLine;
	bool		bUnderLineSameColor;

	int			nLines;
	int			nLineHeightThis;
	int			nLinesThisPage;
	int			nMaxWidth;

	// cache calculation
	int			nSection;
	int			nOffsetYFromFirstLine;

	AUITEXTAREA_NAME_LINK nl;
	AUITEXTAREA_CHANNEL_LINK cl;

	//////////////////////////////////////////////////////////////////////////
	// calculate params (from default font, shouldn't be changed in render function)
	int			_nCharW;
	int			_nCharH;
	bool		_bCalcTextInfo;
	bool		_bCalcRcInfo;
	int			_nTabW;
	int			_nWidthSpace;
	A3DCOLOR	_clDefault;
	int			_len;

	_TAG_RESOLVER_CONTEXT()
	{
		clear();
	}	
	void clear()
	{
		index= 0;
		nCurX= 0;
		nCurY= 0;
		bExceedBottom= false;
		bRotateLayout = false;
		color= 0xffffff;
		color_underline= 0xffffff;
		color_quote= A3DCOLORRGB(255, 220, 138);
		nLines= 0;
		nLineHeightThis= 0;
		nLinesThisPage= 0;
		pCurFont= NULL;
		bFinish= false;
		bUnderLine= false;
		bUnderLineSameColor= false;
		bInQuote= false;
		_nCharW= 0; 
		_nCharH= 0;
		_bCalcTextInfo= false; 
		_nTabW= 0;
		_nWidthSpace= 0;
		_clDefault= A3DCOLORRGB(255,255,255);
		_len= 0;
		_bCalcRcInfo= false;
		nSection= 0;
		nOffsetYFromFirstLine= 0;
		nMaxWidth= 0;
		iQuoteBeginPos= 0;
		nl.rc.Clear();
		nCurShadow = 0;

		bInChannelQuote = 0;
		iChannelQuoteBeginPos = 0;
		color_clQuote = A3DCOLORRGB(255, 220, 138);
		cl.rc.Clear();
	}
}TAG_RESOLVER_CONTEXT;

struct TAG_PARAMS
{
	TAG_OP_IN_PARAMS inParams;
	TAG_OP_OUT_PARAMS outParams;
	TAG_RESOLVER_CONTEXT context;
};

struct AUITAG_CACHE_IN_PARAMS
{
	bool	_bRender;
	int		_nStartLine;
	int		_x;
	int		_y;
	int		_nAlpha;
	A3DRECT* _pClip;
	AUITAG_CACHE_IN_PARAMS(bool bRender, int nStartLine, int x, int y, int nAlpha, A3DRECT* pClip) :
	_bRender(bRender), _nStartLine(nStartLine), _x(x), _y(y), _pClip(pClip), _nAlpha(nAlpha) {}
};

#define DECLARE_AUITEXT_TAG(class_name, base_class) \
	virtual AUITAG_NODE* CreateNode(AUIRichTextPrinter* pPrinter);  \
	virtual const char* GetClassName() const { return s_sz##class_name;}; \
	static const char* s_sz##class_name; 

#define IMPLEMENT_AUITEXT_TAG(class_name,base_classname) \
	AUITAG_NODE* class_name::CreateNode(AUIRichTextPrinter* pPrinter) \
	{ \
		if (!pPrinter) \
			return NULL; \
		AUITAG_NODE* pNodeBase = pPrinter->GetNode(#base_classname); \
		ASSERT(pNodeBase && #base_classname" not registered"); \
		if (!pNodeBase) \
			return NULL; \
		AUITAG_NODE* pNode = pPrinter->GetNode(#class_name); \
		if (pNode) \
		{ \
			if (pNode->_pBase) pNode->_pBase->AppendChild(pNode); \
			return pNode; \
		} \
		m_pNode = new AUITAG_NODE(new class_name, pNodeBase, #class_name); \
		return m_pNode; \
	}\
	const char* class_name::s_sz##class_name = #class_name;

bool AUI_ConvertColor(const ACHAR* pszText, A3DCOLOR& color);


inline bool IsNewLineNeed( const TAG_OP_IN_PARAMS &inParams, const TAG_RESOLVER_CONTEXT &context, int nCurTagWidth)
{
	if(inParams._bVertical)
		return ( inParams._bAutoReturn && inParams._pRect && context.nCurY + nCurTagWidth > inParams._pRect->bottom );
	else
		return ( inParams._bAutoReturn && inParams._pRect && context.nCurX + nCurTagWidth > inParams._pRect->right );
}


inline bool CanDraw( const TAG_OP_IN_PARAMS &inParams, const TAG_RESOLVER_CONTEXT &context )
{
	return inParams._bDrawToScreen && !context.bExceedBottom && context.nLines >= inParams._nStartLine;
}


class AUITAG_NODE;
class AUITextTag
{
	friend class AUITAG_NODE;
	friend class AUIRichTextPrinter;
public:
	AUITextTag() : m_pNode(NULL) {};
	virtual ~AUITextTag() {}	

	// override 
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const {return false;};

	// steps: 
	//		1. Calculate your rect and check (according context._curX, context._pRect...)whether a new line is needed.
	//		2. Do render on the position updated from step1. (some tags may only change render context)
	//		3. Update context.nCurX,Y, context.index ...  to prepare for next tag's renderer.
	// demo:
	//		SIZE szThisTag = Calculate ...;
	//		// if szThisTag={0,0}, skip the following two lines of codes.
	//		if (AUITextTag::IsNewLineNeed(...))  
	//			{ if (!AUITextTag::UpdateNewLine(...)) return false; }
	//		Do render with(context._nCurX, context.nCurY, context.pCurFont...)
	//		Update render context(context.index,context.nCurXY+szThisTag,context.nThisLineHeight...)
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const {return true;};
	
	virtual AUITAG_NODE* CreateNode(AUIRichTextPrinter* pPrinter);
	virtual const char* GetClassName() const {return s_szAUITextTag;}

protected:
	bool OutputSubTags(const TAG_OP_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams, TAG_RESOLVER_CONTEXT& context) const;
	
	// if ([current tag is out of the render rect] && [inParams shows no needs to calculate the size or lines of the whole text])
	// return false. so if the function return false, there is no needs to render the following text.
	static bool UpdateNewline( const TAG_OP_IN_PARAMS &inParams, TAG_RESOLVER_CONTEXT &context, bool bManualReturn = false);

	static const char* s_szAUITextTag;	
	AUITAG_NODE* m_pNode;
};

class AUITextTagCache
{
	friend class AUITextTagCacheManager;
	friend class AUITagNPCLink;
	friend class AUITagPositionLink;
public:
	AUITextTagCache(int nIndexInStr, const A3DPOINT2& ptPos, const A3DPOINT2& ptSize) 
		: m_IndexInStr(nIndexInStr), m_ptPos(ptPos), m_ptSize(ptSize), m_cCode(AUICOMMON_ITEM_CODE_START-1)
	{}
	virtual ~AUITextTagCache() {};
	virtual bool Output(const ACHAR* pText, const POINT& ptOffset, const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const = 0;
	void Offset(int Y) {m_ptPos.y += Y;}
	inline A3DPOINT2 GetSize() const {return m_ptSize;}
	inline A3DPOINT2 GetPos() const {return m_ptPos;}
	virtual void ClearItem(EditBoxItemsSet& itemSet) 
	{ 
		itemSet.DelItemByChar(m_cCode); 
	}
	void SetItemCode(ACHAR cItem)
	{
		m_cCode = cItem;
	}
protected:
	A3DPOINT2	m_ptPos;
	A3DPOINT2	m_ptSize;
	int			m_IndexInStr;
	ACHAR		m_cCode;
};

// TagParent type is ignored since 2011-11-24
template <class Tag, class TagParent = Tag>
class AUITagRegister
{
public:
	AUITagRegister()
	{
		AUIRichTextPrinter::GetSingleton().RegisterTagtype(Tag());
	}
};

// unregister of built-in types will be ignored
template <class Tag>
class AUITagUnregister
{
public:
	AUITagUnregister()
	{
		AUIRichTextPrinter::GetSingleton().UnRegisterTagType(Tag());
	}
};

class AUITAG_NODE
{
public:
	AUITAG_NODE*	_pBase;
	AUITAG_NODE*	_pNext;
	AUITAG_NODE*	_pPrev;
	AUITAG_NODE*	_pFirstChild;
	AUITAG_NODE*	_pLastChild;
	AUITextTag*		_pTag;
	const char*		_szClassName;

	AUITAG_NODE(AUITextTag* ptg, AUITAG_NODE* pNodeBase, const char* szClassName);
	~AUITAG_NODE();

	bool AppendChild(AUITAG_NODE* pNode);	
};

class AUIRichTextPrinter
{
	friend class AUITAG_NODE;
	friend class AUITagNPCLink;
	friend class AUITagPositionLink;
public:
	static AUIRichTextPrinter& GetSingleton() {return ms_TextPrinter;}
	
	AUITAG_NODE* GetNode(const char* szClassName) const;
	void RegisterTagtype(AUITextTag& tg);
	bool UnRegisterTagType(AUITextTag& tg);
	
	// Helper functions for debug
	void GetClassHierarchyInRuntime(char* szBuffer, int iLenBuffer) const;
	
	~AUIRichTextPrinter();
	bool Output(TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
	bool CalcSize(TAG_OP_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const;
	static bool InitContext(TAG_OP_IN_PARAMS& inParams, const TAG_OP_OUT_PARAMS& outParams, TAG_RESOLVER_CONTEXT& context);

protected:
	AUIRichTextPrinter();

	void RegisterTagtypeBuiltIn(AUITextTag& tg);
	const AUITextTag* GetTag(const ACHAR* pszText, int index, int len) const;
	const AUITextTag* RenderPlainText(TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, const TAG_OP_OUT_PARAMS& outParams) const;

protected:
	static AUIRichTextPrinter ms_TextPrinter;
	AUITAG_NODE* m_pNodeRoot;
	abase::hash_map<const char*, AUITAG_NODE*> m_mpNodesBuildIn;
	abase::hash_map<const char*, AUITAG_NODE*> m_mpNodesCustom;
};

class AUITextTagReturn : public AUITextTag
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagReturn, AUITextTag)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

class AUITextTagCtrl : public AUITextTag
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagCtrl, AUITextTag)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

class AUITextTagCtrlFont : public AUITextTagCtrl
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagCtrlFont, AUITextTagCtrl)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

class AUITextTagCtrlUnderline : public AUITextTagCtrl
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagCtrlUnderline, AUITextTagCtrl)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

class AUITextTagCtrlUnderlineX : public AUITextTagCtrl
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagCtrlUnderlineX, AUITextTagCtrl)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

class AUITextTagCtrlWordspace : public AUITextTagCtrl
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagCtrlWordspace, AUITextTagCtrl)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

class AUITextTagCtrlDefaultColor : public AUITextTagCtrl
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagCtrlDefaultColor, AUITextTagCtrl)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

class AUITextTagCtrlChannel : public AUITextTagCtrl
{
public:
	class AUITextTagCtrlChannelCache : public AUITextTagCache
	{
	public:
		friend class AUITextTagCtrlChannel;
		AUITextTagCtrlChannelCache(int indexInStr, const A3DPOINT2& pt, const A3DPOINT2& ptSize, int lenStr) : 
			AUITextTagCache(indexInStr, pt, ptSize), m_nLenstr(lenStr) {}
		virtual bool Output(const ACHAR* pText, const POINT& ptOffset, const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const;
	protected:
		int m_nLenstr;
	};
public:
	DECLARE_AUITEXT_TAG(AUITextTagCtrlChannel, AUITextTagCtrl)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

class AUITextTagCtrlColor : public AUITextTagCtrl
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagCtrlColor, AUITextTagCtrl)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

class AUITextTagItem : public AUITextTag
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagItem, AUITextTag)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;

	virtual bool IsMyType(const EditBoxItemBase* pItem) const {return false;}
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, 
		TAG_OP_OUT_PARAMS& outParams, EditBoxItemBase* pItem, ACHAR code) const 
	{return true;}

protected:	
	bool OutputSubTags(const TAG_OP_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams, TAG_RESOLVER_CONTEXT& context, 
		EditBoxItemBase* pItem, ACHAR code) const;
};

class AUITextTagItemImage : public AUITextTagItem
{
protected:
	class AUITextTagItemImageCache : AUITextTagCache
	{
		friend class AUITextTagItemImage;
	public:
		AUITextTagItemImageCache(int indexInStr, const A3DPOINT2& pt, const A3DPOINT2& ptSize, EditBoxItemBase* pItem, A2DSprite* pSprite, int nItem, float fScale, A3DCOLOR color) 
			: AUITextTagCache(indexInStr, pt, ptSize), m_pItem(pItem), m_pSprite(pSprite), m_nItem(nItem), m_fScale(fScale), m_color(color) {}
		bool Output(const ACHAR* pText, const POINT& ptOffset, const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const;
	protected:
		EditBoxItemBase* m_pItem;
		A2DSprite*	m_pSprite;
		int			m_nItem;
		float		m_fScale;
		A3DCOLOR	m_color;
	};
public:
	DECLARE_AUITEXT_TAG(AUITextTagItemImage, AUITextTagItem)
	virtual bool IsMyType(const EditBoxItemBase* pItem) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams, EditBoxItemBase* pItem, ACHAR code) const;
};

class AUITextTagItemEmotion : public AUITextTagItem
{
protected:
	class AUITextTagItemEmotionCache : AUITextTagCache
	{
		friend class AUITextTagItemEmotion;
	public:
		AUITextTagItemEmotionCache(int indexInStr, const A3DPOINT2& pt, const A3DPOINT2& ptSize, A2DSprite* pSprite, AUITEXTAREA_EMOTION* pem) 
			: AUITextTagCache(indexInStr, pt, ptSize), m_pSprite(pSprite), m_pem(pem) {}
		bool Output(const ACHAR* pText, const POINT& ptOffset, const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const;
	protected:
		A2DSprite*	m_pSprite;
		AUITEXTAREA_EMOTION* m_pem;
	};
public:
	DECLARE_AUITEXT_TAG(AUITextTagItemEmotion, AUITextTagItem)
	virtual bool IsMyType(const EditBoxItemBase* pItem) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams, EditBoxItemBase* pItem, ACHAR code) const;
};

class AUITextTagItemText : public AUITextTagItem
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagItemText, AUITextTagItem)
	virtual bool IsMyType(const EditBoxItemBase* pItem) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams, EditBoxItemBase* pItem, ACHAR code) const;
};

class AUITextTagItemBox : public AUITextTagItem
{
protected:
	class AUITextTagItemBoxCache : public AUITextTagCache
	{
		friend class AUITextTagItemBox;
		AUITextTagItemBoxCache(int indexInStr, const A3DPOINT2& pt, const A3DPOINT2& ptSize, A3DFlatCollector* pFC, A3DWireCollector* pWC, A3DCOLOR clFC, A3DCOLOR clWC, SIZE szBox) 
			: AUITextTagCache(indexInStr, pt, ptSize), m_pFC(pFC), m_pWC(pWC), m_clFC(clFC), m_clWC(clWC), m_szBox(szBox) {}
		virtual bool Output(const ACHAR* pText, const POINT& ptOffset, const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const;
	protected:
		A3DFlatCollector*	m_pFC;
		A3DWireCollector*	m_pWC;
		A3DCOLOR			m_clFC;
		A3DCOLOR			m_clWC;
		SIZE				m_szBox;
	};
public:
	DECLARE_AUITEXT_TAG(AUITextTagItemBox, AUITextTagItem)
	virtual bool IsMyType(const EditBoxItemBase* pItem) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams, EditBoxItemBase* pItem, ACHAR code) const;
};

class AUITextTagQuote : public AUITextTag
{
protected:
	class AUITextTagQuoteCache : public AUITextTagCache
	{
	public:
		friend class AUITextTagQuote;
		AUITextTagQuoteCache(int indexInStr, const A3DPOINT2& pt, const A3DPOINT2& ptSize, int lenStr) : 
			AUITextTagCache(indexInStr, pt, ptSize), m_nLenstr(lenStr) {}
		virtual bool Output(const ACHAR* pText, const POINT& ptOffset, const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const;
	protected:
		int m_nLenstr;
	};
public:
	DECLARE_AUITEXT_TAG(AUITextTagQuote, AUITextTag)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

class AUITextTagFakReturn : public AUITextTag
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagFakReturn, AUITextTag)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

class AUITextTagTable : public AUITextTag
{
public:
	DECLARE_AUITEXT_TAG(AUITextTagTable, AUITextTag)
	virtual bool IsMyType(const ACHAR* pszText, int index, int len) const;
	virtual bool Output(const TAG_OP_IN_PARAMS& inParams, TAG_RESOLVER_CONTEXT& context, TAG_OP_OUT_PARAMS& outParams) const;
};

class AUITextTagTextCache : public AUITextTagCache
{
	friend class AUIRichTextPrinter;
	friend class AUITextTagItemText;
	friend class AUITextTagReturn;
	friend class AUITagNPCLink;
	friend class AUITagPositionLink;

protected:
	AUITextTagTextCache(
		EditBoxItemBase* pItem,
		A3DFTFont* pFont,
		A3DPOINT2 ptPos,
		A3DPOINT2 ptSize,
		int nIndexInStr, 
		int nLen,
		bool bShadow, 
		A3DCOLOR clShadow, 
		A3DCOLOR clText, 
		bool bUnderline, 
		A3DCOLOR clUnderline, 
		float fThickUnderline)
		:AUITextTagCache(nIndexInStr, ptPos, ptSize),
		m_pItem(pItem),
		m_pFont(pFont),
		m_nLenStr(nLen),
		m_bShadow(bShadow),
		m_clText(clText),
		m_clShadow(clShadow),
		m_bUnderline(bUnderline),
		m_clUnderline(clUnderline),
		m_fThickUnderline(fThickUnderline)
	{
	}
	virtual bool Output(const ACHAR* pText, const POINT& ptOffset, const AUITAG_CACHE_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams) const;

protected:
	A3DFTFont*		m_pFont;
	EditBoxItemBase* m_pItem;
	int				m_nLenStr;
	bool			m_bShadow;
	A3DCOLOR		m_clText;
	A3DCOLOR		m_clShadow;
	bool			m_bUnderline;
	A3DCOLOR		m_clUnderline;
	float			m_fThickUnderline;
};

class AUITextTagCacheManager
{
	friend class AUIRichTextPrinter;
public:
	struct cacheLine
	{
		abase::vector<AUITextTagCache*> m_aCaches;
		cacheLine(int offsetY, AUITextTagCache* pCache) : m_nOffsetY(offsetY)
		{
			if (pCache) 
			{
				m_aCaches.push_back(pCache);
				m_nLineH = pCache->GetSize().y;
			}
		}
		~cacheLine()
		{
			for (size_t i=0; i<m_aCaches.size(); i++)
				delete m_aCaches[i];
		}
		void Offset(int Y)
		{
			m_nOffsetY += Y;
			for (size_t i=0; i<m_aCaches.size(); i++)
				m_aCaches[i]->Offset(Y);
		}
		void AddCache(AUITextTagCache* pCache)
		{
			if (!pCache)
				return;
			m_aCaches.push_back(pCache);
			m_nLineH = a_Max(pCache->GetSize().y, m_nLineH);
		}
		void ClearItem(EditBoxItemsSet& itemSet)
		{
			for (size_t i=0; i<m_aCaches.size(); i++)
				m_aCaches[i]->ClearItem(itemSet);
		}
		int m_nOffsetY;
		int m_nLineH;
	};

	struct cacheSection
	{
		abase::vector<cacheLine*> m_aCacheLines;
		cacheSection(cacheLine* pCacheLine)
		{
			if (pCacheLine) m_aCacheLines.push_back(pCacheLine);
		}
		~cacheSection()
		{
			for (size_t i=0; i<m_aCacheLines.size(); i++)
				delete m_aCacheLines[i];
		}
		void Offset(int Y)
		{
			for (size_t i=0; i<m_aCacheLines.size(); i++)
				m_aCacheLines[i]->Offset(Y);
		}
		void ClearItem(EditBoxItemsSet& itemSet)
		{
			for (size_t i=0; i<m_aCacheLines.size(); i++)
				m_aCacheLines[i]->ClearItem(itemSet);
		}
	};

	AUITextTagCacheManager();
	~AUITextTagCacheManager();
	void Release();
	void ClearCache();
	void AddCache(AUITextTagCache* pCache, int nSectionIndex, int nLineIndex);
	bool Output(AUITAG_CACHE_IN_PARAMS& inParam, TAG_OP_OUT_PARAMS& outParams) const;
	bool AddCache(TAG_OP_IN_PARAMS& inParams, TAG_OP_OUT_PARAMS& outParams);
	int  SetMaxSection(int nMaxSection, EditBoxItemsSet& itemsSet);
	const abase::vector<cacheSection*>& GetCache() const
	{
		return m_aCaches;
	}

protected:
	abase::vector<cacheSection*> m_aCaches;
	TAG_RESOLVER_CONTEXT m_context;
	ACString m_str;
	int m_nLines;
};
#endif // !defined(AFX_AUITAG_H__F5D79DDA_CEE1_4AFA_9BD0_DC83840670E1__INCLUDED_)
