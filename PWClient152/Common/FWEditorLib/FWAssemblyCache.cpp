// FWAssemblyCache.cpp: implementation of the FWAssemblyCache class.
//
//////////////////////////////////////////////////////////////////////

#include "FWAssemblyCache.h"
#include "A3DGFXEx.h"
#include "FWAssemblyProfile.h"
#include "FWFormulaList.h"
#include "A3DGFXElement.h"
#include "A3DParticleSystemEx.h"
#include "A3DParticleEmitter.h"
#include "AF.h"
#include "FWLog.h"
#include "Global.h"
#include "FWAlgorithm.h"

using namespace FWAlgorithm;

#define new A_DEBUG_NEW

IMPLEMENT_SINGLETON(FWAssemblyCache)

FWAssemblyCache::FWAssemblyCache()
{
	m_bInited = false;
}

bool FWAssemblyCache::Init(A3DDevice *pDevice)
{
	if (m_bInited) return true;

#ifdef _ENABLE_ASSEMBLY_CACHE
	FWLog::GetInstance()->Log(
		"FWAssemblyCache::Init(), AssemblyCache will be inited with cache enabled");
#else
	FWLog::GetInstance()->Log(
		"FWAssemblyCache::Init(), AssemblyCache will be inited with cache diabled");
#endif

	// read gfx list file
	AString strFile;
	strFile.Format("%s\\gfx\\烟花\\效果.txt", g_szWorkDir);
	ATextTabFile file;
	if (!file.Open(strFile))
		return false;

	int nID = 0;
	bool bAllSucceed = true;
	for (int row = 0; row < file.GetRowNum(); row++)
	{
		// on row must have exactly 3 cols
		if (file.GetColNum(row) != 3) continue;

		FWAssemblyInfo *pInfo = new FWAssemblyInfo;

		BEGIN_FAKE_WHILE;

		pInfo->m_pDevice = pDevice;

		// read type
		pInfo->Type = GetIntTypeFromStrType(file.GetItem(row, 0));
		CHECK_BREAK(IsValidType(pInfo->Type));

		// read name
		pInfo->Name = file.GetItem(row, 1);

		// read file name
		pInfo->strFileName = file.GetItem(row, 2);
		pInfo->strFileName.CutRight(1); // remove '\t'
		
		// generate ID
		pInfo->ID = GetMaxIDOfType(pInfo->Type) + 1;

#ifdef _ENABLE_ASSEMBLY_CACHE
		// read data to cache (member variable)
		pInfo->m_pProfile = pInfo->LoadProfile();
		CHECK_BREAK_LOG1(pInfo->m_pProfile, 
			"FWAssemblyCache::Init(), Unable to load profile of ID : %d", 
			pInfo->ID);
		
		pInfo->m_pFormulaList = pInfo->LoadFormulaList();
		CHECK_BREAK_LOG1(pInfo->m_pFormulaList, 
			"FWAssemblyCache::Init(), Unable to load formulaList of ID : %d", 
			pInfo->ID);
		
		pInfo->m_pAssembly = pInfo->LoadAssembly();
		CHECK_BREAK_LOG1(pInfo->m_pAssembly,
			"FWAssemblyCache::Init(), Unable to load gfx of ID : %d", 
			pInfo->ID);

		// precompute partical count if possible
		if (pInfo->LoadIsParticalCountConstant())
			pInfo->ParticalCount = ComputeParticalCount(pInfo->m_pAssembly);

		pInfo->SoundCount = ComputeSoundCount(pInfo->m_pAssembly);
#endif


		// add to list
		AddTail(pInfo);		

		END_FAKE_WHILE;

		BEGIN_ON_FAIL_FAKE_WHILE;

		// just ignore current gfx if error occured
		FWLog::GetInstance()->Log(
			"FWAssemblyCache::Init(), Load assembly info [%s] failed", 
			(pInfo->Name.IsEmpty() ? "[UnknownName]" : pInfo->Name));

		SAFE_DELETE(pInfo);
		bAllSucceed = false;

		END_ON_FAIL_FAKE_WHILE;
	}

	m_bInited = true;

	if (!bAllSucceed)
	{
		FWLog::GetInstance()->Log("Not All assembly load successfully!");
	}
	return bAllSucceed;	
}


int FWAssemblyCache::GetIntTypeFromStrType(const AString &strType)
{
	if (strType == "Simple")
		return TYPE_SIMPLE;
	else if (strType == "Component")
		return TYPE_COMPONENT;
	else if (strType == "Text")
		return TYPE_TEXT;
	else if (strType == "Lanch")
		return TYPE_LANCH;
	else
	{
		ASSERT(false);
		return TYPE_UNKNOWN;
	}
}



int FWAssemblyCache::GetDefaultProfileIDByType(int nType)
{
	ASSERT(IsValidType(nType));

	return GetMinIDOfType(nType);
}


FWAssembly * FWAssemblyInfo::LoadAssembly()
{
	ASSERT(!strFileName.IsEmpty());

	AString strFullPath;
	strFullPath.Format("%s\\gfx\\烟花\\%s.gfx", g_szWorkDir, strFileName);
	
	FWAssembly *pAssembly = new FWAssembly;

	if (!pAssembly->Load(m_pDevice, strFullPath))
		SAFE_DELETE(pAssembly);

	return pAssembly;
}

