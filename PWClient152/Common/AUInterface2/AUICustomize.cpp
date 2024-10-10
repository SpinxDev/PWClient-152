// Filename	: AUICustomize.cpp
// Creator	: Xiao Zhou
// Date		: May 23, 2006
// Desc		: AUICustomize is the class of custiomize control.

#include "AUI.h"

#include "AM.h"
#include "A3DMacros.h"
#include "A3DGDI.h"
#include "AScriptFile.h"
#include "A3DEngine.h"
#include "A3DDevice.h"
#include "A3DViewport.h"

#include "AUIDialog.h"
#include "AUIManager.h"
#include "AUICustomize.h"
#include "AUICommon.h"


AUICustomize::AUICustomize() : AUIObject()
{
	m_nType = AUIOBJECT_TYPE_CUSTOMIZE;
	m_szObjectType = "CUSTOMIZE";
}

AUICustomize::~AUICustomize()
{
}

bool AUICustomize::Save(FILE *file)
{
	fprintf(file, "%s \"%s\" \"%s\" \"%s\" %d %d %d %d", m_szObjectType, m_szName,
		m_strCommand, m_strSound, m_x, m_y, m_nWidth, m_nHeight);

	return true;
}

bool AUICustomize::GetProperty(char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	memset(Property, 0, sizeof(AUIOBJECT_SETPROPERTY));

	if( 0 == strcmpi(pszPropertyName, "Object Type") )
		strcpy(Property->c, m_szObjectType);
	else
		return AUIObject::GetProperty(pszPropertyName, Property);
	
	return true;
}

bool AUICustomize::SetProperty(char *pszPropertyName, PAUIOBJECT_SETPROPERTY Property)
{
	if( 0 == strcmpi(pszPropertyName, "Object Type") )
		m_szObjectType = Property->c;
	else
		return AUIObject::SetProperty(pszPropertyName, Property);

	return true;
}

bool AUICustomize::Render(void)
{
	extern A3DGDI *g_pA3DGDI;
	POINT ptPos = GetPos();

	ptPos.x += m_nOffX;
	ptPos.y += m_nOffY;

	A3DRECT rect(ptPos.x, ptPos.y, ptPos.x + m_nWidth - 1, ptPos.y + m_nHeight - 1);
	g_pA3DGDI->Draw2DRectangle(rect, A3DCOLORRGBA(64, 64, 64, 64));

	return true;
}


