/*
 * FILE: EC_BBSFileDownloader.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: XiaoZhou, 2005/6/27
 *
 * HISTORY: 
 *
 * Copyright (c) 2004 Archosaur Studio, All Rights Reserved.
 */

#include "AM.h"
#include "Wininet.h"

#ifndef EC_BBSFILEDOWNLOADER_H
#define EC_BBSFILEDOWNLOADER_H

enum ContentType{
	CT_APPLICATION,
	CT_MULTIPART,
};

enum Error_code{
	ERR_URL_PARSE   = 1,    // URL 解析错误
	ERR_GET_ADDR    = 2,    // 无法解析服务器地址 
	ERR_OVERFLOW    = 3,    // 写文件越界
	ERR_CREATE_FILE = 4,    // 无法创建文件
	ERR_REQ_DENIED  = 5,	// 服务器拒绝请求
	ERR_CONNECTION  = 6,    // 网络连接错误
	ERR_QUARYINFO	= 7,    // 网络连接错误
	ERR_UNKNOWN     = 8,    // 俺也不知道
};

#define SESSION_FLAG   INTERNET_FLAG_EXISTING_CONNECT | INTERNET_FLAG_NO_AUTO_REDIRECT \
						| INTERNET_FLAG_RELOAD | INTERNET_FLAG_DONT_CACHE

class CECBBSFileDownloader
{

public:
	CECBBSFileDownloader();
	~CECBBSFileDownloader();
	bool DownloadByUrl(LPCSTR url, LPCSTR localfn, LPCSTR postdata = NULL, int postlen = 0,
						int contenttype = CT_APPLICATION);
	DWORD Connect();
	DWORD Redirect();
	bool Run();
	void Read();
private:
	HINTERNET	m_hInternet;
	HINTERNET	m_hHttpSession;
	HINTERNET	m_hRequest;
	char		*m_pPostData;
	int			m_nPostLength;
	int			m_iContantType;
	AString		m_aUrl;
	AString		m_aServer;
	AString		m_aFileName;
	AString		m_aLocalfn;
	AString		m_aMethod;
};

extern const AString MultiPartBoundary;

#endif