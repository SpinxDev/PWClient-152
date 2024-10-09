// Filename	: AUI_ImageGrid.h
// Creator	: Xu Wenbin
// Date		: 2010/5/19

#pragma once

#include <A2DSprite.h>
#include <AUIDialog.h>
#include <AUIImagePicture.h>
#include <AUIScroll.h>
#include <vector.h>

//
//	AUI_ImageGrid: �������������е� AUIImagePicture����ʾ���������Ƶ� AUIImagePicture
//
class AUI_ImageGrid
{
public:
	AUI_ImageGrid();
	virtual ~AUI_ImageGrid();

	bool IsReady()const;

	void ResetContent();
	bool Init(PAUIDIALOG pDlgParent, const char *pszFirstName, int nMaxRowVisible, int nMaxColVisible, const char *pszScrollName, const char *pszBackground, A2DSprite *pSpriteSelected, A2DSprite *pSpriteMouseOn);
	void Append(DWORD dwData, void *pvData, const ACHAR *pszHint, A2DSprite *pA2DSprite, bool bUpdateView = false);

	int GetMaxRow()const{ return m_nMaxRowVisible; }
	int GetMaxCol()const{ return m_nMaxColVisible; }
	int GetFirstIndex()const{ return m_nFirstIndex; }
	AString GetScrollName()const{ return m_pScroll ? m_pScroll->GetName() : NULL; }
	PAUISCROLL GetScroll()const{ return m_pScroll; }
	PAUIOBJECT GetBkground()const{ return m_pBkground; }
	PAUIDIALOG GetDialog()const{ return m_pDlgOwner; }

	int GetCount()const{ return (int)m_data.size(); }

	PAUIOBJECT OnMouse_Clicked(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	bool OnMouse_Wheel(WPARAM wParam, LPARAM lParam, PAUIOBJECT pObj);
	bool CanAcceptMouseWheel(PAUIOBJECT pObj);
	bool IsMouseOn(PAUIOBJECT pObj);
	bool IsSelected(PAUIOBJECT pObj);
	void OnTick();
	
	void Show(bool bShow);
	bool IsShown()const{ return m_bShow; }

	// Reset the images binding to this grid.
	// Set bRelease = true will cause the real release operation.
	void ResetImages(bool bRelease);

private:

	AString MakeName(int nRealRow, int nRealCol)const;
	int GetFirstOffset()const { return m_nFirstOffset; }
	PAUIIMAGEPICTURE GetObject(int nRealRow, int nRealCol);
	PAUIIMAGEPICTURE GetVirtualObject(int nVirtualRow, int nVirtualCol);
	bool GetPrefixIndex(const char *pszName, AString &strPrefix, int &nIndex)const;
	void UpdateView(int m_nFirstOffset);
	int GetBarLevelFromOffset(int nOffset)const;
	int GetOffsetFromBarLevel(int nBarLevel)const;

	struct GridItemData
	{
		DWORD dwData;
		void * pvData;
		ACString strHint;
		A2DSprite *pA2DSprite;
	};
	const GridItemData *GetItemData(int nVirtualRow, int nVirtualCol)const;

	static void GridImageRenderCallback(const A3DRECT &rc, DWORD param1, DWORD param2, DWORD param3);

private:
	
	PAUIDIALOG m_pDlgOwner;	//	�ؼ��ĸ��Ի���
	AString m_strPrefix;	//	�ؼ�������ǰ׺���� Img_1 ��ǰ׺�� Img_
	int  m_nFirstIndex;		//	�ؼ�����ʼ�±꣬������ͬ��ؼ��ķ�Χ�� Img_5~Img_15���򷵻�ֵ�� 5
	int  m_nMaxRowVisible;	//	���ɼ��У����ڴ����Ժ󣬽�ʹ�ù������л�
	int  m_nMaxColVisible;	//	���ɼ���
	PAUIOBJECT m_pBkground;	//	�����ؼ�
	PAUISCROLL m_pScroll;	//	�����������ڲ�ͬҳ�л�
	int  m_nFirstOffset;	//	��ǰ��һ���ɼ��Ŀؼ��� index

	typedef abase::vector<PAUIIMAGEPICTURE> ObjectArray;	//	�ؼ��б�
	ObjectArray m_objs;

	abase::vector<GridItemData> m_data;	//	���пɼ������ɼ��ؼ����������б�

	bool m_bShow;				//	�Ƿ�ɼ�

	A2DSprite *	m_pSpriteSelected;	//	���ѡ��ʱ��ʾ��ͼƬ
	A2DSprite *	m_pSpriteMouseOn;	//	��꾭��ʱ��ʾ��ͼƬ
	int  m_nSelectedVirtualRow;	//	��ǰѡ�����߼�����
	int  m_nSelectedVirtualCol;	//	
};
