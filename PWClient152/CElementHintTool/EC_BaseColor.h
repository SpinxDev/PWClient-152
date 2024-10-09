// Filename	: EC_BaseColor.h
// Creator	: zhangyitian
// Date		: 2014/8/20

#ifndef _ELEMENTCLIENT_EC_BASECOLOR_H_
#define _ELEMENTCLIENT_EC_BASECOLOR_H_

#include <ABaseDef.h>
#include <AAssist.h>

class CECBaseColor {
public:
	enum {
		MAX_BASE_COLOR_COUNT = 32,
	};
	struct BaseColor {
		BaseColor() {}
		BaseColor(const int id, const ACString& name, const DWORD color) : m_id(id), m_name(name), m_color(color) {
		}
		int m_id;
		ACString m_name;
		DWORD m_color;
	};
	const BaseColor* GetBaseColorFromName(const ACString& name) const;
	const BaseColor* GetBaseColorFromColor(const DWORD color) const;
	const BaseColor* GetBaseColorFromIndex(int index) const;
	const BaseColor* GetBaseColorFromID(int id) const;
	int GetBaseColorCount() const { return MAX_BASE_COLOR_COUNT; }

	static CECBaseColor& Instance();

private:
	BaseColor m_colorArray[MAX_BASE_COLOR_COUNT];

	CECBaseColor();
	CECBaseColor(const CECBaseColor &);
	CECBaseColor& operator = (const CECBaseColor &);
};

#endif