/*
 * FILE: EC_Utility.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: Shizhenhua, 2013/6/20
 *
 * HISTORY: 
 *
 * Copyright (c) 2011 ACE Studio, All Rights Reserved.
 */


#pragma once

#include <AAssist.h>
#include <A3DTypes.h>
#include <time.h>
#include <hashtab.h>


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////

#define STRUCT_SIZE(ss) sizeof(ss) / sizeof(ss[0])


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

extern A3DVECTOR3 g_vOrigin;
extern A3DVECTOR3 g_vAxisX;
extern A3DVECTOR3 g_vAxisY;
extern A3DVECTOR3 g_vAxisZ;


///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

bool glb_CreateDirectory(const char* szDir);
bool glb_FileExist(const char* szFile);
void glb_ChangeExtension(char* szFile, char* szNewExt);
void glb_ClearExtension(char* szFile);

// 显示ECModel的导出选项
bool ShowModelOption(const char* szModelPath);

// 生成装备的A3DSkin路径
// Param1: 装备的名字
// Param2: 输出路径列表
// Param3: 是否有性别之分，0为无性别，1为男性，2为女性
void GenSkinPath(const char* szEquipName, abase::vector<AString>& filepaths, int sex=0);

// 所有NPC、怪物、宠物的数目
int GetNumAllNPCs();

// 获取所有武器、飞剑、翅膀的个数
int GetNumAllEquips();

// 浮点相等判断
bool FloatEqual(float f1, float f2);

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////
