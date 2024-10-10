//#include "AGPAPerfIntegration.h"
#include "FEventSystem.h"
#include "FEventProject.h"
#include "FEffectFactory.h"
#include "FReverb.h"
#include "FEventManager.h"
#include "FLogManager.h"
#include "FEventGroup.h"
#include "xml/xmlcommon.h"
#include "FAudioManager.h"
#include "FReverbDataManager.h"
#include "FIntervalManager.h"
#include "FAudioInstanceManager.h"
#include <fmod.hpp>
#include <ctime>
#include <vector>
#include <AMemory.h>
#include <AFileImage.h>

using namespace AudioEngine;

FMOD_RESULT F_CALLBACK afile_open(const char* name, int unicode, unsigned int *filesize, void** handle, void** userdata)
{
	if(!name || name[0] == 0)
		return FMOD_ERR_INVALID_HANDLE;
	AFileImage* pFile = new AFileImage;
	if(!pFile->Open(name, AFILE_OPENEXIST))
	{		
		return FMOD_ERR_FILE_NOTFOUND;
	}
	*filesize = pFile->GetFileLength();
	*handle = pFile;
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK afile_close(void* handle, void* userdata)
{
	if(!handle)
		return FMOD_ERR_INVALID_HANDLE;
	AFileImage* pFile = (AFileImage*)handle;
	if(!pFile->Close())
	{
		delete pFile;
		return FMOD_ERR_FILE_UNWANTED;
	}
	delete pFile;
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK afile_read(void* handle, void* buffer, unsigned int sizebytes, unsigned int* bytesread, void* userdata)
{
	if(!handle)
		return FMOD_ERR_INVALID_HANDLE;
	AFileImage* pFile = (AFileImage*)handle;
	DWORD dwRead;
	if(!pFile->Read(buffer, sizebytes, &dwRead))
	{
		if(pFile->GetPos() + sizebytes > pFile->GetFileLength())
			return FMOD_ERR_FILE_EOF;
		else
			return FMOD_ERR_FILE_UNWANTED;
	}
	*bytesread = dwRead;
	return FMOD_OK;
}

FMOD_RESULT F_CALLBACK afile_seek(void* handle, unsigned int pos, void* userdata)
{
	if(!handle)
		return FMOD_ERR_INVALID_HANDLE;
	AFileImage* pFile = (AFileImage*)handle;
	if(!pFile->Seek(pos, AFILE_SEEK_SET))
		return FMOD_ERR_FILE_COULDNOTSEEK;
	return FMOD_OK;
}

EventSystem::EventSystem(void)
: m_pSystem(0)
, m_pEffectFactory(0)
, m_pLogManager(0)
, m_pReverbDataManager(0)
, m_bFailedInit(false)
, m_pCompressorDsp(0)
{
}

EventSystem::~EventSystem(void)
{
	release();
}

bool EventSystem::Init(int maxchannels, INITFLAGS flags, void *extradriverdata)
{
	release();
	m_pLogManager = new LogManager;
	if(!m_pLogManager->Init())
	{
		delete m_pLogManager;
		m_pLogManager = 0;
		m_bFailedInit = true;
		return false;
	}
	m_pLogManager->GetEngineLog()->EnableFlushImmediate();

	srand((unsigned int)(time(0)*time(0)));
	if(FMOD_OK != FMOD::System_Create(&m_pSystem))
	{
		m_pLogManager->GetEngineLog()->Write(LOG_ERROR, "EventSystem::Init, System_Create, 创建FMOD_System失败");
		m_bFailedInit = true;
		return false;
	}
	if(FMOD_OK != m_pSystem->init(maxchannels, flags, extradriverdata))
	{
		m_pLogManager->GetEngineLog()->Write(LOG_ERROR, "EventSystem::Init, System->init, 音频引擎初始化失败");
		m_bFailedInit = true;
		return false;	
	}
	if(FMOD_OK != m_pSystem->setFileSystem(afile_open, afile_close, afile_read, afile_seek, 0, 0, 2048))
	{
		m_pLogManager->GetEngineLog()->Write(LOG_ERROR, "EventSystem::Init, System->setFileSystem, 设置FILE回调函数失败");
		m_bFailedInit = true;
		return false;
	}

	if(FMOD_OK == m_pSystem->createDSPByType(FMOD_DSP_TYPE_COMPRESSOR, &m_pCompressorDsp))
	{
		FMOD::ChannelGroup* pMasterChannelGroup = 0;
		if(FMOD_OK == m_pSystem->getMasterChannelGroup(&pMasterChannelGroup))
		{
			m_pCompressorDsp->setParameter(0, 0.0f);
			m_pCompressorDsp->setParameter(1, 50.0f);
			m_pCompressorDsp->setParameter(2, 50.0f);
			m_pCompressorDsp->setParameter(3, 0.0f);

			pMasterChannelGroup->addDSP(m_pCompressorDsp, 0);
		}			
	}

	m_pEffectFactory = new EffectFactory;
	if(!m_pEffectFactory->Init(this))
	{
		m_pLogManager->GetEngineLog()->Write(LOG_ERROR, "EventSystem::Init, m_pEffectFactory->init, 效果器工厂初始化失败");
		m_bFailedInit = true;
		return false;
	}

	if(!AudioInstanceManager::GetInstance().Init(this))
	{
		m_pLogManager->GetEngineLog()->Write(LOG_ERROR, "EventSystem::Init, AudioInstanceManager->init, 音频文件实例管理器初始化失败");
		m_bFailedInit = true;
		return false;
	}
	m_dwTickCount = GetTickCount();
	/* FOR DEBUG
	FMOD::Debug_SetLevel(FMOD_DEBUG_LEVEL_ERROR|FMOD_DEBUG_LEVEL_WARNING);
	//*/
	return true;
}

bool EventSystem::Load(const char* szPath, EventProject*& pEventProject)
{
	if(szPath == 0 || strlen(szPath) == 0)
		return false;	
	pEventProject = new EventProject;
	if(!pEventProject->Init(this))
	{
		delete pEventProject;
		return false;
	}
	if(!pEventProject->LoadXML(szPath))
	{
		delete pEventProject;
		return false;
	}
	m_listEventProject.push_back(pEventProject);	
	return true;
}

bool EventSystem::LoadBinary(const char* szPath, EventProject*& pEventProject)
{
	if(szPath == 0 || strlen(szPath) == 0)
		return false;	
	pEventProject = new EventProject;
	if(!pEventProject->Init(this))
	{
		delete pEventProject;
		return false;
	}
	if(!pEventProject->Load(szPath))
	{
		delete pEventProject;
		return false;
	}
	m_listEventProject.push_back(pEventProject);	
	return true;
}

void EventSystem::release()
{
	AudioInstanceManager::GetInstance().Release();

	releaseProjects();

	ReverbList::const_iterator itReverb = m_listReverb.begin();
	for (; itReverb!=m_listReverb.end(); ++itReverb)
	{
		Reverb* pReverb = *itReverb;
		if(!pReverb)
			continue;
		delete pReverb;
	}
	m_listReverb.clear();	

	if(m_pCompressorDsp)
	{
		m_pCompressorDsp->remove();
		m_pCompressorDsp->release();
	}
	m_pCompressorDsp = 0;

	delete m_pReverbDataManager;
	m_pReverbDataManager = 0;
	delete m_pEffectFactory;
	m_pEffectFactory = 0;
	delete m_pLogManager;
	m_pLogManager = 0;
	if(m_pSystem)
		m_pSystem->release();
	m_pSystem = 0;
}

void EventSystem::releaseProjects()
{
	EventProjectList::const_iterator it = m_listEventProject.begin();
	for (; it!=m_listEventProject.end(); ++it)
	{
		EventProject* pEventProject = *it;
		if(!pEventProject)
			continue;
		delete pEventProject;
	}
	m_listEventProject.clear();
}

int EventSystem::GetProjectNum() const
{
	return (int)m_listEventProject.size();
}

EffectFactory* EventSystem::GetEffectFactory() const
{
	return m_pEffectFactory;
}

FMOD::System* EventSystem::GetSystem() const
{
	return m_pSystem;
}

bool EventSystem::Update()
{
	if(!m_pSystem)
		return true;
	DWORD dwCurTickCount = GetTickCount();
	DWORD dwEllapsedTime = dwCurTickCount - m_dwTickCount;
	IntervalManager::GetInstance().Update(dwEllapsedTime);
	m_dwTickCount = dwCurTickCount;

	EventProjectList::const_iterator it=m_listEventProject.begin();
	for (; it!=m_listEventProject.end(); ++it)
	{
		EventProject* pEventProject = *it;
		if(!pEventProject)
			return false;
		if(!pEventProject->GetEventManager()->Update(dwEllapsedTime))
			return false;
	}

	AudioInstanceManager::GetInstance().Update(dwEllapsedTime);

	{
		CriticalSectionController csc(&m_csForThreadSafe);
		FMOD_RESULT fmod_result = m_pSystem->update();
		if(FMOD_OK != fmod_result)
		{
			//* FOR DEBUG
			static int s_iErrorCount = 0;
			static FMOD_RESULT fr_last_err = FMOD_OK;
			if(s_iErrorCount %500 == 0 || fr_last_err != fmod_result)
			{
				m_pLogManager->GetEngineLog()->Write(LOG_ERROR, "FMOD update failed：%d, Error count：%d ", fmod_result, s_iErrorCount);
			}
			++s_iErrorCount;		
			fr_last_err = fmod_result;
			//*/
		}
	}	

/* FOR DEBUG
	static int s_iCount = 0;
	if(s_iCount % 500 == 0)
	{
	unsigned int uiBits = GetUsedMemoryBits();
	float fTotal = 0, fDsp = 0;
	GetCPUUsage(&fDsp,0,0,0,&fTotal);
	m_pLogManager->GetEngineLog()->Write(LOG_ERROR, "内存使用：%.2f M\tCPU占用：%.2f %%, DSP CPU：%.2f %%, Channels：%d", uiBits/1000000.0f, fTotal, fDsp, GetPlayingChannelsNum());

	unsigned int uiMemDsp = 0, uiMemChannel = 0, uiMemChannelGroup = 0, uiMemSound = 0;
	m_pSystem->getMemoryInfo(FMOD_MEMBITS_DSP|FMOD_MEMBITS_DSPCODEC|FMOD_MEMBITS_DSPCONNECTION, 0, &uiMemDsp, 0);
	m_pSystem->getMemoryInfo(FMOD_MEMBITS_CHANNEL, 0, &uiMemChannel, 0);
	m_pSystem->getMemoryInfo(FMOD_MEMBITS_CHANNELGROUP, 0, &uiMemChannelGroup, 0);
	m_pSystem->getMemoryInfo(FMOD_MEMBITS_SOUND, 0, &uiMemSound, 0);
	m_pLogManager->GetEngineLog()->Write(LOG_ERROR, "内存详细————DSP：%.2f M\tChannel：%.2f M\tChannel Group：%.2f M\tSound：%.2f M", uiMemDsp/1000000.0f, uiMemChannel/1000000.0f, uiMemChannelGroup/1000000.0f, uiMemSound/1000000.0f);

	}
	++s_iCount;
//*/

	return true;
}

Reverb* EventSystem::CreateReverb(const char* szName)
{
	Reverb* pReverb = new Reverb;
	if(!pReverb->Init(this))
	{
		delete pReverb;
		return 0;
	}
	pReverb->SetName(szName);
	m_listReverb.push_back(pReverb);
	return pReverb;
}

bool EventSystem::DestroyReverb(Reverb* pReverb)
{	
	ReverbList::iterator it = m_listReverb.begin();
	for (; it!=m_listReverb.end(); ++it)
	{
		if(pReverb == *it)
		{
			pReverb->SetActive(false);
			if(m_listReverb.size() == 1)
				return true;
			delete pReverb;
			m_listReverb.erase(it);
			return true;
		}
	}
	return false;
}

EventProject* EventSystem::CreateProject(const char* szName)
{
	if(szName == 0 || strlen(szName) == 0)
		return false;	
	EventProject* pEventProject = new EventProject;
	if(!pEventProject->Init(this))
	{
		delete pEventProject;
		return false;
	}
	pEventProject->SetName(szName);
	m_listEventProject.push_back(pEventProject);	
	return pEventProject;
}

EventProject* EventSystem::GetProjectByIndex(int idx) const
{
	EventProjectList::const_iterator it=m_listEventProject.begin();
	for (int i=0; it!=m_listEventProject.end(); ++it,++i)
	{
		if(i == idx)
			return *it;
	}
	return 0;
}

EventProject* EventSystem::GetProjectByName(const char* szName) const
{
	EventProjectList::const_iterator it=m_listEventProject.begin();
	for (; it!=m_listEventProject.end(); ++it)
	{
		EventProject* pEventProject = *it;
		if(!pEventProject)
			continue;
		if(0 == strcmp(pEventProject->GetName(), szName))
			return pEventProject;
	}
	return 0;
}

bool EventSystem::Set3DListenerAttributes(const VECTOR& pos, const VECTOR& vel, const VECTOR& forward, const VECTOR& up)
{
	FMOD_VECTOR fmod_pos, fmod_vel, fmod_forward, fmod_up;
	fmod_pos.x = pos.x;
	fmod_pos.y = pos.y;
	fmod_pos.z = pos.z;

	fmod_vel.x = vel.x;
	fmod_vel.y = vel.y;
	fmod_vel.z = vel.z;

	fmod_forward.x = forward.x;
	fmod_forward.y = forward.y;
	fmod_forward.z = forward.z;

	fmod_up.x = up.x;
	fmod_up.y = up.y;
	fmod_up.z = up.z;

	if(FMOD_OK!=m_pSystem->set3DListenerAttributes(0, &fmod_pos, &fmod_vel, &fmod_forward, &fmod_up))
		return false;
	return true;
}

bool EventSystem::LoadProjects()
{
	releaseProjects();
	if(m_bFailedInit)
	{
		m_pLogManager->GetEngineLog()->Write(LOG_ERROR, "EventSystem::LoadProjects 音频系统初始化失败，禁止加载音频工程。");
		return false;
	}
	if(m_strFolder.size() == 0)
		m_strFolder = ".";
	char szPath[MAX_PATH] = {0};
	char szFullPath[MAX_PATH] = {0};
	sprintf(szPath, "%s\\Projects\\ProjectList.txt", m_strFolder.c_str());

	AFileImage file;
	if(!file.Open(szPath, AFILE_OPENEXIST|AFILE_TEXT|AFILE_TEMPMEMORY))
	{
		m_pLogManager->GetEngineLog()->Write(LOG_ERROR, "EventSystem::LoadProjects 无法打开文件%s", szPath);
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

	TiXmlNode* pNode = doc.FirstChild("ProjectList");
	if (!pNode)
		return false;

	unsigned int ver = 0;

	TiXmlElement* root = pNode->ToElement();
	QueryElement(root, "version", ver);
	int iProjectNum = 0;
	QueryElement(root, "ProjectNum", iProjectNum);

	TiXmlNode* pProjectNode = root->FirstChild("Project");
	for (int i=0; i<iProjectNum; ++i)
	{
		std::string strName;
		QueryElement(pProjectNode, "Name", strName);
		sprintf(szFullPath, "%s\\Projects\\%s\\%s.xml", m_strFolder.c_str(), strName.c_str(), strName.c_str());
		pProjectNode = pProjectNode->NextSibling("Project");
		EventProject* pEventProject = 0;
		if(!Load(szFullPath, pEventProject))
			continue;
	}
	return true;
}

bool EventSystem::LoadBinaryProjects()
{
	releaseProjects();
	if(m_bFailedInit)
	{
		m_pLogManager->GetEngineLog()->Write(LOG_ERROR, "EventSystem::LoadProjects 音频系统初始化失败，禁止加载音频工程。");
		return false;
	}
	if(m_strFolder.size() == 0)
		m_strFolder = ".";
	char szPath[MAX_PATH] = {0};
	char szFullPath[MAX_PATH] = {0};
	sprintf(szPath, "%s\\Projects\\ProjectList.txt", m_strFolder.c_str());

	AFileImage file;
	if(!file.Open(szPath, AFILE_OPENEXIST|AFILE_TEXT|AFILE_TEMPMEMORY))
	{
		m_pLogManager->GetEngineLog()->Write(LOG_ERROR, "EventSystem::LoadProjects 无法打开文件%s", szPath);
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

	TiXmlNode* pNode = doc.FirstChild("ProjectList");
	if (!pNode)
		return false;

	unsigned int ver = 0;

	TiXmlElement* root = pNode->ToElement();
	QueryElement(root, "version", ver);
	int iProjectNum = 0;
	QueryElement(root, "ProjectNum", iProjectNum);

	TiXmlNode* pProjectNode = root->FirstChild("Project");
	for (int i=0; i<iProjectNum; ++i)
	{
		std::string strName;
		QueryElement(pProjectNode, "Name", strName);
		sprintf(szFullPath, "%s\\Projects\\%s\\%s.data", m_strFolder.c_str(), strName.c_str(), strName.c_str());
		pProjectNode = pProjectNode->NextSibling("Project");
		EventProject* pEventProject = 0;
		if(!LoadBinary(szFullPath, pEventProject))
			continue;
	}
	return true;
}

void splitByChar(const char* sz, char c, std::vector<std::string>& result)
{
	std::string str(sz);
	size_t pos = str.find_first_of(c);
	while(-1 != pos)
	{
		result.push_back(str.substr(0, pos));
		str = str.substr(pos+1, str.size());
		pos = str.find_first_of(c);
	}
	result.push_back(str);
}

Event* EventSystem::GetEvent(const char* szGuid)
{
	if(!szGuid || strlen(szGuid) == 0)
		return 0;
	Guid guid;
	if(!guid.BuildFromString(szGuid))
		return 0;
	EventProjectList::const_iterator it = m_listEventProject.begin();
	for (; it!=m_listEventProject.end(); ++it)
	{
		EventProject* pEventProject = *it;
		if(!pEventProject)
			continue;
		Event* pEvent = pEventProject->GetEventManager()->GetEvent(guid);
		if(!pEvent)
			continue;
		return pEvent;
	}
	return 0;
}

void EventSystem::SetRootFolder(const char* szFolder)
{
	m_strFolder = szFolder;
}

LogManager* EventSystem::GetLogManager() const
{
	return m_pLogManager;
}

bool EventSystem::InitReverbDataManager(const char* szPath)
{
	delete m_pReverbDataManager;
	m_pReverbDataManager = 0;
	m_pReverbDataManager = new ReverbDataManager;
	if(!m_pReverbDataManager->Init(this))
		return false;
	if(!m_pReverbDataManager->LoadXML(szPath))
		return false;
	return true;
}

ReverbDataManager* EventSystem::GetReverbDataManager() const
{
	return m_pReverbDataManager;
}

bool EventSystem::SetClassVolume(int iClass, float fVolume)
{
	int iEventProjectNum = GetProjectNum();
	for (int i=0; i<iEventProjectNum; ++i)
	{
		EventProject* pEventProject = GetProjectByIndex(i);
		if(!pEventProject)
			continue;
		if(!pEventProject->SetClassVolume(iClass, fVolume))
			continue;
	}
	return true;
}

bool EventSystem::SetClassMute(int iClass, bool bMute)
{
	int iEventProjectNum = GetProjectNum();
	for (int i=0; i<iEventProjectNum; ++i)
	{
		EventProject* pEventProject = GetProjectByIndex(i);
		if(!pEventProject)
			continue;
		if(!pEventProject->SetClassMute(iClass, bMute))
			continue;
	}
	return true;
}

unsigned int EventSystem::GetUsedMemoryBits() const
{
	if(!m_pSystem)
		return 0;
	unsigned int uiUsedMemory = 0;
	m_pSystem->getMemoryInfo(FMOD_MEMBITS_ALL, 0, &uiUsedMemory, 0);
	return uiUsedMemory;
}

bool EventSystem::GetCPUUsage(float* pfDsp, float* pfStream, float* pfGeometry, float* pfUpdate, float* pfTotal) const
{
	if(!m_pSystem)
		return false;
	if(FMOD_OK != m_pSystem->getCPUUsage(pfDsp, pfStream, pfGeometry, pfUpdate, pfTotal))
		return false;
	return true;
}

int EventSystem::GetPlayingChannelsNum() const
{
	if(!m_pSystem)
		return 0;
	int iNum = 0;
	if(FMOD_OK != m_pSystem->getChannelsPlaying(&iNum))
		return 0;
	return iNum;
}