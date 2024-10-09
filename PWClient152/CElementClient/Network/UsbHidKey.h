// UsbHidKey.h: interface for the CUsbHidKey class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_USBHIDKEY_H__097E3EB8_BE96_44A1_B8AA_F3992282778A__INCLUDED_)
#define AFX_USBHIDKEY_H__097E3EB8_BE96_44A1_B8AA_F3992282778A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

#include <string>
#include <list>

namespace HIDUSBKEY {

typedef struct _HIDP_PREPARSED_DATA* PHIDP_PREPARSED_DATA;

typedef struct _HIDD_ATTRIBUTES {
    ULONG   Size; // = sizeof (struct _HIDD_ATTRIBUTES)
    USHORT  VendorID;
    USHORT  ProductID;
    USHORT  VersionNumber;
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;


typedef void (__stdcall *pFuncHidD_GetHidGuid)( LPGUID HidGuid);
typedef BOOLEAN (__stdcall *pFuncHidD_GetPreparsedData)( HANDLE HidDeviceObject, PHIDP_PREPARSED_DATA* PreparsedData);
typedef BOOLEAN (__stdcall *pFuncHidD_FreePreparsedData)( PHIDP_PREPARSED_DATA PreparsedData);
typedef BOOLEAN (__stdcall *pFuncHidD_GetAttributes)( HANDLE HidDeviceObject, PHIDD_ATTRIBUTES Attributes);
typedef BOOLEAN (__stdcall *pFuncHidD_GetProductString)( HANDLE HidDeviceObject, PVOID Buffer, ULONG BufferLength);
typedef BOOLEAN (__stdcall *pFuncHidD_SetFeature)( HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength);
typedef BOOLEAN (__stdcall *pFuncHidD_GetFeature)( HANDLE HidDeviceObject, PVOID ReportBuffer, ULONG ReportBufferLength);

#define  HID_DECLARE_MEMBER( name) pFuncHidD_##name	m_p##name
#define	 HID_LOADDLL_MENBER( name) m_p##name = (pFuncHidD_##name)GetProcAddress( m_hDll, "HidD_"#name)

class CHidFunc  
{
	HINSTANCE	m_hDll;
public:
	CHidFunc()
	{
		const wchar_t* const szDllName = L"hid.dll";
		m_hDll = LoadLibraryW( szDllName);
		
		HID_LOADDLL_MENBER( GetHidGuid);
		HID_LOADDLL_MENBER( GetPreparsedData);
		HID_LOADDLL_MENBER( FreePreparsedData);
		HID_LOADDLL_MENBER( GetAttributes);
		HID_LOADDLL_MENBER( GetProductString);
		HID_LOADDLL_MENBER( SetFeature);
		HID_LOADDLL_MENBER( GetFeature);
	}
	~CHidFunc()
	{
		FreeLibrary( m_hDll);
	}
private:
	CHidFunc( const CHidFunc& src)
	{
	}
	CHidFunc& operator=( const CHidFunc& src)
	{
		return *this;
	}
public:
	bool IsReady() const
	{
		if( NULL == m_hDll)
			return false;
		if( NULL == m_pGetHidGuid)
			return false;
		if( NULL == m_pGetPreparsedData)
			return false;
		if( NULL == m_pFreePreparsedData)
			return false;
		if( NULL == m_pGetAttributes)
			return false;
		if( NULL == m_pGetProductString)
			return false;
		if( NULL == m_pSetFeature)
			return false;
		if( NULL == m_pGetFeature)
			return false;
		return true;
	}
public:
	HID_DECLARE_MEMBER( GetPreparsedData);		// m_pGetPreparsedData
	HID_DECLARE_MEMBER( FreePreparsedData);		// m_pFreePreparsedData
	HID_DECLARE_MEMBER( GetAttributes);			// m_pGetAttributes
	HID_DECLARE_MEMBER( GetProductString);		// m_pGetProductString
	HID_DECLARE_MEMBER( SetFeature);			// m_pSetFeature
	HID_DECLARE_MEMBER( GetFeature);			// m_pGetFeature
	HID_DECLARE_MEMBER( GetHidGuid);			// m_pGetHidGuid
};

///////////////////////////////////////////////////////////////////////////////////////////////

typedef PVOID HDEVINFO;

typedef struct _SP_DEVINFO_DATA {
    DWORD cbSize;
    GUID  ClassGuid;
    DWORD DevInst;    // DEVINST handle
    ULONG_PTR Reserved;
} SP_DEVINFO_DATA, *PSP_DEVINFO_DATA;

typedef struct _SP_DEVICE_INTERFACE_DATA {
    DWORD cbSize;
    GUID  InterfaceClassGuid;
    DWORD Flags;
    ULONG_PTR Reserved;
} SP_DEVICE_INTERFACE_DATA, *PSP_DEVICE_INTERFACE_DATA;

typedef struct _SP_DEVICE_INTERFACE_DETAIL_DATA_A {
	enum
	{
		SIZE_CB = 5,
	};
    DWORD  cbSize;
    char   DevicePath[ANYSIZE_ARRAY];
} SP_DEVICE_INTERFACE_DETAIL_DATA_A, *PSP_DEVICE_INTERFACE_DETAIL_DATA_A;

typedef BOOL (WINAPI *pFuncSetupDiDestroyDeviceInfoList)( HDEVINFO DeviceInfoSet);
typedef HDEVINFO (WINAPI *pFuncSetupDiGetClassDevsA)( GUID *ClassGuid, PCSTR Enumerator, HWND hwndParent, DWORD Flags);
typedef BOOL (WINAPI *pFuncSetupDiEnumDeviceInterfaces)( HDEVINFO DeviceInfoSet, PSP_DEVINFO_DATA DeviceInfoData, GUID *InterfaceClassGuid, DWORD MemberIndex, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData);
typedef BOOL (WINAPI *pFuncSetupDiGetDeviceInterfaceDetailA)( HDEVINFO DeviceInfoSet, PSP_DEVICE_INTERFACE_DATA DeviceInterfaceData, PSP_DEVICE_INTERFACE_DETAIL_DATA_A DeviceInterfaceDetailData, DWORD DeviceInterfaceDetailDataSize, PDWORD RequiredSize, PSP_DEVINFO_DATA DeviceInfoData);

enum
{
	DIGCF_PRESENT			= 0x00000002,
		DIGCF_DEVICEINTERFACE	= 0x00000010,
};

#define  SETUP_DECLARE_MEMBER( name) pFuncSetupDi##name	m_p##name
#define	 SETUP_LOADDLL_MENBER( name) m_p##name = (pFuncSetupDi##name)GetProcAddress( m_hDll, "SetupDi"#name)

class CSetupFunc
{
	HINSTANCE	m_hDll;
public:
	CSetupFunc()
	{
		const wchar_t* const szDllName = L"SETUPAPI.DLL";
		m_hDll = LoadLibraryW( szDllName);
		
		SETUP_LOADDLL_MENBER( DestroyDeviceInfoList);	
		SETUP_LOADDLL_MENBER( GetClassDevsA);					
		SETUP_LOADDLL_MENBER( EnumDeviceInterfaces);	
		SETUP_LOADDLL_MENBER( GetDeviceInterfaceDetailA);
	}
	~CSetupFunc()
	{
		FreeLibrary( m_hDll);
	}
private:
	CSetupFunc( const CSetupFunc& src)
	{
	}
	CSetupFunc& operator=( const CSetupFunc& src)
	{
		return *this;
	}
public:
	bool IsReady() const
	{
		if( NULL == m_hDll)
			return false;
		if( NULL == m_pDestroyDeviceInfoList)
			return false;
		if( NULL == m_pGetClassDevsA)
			return false;
		if( NULL == m_pEnumDeviceInterfaces)
			return false;
		if( NULL == m_pGetDeviceInterfaceDetailA)
			return false;
		return true;
	}
public:
	SETUP_DECLARE_MEMBER( DestroyDeviceInfoList);			// m_pDestroyDeviceInfoList
	SETUP_DECLARE_MEMBER( GetClassDevsA);					// m_pGetClassDevsA
	SETUP_DECLARE_MEMBER( EnumDeviceInterfaces);			// m_pEnumDeviceInterfaces
	SETUP_DECLARE_MEMBER( GetDeviceInterfaceDetailA);		// m_pGetDeviceInterfaceDetailA
};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class MD5Hash
{
	unsigned int state[4];
	unsigned int low_count, high_count, remain;
	unsigned char buffer[64];
	
	void transform (const unsigned char block[64])
	{
		#define f(x, y, z) (z ^ (x & (y ^ z)))
		#define g(x, y, z) (y ^ (z & (x ^ y)))
		#define h(x, y, z) ((x) ^ (y) ^ (z))
		#define i(x, y, z) ((y) ^ ((x) | (~z)))
		#define rotate_left(x, n) (((x) << (n)) | ((x) >> (32-(n))))

		unsigned int a = state[0], b = state[1], c = state[2], d = state[3];
		unsigned int *x = (unsigned int *)block;

		/* round 1 */
		#define ff(a, b, c, d, x, s, ac) { \
			(a) += f ((b), (c), (d)) + (x) + (unsigned int)(ac); \
			(a) = rotate_left ((a), (s)); \
			(a) += (b); \
		}
		ff (a, b, c, d, x[ 0],   7, 0xd76aa478); /* 1 */
		ff (d, a, b, c, x[ 1],  12, 0xe8c7b756); /* 2 */
		ff (c, d, a, b, x[ 2],  17, 0x242070db); /* 3 */
		ff (b, c, d, a, x[ 3],  22, 0xc1bdceee); /* 4 */
		ff (a, b, c, d, x[ 4],   7, 0xf57c0faf); /* 5 */
		ff (d, a, b, c, x[ 5],  12, 0x4787c62a); /* 6 */
		ff (c, d, a, b, x[ 6],  17, 0xa8304613); /* 7 */
		ff (b, c, d, a, x[ 7],  22, 0xfd469501); /* 8 */
		ff (a, b, c, d, x[ 8],   7, 0x698098d8); /* 9 */
		ff (d, a, b, c, x[ 9],  12, 0x8b44f7af); /* 10 */
		ff (c, d, a, b, x[10],  17, 0xffff5bb1); /* 11 */
		ff (b, c, d, a, x[11],  22, 0x895cd7be); /* 12 */
		ff (a, b, c, d, x[12],   7, 0x6b901122); /* 13 */
		ff (d, a, b, c, x[13],  12, 0xfd987193); /* 14 */
		ff (c, d, a, b, x[14],  17, 0xa679438e); /* 15 */
		ff (b, c, d, a, x[15],  22, 0x49b40821); /* 16 */
		#undef ff

		/* round 2 */
		#define gg(a, b, c, d, x, s, ac) { \
			(a) += g ((b), (c), (d)) + (x) + (unsigned int)(ac); \
			(a) = rotate_left ((a), (s)); \
			(a) += (b); \
		}
		gg (a, b, c, d, x[ 1],   5, 0xf61e2562); /* 17 */
		gg (d, a, b, c, x[ 6],   9, 0xc040b340); /* 18 */
		gg (c, d, a, b, x[11],  14, 0x265e5a51); /* 19 */
		gg (b, c, d, a, x[ 0],  20, 0xe9b6c7aa); /* 20 */
		gg (a, b, c, d, x[ 5],   5, 0xd62f105d); /* 21 */
		gg (d, a, b, c, x[10],   9,  0x2441453); /* 22 */
		gg (c, d, a, b, x[15],  14, 0xd8a1e681); /* 23 */
		gg (b, c, d, a, x[ 4],  20, 0xe7d3fbc8); /* 24 */
		gg (a, b, c, d, x[ 9],   5, 0x21e1cde6); /* 25 */
		gg (d, a, b, c, x[14],   9, 0xc33707d6); /* 26 */
		gg (c, d, a, b, x[ 3],  14, 0xf4d50d87); /* 27 */
		gg (b, c, d, a, x[ 8],  20, 0x455a14ed); /* 28 */
		gg (a, b, c, d, x[13],   5, 0xa9e3e905); /* 29 */
		gg (d, a, b, c, x[ 2],   9, 0xfcefa3f8); /* 30 */
		gg (c, d, a, b, x[ 7],  14, 0x676f02d9); /* 31 */
		gg (b, c, d, a, x[12],  20, 0x8d2a4c8a); /* 32 */
		#undef gg 
	
		/* round 3 */
		#define hh(a, b, c, d, x, s, ac) { \
			(a) += h ((b), (c), (d)) + (x) + (unsigned int)(ac); \
			(a) = rotate_left ((a), (s)); \
			(a) += (b); \
		}
		hh (a, b, c, d, x[ 5],   4, 0xfffa3942); /* 33 */
		hh (d, a, b, c, x[ 8],  11, 0x8771f681); /* 34 */
		hh (c, d, a, b, x[11],  16, 0x6d9d6122); /* 35 */
		hh (b, c, d, a, x[14],  23, 0xfde5380c); /* 36 */
		hh (a, b, c, d, x[ 1],   4, 0xa4beea44); /* 37 */
		hh (d, a, b, c, x[ 4],  11, 0x4bdecfa9); /* 38 */
		hh (c, d, a, b, x[ 7],  16, 0xf6bb4b60); /* 39 */
		hh (b, c, d, a, x[10],  23, 0xbebfbc70); /* 40 */
		hh (a, b, c, d, x[13],   4, 0x289b7ec6); /* 41 */
		hh (d, a, b, c, x[ 0],  11, 0xeaa127fa); /* 42 */
		hh (c, d, a, b, x[ 3],  16, 0xd4ef3085); /* 43 */
		hh (b, c, d, a, x[ 6],  23,  0x4881d05); /* 44 */
		hh (a, b, c, d, x[ 9],   4, 0xd9d4d039); /* 45 */
		hh (d, a, b, c, x[12],  11, 0xe6db99e5); /* 46 */
		hh (c, d, a, b, x[15],  16, 0x1fa27cf8); /* 47 */
		hh (b, c, d, a, x[ 2],  23, 0xc4ac5665); /* 48 */
		#undef hh 

