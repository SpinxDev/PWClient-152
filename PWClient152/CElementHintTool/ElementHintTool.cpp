// ElementHintTool.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <windows.h>   // For GRoleInventory
#include <gnoctets.h>  // For Octets
#include <AMemory.h>   // For A_DEBUG_NEW
#include <AFI.h>       // For af_Initialize af_Finalize
#include "EC_Global.h" // For glb_InitLogSystem glbCloseLogSystem
#include <AFilePackage.h>
#include <AFilePackMan.h>
#include "GTransForm/gtransformclient.hpp"
#include "ElementHintProcess.h"

#ifdef HINT_TOOL_DEBUG
#include "./GTransForm/getitemdesc.hpp"
#include "./GTransForm/getitemdesc_re.hpp"
#include "./GTransForm/getencrypteditemdesc.hpp"
#include "./GTransForm/getencrypteditemdesc_re.hpp"
#include "./GTransForm/getroledesc.hpp"
#include "./GTransForm/getroledesc_re.hpp"
#include "./GTransForm/getroleequipmentdesc.hpp"
#include "./GTransForm/getroleequipmentdesc_re.hpp"
#include "./GTransForm/getrolepetcorraldesc.hpp"
#include "./GTransForm/getrolepetcorraldesc_re.hpp"
#include "./GTransForm/getroleskilldesc.hpp"
#include "./GTransForm/getroleskilldesc_re.hpp"
#include "./GTransForm/gwebtraderolebrief"
#include "./GTransForm/snsroleequipment"
#include "./GTransForm/snsrolepetcorral"
#include "./GTransForm/snsroleskills"

#include <locale.h>
#include <fstream>
#include <iostream>
#endif

HANDLE  g_hMutex = NULL;
const char *g_szMutex = "WMGJ_ElementHintTool";
const char * g_szLogFileName = "wmgj_HintTool.log";

#define new A_DEBUG_NEW          // For new & delete

using namespace GNET;

bool initInstance();
bool setup(void);
void clear();
void clearInstance();

#ifdef HINT_TOOL_DEBUG
bool getFileOctets(const char *szFile, GNET::Octets &data);
bool getOneDemandStub(GNET::GetItemDesc &in);
void putOneDemandStub(GNET::GetItemDesc_Re &out);
bool getOneDemandStub(GNET::GetRoleDesc &in);
void putOneDemandStub(GNET::GetRoleDesc_Re &out);
bool getOneDemandStub(GNET::GetRoleEquipmentDesc &in);
void putOneDemandStub(GNET::GetRoleEquipmentDesc_Re &out);
bool getOneDemandStub(GNET::GetRolePetCorralDesc &in);
void putOneDemandStub(GNET::GetRolePetCorralDesc_Re &out);
bool getOneDemandStub(GNET::GetRoleSkillDesc &in);
void putOneDemandStub(GNET::GetRoleSkillDesc_Re &out);
void putUnicode(const wchar_t *pszHint);
void putOctet(const Octets &data);

void converAllRoleFiles();
#endif

#ifdef HINT_TOOL_DEBUG
template<typename I, typename O>
void test(I, O)
{
	I input;
	O output;
	while (getOneDemandStub(input))
	{
		processOneDemand(input, output);
		putOneDemandStub(output);
	}
}
#endif

int main(int argc, char* argv[])
{
	if (!initInstance())
		return -1;

	if (setup())
	{
#ifdef HINT_TOOL_DEBUG
		printf("TEST equipment...\n");
		test(GNET::GetRoleEquipmentDesc(), GNET::GetRoleEquipmentDesc_Re());
		
		printf("TEST petcorral...\n");
		test(GNET::GetRolePetCorralDesc(), GNET::GetRolePetCorralDesc_Re());
		
		printf("TEST skill...\n");
		test(GNET::GetRoleSkillDesc(), GNET::GetRoleSkillDesc_Re());

		printf("TEST role...\n");
		test(GNET::GetRoleDesc(), GNET::GetRoleDesc_Re());

		printf("TEST item...\n");
		test(GNET::GetItemDesc(), GNET::GetItemDesc_Re());

		printf("TEST encrypted item...\n");
		test(GNET::GetEncryptedItemDesc(), GNET::GetEncryptedItemDesc_Re());
#else
		while (true)
		{
			Sleep(1000);
		}
#endif
	}
	
	Log_Info("ElementHintTool is to exit!");
	clear();
	clearInstance();
	return 0;
}

