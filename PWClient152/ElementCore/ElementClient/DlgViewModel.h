// Filename	: DlgViewModel.h
// Creator	: Xu Wenbin
// Date		: 2013/2/21

#pragma once

#include "DlgBase.h"
#include <AUIImagePicture.h>
#include <AUIStillImageButton.h>
#include <A3DTypes.h>
#include <A3DGeometry.h>

class CECModel;

//	Ԥ��һ�� ECM ģ�ͣ���衢�ɽ�����ͨ�ý���
class CDlgViewModel : public CDlgBase  
{
	AUI_DECLARE_COMMAND_MAP()

public:
	CDlgViewModel();
	virtual ~CDlgViewModel();

	void OnCommand_CANCEL(const char * szCommand);

	void SetModel(CECModel *pModel, const char *szActToPlay=NULL);

	bool IsModelLoaded() {return m_pModel != NULL; }

protected:
	PAUIIMAGEPICTURE		m_pImg_Model;
	PAUISTILLIMAGEBUTTON	m_pBtn_LeftTurn;
	PAUISTILLIMAGEBUTTON	m_pBtn_RightTurn;
	PAUISTILLIMAGEBUTTON	m_pBtn_MoveNear;
	PAUISTILLIMAGEBUTTON	m_pBtn_MoveFar;

	int			m_nAngle;
	int			m_nMove;
	CECModel*	m_pModel;
	A3DAABB		m_aabb;
	AString		m_strAct;
	bool		m_bAABBDirty;

	virtual bool OnInitDialog();
	virtual void OnShowDialog();
	virtual void OnTick();

	void Reset();
	void UpdateAABB();
	void UpdateRenderCallback();
};

//	��ȡԤ��ģ�͵���
//	����ÿ��ģ�ͣ���衢�ɽ����ļ���Լ����ͬ�������Ӵ���ȡ��
class CECIvtrItem;
class CECViewModelExtractor
{
public:
	static bool CanExtract(CECIvtrItem *pItem);
	static CECModel * Extract(CECIvtrItem *pItem);
};

//	�ж���Ʒ���ģ���ܷ�Ԥ�����࣬��������ȼ���ְҵ�Ȼ�������
class CECViewModelCondition
{
public:
	static bool Meet(CECIvtrItem *pItem);
};