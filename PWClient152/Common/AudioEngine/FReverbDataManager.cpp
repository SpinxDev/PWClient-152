#include "FReverbDataManager.h"
#include "xml\xmlcommon.h"
#include "FEventSystem.h"
#include "FLogManager.h"
#include <ObjBase.h>
#include <io.h>
#include <AMemory.h>
#include <AFileImage.h>

static const unsigned int version = 0x100001;

using namespace AudioEngine;

ReverbDataManager::ReverbDataManager(void)
: m_pEventSystem(0)
{
}

ReverbDataManager::~ReverbDataManager(void)
{
	release();
}

bool ReverbDataManager::Init(EventSystem* pEventSystem)
{
	if(!pEventSystem)
		return false;
	m_pEventSystem = pEventSystem;
	return true;
}

void ReverbDataManager::release()
{
	for (size_t i =0; i<m_arrReverb.size(); ++i)
	{
		delete m_arrReverb[i];
	}
	m_arrReverb.clear();
}

bool ReverbDataManager::LoadXML(const char* szPath)
{
	if(0 != _access(szPath, 0))
	{
		createPresets();
		if(!SaveXML(szPath))
			return false;
	}

	release();

	AFileImage file;
	if(!file.Open(szPath, AFILE_OPENEXIST|AFILE_TEXT|AFILE_TEMPMEMORY))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "ReverbDataManager::LoadXML 无法打开文件%s", szPath);
		return false;
	}

	DWORD dwLength = file.GetFileLength();
	char* pBuf = (char *)a_malloctemp(dwLength + 1);
	memset(pBuf, 0, dwLength + 1);
	DWORD dwRead;
	file.Read(pBuf, dwLength, &dwRead);
	file.Close();
	pBuf[dwLength] = 0;

	TiXmlDocument doc;
	doc.Parse(pBuf);
	a_freetemp(pBuf);

	TiXmlNode* pNode = doc.FirstChild("ReverbDataManager");
	if (!pNode)
		return false;

	unsigned int ver = 0;

	TiXmlElement* root = pNode->ToElement();
	QueryElement(root, "version", ver);
	int iReverbNum = 0;
	QueryElement(root, "ReverbNum", iReverbNum);

	TiXmlNode* pReverbNode = root->FirstChild("Reverb");
	for (int i=0; i<iReverbNum; ++i)
	{
		REVERB* pReverb = new REVERB;
		TiXmlElement* ele = pReverbNode->ToElement();
		QueryElement(ele, "Name", pReverb->strName);
		QueryElement(ele, "Instance", pReverb->prop.Instance);
		QueryElement(ele, "Environment", pReverb->prop.Environment);
		QueryElement(ele, "EnvDiffusion", pReverb->prop.EnvDiffusion);
		QueryElement(ele, "Room", pReverb->prop.Room);
		QueryElement(ele, "RoomHF", pReverb->prop.RoomHF);
		QueryElement(ele, "RoomLF", pReverb->prop.RoomLF);
		QueryElement(ele, "DecayTime", pReverb->prop.DecayTime);
		QueryElement(ele, "DecayHFRatio", pReverb->prop.DecayHFRatio);
		QueryElement(ele, "DecayLFRatio", pReverb->prop.DecayLFRatio);
		QueryElement(ele, "Reflections", pReverb->prop.Reflections);
		QueryElement(ele, "ReflectionsDelay", pReverb->prop.ReflectionsDelay);
		QueryElement(ele, "Reverb", pReverb->prop.Reverb);
		QueryElement(ele, "ReverbDelay", pReverb->prop.ReverbDelay);
		QueryElement(ele, "ModulationTime", pReverb->prop.ModulationTime);
		QueryElement(ele, "ModulationDepth", pReverb->prop.ModulationDepth);
		QueryElement(ele, "HFReference", pReverb->prop.HFReference);
		QueryElement(ele, "LFReference", pReverb->prop.LFReference);
		QueryElement(ele, "Diffusion", pReverb->prop.Diffusion);
		QueryElement(ele, "Density", pReverb->prop.Density);
		QueryElement(ele, "Flags", pReverb->prop.Flags);
		
		m_arrReverb.push_back(pReverb);
		pReverbNode = pReverbNode->NextSibling("Reverb");
	}
	return true;
}