		/* round 4 */
		#define ii(a, b, c, d, x, s, ac) { \
			(a) += i ((b), (c), (d)) + (x) + (unsigned int)(ac); \
			(a) = rotate_left ((a), (s)); \
			(a) += (b); \
		}
		ii (a, b, c, d, x[ 0],   6, 0xf4292244); /* 49 */
		ii (d, a, b, c, x[ 7],  10, 0x432aff97); /* 50 */
		ii (c, d, a, b, x[14],  15, 0xab9423a7); /* 51 */
		ii (b, c, d, a, x[ 5],  21, 0xfc93a039); /* 52 */
		ii (a, b, c, d, x[12],   6, 0x655b59c3); /* 53 */
		ii (d, a, b, c, x[ 3],  10, 0x8f0ccc92); /* 54 */
		ii (c, d, a, b, x[10],  15, 0xffeff47d); /* 55 */
		ii (b, c, d, a, x[ 1],  21, 0x85845dd1); /* 56 */
		ii (a, b, c, d, x[ 8],   6, 0x6fa87e4f); /* 57 */
		ii (d, a, b, c, x[15],  10, 0xfe2ce6e0); /* 58 */
		ii (c, d, a, b, x[ 6],  15, 0xa3014314); /* 59 */
		ii (b, c, d, a, x[13],  21, 0x4e0811a1); /* 60 */
		ii (a, b, c, d, x[ 4],   6, 0xf7537e82); /* 61 */
		ii (d, a, b, c, x[11],  10, 0xbd3af235); /* 62 */
		ii (c, d, a, b, x[ 2],  15, 0x2ad7d2bb); /* 63 */
		ii (b, c, d, a, x[ 9],  21, 0xeb86d391); /* 64 */
		#undef ii 
		#undef rotate_left 
		#undef i
		#undef h
		#undef g
		#undef f
		state[0] += a;
		state[1] += b;
		state[2] += c;
		state[3] += d;
	}
	void update( const unsigned char *input, unsigned int inputlen)
	{
		if ( remain )
		{
			unsigned int copy_len = 64 - remain;
			if ( inputlen < copy_len )
			{
				memcpy( buffer + remain, input, inputlen );
				remain += inputlen;
				return;
			}
			memcpy(buffer + remain, input, copy_len);
			transform( buffer );
			inputlen -= copy_len;
			input    += copy_len;
			if ( (low_count += 512) == 0 ) high_count++;
		}
		for ( ;inputlen >= 64 ; inputlen -= 64, input += 64)
		{
			transform( input );
			if ( (low_count += 512) == 0 ) high_count++;
		}
		if ( (remain = inputlen) > 0 ) memcpy ( buffer, input, remain );
	}
	void init()
	{
		state[0] = 0x67452301;
		state[1] = 0xefcdab89;
		state[2] = 0x98badcfe;
		state[3] = 0x10325476;
		low_count = high_count = remain = 0;
	}
