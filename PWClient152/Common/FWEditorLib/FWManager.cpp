// FWManager.cpp: implementation of the FWManager class.
//
//////////////////////////////////////////////////////////////////////

#include "FWManager.h"
#include "FWDoc.h"
#include "FWView.h"
#include "FWFontEnum.h"
#include "FWAssemblyCache.h"
#include "FWModifierCreator.h"
#include "FWLog.h"
#include "FWArtCreator.h"


#define new A_DEBUG_NEW




FWManager::FWManager() :
	m_pDoc(NULL),
	m_pView(NULL),
	m_bInited(false)
{
}

FWManager::~FWManager()
{
	Release();
}

bool FWManager::Init(A3DEngine *pA3DEngine, A3DDevice *pA3DDevice)
{
	BEGIN_FAKE_WHILE;

	m_pDoc = new FWDoc;
	m_pView = new FWView;

	CHECK_BREAK(m_pView->Init(pA3DEngine, pA3DDevice));
	
	m_pDoc->AttachView(m_pView);

	m_bInited = true;

	END_FAKE_WHILE;

	BEGIN_ON_FAIL_FAKE_WHILE;

	Release();

	END_ON_FAIL_FAKE_WHILE;

	RETURN_FAKE_WHILE_RESULT;

}

void FWManager::Release()
{
	SAFE_DELETE(m_pView);
	SAFE_DELETE(m_pDoc);
	
	m_bInited = false;
}

FWDoc * FWManager::GetDoc()
{
	ASSERT(m_pDoc);

	return m_pDoc;
}

FWView * FWManager::GetView()
{
	ASSERT(m_pView);

	return m_pView;
}

void FWManager::Tick(DWORD dwTickTime)
{
	if (!m_bInited) return;

	GetView()->Tick(dwTickTime);
}

void FWManager::Render(A3DGFXExMan *pGfxExMan)
{
	if (!m_bInited) return;

	GetView()->OnDraw(pGfxExMan);
}


bool AfxInitFWEditor(A3DDevice *pDevice)
{
	BEGIN_FAKE_WHILE;
	
	CHECK_BREAK(FWLog::GetInstance()->Init("FWEditor.log", "FWEditor Log"));

	CHECK_BREAK(FWAssemblyCache::GetInstance()->Init(pDevice));
	
	CHECK_BREAK(FWModifierCreator::GetInstance()->Init());

	CHECK_BREAK(FWFontEnum::GetInstance()->Init(::GetDC(::GetDesktopWindow())));

	CHECK_BREAK(FWArtCreator::GetInstance()->Init());
	
	a_InitRandom();
	
	END_FAKE_WHILE;
	
	BEGIN_ON_FAIL_FAKE_WHILE;
	
	AfxReleaseFWEditor();
	
	END_ON_FAIL_FAKE_WHILE;
	
	RETURN_FAKE_WHILE_RESULT;
	
}

void AfxReleaseFWEditor()
{
	FWArtCreator::DestroyInstance();
	FWAssemblyCache::DestroyInstance();
	FWModifierCreator::DestroyInstance();
	FWFontEnum::DestroyInstance();
	FWLog::DestroyInstance();
}

void FWManager::SetVisible(bool bVisible)
{
	GetView()->SetVisible(bVisible);
}

bool FWManager::IsVisible()
{
	return GetView()->IsVisible();
}
