#ifndef _FWARCHIVE_H_
#define _FWARCHIVE_H_

#include <AString.h>
#include <AArray.h>
#include <AAssist.h>
#include <APoint.h>
#include <ARect.h>
#include <A3DVector.h>

class FWObject;
class FWRuntimeClass;
class AFile;
template <class T> class ARect;
class A3DVECTOR3;

#pragma warning(disable : 4290)

class FWExceptionArchive
{
protected:
	const char * m_szMsg;
	int m_nID;
public:
	enum
	{
		COMMON_ERROR = 0,
		COMMON_READ_ERROR = 0,
		COMMON_WRITE_ERROR = 1,
		UNKNOWN_RUNTIME_CLASS_FLAG,
		INVALID_RUNTIME_CLASS_INDEX,
		INVALID_RUNTIME_CLASS_CONTENT,
	};
	FWExceptionArchive(int nID = COMMON_ERROR, const char * szMsg = NULL)
	{
		m_nID = nID;
		m_szMsg = szMsg;
	}
	const char * GetErrMsg() { return m_szMsg; }
	int GetErrID() { return m_nID; }
};


//	a part of serialization mechanism
class FWArchive  
{
protected:
	int FindInRuntimeClassArray(FWRuntimeClass *pClass);
	AFile * m_pFile;
	int m_nMode;
	int m_nVersion;

	typedef AArray<FWRuntimeClass *, FWRuntimeClass *> RuntimeClassArray;
	RuntimeClassArray m_aryRumtimeClass;

	enum {FLAG_RUNTIME_CLASS_CONTENT, FLAG_RUNTIME_CLASS_INDEX};
public:
	enum {MODE_STORE, MODE_LOAD};

	FWArchive(AFile *pFile, int nMode);
	virtual ~FWArchive();

	int GetVersion() { return m_nVersion; }
	void WriteVersion(int nVersion) 
	{ 
		ASSERT(IsStoring()); 
		m_nVersion = nVersion; 
		*this << m_nVersion; 
	}
	void ReadVesoin() { ASSERT(IsLoading()); *this >> m_nVersion; }
	
	// this function does nothing, since no cache is 
	// applied to FWArchive now
	void Flush();

	bool IsStoring();
	bool IsLoading();


	UINT Read(void *lpBuf, UINT uMax) throw(FWExceptionArchive);
	void Write(void *lpBuf, UINT uLen) throw(FWExceptionArchive);

	FWObject* ReadObject();
	void WriteObject(const FWObject* pObj);


	// note : different from FWArchive
	// the WriteString() will write the terminating null character to file
	// and the parameter uMax of ReadString() stand for the char count
	// of lpsz/lpwsz including the terminating null character
	void WriteStringA(LPCSTR lpsz);
	void WriteStringW(LPCWSTR lpwsz);
	LPSTR ReadStringA(LPSTR lpsz, UINT uMax);
	LPWSTR ReadStringW(LPWSTR lpwsz, UINT uMax);
	bool ReadStringAString(AString & strString);
	bool ReadStringACString(ACString & strString);
#ifdef UNICODE
	#define WriteString WriteStringW	
	#define ReadString ReadStringW
#else
	#define WriteString WriteStringA	
	#define ReadString ReadStringA
#endif

	// store operator
	friend FWArchive& operator <<( FWArchive &ar, const FWObject* pOb )
	{ ar.WriteObject(pOb); return ar; }
	FWArchive& operator <<( char c)
	{ Write(&c, sizeof(char)); return *this; }
	FWArchive& operator <<( BYTE by )
	{ Write(&by, sizeof(BYTE)); return *this; }
	FWArchive& operator <<( WORD w )
	{ Write(&w, sizeof(WORD)); return *this; }
	FWArchive& operator <<( int i )
	{ Write(&i, sizeof(int)); return *this; }
	FWArchive& operator <<( LONG l )
	{ Write(&l, sizeof(LONG)); return *this; }
	FWArchive& operator <<( DWORD dw )
	{ Write(&dw, sizeof(DWORD)); return *this; }
	FWArchive& operator <<( float f )
	{ Write(&f, sizeof(float)); return *this; }
	FWArchive& operator <<( double d )
	{ Write(&d, sizeof(double)); return *this; }
	FWArchive& operator <<( short s)
	{ Write(&s, sizeof(short)); return *this; }
	FWArchive& operator <<( LPCSTR lpsz )
	{ WriteStringA(lpsz); return *this; }
	FWArchive& operator <<( LPCWSTR lpwsz )
	{ WriteStringW(lpwsz); return *this; }
	FWArchive& operator <<( const AString &str)
	{ WriteStringA((LPCSTR) str); return *this; }
	FWArchive& operator <<( const ACString &str)
	{ WriteString((LPCTSTR)str); return *this; }
	FWArchive& operator <<( const RECT & rect)
	{ *this << rect.left << rect.top << rect.right << rect.bottom; return *this; } 
	template <class T>
	FWArchive& operator <<( const ARect<T> & rect)
	{ *this << rect.left << rect.top << rect.right << rect.bottom; return *this; } 
	FWArchive& operator <<( const POINT & pt)
	{ *this << pt.x << pt.y; return *this; }
	template <class T>
	FWArchive& operator <<( const APoint<T> & pt)
	{ *this << pt.x << pt.y; return *this; }
	FWArchive& operator <<( const A3DVECTOR3 &vec)
	{ *this << vec.x << vec.y << vec.z; return *this; }


	// load operator	
	friend FWArchive& operator >>( FWArchive &ar, const FWObject*& pOb )
	{ pOb = ar.ReadObject(); return ar; }
	FWArchive& operator >>( char & c)
	{ Read(&c, sizeof(char)); return *this; }
	FWArchive& operator >>( BYTE & by )
	{ Read(&by, sizeof(BYTE)); return *this; }
	FWArchive& operator >>( WORD & w )
	{ Read(&w, sizeof(WORD)); return *this; }
	FWArchive& operator >>( int & i )
	{ Read(&i, sizeof(int)); return *this; }
	FWArchive& operator >>( LONG & l )
	{ Read(&l, sizeof(LONG)); return *this; }
	FWArchive& operator >>( DWORD & dw )
	{ Read(&dw, sizeof(DWORD)); return *this; }
	FWArchive& operator >>( float & f )
	{ Read(&f, sizeof(float)); return *this; }
	FWArchive& operator >>( double & d )
	{ Read(&d, sizeof(double)); return *this; }
	FWArchive& operator >>( short& s)
	{ Read(&s, sizeof(short)); return *this; }
	FWArchive& operator >>( AString &str)
	{ ReadStringAString(str); return *this; }
	FWArchive& operator >>( ACString & str)
	{ ReadStringACString(str); return *this;}
	FWArchive& operator >>( RECT &rect)
	{ *this >> rect.left >> rect.top >> rect.right >> rect.bottom; return *this; } 
	template <class T>
	FWArchive& operator >>( ARect<T> &rect)
	{ *this >> rect.left >> rect.top >> rect.right >> rect.bottom; return *this; } 
	FWArchive& operator >>( POINT & pt)
	{ *this >> pt.x >> pt.y; return *this; }
	template <class T>
	FWArchive& operator >>( APoint<T> & pt)
	{ *this >> pt.x >> pt.y; return *this; }
	FWArchive& operator >>( A3DVECTOR3 &vec)
	{ *this >> vec.x >> vec.y >> vec.z; return *this; }
};

#endif 
