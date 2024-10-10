#pragma once

#include "FExportDecl.h"

#include <ObjBase.h>
#include <Guiddef.h>
#include <string>

namespace AudioEngine
{
	class _EXPORT_DLL_ Guid
	{
	public:
		Guid(void);
		~Guid(void);
	public:
		friend bool				operator < (const Guid& left, const Guid& right)
		{
// 			const int c_dim = 39;
// 			wchar_t szLeft[c_dim] = {0};
// 			wchar_t szRight[c_dim] = {0};
// 			::StringFromGUID2(left.guid, szLeft, c_dim);
// 			::StringFromGUID2(right.guid, szRight, c_dim);
// 			if(wcscmp(szLeft, szRight) < 0)
// 				return true;
			if(strcmp(left.GetString(), right.GetString()) < 0)
				return true;
			return false;
		}
		bool					BuildFromString(const char* sz);
		const char*				GetString() const;
	public:
		GUID					guid;
	protected:
		mutable std::string		m_str;
	};

	class GuidGen
	{
	private:
		GuidGen(void);
		~GuidGen(void);
	public:
		static GuidGen&			GetInstance();
		Guid					Generate();
	};
}