#ifdef HINT_TOOL_DEBUG
bool getFileOctets(const char *szFile, GNET::Octets &data)
{
	FILE *fp = fopen(szFile, "rb");
	if (!fp)
	{
		// 文件无法打开，返回 false
		return false;
	}

	// 获取字节流大小（即文件大小）
	fseek(fp, 0, SEEK_END);
	long len = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	// 读取所有文件内容作为字符流返回
	if (len>0)
	{
		data.resize(len);
		fread(data.begin(), sizeof(char), len, fp);
	}
	else
	{
		// 内容为空
		data.clear();
	}

	// 关闭文件
	fclose(fp);
	return true;
}

bool getOneDemandStub(GNET::GetItemDesc &data)
{
	// 从二进制文件中直接读取包裹内容
	// 返回 true 表示还可能有后续请求

	static int s_count;
	AString filePath;
	filePath.Format("a\\__%d", s_count++);
	return getFileOctets(filePath, data.detail);
}

bool getOneDemandStub(GNET::GetEncryptedItemDesc &data)
{
	// 从二进制文件中直接读取包裹内容
	// 返回 true 表示还可能有后续请求
	bool bRet = false;

	static int s_count;
	AString filePath;
	filePath.Format("a\\__%d", s_count++);
	GNET::Octets decryptedItem;
	if (getFileOctets(filePath, decryptedItem))
	{
		data.tid = 1234;

		char buf[256] = {0};

		GNET::Octets md5 = GNET::MD5Hash::Digest(decryptedItem);
		data.checksum.clear();
		data.checksum.reserve(md5.size()*2);
		const byte *p = NULL;
		for (p = (const byte *)md5.begin(); p != (const byte *)md5.end(); p++)
		{
			sprintf(buf, "%02x", *p);
			data.checksum.insert(data.checksum.end(), buf, 2);
		}

		byte key_buf[128] = {
			0xbf, 0x61, 0x09, 0x72, 0x1d, 0xbb, 0x6e, 0xe9, 0x9a, 0x01, 0x3d, 0x47, 0x0a, 0x11, 0x60, 0xbe, 
			0xf8, 0x48, 0x36, 0x3f, 0xdf, 0xf5, 0x9a, 0xc5, 0xca, 0x1c, 0x79, 0xcb, 0xbb, 0x90, 0x96, 0xde, 
			0x58, 0xff, 0x60, 0xbe, 0x8d, 0x4e, 0xd8, 0xe1, 0x83, 0x47, 0xd0, 0x57, 0x6d, 0x62, 0x49, 0x66, 
			0x71, 0x0d, 0x6a, 0xfe, 0x3b, 0xdc, 0xd2, 0x33, 0xd7, 0x8f, 0x5c, 0x5e, 0x4b, 0x86, 0x25, 0xd0, 
			0x6f, 0xd2, 0xd0, 0x8c, 0x53, 0x46, 0xd0, 0xc4, 0x3a, 0x10, 0xae, 0xcf, 0x75, 0xcd, 0xfd, 0x74,
			0xe8, 0xfe, 0x80, 0x0b, 0x14, 0xf9, 0x94, 0x66, 0xcc, 0x19, 0x7d, 0xda, 0x89, 0x27, 0x91, 0xab, 
			0x5c, 0x73, 0xb0, 0x78, 0xc9, 0x1c, 0x48, 0xd9, 0xe2, 0xc3, 0xfc, 0xfa, 0x3e, 0x74, 0xb9, 0xec, 
			0x81, 0x66, 0x5f, 0x5b, 0x3e, 0x73, 0x1e, 0xd2, 0x31, 0x99, 0xc7, 0xb5, 0xe7, 0x3e, 0xbc, 0x02,
		};

		ARCFourSecurity arc4;
		arc4.SetParameter(Octets(key_buf, sizeof(key_buf)/sizeof(key_buf[0])));
		GNET::Octets encryptedItem = arc4.Update(decryptedItem);

		data.encrypted_item.clear();
		data.encrypted_item.reserve(encryptedItem.size() * 2);
		for (p = (const byte *)encryptedItem.begin(); p != (const byte *)encryptedItem.end(); p++)
		{
			sprintf(buf, "%02x", *p);
			data.encrypted_item.insert(data.encrypted_item.end(), buf, 2);
		}

		bRet = true;
	}

	return bRet;
}

