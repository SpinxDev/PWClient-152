/********************************************************************
	created:	2005/10/11
	created:	11:10:2005   9:58
	file name:	Archive.h
	author:		yaojun
	
	purpose:	
*********************************************************************/

#pragma once

#include <AFile.h>
#include <AAssist.h>
#include "SDBMisc.h"

// begin namespace SimpleDB
namespace SimpleDB
{

class Archive
{
private:
	AFile * pFile_;
public:
	Archive(AFile *pFile) : pFile_(pFile) { ASSERT(pFile); }

	bool IsEOF()
	{
		return pFile_->GetPos() == pFile_->GetFileLength();
	}

	DWORD Read(void *pBuffer, DWORD bufferLen)
	{
		DWORD readLen = 0;
		if (!pFile_->Read(pBuffer, bufferLen, &readLen))
			throw SimpleDB::SimpleException("SimpleDB::Archive::Read(), read file error");
		return readLen;
	}
	void Write(const void *pBuffer, DWORD bufferLen)
	{
		DWORD readLen = 0;
		if (!pFile_->Write(const_cast<void *>(pBuffer), bufferLen, &readLen))
			throw SimpleDB::SimpleException("SimpleDB::Archive::Write(), write file error");
	}

	template<class TClass>
	Archive & operator << (const TClass &obj)
	{
		obj.Save(*this);
		return *this;
	}
	template<class TClass>
	Archive & operator >> (TClass &obj)
	{
		obj.Load(*this);
		return *this;
	}

	Archive& operator << ( bool b)
	{ Write(&b, sizeof(bool)); return *this; }
	Archive& operator << ( char c)
	{ Write(&c, sizeof(char)); return *this; }
	Archive& operator << ( BYTE by )
	{ Write(&by, sizeof(BYTE)); return *this; }
	Archive& operator << ( short s)
	{ Write(&s, sizeof(short)); return *this; }
	Archive& operator << ( unsigned short w )
	{ Write(&w, sizeof(unsigned short)); return *this; }
#ifdef ANGELICA_2_2
	Archive& operator << ( wchar_t w )
	{ Write(&w, sizeof(wchar_t)); return *this; }
#endif
	Archive& operator << ( int i )
	{ Write(&i, sizeof(int)); return *this; }
	Archive& operator << ( __int64 i )
	{ Write(&i, sizeof(__int64)); return *this; }
	Archive& operator << ( long l )
	{ Write(&l, sizeof(LONG)); return *this; }
	Archive& operator << ( DWORD dw )
	{ Write(&dw, sizeof(DWORD)); return *this; }
	Archive& operator << ( float f )
	{ Write(&f, sizeof(float)); return *this; }
	Archive& operator << ( double d )
	{ Write(&d, sizeof(double)); return *this; }
	Archive& operator << ( const AString & str )
	{
		int length = str.GetLength();
		*this << length;
		Write(static_cast<const char *>(str), sizeof(char) * length);
		return *this;
	}
	Archive& operator << ( const AWString & str )
	{
		int length = str.GetLength();
		*this << length;
		Write(static_cast<const wchar_t *>(str), sizeof(wchar_t) * length);
		return *this;
	}

	Archive& operator >> ( bool & b)
	{ Read(&b, sizeof(bool)); return *this; }
	Archive& operator >> ( char & c)
	{ Read(&c, sizeof(char)); return *this; }
	Archive& operator >> ( BYTE & by )
	{ Read(&by, sizeof(BYTE)); return *this; }
	Archive& operator >> ( short& s)
	{ Read(&s, sizeof(short)); return *this; }
	Archive& operator >> ( unsigned short & w )
	{ Read(&w, sizeof(unsigned short)); return *this; }
#ifdef ANGELICA_2_2
	Archive& operator >> ( wchar_t & w )
	{ Read(&w, sizeof(wchar_t)); return *this; }
#endif
	Archive& operator >> ( int & i )
	{ Read(&i, sizeof(int)); return *this; }
	Archive& operator >> ( __int64 & i )
	{ Read(&i, sizeof(__int64)); return *this; }
	Archive& operator >> ( LONG & l )
	{ Read(&l, sizeof(LONG)); return *this; }
	Archive& operator >> ( DWORD & dw )
	{ Read(&dw, sizeof(DWORD)); return *this; }
	Archive& operator >> ( float & f )
	{ Read(&f, sizeof(float)); return *this; }
	Archive& operator >> ( double & d )
	{ Read(&d, sizeof(double)); return *this; }
	Archive& operator >> ( AString & str )
	{
		int length = 0;
		*this >> length;
		ASSERT(length >= 0);

		if (0 == length)
		{
			str.Empty();
		}
		else
		{
			char * pBuffer = (char*)a_malloctemp(length + 1);
			Read(pBuffer, length * sizeof(char));
			pBuffer[length] = '\0';
			str = pBuffer;
			a_freetemp(pBuffer);
		}
		return *this;
	}
	Archive& operator >> ( AWString & str )
	{
		int length = 0;
		*this >> length;
		ASSERT(length >= 0);
		
		if (0 == length)
		{
			str.Empty();
		}
		else
		{
			wchar_t * pBuffer = (wchar_t*)a_malloctemp(sizeof(wchar_t) * (length + 1));
			Read(pBuffer, length * sizeof(wchar_t));
			pBuffer[length] = L'\0';
			str = pBuffer;
			a_freetemp(pBuffer);
		}
		return *this;
	}
};

namespace CommonPrivate
{
	struct FuncSaveAnyTypeToArchive
	{
	private:
		Archive & ar_;
	public:
		FuncSaveAnyTypeToArchive(Archive & ar) : ar_(ar) {}
		template<typename ValueType>
		void operator () (const ValueType & value)
		{
			ar_ << value;
		}
	};
	struct FuncLoadAnyTypeToArchive
	{
	private:
		Archive & ar_;
	public:
		FuncLoadAnyTypeToArchive(Archive & ar) : ar_(ar) {}
		template<typename ValueType>
		void operator () (ValueType & value)
		{
			ar_ >> value;
		}
	};

}

} // end namespace SimpleDB

