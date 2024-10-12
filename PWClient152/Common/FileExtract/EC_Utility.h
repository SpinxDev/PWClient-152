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

// ��ʾECModel�ĵ���ѡ��
bool ShowModelOption(const char* szModelPath);

// ����װ����A3DSkin·��
// Param1: װ��������
// Param2: ���·���б�
// Param3: �Ƿ����Ա�֮�֣�0Ϊ���Ա�1Ϊ���ԣ�2ΪŮ��
void GenSkinPath(const char* szEquipName, abase::vector<AString>& filepaths, int sex=0);

// ����NPC������������Ŀ
int GetNumAllNPCs();

// ��ȡ�����������ɽ������ĸ���
int GetNumAllEquips();

// ��������ж�
bool FloatEqual(float f1, float f2);

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////
