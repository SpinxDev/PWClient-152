// ElementHintToolServer.cpp : Defines the entry point for the console application.
//

#include <stdio.h>
#include <windows.h>   // For GRoleInventory
#include <gnoctets.h>  // For Octets
#include <AMemory.h>   // For A_DEBUG_NEW
#include <AFI.h>       // For af_Initialize af_Finalize
#include <AFilePackage.h>
#include <AString.h>
#include "GTransform/gtransformserver.hpp"
#include <groleinventory> // For GRoleInventory
#include "./GTransform/getitemdesc.hpp"

#include <locale.h>
#include <iostream>

#define new A_DEBUG_NEW          // For new & delete

using namespace GNET;

int setup();

int main(int argc, char* argv[])
{
	setup();

	printf("请输入回车开始与小工具通信测试\n");
	int id(0);
	while (std::cin >> id)
	{
		GRoleInventory item(id,0,1);
		Marshal::OctetsStream os;
		os << item;
		GetItemDesc p(1, 2, os);
		bool send = GTransformServer::GetInstance()->SendProtocol(p);
		if (!send)
			std::cout << "发送失败，请先启动小工具...\n";
	}
	return 0;
}

int setup()
{
	// 在控制台输出中文用于调试
	setlocale(LC_ALL, "");

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

	char szWorkDir[MAX_PATH] = {0};
	GetCurrentDirectoryA(MAX_PATH, szWorkDir);
	AString strNetworkConfig = szWorkDir;
	strNetworkConfig += "\\server.conf";
	if (_access(strNetworkConfig, R_OK) == 0)
	{
		Conf *conf = Conf::GetInstance(strNetworkConfig);
		GTransformServer *manager = GTransformServer::GetInstance();
		manager->SetAccumulate(atoi(conf->find(manager->Identification(), "accumulate").c_str()));
		Protocol::Server(manager);
	}
	else
	{
		printf("Cannot read file %s\n", (const char *)strNetworkConfig);
		return false;
	}
	
	GNET::PollIO::Init();
	return 1;
}

