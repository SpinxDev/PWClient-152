#pragma once

#include "resource.h"
#include "FSoundInstance.h"
#include "PropListCtrl/PropertyInterface.h"

using AudioEngine::SOUND_INSTANCE_PROPERTY;

class CBCGPPropertyListWrapper;
class CPropertyArray;
class CPropertyUpdateListener;

// CDlgSoundInsProp �Ի���

class CDlgSoundInsProp : public CBCGPDialog, public CPropertyInterface
{
	DECLARE_DYNAMIC(CDlgSoundInsProp)

public:
	CDlgSoundInsProp(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~CDlgSoundInsProp();
public:
	void SetProp(const SOUND_INSTANCE_PROPERTY& prop, float fMinValue, float fMaxValue, const char* szNote);
	SOUND_INSTANCE_PROPERTY GetProp() const { return m_Prop; }
	const char* GetNote() const { return m_strNote.c_str(); }

// �Ի�������
	enum { IDD = IDD_SOUND_INS_PROP };

protected:
	virtual BOOL OnInitDialog();
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
protected:
	void buildProperty();
	virtual void UpdateProperty(bool bSave, bool bIsCommit = false);
protected:
	SOUND_INSTANCE_PROPERTY m_Prop;
	float m_fMinValue;
	float m_fMaxValue;
	std::string m_strNote;
	CBCGPPropertyListWrapper* m_pBCGPPropListWrapper;
	CPropertyArray* m_pProperties;
	CPropertyUpdateListener* m_pPropertyListener;
};
