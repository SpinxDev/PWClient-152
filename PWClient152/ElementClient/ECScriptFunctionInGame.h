#pragma once

#include "ECScriptFunctionBase.h"
#include <AString.h>

// �Ƿ��������ѡ����һ����ʾ
// bHasNext	trueΪ��ѡ��һ��,falseΪ����ѡ��һ��
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

// ��ʾ�����ذ�������
// bShow	trueΪ��ʾ,falseΪ����
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

// ���õ�ǰ��ʾ�İ�����ʾ
// nStringID	������ʾ���ַ���ID
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

// ��ʾ�����ش���
// szDialogName	��������
// bShow		trueΪ��ʾ,falseΪ����
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

// ���ô��ڿؼ��Ƿ���˸
// szDialogName	��������
// szItemName	�ؼ�����
// bFlash		�Ƿ���˸
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

// ����һ��WAV�ļ�
// m_strFileName �ļ�����������·����·���ǹ̶���
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

// ֹͣ��ǰ���ڲ��ŵ�WAV
// m_bFadeOut �Ƿ�ʹ�õ���Ч��
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

// ��ʼһ����ʱ��
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

// ���õ�ǰ��ʾ�İ���ͼƬ
class CECScriptFunctionSetImage : public CECScriptFunctionBase {
	AString m_strImgFile;
public:
	CECScriptFunctionSetImage() {
		AddParam(m_strImgFile);
	}
	virtual void Execute(CECScriptContext *pContext);
};

// ����Tip��������ƺ�����
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

// չʾTip����
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