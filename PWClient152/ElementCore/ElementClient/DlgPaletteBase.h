// Filename	: DlgPaletteBase.h
// Creator	: zhangyitian
// Date		: 2014/7/29
#ifndef _ELEMENTCLIENT_DLGPALETTEBASE_H_
#define _ELEMENTCLIENT_DLGPALETTEBASE_H_

#include "DlgBase.h"

#include "AString.h"

// 将调色板功能提炼出一个基类，仅包含一个调色板的ImagePicture，能够进行基本的选取颜色功能

class AUIImagePicture;
class CPaletteControl;
class A2DSprite;

class CDlgPaletteBase : public CDlgBase {

	AUI_DECLARE_EVENT_MAP();

public:
	CDlgPaletteBase(const AString& strImgFile, bool bChangeColorOnMove = true);

protected:
	virtual bool OnInitDialog();
	virtual bool Release();

	void OnLButtonDownPicPallete(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnMouseMovePicPallete(WPARAM wParam, LPARAM lParam, AUIObject * pObj);
	void OnLButtonDownBaseColor(WPARAM wParam, LPARAM lParam, AUIObject * pObj);

	virtual void OnSetColor(const A3DCOLOR& color) = 0;

private:
	AUIImagePicture *	m_pPicPane;
	AUIImagePicture *	m_pPicIndicator;
	CPaletteControl *	m_pPallete;
	A2DSprite *			m_pA2DSpriteColorPic;

	AString m_strImgFile;		// 调色板图片
	bool m_bChangeColorOnMove;	// 鼠标移动的时候改变颜色，默认为true，子类可修改
	
};

///////////////////////////////////////////////////////////////////////////////////////

// 用于时装的调色板，具有全、上、下、手、鞋五个按钮

class AUIRadioButton;

class CDlgPaletteFashion : public CDlgPaletteBase {

	AUI_DECLARE_COMMAND_MAP();

public:
	enum enumChangeTarget{
		CHANGE_UPPER,
		CHANGE_LOWER,
		CHANGE_WAIST,
		CHANGE_SHOES,
		CHANGE_ALL,
		CHANGE_NUM,
	};

	CDlgPaletteFashion(const AString& strImgFile, bool bChangeColorOnMove = true);

	void OnCommand_Suite(const char * szCommand);
	void OnCommand_UpperBody(const char * szCommand);
	void OnCommand_LowerBody(const char * szCommand);
	void OnCommand_Waist(const char * szCommand);
	void OnCommand_Shoes(const char * szCommand);

protected:
	virtual bool OnInitDialog();
	virtual void OnChangeTarget() {};
	void ChangeTarget(enumChangeTarget newTarget);
	int GetEnabledTargetCount();

protected:
	enumChangeTarget	m_changeTarget;
	AUIRadioButton *	m_pRdos[CHANGE_NUM];
};

#endif