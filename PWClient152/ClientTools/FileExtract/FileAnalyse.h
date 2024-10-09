/*
 * FILE: FileAnalyse.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/6/19
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once

#include <ABaseDef.h>
#include <AMemory.h>
#include <AAssist.h>
#include <vector.h>
#include <set>
#include <hashmap.h>


///////////////////////////////////////////////////////////////////////////
//  
//  Define and Macro
//  
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//  
//  Types and Global variables
//  
///////////////////////////////////////////////////////////////////////////

class A3DEngine;
class A3DDevice;
class A3DCamera;
class A3DSkin;
class A3DSkinModel;
class A3DGFXEx;
class A3DGFXExMan;
class elementdataman;
class CECModel;
class CECModelMan;
class CPolicyData;
class CPolicyDataManager;
class CTriggerData;


///////////////////////////////////////////////////////////////////////////
//  
//  class FileAnalyse
//  
///////////////////////////////////////////////////////////////////////////


class FileAnalyse
{
public:

	enum FileType
	{
		TYPE_NPCMODEL,
		TYPE_UI,
		TYPE_SKILL,
		TYPE_PLAYERMODEL,
		TYPE_BUFF,
		TYPE_ECMODEL,
		TYPE_EQUIP,
		TYPE_SCALEDNPC,
		TYPE_EQUIPGFXNOHANGER,
		TYPE_FIREWORK,
	};

	typedef abase::vector<AString> FilePathList;
	typedef bool (*CB_Operation) (const char* filename);

	// �������ģ�͵�ѡ��
	struct MODEL_OPTION
	{
		abase::vector<bool> actionExport;
		bool bEquipGfx;

		MODEL_OPTION() : bEquipGfx(false) {}
	};

	// ���װ����Ӧ����Ч
	typedef abase::hash_map<int, std::set<AString> > EquipGfxMap;
	// �������ݹ�ϣ��
	typedef abase::hash_map<int, CPolicyData*> PolicyDataMap;

public:
	virtual ~FileAnalyse();

	// ��ʼ��������
	bool Init();

	// �����ļ����������ļ�
	void Analyse(const char* szText, FileType Type);

	// ��ȡ���
	const FilePathList& GetResult() const { return m_Filelist; }

	// ��ս��
	void Clear();

	// ���ú�̨�̴߳���ļ�������
	void SetCBOperation(CB_Operation pCBOperation) { m_pCBOperation = pCBOperation; }

	// �����ļ���ָ��Ŀ¼
	void Export(const char* szDestDir);

	// ��ȡ����ָ��
	A3DEngine* GetA3DEngine() const { return m_pA3DEngine; }
	// ��ȡ�����豸
	A3DDevice* GetA3DDevice() const { return m_pA3DDevice; }
	// ��ȡGfxExMan
	A3DGFXExMan* GetGFXExMan() const { return m_pGFXExMan; }
	// ��ȡModelMan
	CECModelMan* GetModelMan() const { return m_pModelMan; }
	// ��ȡCameraָ��
	A3DCamera* GetA3DCamera() const { return m_pA3DCamera; }
	// ��ȡelementdata
	elementdataman* GetDataMan() const { return m_pDataMan; }

	// ��ȡ�������ģ�͵�ѡ��
	MODEL_OPTION& GetModelOption() { return m_ECMOption; }

	// ��ȡʵ��
	static FileAnalyse& GetInstance();

protected:
	A3DEngine* m_pA3DEngine;
	A3DDevice* m_pA3DDevice;
	A3DGFXExMan* m_pGFXExMan;
	CECModelMan* m_pModelMan;
	A3DCamera* m_pA3DCamera;
	elementdataman* m_pDataMan;
	FilePathList m_Filelist;
	std::set<AString> m_FileDup;	// ����ļ�����
	CB_Operation m_pCBOperation;	// ��̨�̲߳����Ļص�����
	CPolicyDataManager* m_pPolicies;	// ��������
	PolicyDataMap m_PolicyMap;		// �������ݹ�ϣ��

	MODEL_OPTION m_ECMOption;		// �������ģ�͵�ѡ��
	EquipGfxMap m_EquipGfx;			// ���װ������Ӧ����Ч

private:
	FileAnalyse();

	// ��ʼ��A3DEngine
	bool InitA3DEngine();
	// ����װ����Ӧ��Ч����Ϣ
	bool LoadEquipGfx();

	// ����NPCģ��������ļ�
	bool AnalyseNPCModel(int tid);
	// ����UI����������ļ�
	bool AnalyseUI(const char* szFilename);
	// ����UI����DCF�ļ�
	bool AnalyseDCFFile(const char* szDCFFile);
	// ����������ģ��
	bool AnalysePlayerModel(const char* szProf);
	// �������������Դ
	bool AnalyseSkill(int skill_id);
	// ����BUFF�����Ч��Դ
	bool AnalyseBuff(int buff_id);
	// ����װ����ص���Դ
	bool AnalyseEquip(int equip_id);
	// ���������Ź���NPC
	bool AnalyseScaledNPC();
	// ��������Ч��û�йҵ��ҵ��ϵ�װ��
	bool AnalyseEquipGfxNoHanger();
	// �����̻���ص���Դ
	bool AnalyseFirework(int tid);

	// ����Gfx������ļ�
	bool AnalyseGfx(A3DGFXEx* pGFX);
	bool AnalyseGfxByName(const char* filename);
	// ����A3DSkin��Դ
	bool AnalyseSkin(A3DSkin* pSkin);
	bool AnalyseSkinByName(const char* filename);
	// ����A3DSkinModel��Դ
	bool AnalyseSkinModel(A3DSkinModel* pModel);
	// ����CECModel��Դ
	bool AnalyseECModel(CECModel* pModel);
	bool AnalyseECModelByName(const char* filename, bool bShowOption=false);
	// ����sgc�ļ�
	bool AnalyseSGCFile(const char* szSGCFile);
	// �������ģ�͵�LUA�ű�
	bool AnalyseModelScript(const char* szLuaText);
	// �������Դ�����
	bool AnalysePolicyTrigger(CPolicyData* pPolicy, CTriggerData* pTrigger);

	// ����ļ���¼
	bool AddFileRecord(const char* filename);

	// ����ָ�����ļ�
	void ExportFile(const char* szFile, const char* szDestDir);
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////
