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

	// 导出玩家模型的选项
	struct MODEL_OPTION
	{
		abase::vector<bool> actionExport;
		bool bEquipGfx;

		MODEL_OPTION() : bEquipGfx(false) {}
	};

	// 玩家装备对应的特效
	typedef abase::hash_map<int, std::set<AString> > EquipGfxMap;
	// 策略数据哈希表
	typedef abase::hash_map<int, CPolicyData*> PolicyDataMap;

public:
	virtual ~FileAnalyse();

	// 初始化分析器
	bool Init();

	// 分析文件所依赖的文件
	void Analyse(const char* szText, FileType Type);

	// 获取结果
	const FilePathList& GetResult() const { return m_Filelist; }

	// 清空结果
	void Clear();

	// 设置后台线程处理的监听函数
	void SetCBOperation(CB_Operation pCBOperation) { m_pCBOperation = pCBOperation; }

	// 导出文件到指定目录
	void Export(const char* szDestDir);

	// 获取引擎指针
	A3DEngine* GetA3DEngine() const { return m_pA3DEngine; }
	// 获取引擎设备
	A3DDevice* GetA3DDevice() const { return m_pA3DDevice; }
	// 获取GfxExMan
	A3DGFXExMan* GetGFXExMan() const { return m_pGFXExMan; }
	// 获取ModelMan
	CECModelMan* GetModelMan() const { return m_pModelMan; }
	// 获取Camera指针
	A3DCamera* GetA3DCamera() const { return m_pA3DCamera; }
	// 获取elementdata
	elementdataman* GetDataMan() const { return m_pDataMan; }

	// 获取导出玩家模型的选项
	MODEL_OPTION& GetModelOption() { return m_ECMOption; }

	// 获取实例
	static FileAnalyse& GetInstance();

protected:
	A3DEngine* m_pA3DEngine;
	A3DDevice* m_pA3DDevice;
	A3DGFXExMan* m_pGFXExMan;
	CECModelMan* m_pModelMan;
	A3DCamera* m_pA3DCamera;
	elementdataman* m_pDataMan;
	FilePathList m_Filelist;
	std::set<AString> m_FileDup;	// 检测文件重名
	CB_Operation m_pCBOperation;	// 后台线程操作的回调函数
	CPolicyDataManager* m_pPolicies;	// 策略数据
	PolicyDataMap m_PolicyMap;		// 策略数据哈希表

	MODEL_OPTION m_ECMOption;		// 导出玩家模型的选项
	EquipGfxMap m_EquipGfx;			// 玩家装备所对应的特效

private:
	FileAnalyse();

	// 初始化A3DEngine
	bool InitA3DEngine();
	// 加载装备对应特效的信息
	bool LoadEquipGfx();

	// 分析NPC模型所需的文件
	bool AnalyseNPCModel(int tid);
	// 分析UI界面所需的文件
	bool AnalyseUI(const char* szFilename);
	// 分析UI界面DCF文件
	bool AnalyseDCFFile(const char* szDCFFile);
	// 分析玩家相关模型
	bool AnalysePlayerModel(const char* szProf);
	// 分析技能相关资源
	bool AnalyseSkill(int skill_id);
	// 分析BUFF相关特效资源
	bool AnalyseBuff(int buff_id);
	// 分析装备相关的资源
	bool AnalyseEquip(int equip_id);
	// 分析被缩放过的NPC
	bool AnalyseScaledNPC();
	// 分析有特效但没有挂到挂点上的装备
	bool AnalyseEquipGfxNoHanger();
	// 分析烟花相关的资源
	bool AnalyseFirework(int tid);

	// 分析Gfx所需的文件
	bool AnalyseGfx(A3DGFXEx* pGFX);
	bool AnalyseGfxByName(const char* filename);
	// 分析A3DSkin资源
	bool AnalyseSkin(A3DSkin* pSkin);
	bool AnalyseSkinByName(const char* filename);
	// 分析A3DSkinModel资源
	bool AnalyseSkinModel(A3DSkinModel* pModel);
	// 分析CECModel资源
	bool AnalyseECModel(CECModel* pModel);
	bool AnalyseECModelByName(const char* filename, bool bShowOption=false);
	// 分析sgc文件
	bool AnalyseSGCFile(const char* szSGCFile);
	// 分析玩家模型的LUA脚本
	bool AnalyseModelScript(const char* szLuaText);
	// 分析策略触发器
	bool AnalysePolicyTrigger(CPolicyData* pPolicy, CTriggerData* pTrigger);

	// 添加文件记录
	bool AddFileRecord(const char* filename);

	// 导出指定的文件
	void ExportFile(const char* szFile, const char* szDestDir);
};

///////////////////////////////////////////////////////////////////////////
//  
//  Inline Functions
//  
///////////////////////////////////////////////////////////////////////////