public:
	MD5Hash() { init(); }
	void Update( const unsigned char *input, unsigned int inputlen)
	{
		update( input, inputlen);
	}
	const unsigned char * Final()
	{
		static unsigned char padding[64] = {
			0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
		};
		unsigned int l = low_count, h = high_count;
		if ( (l += remain * 8) < l ) h++;
		unsigned int index = remain  & 0x3f;
		unsigned int padlen = (index < 56) ? (56 - index) : (120 - index);
		update (padding, padlen);
		update ((const unsigned char *)&l, sizeof(l));
		update ((const unsigned char *)&h, sizeof(h));

		return (const unsigned char *)state;
	}

	static void Digest( const unsigned char *input, unsigned int inputlen, unsigned char* szOut)
	{
		MD5Hash ctx;
		ctx.update( input, inputlen);
		memcpy( szOut, ctx.Final(), 16);
	}

	const unsigned char* GetState() const
	{
		return (const unsigned char *)state;
	}
};

class HMAC_MD5Hash 
{
	unsigned char k_opad[64];
	unsigned char Output[16];
	MD5Hash md5hash;
public:
	HMAC_MD5Hash(){ }
	void SetParameter( const unsigned char* szKey, size_t nSize)
	{
		unsigned char k_ipad[64];
		unsigned char *ipad = k_ipad;
		unsigned char *opad = k_opad;

		if ( nSize > 64)
		{
			unsigned char	Output[16];

			MD5Hash::Digest( szKey, nSize, Output);

			memcpy( ipad, Output, sizeof(Output));
			memcpy( opad, Output, sizeof(Output));
			nSize = sizeof(Output);
		}
		else
		{
			memcpy( ipad, szKey, nSize);
			memcpy( opad, szKey, nSize);
		}
		
		for(size_t i = 0; i < nSize; i++) 
		{ 
			ipad[i] ^= 0x36; opad[i] ^= 0x5c; 
		}

		memset( ipad + nSize, 0x36, 64 - nSize);
		memset( opad + nSize, 0x5c, 64 - nSize);
		
		md5hash.Update( k_ipad, 64);
	}
	void Update( const unsigned char *input, unsigned int inputlen) 
	{ 
		md5hash.Update( input, inputlen); 
	}
	const unsigned char * Final()
	{
		MD5Hash ctx;
		ctx.Update( k_opad, 64);
		ctx.Update( md5hash.Final(), 16);
		memcpy( Output, ctx.Final(), 16);
		return Output;
	}
	const unsigned char * GetResult()
	{
		return Output;
	}
	static void Partial_HMAC_Md5_init_key( const unsigned char* pKey, size_t nSize, unsigned char* ipadkey, unsigned char* opadkey)
	{
		unsigned char ipad[64];
		unsigned char opad[64];
		
		if ( nSize > 64)
		{
			unsigned char	Output[16];
			
			MD5Hash::Digest( pKey, nSize, Output);
			
			memcpy( ipad, Output, sizeof(Output));
			memcpy( opad, Output, sizeof(Output));
			nSize = sizeof(Output);
		}
		else
		{
			memcpy( ipad, pKey, nSize);
			memcpy( opad, pKey, nSize);
		}
		
		for(size_t i = 0; i < nSize; i++) 
		{ 
			ipad[i] ^= 0x36; opad[i] ^= 0x5c; 
		}
		
		memset( ipad + nSize, 0x36, 64 - nSize);
		memset( opad + nSize, 0x5c, 64 - nSize);

		{
			MD5Hash ctx;

			ctx.Update( ipad, sizeof(ipad));
			memcpy( ipadkey, ctx.GetState(), 16);
		}

		{
			MD5Hash ctx;

			ctx.Update( opad, sizeof(opad));
			memcpy( opadkey, ctx.GetState(), 16);
		}
	}
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class CHidDevice
{
public:
	HANDLE					m_hDevice;
	PHIDP_PREPARSED_DATA	m_pPData;
	CHidFunc&				m_HidFunc;
public:
	CHidDevice( CHidFunc& Func)
		: m_HidFunc( Func), m_hDevice( NULL), m_pPData( NULL)
	{
	}
	~CHidDevice()
	{
		if( m_pPData)
			m_HidFunc.m_pFreePreparsedData( m_pPData);
		if( m_hDevice)
			CloseHandle( m_hDevice);
	}
public:
	typedef std::list<std::string> CPathList;
private:
	class CDeviceInfo
	{
		CSetupFunc&		m_SetupFunc;
	public:
		HDEVINFO		m_hInfo;
	public:
		CDeviceInfo( CSetupFunc& SetupFunc)
			: m_SetupFunc( SetupFunc), m_hInfo( NULL)
		{
		}
		~CDeviceInfo()
		{
			if( m_hInfo && INVALID_HANDLE_VALUE != m_hInfo)
				m_SetupFunc.m_pDestroyDeviceInfoList( m_hInfo);
		}
	};
	class CDetailData
	{
		char*	m_pData;
	public:
		CDetailData()
			: m_pData( NULL)
		{
		}
		~CDetailData()
		{
			if( m_pData)
				delete[] m_pData;
		}
	public:
		void Alloc( DWORD dwSize)
		{
			if( m_pData)
				delete[] m_pData;
			m_pData = new char[dwSize];
			
			((PSP_DEVICE_INTERFACE_DETAIL_DATA_A)m_pData)->cbSize = SP_DEVICE_INTERFACE_DETAIL_DATA_A::SIZE_CB;
		}
		PSP_DEVICE_INTERFACE_DETAIL_DATA_A GetBuffer()
		{
			return (PSP_DEVICE_INTERFACE_DETAIL_DATA_A)m_pData;
		}
		const char* GetPath()
		{
			return ((PSP_DEVICE_INTERFACE_DETAIL_DATA_A)m_pData)->DevicePath;
		}
	};
public:	
	static bool EnumHidDeives( CPathList& PathList, CHidFunc& Func, CSetupFunc& SetupFunc)
	{
		CDeviceInfo		DeviceInfo( SetupFunc);
		GUID			guid;
		
		Func.m_pGetHidGuid( &guid);
		
		DeviceInfo.m_hInfo = SetupFunc.m_pGetClassDevsA( &guid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
		if( INVALID_HANDLE_VALUE == DeviceInfo.m_hInfo)
			return false;
		
		DWORD	dwLastError;
		DWORD	dwNeedSize;
		
		SP_DEVICE_INTERFACE_DATA		spdid;
		spdid.cbSize = sizeof( SP_DEVICE_INTERFACE_DATA);
		
		for( DWORD i = 0; ; i ++)
		{
			BOOL bResult = SetupFunc.m_pEnumDeviceInterfaces( DeviceInfo.m_hInfo, NULL, &guid, i, &spdid);
			dwLastError = GetLastError();
			if( !bResult && ERROR_NO_MORE_ITEMS == dwLastError)
				break;
			
			SetupFunc.m_pGetDeviceInterfaceDetailA( DeviceInfo.m_hInfo, &spdid, NULL, NULL, &dwNeedSize, NULL);
			dwLastError = GetLastError();
			if( ERROR_INSUFFICIENT_BUFFER != dwLastError)
				continue;
			
			CDetailData DetailData;
			
			DetailData.Alloc( dwNeedSize);

			bResult = SetupFunc.m_pGetDeviceInterfaceDetailA( DeviceInfo.m_hInfo, &spdid, DetailData.GetBuffer(), dwNeedSize, NULL, NULL);
  			if( bResult)
				PathList.push_back( DetailData.GetPath());
		}
		return true;
	}
public:
	void Close()
	{
		if( m_pPData)
		{
			m_HidFunc.m_pFreePreparsedData( m_pPData);
			m_pPData = NULL;
		}
		if( m_hDevice)
		{
			CloseHandle( m_hDevice);
			m_hDevice = NULL;
		}
	}
	bool Open( const char* szPath)
	{
		Close();

		m_hDevice = CreateFileA( szPath, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
		if( INVALID_HANDLE_VALUE == m_hDevice)
		{
			m_hDevice = NULL;
			return false;
		}

		return TRUE == m_HidFunc.m_pGetPreparsedData( m_hDevice, &m_pPData);
	}
	operator HANDLE() const
	{
		return m_hDevice;
	}
	operator PHIDP_PREPARSED_DATA() const
	{
		return m_pPData;
	}
};

class CUsbHidKeyFinder 
{
	CHidFunc&				m_HidFunc;
	CSetupFunc&				m_SetupFunc;
	CHidDevice::CPathList	m_Paths;	
public:
	CUsbHidKeyFinder( CHidFunc& Func, CSetupFunc& SetupFunc)
		: m_HidFunc( Func), m_SetupFunc( SetupFunc)
	{
	}
	virtual ~CUsbHidKeyFinder()
	{
	}
public:
	void FindKeys()
	{
		const int nBufferSize = 256;
		char	szBuffer[nBufferSize];
		HIDD_ATTRIBUTES		ha;

		CHidDevice::CPathList	TempPath;
	
		m_Paths.clear();

		if( !CHidDevice::EnumHidDeives( TempPath, m_HidFunc, m_SetupFunc))
			return;
		
		for( CHidDevice::CPathList::iterator it = TempPath.begin(); it != TempPath.end(); ++ it)
		{
			const std::string& strPath = *it;

			CHidDevice		device( m_HidFunc);
			
			if( !device.Open( strPath.c_str()))
				continue;

			ha.Size = sizeof(HIDD_ATTRIBUTES);
			if( !m_HidFunc.m_pGetAttributes( device, &ha))
				continue;
			
			if( 0x0111 != ha.VendorID || 0x0111 != ha.ProductID)
				continue;

			if( !m_HidFunc.m_pGetProductString( device, szBuffer, nBufferSize))
				continue;
			if( 0 != wcsicmp( (const wchar_t*)szBuffer, L"www.wanmei.com"))
				continue;
			
			device.Close();
			m_Paths.push_back( strPath);
		} //for( CHidDevice::CPathList::iterator it = TempPath.begin(); it != TempPath.end(); ++ it)
	}
	int GetKeyCount() const
	{
		return m_Paths.size();
	}
	const char* GetFirstKeyPath() const
	{
		if( m_Paths.empty())
			return NULL;
		const std::string& strPath = *m_Paths.begin();
		return strPath.c_str();
	}
};

class CUsbHidKey
{
	enum
	{
		SIZE_FEATURE		= 0x41,
		SIZE_SERIALNUMBER	= 0x08,
		SIZE_HMAC_KEY		= 0x10,
		
		FEATURE_IO_ENDOUT	= 0x2E,
		FEATURE_IO_ENDIN	= 0xD3,
		FEATURE_IO_UNKNOW	= 0x00,

		FTDATA_CMD_INFO		= 0x01,
		FTDATA_CMD_VERIFY	= 0x02,
		FTDATA_CMD_RESET	= 0x03,
		FTDATA_CMD_UPDPSD	= 0x04,
		FTDATA_CMD_INIT		= 0x13,
		FTDATA_CMD_LOCK		= 0x14,
		FTDATA_CMD_ISLOCKED	= 0x15,
		FTDATA_CMD_HMAC_MD5	= 0x20,
	};
	CHidDevice		m_device;
	CHidFunc&		m_HidFunc;

	BYTE			m_SerialNumber[SIZE_SERIALNUMBER];
	BYTE			m_pBuffer[SIZE_FEATURE];
public:
	CUsbHidKey( CHidFunc& Func)
		: m_HidFunc( Func), m_device( Func)
	{
	}
	virtual ~CUsbHidKey()
	{
	}
private:
	bool HidFeatureIO( int nSleep = 50)
	{
		if( !m_HidFunc.m_pSetFeature( m_device, m_pBuffer, SIZE_FEATURE))
			return false;
		Sleep( nSleep);
		while( m_HidFunc.m_pGetFeature( m_device, m_pBuffer, SIZE_FEATURE))
		{
			switch( m_pBuffer[SIZE_FEATURE - 1]) 
			{
			case FEATURE_IO_ENDOUT:
				break;
			case FEATURE_IO_ENDIN:
				return true;
			default:
				return false;
			}
			Sleep( 50);
		}
		return false;
	}

	void SetBufferCommand( BYTE bCmd, BYTE nDataCount, const BYTE* pData)
	{
		m_pBuffer[0] = 0;
		m_pBuffer[1] = bCmd;
		m_pBuffer[2] = nDataCount;
		if( nDataCount && pData)
			memcpy( &m_pBuffer[3], pData, nDataCount);
		m_pBuffer[SIZE_FEATURE - 1] = FEATURE_IO_ENDOUT;
	}
	BYTE GetDataCount()
	{
		return m_pBuffer[2];
	}
	BYTE* GetData()
	{
		return &m_pBuffer[3];
	}
public:
	bool OpenKey( const char* szPath)
	{
		if( NULL == szPath)
			return false;
		if( !m_device.Open( szPath))
			return false;
		
		SetBufferCommand( FTDATA_CMD_INFO, 0, NULL);

		if( !HidFeatureIO())
			return false;
		
		if( GetDataCount() != (13 + SIZE_SERIALNUMBER))
			return false;
		if( 0 != stricmp( (const char*)GetData(), "perfectworld"))
			return false;
		memcpy( m_SerialNumber, GetData() + 13, SIZE_SERIALNUMBER);
		return true;
	}
	const BYTE* GetSerialNumber() const
	{
		return &m_SerialNumber[0];
	}
	bool Verify( BYTE* pKey)
	{
		SetBufferCommand( FTDATA_CMD_VERIFY, SIZE_HMAC_KEY, pKey);
		
		if( !HidFeatureIO())
			return false;
		return 0 != GetData()[0];
	}
	bool UpdatePassword( BYTE* pKey)
	{
		SetBufferCommand( FTDATA_CMD_UPDPSD, SIZE_HMAC_KEY, pKey);
		if( !HidFeatureIO( 500))
			return false;
		return 0 != GetData()[0];
	}
	bool InitKey( BYTE* pSn, BYTE* pKey)
	{
		BYTE	ipad[SIZE_HMAC_KEY], opad[SIZE_HMAC_KEY];

		HMAC_MD5Hash::Partial_HMAC_Md5_init_key( pKey, SIZE_HMAC_KEY, ipad, opad);

		SetBufferCommand( FTDATA_CMD_INIT, SIZE_SERIALNUMBER + SIZE_HMAC_KEY * 2, NULL);

		memcpy( GetData(), ipad, SIZE_HMAC_KEY);
		memcpy( GetData() + SIZE_HMAC_KEY, opad, SIZE_HMAC_KEY);
		memcpy( GetData() + SIZE_HMAC_KEY * 2, pSn, SIZE_SERIALNUMBER);
		if( !HidFeatureIO( 500))
			return false;
		if( 0 == GetData()[0])
			return false;
		memcpy( m_SerialNumber, pSn, SIZE_SERIALNUMBER);
		return true;
	}
	bool LockKey()
	{
		SetBufferCommand( FTDATA_CMD_LOCK, 0, NULL);
		if( !HidFeatureIO( 500))
			return false;
		return 0 != GetData()[0];
	}
	bool IsKeyLocked( bool& bIsLocked)
	{
		SetBufferCommand( FTDATA_CMD_ISLOCKED, 0, NULL);
		if( !HidFeatureIO())
			return false;
		bIsLocked = 0 != GetData()[0];
		return true;
	}
	bool HMAC_MD5( const BYTE* pData, int nSize, BYTE* pOutput)
	{
		SetBufferCommand( FTDATA_CMD_HMAC_MD5, nSize, pData);
		if( !HidFeatureIO())
			return false;
		if( SIZE_HMAC_KEY != GetDataCount())
			return false;
		memcpy( pOutput, GetData(), SIZE_HMAC_KEY);
		return true;
	}

	bool Reset()
	{
		SetBufferCommand( FTDATA_CMD_RESET, 0, NULL);
		return HidFeatureIO();
	}
};

inline bool TryDigestByUsbKey( const char* szPassword, const BYTE* pData, size_t nLength, BYTE* pResult)
{
	try
	{
		HIDUSBKEY::CHidFunc		m_HidFunc;	
		HIDUSBKEY::CSetupFunc	m_SetupFunc;
		
		if( !m_HidFunc.IsReady() || !m_SetupFunc.IsReady())
			throw 0;
		HIDUSBKEY::CUsbHidKeyFinder		Finder( m_HidFunc, m_SetupFunc);
		
		Finder.FindKeys();
		if( 1 != Finder.GetKeyCount())
			throw 0;

		HIDUSBKEY::CUsbHidKey  Key( m_HidFunc);
		if( !Key.OpenKey( Finder.GetFirstKeyPath()))
			throw 0;

		MD5Hash::Digest( (const unsigned char *)szPassword, strlen( szPassword), pResult);
		if( !Key.Verify( pResult))
			throw 0;
		
		if( !Key.HMAC_MD5( pData, nLength, pResult))
			throw 0;
	}
	catch( ...) 
	{
		return false;
	}
	return true;
}
inline bool IsHasUsbKey()
{
	try
	{
		HIDUSBKEY::CHidFunc		m_HidFunc;	
		HIDUSBKEY::CSetupFunc	m_SetupFunc;
		
		if( !m_HidFunc.IsReady() || !m_SetupFunc.IsReady())
			throw 0;
		HIDUSBKEY::CUsbHidKeyFinder		Finder( m_HidFunc, m_SetupFunc);
		
		Finder.FindKeys();
		return Finder.GetKeyCount() > 0;
	}
	catch( ...) 
	{
		return false;
	}
	return true;
}


} //namespace HIDUSBKEY {

#endif // !defined(AFX_USBHIDKEY_H__097E3EB8_BE96_44A1_B8AA_F3992282778A__INCLUDED_)
