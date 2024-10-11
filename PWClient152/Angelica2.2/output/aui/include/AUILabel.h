// Filename	: AUILabel.h
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUILabel is the class of static text control.

#ifndef _AUILABEL_H_
#define _AUILABEL_H_

#include "AUIObject.h"
#include "AUITextArea.h"

class AUILabel : public AUIObject
{
public:
	AUILabel();
	virtual ~AUILabel();

	virtual bool Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF);
	virtual bool InitXML(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AXMLItem *pItem);
	virtual bool Release(void);
	virtual bool Save(FILE *file);
	virtual bool SaveXML(AXMLItem *pXMLItemDialog);
	virtual bool Tick(void);
	virtual bool Render(void);
	virtual bool OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam);
	/* support properties
	"Vertical Text"			Property->b
	"Align"					Property->i
	AUIObject.Properties
	*/
	virtual bool GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual bool SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property);
	virtual void SetText(const ACHAR *pszText);
	virtual int GetRequiredHeight();
	static bool RegisterPropertyType();

	bool IsMarquee();
	void SetMarquee(bool bMarquee, int nTimes, int nSpeed);
	void SetCurrentEmotion(int nEmotion);
	void SetEmotionList(A2DSprite **pA2DSpriteEmotion,
		abase::vector<AUITEXTAREA_EMOTION> **pvecEmotion);
	void SetImageList(abase::vector<A2DSprite*> *pvecImageList) { m_pvecImageList = pvecImageList; }
	void SetOtherFonts(abase::vector<FONT_WITH_SHADOW>* pvecFonts) { m_pvecOtherFonts = pvecFonts; }
	void SetItemMask(int nMask)	{ m_nItemMask = nMask; }
	EditboxItemType GetItemMask() { return (EditboxItemType)m_nItemMask; }
	
	EditBoxItemsSet* GetItemSet() {return &m_ItemsSet;}
	int GetShadow() { return m_nShadow; }

protected:
	bool m_bMarquee;
	int m_nMarqueePos;
	DWORD m_dwLastTick;
	int m_nMarqueeTimes;
	int m_nMarqueeSpeed;	// Pixels per second.
	int	m_nItemMask;
	bool m_bVerticalText;
	
	int m_nCurEmotionSet;
	A2DSprite **m_pA2DSpriteEmotion;
	abase::vector<AUITEXTAREA_EMOTION> **m_vecEmotion;
	abase::vector<A2DSprite*> *m_pvecImageList;
	abase::vector<FONT_WITH_SHADOW> *m_pvecOtherFonts;
	EditBoxItemsSet m_ItemsSet;
};

typedef AUILabel * PAUILABEL;

#endif //_AUILABEL_H_