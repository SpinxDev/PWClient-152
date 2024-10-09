// Filename	: EC_BaseColor.cpp
// Creator	: zhangyitian
// Date		: 2014/8/20

#include "EC_BaseColor.h"
#include "EC_UIHelper.h"
#include "EC_GameUIMan.h"
#include <A3DMacros.h>
#include <AF.h>

CECBaseColor& CECBaseColor::Instance() {
	static CECBaseColor baseColor;
	return baseColor;
}

CECBaseColor::CECBaseColor() {
	AScriptFile fileColorConfig;
	if (!fileColorConfig.Open("Configs\\purecolorconfig.txt")) {
		a_LogOutput(1, "CECBaseColor::CECBaseColor, Failed to open file Configs\\purecolorconfig.txt");
		return;
	}
	int i;
	for (i = 0; i < MAX_BASE_COLOR_COUNT; i++) {
		int id = fileColorConfig.GetNextTokenAsInt(true);
		int r = fileColorConfig.GetNextTokenAsInt(true);
		int g = fileColorConfig.GetNextTokenAsInt(true);
		int b = fileColorConfig.GetNextTokenAsInt(true);
		m_colorArray[i] = BaseColor(id, _AL(""), A3DCOLORRGB(r, g, b));
	}
	for (i = 0; i < MAX_BASE_COLOR_COUNT; i++) {
		m_colorArray[i].m_name = CECUIHelper::GetGameUIMan()->GetStringFromTable(i + 11360);
	}
}

const CECBaseColor::BaseColor* CECBaseColor::GetBaseColorFromName(const ACString& name) const {
	for (int i = 0; i < MAX_BASE_COLOR_COUNT; i++) {
		if (m_colorArray[i].m_name == name) {
			return &m_colorArray[i];
		}
	}
	return NULL;
}

const CECBaseColor::BaseColor* CECBaseColor::GetBaseColorFromColor(const DWORD color) const {
	for (int i = 0; i < MAX_BASE_COLOR_COUNT; i++) {
		if (m_colorArray[i].m_color == color) {
			return &m_colorArray[i];
		}
	}
	return NULL;
}

const CECBaseColor::BaseColor* CECBaseColor::GetBaseColorFromID(int id) const {
	for (int i = 0; i < MAX_BASE_COLOR_COUNT; i++) {
		if (m_colorArray[i].m_id == id) {
			return &m_colorArray[i];
		}
	}
	return NULL;
}

const CECBaseColor::BaseColor* CECBaseColor::GetBaseColorFromIndex(int index) const {
	ASSERT(index >= 0 && index < MAX_BASE_COLOR_COUNT);
	return &m_colorArray[index];
}