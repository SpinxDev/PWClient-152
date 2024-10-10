#pragma once

typedef enum
{
	SOUNDDEF_ITEM_NONE,
	SOUNDDEF_ITEM_AUDIO,
	SOUNDDEF_ITEM_AUDIO_GROUP,
	SOUNDDEF_ITEM_SOUNDDEF,
	SOUNDDEF_ITEM_ARCHIVE
}SOUNDDEF_ITEM_TYPE;

#define IMAGE_SOUNDDEF			0
#define IMAGE_SOUNDDEF_ARCHIVE	1
#define IMAGE_SOUNDDEF_AUDIO	2
#define IMAGE_SOUNDDEF_AUDIO_GROUP	2

class CTreeCtrlEx;

class CSoundDefDisplay
{
public:
	CSoundDefDisplay(void);
	~CSoundDefDisplay(void);
public:
	bool			BuildImageList(CImageList& imageList);
	bool			UpdateTree(CTreeCtrlEx* pWndTree);
	bool			BuildTree(HTREEITEM hItem);
	bool			RebuildSoundDefItems(HTREEITEM hItem);
	bool			RebuildAudioGroupItems(HTREEITEM hItem);
protected:
	CTreeCtrlEx*	m_pWndTree;
};
