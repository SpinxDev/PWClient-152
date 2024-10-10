#pragma once

typedef enum
{
	BANK_ITEM_NONE,
	BANK_ITEM_AUDIO,
	BANK_ITEM_ARCHIVE,
	BANK_ITEM_BANK
}BANK_ITEM_TYPE;

#define IMAGE_BANK				0
#define IMAGE_BANK_ARCHIVE		1
#define IMAGE_BANK_AUDIO		2

class CTreeCtrlEx;

class CBankDisplay
{
public:
	CBankDisplay(void);
	~CBankDisplay(void);
public:
	bool			BuildImageList(CImageList& imageList);
	bool			UpdateTree(CTreeCtrlEx* pWndTree);
	bool			BuildTree(HTREEITEM hItem);
protected:
	CTreeCtrlEx*	m_pWndTree;
};
