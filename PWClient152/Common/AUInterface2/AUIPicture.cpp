// Filename	: AUIPicture.cpp
// Creator	: Tom Zhou
// Date		: May 13, 2004
// Desc		: AUIPicture is the class of static picture control.

#include "AUI.h"

#include "AUIPicture.h"
#include "AUICommon.h"

AUIPicture::AUIPicture() : AUIObject()
{
}

AUIPicture::~AUIPicture()
{
}

bool AUIPicture::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice, AScriptFile *pASF)
{

	if (!AUIObject::Init(pA3DEngine, pA3DDevice, pASF))
		return AUI_ReportError(__LINE__, 1, "AUIPicture::Init(), failed to call base AUIObject::Init()");

	return true;
}

bool AUIPicture::Release(void)
{
	return AUIObject::Release();
}

bool AUIPicture::Save(FILE *file)
{
	if( !AUIObject::Save(file) ) return false;

	return true;
}

bool AUIPicture::OnDlgItemMessage(UINT msg, WPARAM wParam, LPARAM lParam)
{
	return AUIObject::OnDlgItemMessage(msg, wParam, lParam);
}

bool AUIPicture::Tick(void)
{
	return AUIObject::Tick();
}

bool AUIPicture::GetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	return AUIObject::GetProperty(pszPropertyName, Property);
}

bool AUIPicture::SetProperty(const char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	return AUIObject::SetProperty(pszPropertyName, Property);
}