bool getOneDemandStub(GNET::GetRoleDesc &data)
{
	// 从二进制文件中直接读取角色内容
	// 返回 true 表示还可能有后续请求

	static int s_count;
	AString filePath;
	filePath.Format("b\\__%d", s_count++);
	return getFileOctets(filePath, data.detail);
}

bool getOneDemandStub(GNET::GetRoleEquipmentDesc &data)
{
	// 从二进制文件中直接读取装备内容
	// 返回 true 表示还可能有后续请求

	static int s_count;
	AString filePath;
	filePath.Format("b\\e\\__%d", s_count++);
	return getFileOctets(filePath, data.detail);
}

bool getOneDemandStub(GNET::GetRolePetCorralDesc &data)
{
	// 从二进制文件中直接读取宠物栏内容
	// 返回 true 表示还可能有后续请求

	static int s_count;
	AString filePath;
	filePath.Format("b\\p\\__%d", s_count++);
	return getFileOctets(filePath, data.detail);
}

bool getOneDemandStub(GNET::GetRoleSkillDesc &data)
{
	// 从二进制文件中直接读取宠物栏内容
	// 返回 true 表示还可能有后续请求

	static int s_count;
	AString filePath;
	filePath.Format("b\\s\\__%d", s_count++);
	return getFileOctets(filePath, data.detail);
}

void putOneDemandStub(GNET::GetItemDesc_Re &out)
{
	// 输出可视化结果：用于调试	
	if (out.retcode != RC_SUCCESS)
	{
		//	错误
		printf("Error code = %d\n\n", out.retcode);
		return;
	}

	// 读取名称
	{
		putUnicode(L"名称：");
		if (out.name.size()>0)
		{
			putOctet(out.name);
		}
		else
		{
			putUnicode(L"空");
		}
		printf("\n");
	}
	
	// 读取ID
	{
		putUnicode(L"ID：");
		printf("%d\n", out.id);
	}

	// 读取等级
	{
		putUnicode(L"等级：");
		printf("%d\n", out.level);
	}

	// 读取图标路径
	{
		putUnicode(L"图标：");
		if (out.icon.size()>0)
		{
			putOctet(out.icon);
		}
		else
		{
			putUnicode(L"空");
		}
		printf("\n");
	}
	// 读取物品描述信息
	{
		putOctet(out.desc);
	}
	
	// 输出一条信息后换行，以方便调试
	printf("\n");
	printf("\n");
}

void putOneDemandStub(GNET::GetEncryptedItemDesc_Re &out)
{
	// 输出可视化结果：用于调试	
	if (out.retcode != RC_SUCCESS)
	{
		//	错误
		printf("Error code = %d\n\n", out.retcode);
		return;
	}

	// 读取ID
	{
		putUnicode(L"tid：");
		printf("%d\n", out.tid);
	}
	// 读取物品描述信息
	{
		putOctet(out.desc);
	}

	// 输出一条信息后换行，以方便调试
	printf("\n");
	printf("\n");
}