FWAssemblyProfile * FWAssemblyInfo::LoadProfile()
{
	ASSERT(!strFileName.IsEmpty());

	AString strFullPath;
	strFullPath.Format("%s\\gfx\\烟花\\%s.ini", g_szWorkDir, strFileName);
	
	FWAssemblyProfile * pProfile = new FWAssemblyProfile;

	BEGIN_FAKE_WHILE;
	
	// open file
	AScriptFile file;
	CHECK_BREAK(file.Open(strFullPath));
	
	// read outproperties to profile
	CHECK_BREAK(pProfile->Load(&file));
	pProfile->SetAssemblyID(ID);
	
	// close file
	file.Close();

	END_FAKE_WHILE;

	BEGIN_ON_FAIL_FAKE_WHILE;

	SAFE_DELETE(pProfile);

	END_ON_FAIL_FAKE_WHILE;
	
	return pProfile;
}

FWFormulaList * FWAssemblyInfo::LoadFormulaList()
{
	ASSERT(!strFileName.IsEmpty());

	AString strFullPath;
	strFullPath.Format("%s\\gfx\\烟花\\%s.ini", g_szWorkDir, strFileName);
	
	FWFormulaList * pFormulaList = new FWFormulaList;
	
	BEGIN_FAKE_WHILE;
	
	// open file
	AScriptFile file;
	CHECK_BREAK(file.Open(strFullPath));
	
	// read formulas
	CHECK_BREAK(pFormulaList->Load(&file));
	
	END_FAKE_WHILE;
	
	BEGIN_ON_FAIL_FAKE_WHILE;

	SAFE_DELETE(pFormulaList);

	END_ON_FAIL_FAKE_WHILE;

	return pFormulaList;
}

bool FWAssemblyInfo::LoadIsParticalCountConstant()
{
	ASSERT(!strFileName.IsEmpty());
	
	AString strFullPath;
	strFullPath.Format("%s\\gfx\\烟花\\%s.ini", g_szWorkDir, strFileName);

	BEGIN_FAKE_WHILE;

	// open file
	AScriptFile file;
	CHECK_BREAK(file.Open(strFullPath));

	while (file.GetNextToken(true))
	{
		if (AString(file.m_szToken) == "ParticalCountConstant")
		{
			bool bReadSucceed = false;

			bReadSucceed = file.GetNextToken(false);
			CHECK_BREAK(bReadSucceed);
			CHECK_BREAK(AString(file.m_szToken) == "=");

			bReadSucceed = file.GetNextToken(false);
			CHECK_BREAK(bReadSucceed);
			int nVal = AString(file.m_szToken).ToInt();
			
			return (nVal == 1);
		}
	}

	END_FAKE_WHILE;

	return true;
}

	
FWAssembly * FWAssemblyInfo::CreateAssembly()
{
	ASSERT(ID != -1);
#ifdef _ENABLE_ASSEMBLY_CACHE
	return m_pAssembly->Clone();
#else
	FWAssembly *pAssembly = LoadAssembly();
	if (!pAssembly) 
		FWLog::GetInstance()->Log(
			"FWAssemblyInfo::CreateAssembly(), unable to load gfx [%s] from file!",
			Name);
	return pAssembly;
#endif
}

FWAssemblyProfile * FWAssemblyInfo::CreateProfile()
{
	ASSERT(ID != -1);
#ifdef _ENABLE_ASSEMBLY_CACHE
	return m_pProfile->Clone();
#else
	FWAssemblyProfile *pProfile = LoadProfile();
	if (!pProfile)
		FWLog::GetInstance()->Log(
			"FWAssemblyInfo::CreateProfile(), unable to load profile [%s] from file!",
			Name);
	return pProfile;
#endif

}

FWFormulaList * FWAssemblyInfo::CreateFormulaList()
{
	ASSERT(ID != -1);

	// special :
	// ignore cache here. we read it from file everytime
	// because the formulalist are constant, and it is
	// messy to implement Clone() for it :)
	// since the formula will only be used each time the
	// final gfx set it made, this strategy will not 
	// introduce much overhead
	FWFormulaList *pList = LoadFormulaList();
	if (!pList)
		FWLog::GetInstance()->Log(
			"FWAssemblyInfo::CreateFormulaList(), unable to load formulaList [%s] from file.",
			Name);
	return pList;
}

FWAssemblyInfo::FWAssemblyInfo() 
{
	m_pDevice = NULL;
	Type = FWAssemblyCache::TYPE_UNKNOWN;
	ParticalCount = -1;
	SoundCount = -1;

#ifdef _ENABLE_ASSEMBLY_CACHE
	m_pProfile = NULL;
	m_pAssembly = NULL;
	m_pFormulaList = NULL;
#endif
}

FWAssemblyInfo::~FWAssemblyInfo()
{
#ifdef _ENABLE_ASSEMBLY_CACHE
	SAFE_DELETE(m_pProfile);
	SAFE_DELETE(m_pFormulaList);
	A3DRELEASE(m_pAssembly);
#endif
}




#ifdef _ENABLE_ASSEMBLY_CACHE

const FWAssembly * FWAssemblyInfo::GetAssembly() const
{
	return m_pAssembly;
}

const FWAssemblyProfile * FWAssemblyInfo::GetProfile() const
{
	return m_pProfile;
}

const FWFormulaList * FWAssemblyInfo::GetFormulaList() const
{
	return m_pFormulaList;
}

#endif


int FWAssemblyCache::GetMaxIDOfType(int nType)
{
	ASSERT(IsValidType(nType));

	int nMax = GetMinIDOfType(nType) - 1;

	ALISTPOSITION pos = GetHeadPosition();
	while (pos)
	{
		FWAssemblyInfo *pInfo = GetNext(pos);
		if (pInfo->Type == nType && pInfo->ID > nMax)
			nMax = pInfo->ID;
	}
	return nMax;
}

int FWAssemblyCache::GetMinIDOfType(int nType)
{
	ASSERT(IsValidType(nType));

	return nType * 1000;
}

bool FWAssemblyCache::IsValidType(int nType)
{
	return (nType >= TYPE_DEFAULT && nType < TYPE_COUNT);
}
