#pragma once

#include "ECScriptFunctionBase.h"
#include <AString.h>

// 是否允许玩家选择下一条提示
// bHasNext	true为可选下一条,false为不可选下一条
class CECScriptFunctionSetHasNextTip: public CECScriptFunctionBase
{
	bool m_bHasNext;
public:
	CECScriptFunctionSetHasNextTip()
	{
		AddParam(m_bHasNext);
	}
	virtual void Execute(CECScriptContext *pContext);
};

// 显示或隐藏帮助窗口
// bShow	true为显示,false为隐藏
class CECScriptFunctionShowHelpDialog : public CECScriptFunctionBase
{
	bool m_bShow;
public:
	CECScriptFunctionShowHelpDialog()
	{
		AddParam(m_bShow);
	}
	virtual void Execute(CECScriptContext *pContext);
};

// 设置当前显示的帮助提示
// nStringID	帮助提示的字符串ID
class CECScriptFunctionSetContent : public CECScriptFunctionBase
{
	AString m_strContent;
public:
	CECScriptFunctionSetContent()
	{
		AddParam(m_strContent);
	}
	virtual void Execute(CECScriptContext *pContext);
};

// 显示或隐藏窗口
// szDialogName	窗口名称
// bShow		true为显示,false为隐藏
class CECScriptFunctionShowDialog : public CECScriptFunctionBase
{
	AString m_szDialogName;
	bool m_bShow;
public:
	CECScriptFunctionShowDialog()
	{
		AddParam(m_szDialogName);
		AddParam(m_bShow);
	}
	virtual void Execute(CECScriptContext *pContext);
};

// 设置窗口控件是否闪烁
// szDialogName	窗口名称
// szItemName	控件名称
// bFlash		是否闪烁
class CECScriptFunctionSetFlashDialogItem : public CECScriptFunctionBase
{
	AString m_szDialogName;
	AString m_szItemName;
	bool	m_bFlash;
public:
	CECScriptFunctionSetFlashDialogItem()
	{
		AddParam(m_szDialogName);
		AddParam(m_szItemName);
		AddParam(m_bFlash);
	}
	virtual void Execute(CECScriptContext *pContext);
};

// 播放一个WAV文件
// m_strFileName 文件名，不包括路径，路径是固定的
class CECScriptFunctionPlayVoice : public CECScriptFunctionBase
{
	AString m_strFileName;
public:
	CECScriptFunctionPlayVoice::CECScriptFunctionPlayVoice()
	{
		AddParam(m_strFileName);
	}
	virtual void Execute(CECScriptContext *pContext);
};

// 停止当前正在播放的WAV
// m_bFadeOut 是否使用淡出效果
class CECScriptFunctionStopVoice : public CECScriptFunctionBase
{
	bool m_bFadeOut;
public:
	CECScriptFunctionStopVoice::CECScriptFunctionStopVoice()
	{
		AddParam(m_bFadeOut);
	}
	virtual void Execute(CECScriptContext *pContext);
};

// 开始一个定时器
class CECScriptFucntionResetTimer : public CECScriptFunctionBase
{
	DWORD m_dwDuration;
public:
	CECScriptFucntionResetTimer::CECScriptFucntionResetTimer()
	{
		AddParam(m_dwDuration);
	}
	virtual void Execute(CECScriptContext *pContext);
};

// 设置当前显示的帮助图片
class CECScriptFunctionSetImage : public CECScriptFunctionBase {
	AString m_strImgFile;
public:
	CECScriptFunctionSetImage() {
		AddParam(m_strImgFile);
	}
	virtual void Execute(CECScriptContext *pContext);
};

// 设置Tip界面的名称和内容
class CECScriptFunctionSetTipTitleAndContent : public CECScriptFunctionBase {
	AString m_strTitle;
	AString m_strContent;
public:
	CECScriptFunctionSetTipTitleAndContent() {
		AddParam(m_strTitle);
		AddParam(m_strContent);
	}
	virtual void Execute(CECScriptContext *pContext);
};

// 展示Tip界面
class CECScriptFunctionShowTip : public CECScriptFunctionBase {
	int m_iGfxTime;
	int m_iInTime;
	int m_iStayTime;
	int m_iOutTime;
public:
	CECScriptFunctionShowTip() {
		AddParam(m_iGfxTime);
		AddParam(m_iInTime);
		AddParam(m_iStayTime);
		AddParam(m_iOutTime);
	}
	virtual void Execute(CECScriptContext *pContext);
};