void putOneDemandStub(GNET::GetRoleDesc_Re &out)
{
	//	输出一：输出到文件并显示
	openOctet(out.detail_desc);	
	return;

	//	输出二：输出到控制台（不直观）
	{
		putUnicode(L"职业：");
		printf("%d\n", out.cls);
		printf("\n");
	}
	{
		putUnicode(L"性别：");
		printf("%d\n", out.gender);
		printf("\n");
	}
	{
		putUnicode(L"等级：");
		printf("%d\n", out.level);
		printf("\n");
	}
	{
		putUnicode(L"名称：");
		if (out.name.size()>0)
		{
			putOctet(out.name);
		}
		else
		{
			putUnicode(L"空");
		}
		printf("\n");
	}
	{
		putUnicode(L"图标：");
		if (out.icon.size()>0)
		{
			putOctet(out.icon);
		}
		else
		{
			putUnicode(L"空");
		}
		printf("\n");
	}
	{
		putUnicode(L"基本描述：");
		if (out.basic_desc.size()>0)
		{
			putOctet(out.basic_desc);
		}
		else
		{
			putUnicode(L"空");
		}
		printf("\n");
	}
	{
		putUnicode(L"详细描述：");
		if (out.detail_desc.size()>0)
		{
			putOctet(out.detail_desc);
		}
		else
		{
			putUnicode(L"空");
		}
		printf("\n");
	}
}

void putOneDemandStub(GNET::GetRoleEquipmentDesc_Re &out)
{
	openOctet(out.desc);
}

void putOneDemandStub(GNET::GetRolePetCorralDesc_Re &out)
{
	openOctet(out.desc);
}

void putOneDemandStub(GNET::GetRoleSkillDesc_Re &out)
{
	openOctet(out.desc);
}

void putUnicode(const wchar_t *pszHint)
{
	AWString strHint = pszHint;
	int len = strHint.GetLength();
	int i(0);
	while (i<len)
	{
		wchar_t w1 = strHint[i];
		wprintf(L"%c", w1);

		// 在输出 /r 后换行，以方便调试查看
		if (w1==L'r')
		{
			if (i>0 && strHint[i-1]==L'\\')
			{
				wprintf(L"%c", L'\n');
			}
		}
		++i;
	}
}

void putOctet(const Octets &data)
{
	size_t len = data.size();
	if (!len)
		return;
	char *str = new char[len+1];
	memcpy(str, data.begin(), len);
	str[len] = '\0';
	putUnicode(UTF8ToUnicode(str));
	delete []str;
}

#endif

bool setup()
{
	// 初始化多线程访问控制
	initMultiThreadAccess();

	// 在控制台输出中文用于调试
#ifdef ANGELICA_2_2
	setlocale(LC_ALL, "");
#endif

	af_Initialize();
	glb_InitLogSystem(g_szLogFileName);
	
	//	Set current directory as work directory
	GetCurrentDirectoryA(MAX_PATH, g_szWorkDir);
	af_SetBaseDir(g_szWorkDir);
	g_AFilePackMan.OpenFilePackage("configs.pck");
	
	// 资源加载
	if (!g_pGame->Init())
		return false;	
	
	// 初始化网络
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	
	wVersionRequested = MAKEWORD( 2, 2 );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		return 0;
	}
	
	if ( LOBYTE( wsaData.wVersion ) != 2 ||	HIBYTE( wsaData.wVersion ) != 2 ) 
	{
		WSACleanup();
		return 0; 
	}
	
	AString strNetworkConfig = g_szWorkDir;
	strNetworkConfig += "\\client.conf";
	if (_access(strNetworkConfig, R_OK) == 0)
	{
		Conf *conf = Conf::GetInstance(strNetworkConfig);
		GTransformClient *manager = GTransformClient::GetInstance();
		manager->SetAccumulate(atoi(conf->find(manager->Identification(), "accumulate").c_str()));
		Protocol::Client(manager);
	}
	else
	{
		Log_Info("Cannot read file %s", (const char *)strNetworkConfig);
		return false;
	}
	
	// 资源加载完成后，启动侦听线程
	GNET::PollIO::Init();
	return true;
}