bool ReverbDataManager::SaveXML(const char* szPath)
{
	TiXmlDocument doc;
	TiXmlElement* root = new TiXmlElement("ReverbDataManager");
	doc.LinkEndChild(root);

	AddElement(root, "version", version);
	int iReverbNum = GetReverbNum();
	AddElement(root, "ReverbNum", iReverbNum);
	for (int i=0; i<iReverbNum; ++i)
	{
		REVERB* pReverb = m_arrReverb[i];
		TiXmlElement* ele = new TiXmlElement("Reverb");
		root->LinkEndChild(ele);
		AddElement(ele, "Name", pReverb->strName);
		AddElement(ele, "Instance", pReverb->prop.Instance);
		AddElement(ele, "Environment", pReverb->prop.Environment);
		AddElement(ele, "EnvDiffusion", pReverb->prop.EnvDiffusion);
		AddElement(ele, "Room", pReverb->prop.Room);
		AddElement(ele, "RoomHF", pReverb->prop.RoomHF);
		AddElement(ele, "RoomLF", pReverb->prop.RoomLF);
		AddElement(ele, "DecayTime", pReverb->prop.DecayTime);
		AddElement(ele, "DecayHFRatio", pReverb->prop.DecayHFRatio);
		AddElement(ele, "DecayLFRatio", pReverb->prop.DecayLFRatio);
		AddElement(ele, "Reflections", pReverb->prop.Reflections);
		AddElement(ele, "ReflectionsDelay", pReverb->prop.ReflectionsDelay);
		AddElement(ele, "Reverb", pReverb->prop.Reverb);
		AddElement(ele, "ReverbDelay", pReverb->prop.ReverbDelay);
		AddElement(ele, "ModulationTime", pReverb->prop.ModulationTime);
		AddElement(ele, "ModulationDepth", pReverb->prop.ModulationDepth);
		AddElement(ele, "HFReference", pReverb->prop.HFReference);
		AddElement(ele, "LFReference", pReverb->prop.LFReference);
		AddElement(ele, "Diffusion", pReverb->prop.Diffusion);
		AddElement(ele, "Density", pReverb->prop.Density);
		AddElement(ele, "Flags", pReverb->prop.Flags);
		pReverb->prop = pReverb->prop;
	}

	TiXmlPrinter printer;
	doc.Accept(&printer);

	FILE* pFile = 0;
	if(0 != fopen_s(&pFile, szPath, "w"))
	{
		m_pEventSystem->GetLogManager()->GetEngineLog()->Write(LOG_ERROR, "ReverbDataManager::SaveXML 无法打开文件%s", szPath);
		return false;
	}
	fwrite((LPVOID)printer.CStr(), printer.Size(), 1, pFile);
	fclose(pFile);
	return true;
}

#define CREATE_PRESET(id, PRESET, NAME) REVERB* pReverb##id = new REVERB;\
REVERB_PROPERTIES prop##id = PRESET;\
pReverb##id->strName = NAME;\
pReverb##id->prop = prop##id;\
m_arrReverb.push_back(pReverb##id);

void ReverbDataManager::createPresets()
{
	CREATE_PRESET(1, REVERB_PRESET_OFF, "Off");
	CREATE_PRESET(2, REVERB_PRESET_GENERIC, "Generic");
	CREATE_PRESET(3, REVERB_PRESET_PADDEDCELL, "Padded Cell");
	CREATE_PRESET(4, REVERB_PRESET_ROOM, "Room");
	CREATE_PRESET(5, REVERB_PRESET_BATHROOM, "Bathroom");
	CREATE_PRESET(6, REVERB_PRESET_LIVINGROOM, "Living Room");
	CREATE_PRESET(7, REVERB_PRESET_STONEROOM, "Stone Room");
	CREATE_PRESET(8, REVERB_PRESET_AUDITORIUM, "Auditorium");
	CREATE_PRESET(9, REVERB_PRESET_CONCERTHALL, "Concert Hall");
	CREATE_PRESET(10, REVERB_PRESET_CAVE, "Cave");
	CREATE_PRESET(11, REVERB_PRESET_ARENA, "Arena");
	CREATE_PRESET(12, REVERB_PRESET_HANGAR, "Hangar");
	CREATE_PRESET(13, REVERB_PRESET_CARPETTEDHALLWAY, "Carpetted Hallway");
	CREATE_PRESET(14, REVERB_PRESET_HALLWAY, "Hallway");
	CREATE_PRESET(15, REVERB_PRESET_STONECORRIDOR, "Stone Corridor");
	CREATE_PRESET(16, REVERB_PRESET_ALLEY, "Alley");
	CREATE_PRESET(17, REVERB_PRESET_FOREST, "Forest");
	CREATE_PRESET(18, REVERB_PRESET_CITY, "City");
	CREATE_PRESET(19, REVERB_PRESET_MOUNTAINS, "Mountains");
	CREATE_PRESET(20, REVERB_PRESET_QUARRY, "Quarry");
	CREATE_PRESET(21, REVERB_PRESET_PLAIN, "Plain");
	CREATE_PRESET(22, REVERB_PRESET_PARKINGLOT, "Parking Lot");
	CREATE_PRESET(23, REVERB_PRESET_SEWERPIPE, "Sewer Pipe");
	CREATE_PRESET(24, REVERB_PRESET_UNDERWATER, "Underwater");
}

int ReverbDataManager::GetReverbNum() const
{
	return (int)m_arrReverb.size();
}

REVERB* ReverbDataManager::GetReverbByIndex(int idx) const
{
	return m_arrReverb[idx];
}

REVERB* ReverbDataManager::CreateReverb(const char* szName)
{
	REVERB* pReverb = new REVERB;
	pReverb->strName = szName;
	m_arrReverb.push_back(pReverb);
	return pReverb;
}

bool ReverbDataManager::DeleteReverb(REVERB* pReverb)
{
	for (size_t i=0; i<m_arrReverb.size(); ++i)
	{
		if(pReverb == m_arrReverb[i])
		{
			delete m_arrReverb[i];
			m_arrReverb.erase(m_arrReverb.begin() + i);
			return true;
		}
	}
	return false;
}

REVERB* ReverbDataManager::GetReverbByName(const char* szName) const
{
	for (size_t i=0; i<m_arrReverb.size(); ++i)
	{
		if(0 == strcmp(szName, m_arrReverb[i]->strName.c_str()))
		{
			return m_arrReverb[i];
		}
	}
	return 0;
}