void clear()
{
	// 停止网络
	GNET::PollIO::Close();
	Sleep(1000);
	WSACleanup();

	// 资源清空
	g_pGame->Release();
	
	//	Close log system
	glb_CloseLogSystem();
	
	af_Finalize();

	// 删除多线程访问控制资源
	destroyMultiThreadAccess();
}

bool initInstance()
{
	//	检查是否有多个实例运行
	g_hMutex = CreateMutexA(NULL, TRUE, g_szMutex);
	DWORD dwLastError = GetLastError();
	if (dwLastError == ERROR_ALREADY_EXISTS)
	{
		//	已经存在一个实例
		printf("An ElementHintTool instance already exists!");
		CloseHandle(g_hMutex);
		return false;
	}
	if (!g_hMutex)
	{
		printf("Create instance mutex failed!");
		return false;
	}
	return true;
}

void clearInstance()
{
	if (g_hMutex)
	{
		CloseHandle(g_hMutex);
		g_hMutex = NULL;
	}
}

#ifdef HINT_TOOL_DEBUG
//-----------------------------------------------------------
//	临时转换函数，将寻宝网角色数据分拆为SNS测试所需装备、技能、宠物栏数据
void convertRoleFile(const char *szRoleFile, const char *szEquipFile, const char *szSkillFile, const char *szPetFile)
{
	while (true)
	{
		GetRoleDesc roledesc;
		if (!getFileOctets(szRoleFile, roledesc.detail))
			break;		
		
		GWebTradeRoleBrief role;
		try
		{
			int id(0);
			int category(0);
			Marshal::OctetsStream(roledesc.detail) >> id >> category >>  role;
		}catch (...)
		{
			//	文件中的数据有错，无法转换为合法格式的内容
			break;
		}

		//	写数据
		Marshal::OctetsStream os;

		//	准备装备数据
		SNSRoleEquipment sns_equip;
		sns_equip.equipment = role.equipment;
		os.clear();
		os << sns_equip;
		//	写装备到文件
		putOctetToFile(os, szEquipFile, true);

		//	准备技能数据
		SNSRoleSkills sns_skills;
		sns_skills.skills = role.skills;
		os.clear();
		os << sns_skills;
		//	写技能到文件
		putOctetToFile(os, szSkillFile, true);

		//	准备宠物数据
		SNSRolePetCorral sns_petcorral;
		sns_petcorral.petcorral = role.petcorral;
		os.clear();
		os << sns_petcorral;
		//	写宠物到文件
		putOctetToFile(os, szPetFile, true);

		break;
	}
}

//	临时转换函数，遍历并转换所有角色数据
void converAllRoleFiles()
{
	AString strRolePath = "b\\";
	WIN32_FIND_DATAA fd;
	HANDLE hFind = ::FindFirstFileA(strRolePath + "*.*", &fd);
	if (hFind != INVALID_HANDLE_VALUE)
	{
		//	创建转换后的数据目录
		AString strEquipPath = "b\\e\\";
		AString strSkillPath = "b\\s\\";
		AString strPetPath = "b\\p\\";

		::CreateDirectoryA(strEquipPath, NULL);
		::CreateDirectoryA(strSkillPath, NULL);
		::CreateDirectoryA(strPetPath, NULL);

		while (true)
		{
			if (fd.dwFileAttributes == FILE_ATTRIBUTE_ARCHIVE)
				convertRoleFile(strRolePath+fd.cFileName, strEquipPath+fd.cFileName, strSkillPath+fd.cFileName, strPetPath+fd.cFileName);
			if (!::FindNextFileA(hFind, &fd))
				break;
		}
		::FindClose(hFind);
	}
}
#endif
