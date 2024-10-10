#include "FSoundPlayList.h"
#include "FSoundDef.h"
#include "FSoundInstanceTemplate.h"
#include "FAudioGroup.h"

using namespace AudioEngine;

/*
SoundPlayList::SoundPlayList(void)
{
	m_iCurIndex = -1;
}

SoundPlayList::~SoundPlayList(void)
{
}

bool SoundPlayList::GenerateList(SoundInstanceTemplate* pSoundInstanceTemplate)
{
	m_iLastCurIndex = m_iCurIndex;
	m_arrLastPlayIndex = m_arrPlayIndex;

	m_iCurIndex = -1;
	m_arrPlayIndex.clear();

	m_setNoRepeatIndex.clear();

	if(!pSoundInstanceTemplate)
		return false;
	m_pSoundInstanceTemplate = pSoundInstanceTemplate;
	int idx = getNextPlayIndex();
	while (idx != -1)
	{
		m_arrPlayIndex.push_back(idx);
		idx = getNextPlayIndex();
	}

	SoundDef* pSoundDef = m_pSoundInstanceTemplate->GetSoundDef();

	PlayIndexArray arrPlayIndex;
	for (size_t i=0; i<m_arrPlayIndex.size(); ++i)
	{
		int iGroupIdx = m_arrPlayIndex[i];
		int iStartIndex = getAudioGroupStartIndex(iGroupIdx);
		int iAudioTemplateNum = pSoundDef->GetAudioGroupByIndex(iGroupIdx)->GetAudioTemplateNum();
		for (int j=0; j<iAudioTemplateNum; ++j)
		{
			arrPlayIndex.push_back(iStartIndex + j);
		}
	}
	m_arrPlayIndex.swap(arrPlayIndex);

	return true;
}

int SoundPlayList::getAudioGroupStartIndex(int iGroupIdx) const
{
	if(!m_pSoundInstanceTemplate)
		return -1;
	SoundDef* pSoundDef = m_pSoundInstanceTemplate->GetSoundDef();
	int num = pSoundDef->GetAudioGroupNum();
	if(iGroupIdx >= num || iGroupIdx < 0)
		return -1;
	int idx = 0;
	for (int i=0; i<iGroupIdx; ++i)
	{
		idx += pSoundDef->GetAudioGroupByIndex(i)->GetAudioTemplateNum();
	}
	return idx;
}

int SoundPlayList::getNextPlayIndex() const
{
	if(!m_pSoundInstanceTemplate)
		return -1;
	SoundDef* pSoundDef = m_pSoundInstanceTemplate->GetSoundDef();
	int num = pSoundDef->GetAudioGroupNum();
	if(num == 0)
		return -1;

	int iPlayListCnt = (int)m_arrPlayIndex.size();

	SOUND_DEF_PROPERTY prop;
	pSoundDef->GetProperty(prop);

	SOUND_INSTANCE_PROPERTY InsProp;
	m_pSoundInstanceTemplate->GetProperty(InsProp);

	switch(InsProp.playNum)
	{
	case PLAY_NUM_ONE_AT_A_TIME:
		{
			switch (prop.playMode)
			{
			case PLAY_MODE_SEQUENTIAL_EVENT_RESTRT:
				{
					switch (InsProp.stopMode)
					{
					case STOP_MODE_CUTOFF:
					case STOP_MODE_PLAYTOEND:
						if(iPlayListCnt < InsProp.iLoopCount)
							return 0;
						else
							return -1;
						break;
					}
				}
				break;
			case PLAY_MODE_SEQUENTIAL:
				{
					int retidx = -1;
					if(m_arrPlayIndex.size() == 0)
					{
						if(m_arrLastPlayIndex.size() == 0)
							retidx = 0;
						else
						{
							retidx = (m_arrLastPlayIndex[m_iLastCurIndex%m_arrLastPlayIndex.size()]+1)%num;
						}
					}
					else
					{
						retidx = m_arrPlayIndex.back();
					}
					switch (InsProp.stopMode)
					{
					case STOP_MODE_CUTOFF:
					case STOP_MODE_PLAYTOEND:
						if(iPlayListCnt < InsProp.iLoopCount)
							return retidx;
						else
							return -1;
						break;
					}
				}
				break;
			case PLAY_MODE_RANDOM:				
			case PLAY_MODE_RANDOM_NOREPEAT:
				{
					int idx;
					if(iPlayListCnt == 0)
						idx = rand()%num;
					switch (InsProp.stopMode)
					{
					case STOP_MODE_CUTOFF:
					case STOP_MODE_PLAYTOEND:
						if(iPlayListCnt < InsProp.iLoopCount)
							return idx;
						else
							return -1;
						break;
					}
				}
				break;
			}
		}
		break;
	case PLAY_NUM_ALL:
		{			
			switch (prop.playMode)
			{
			case PLAY_MODE_SEQUENTIAL_EVENT_RESTRT:
				{
					switch (InsProp.stopMode)
					{
					case STOP_MODE_CUTOFF:
					case STOP_MODE_PLAYTOEND:
						if(iPlayListCnt < InsProp.iLoopCount * num)
						{
							return iPlayListCnt%num;
						}
						else
							return -1;
						break;
					}
				}
				break;
			case PLAY_MODE_SEQUENTIAL:
				{
					int retidx = -1;
					if(m_arrPlayIndex.size() == 0)
					{
						if(m_arrLastPlayIndex.size() == 0)
							retidx = 0;
						else
						{
							retidx = (m_arrLastPlayIndex[m_iLastCurIndex%m_arrLastPlayIndex.size()]+1)%num;
						}
					}
					else
					{
						retidx = (m_arrPlayIndex.back()+1)%num;
					}

					switch (InsProp.stopMode)
					{
					case STOP_MODE_CUTOFF:
					case STOP_MODE_PLAYTOEND:
						if(iPlayListCnt < InsProp.iLoopCount * num)
						{
							return retidx;
						}
						else
							return -1;
						break;
					}
				}
				break;
			case PLAY_MODE_RANDOM:
				{
					int idx = rand()%num;
					switch (InsProp.stopMode)
					{
					case STOP_MODE_CUTOFF:
					case STOP_MODE_PLAYTOEND:
						if(iPlayListCnt < InsProp.iLoopCount * num)
						{
							return idx;
						}
						else
							return -1;
						break;
					}
				}
				break;
			case PLAY_MODE_RANDOM_NOREPEAT:
				{
					int idx = getNoRepeatPlayIndex();					
					switch (InsProp.stopMode)
					{
					case STOP_MODE_CUTOFF:
					case STOP_MODE_PLAYTOEND:
						if(iPlayListCnt < InsProp.iLoopCount * num)
						{
							if(idx == -1)
							{
								m_setNoRepeatIndex.clear();
								idx = getNoRepeatPlayIndex();
							}
							m_setNoRepeatIndex.insert(idx);
							return idx;
						}
						else
							return -1;
						break;
					}
				}
				break;
			}
		}
		break;
	}
	return -1;
}

int SoundPlayList::getNoRepeatPlayIndex() const
{
	int num = m_pSoundInstanceTemplate->GetSoundDef()->GetAudioGroupNum();
	if(num == (int)m_setNoRepeatIndex.size())
		return -1;
	while (1)
	{
		int idx = rand()%num;
		if(m_setNoRepeatIndex.find(idx) == m_setNoRepeatIndex.end())
			return idx;
	}
	return -1;
}

int SoundPlayList::GetPlayIndexByIndex(int idx) const
{
	if(idx < 0 || idx >= GetPlayListNum())
		return -1;
	return m_arrPlayIndex[idx];
}

bool SoundPlayList::GetNextPlayIndex(int& idx)
{
	++m_iCurIndex;
	m_iCurIndex = m_iCurIndex%GetPlayListNum();
	idx = GetPlayIndexByIndex(m_iCurIndex);
	if(idx == -1)
		return false;
	return true;
}

bool SoundPlayList::GetCurPlayIndex(int& idx) const
{	
	idx = GetPlayIndexByIndex(m_iCurIndex);
	if(idx == -1)
		return false;
	return true;
}

*/


SoundPlayList::SoundPlayList()
{
	m_iPlayIndex = 0;
	m_pSoundInstanceTemplate = 0;
	m_bHasNextIndex = true;
	m_iCurIndex = 0;
}

SoundPlayList::~SoundPlayList()
{
}

int SoundPlayList::getAudioGroupStartRef(int iGroupIndex)
{
	if (!m_pSoundInstanceTemplate)
		return 0;
	SoundDef* pSoundDef = m_pSoundInstanceTemplate->GetSoundDef();
	if(!pSoundDef)
		return 0;
	int iAudioGroupNum = pSoundDef->GetAudioGroupNum();
	int iStartRef = 0;
	for (int i=0; i<iGroupIndex; ++i)
	{
		iStartRef += pSoundDef->GetAudioGroupByIndex(i)->GetAudioTemplateNum();
	}
	return iStartRef;
}

RefArray SoundPlayList::calRefArrayByPlayMode(int iNum, SPL_PLAY_MODE playMode)
{
	RefArray arrRef;
	switch(playMode)
	{
	case SPLPM_SEQUENTIAL:
		for (int i=0; i<iNum; ++i)
		{
			arrRef.push_back(i);
		}
		return arrRef;
	case SPLPM_RANDOM:
		for (int i=0; i<iNum; ++i)
		{
			arrRef.push_back(rand()%iNum);
		}
		return arrRef;
	case SPLPM_RANDOM_NOREPEAT:
		{
			RefSet setNoRepeatRef;
			while(1)
			{
				if(arrRef.size() == iNum)
					return arrRef;
				int idx = rand()%iNum;
				if(setNoRepeatRef.find(idx) != setNoRepeatRef.end())
					continue;
				arrRef.push_back(idx);
				setNoRepeatRef.insert(idx);
			}
		}
	}
	return arrRef;
}

void SoundPlayList::calOneLoopRefArray()
{
	if (!m_pSoundInstanceTemplate)
		return;
	SoundDef* pSoundDef = m_pSoundInstanceTemplate->GetSoundDef();
	if(!pSoundDef)
		return;
	m_arrOneLoopPlayRef.clear();

	SOUND_DEF_PROPERTY soundDefProp;
	pSoundDef->GetProperty(soundDefProp);
	soundDefProp.playMode;

	RefArray arrOneLoopGroupPlayRef;
	int iAudioGroupNum = pSoundDef->GetAudioGroupNum();
	switch(soundDefProp.playMode)
	{
	case PLAY_MODE_SEQUENTIAL_EVENT_RESTART:
	case PLAY_MODE_SEQUENTIAL:
		arrOneLoopGroupPlayRef = calRefArrayByPlayMode(iAudioGroupNum, SPLPM_SEQUENTIAL);
		break;
	case PLAY_MODE_RANDOM:
		arrOneLoopGroupPlayRef = calRefArrayByPlayMode(iAudioGroupNum, SPLPM_RANDOM);
		break;
	case PLAY_MODE_RANDOM_NOREPEAT:
		arrOneLoopGroupPlayRef = calRefArrayByPlayMode(iAudioGroupNum, SPLPM_RANDOM_NOREPEAT);
		break;
	}

	size_t size = arrOneLoopGroupPlayRef.size();
	for (size_t i=0; i<size; ++i)
	{
		int iGroupRef = arrOneLoopGroupPlayRef[i];
		int iStartGroupRef = getAudioGroupStartRef(iGroupRef);
		AudioGroup* pAudioGroup = pSoundDef->GetAudioGroupByIndex(iGroupRef);
		SPL_PLAY_MODE playMode;
		switch(pAudioGroup->GetPlayMode())
		{
		case AGPM_SEQUENTIAL:
			playMode = SPLPM_SEQUENTIAL;
			break;
		case AGPM_RANDOM:
			playMode = SPLPM_RANDOM;
			break;
		case AGPM_RANDOM_NOREPEAT:
			playMode = SPLPM_RANDOM_NOREPEAT;
			break;
		}
		int iAudioTemplateNum = pAudioGroup->GetAudioTemplateNum();
		if(iAudioTemplateNum > 1)
		{
			RefArray arrPlayRef = calRefArrayByPlayMode(iAudioTemplateNum, playMode);
			for (size_t j = 0; j < arrPlayRef.size(); ++j)
			{
				m_arrOneLoopPlayRef.push_back(iStartGroupRef + arrPlayRef[j]);
			}
		}
		else if(iAudioTemplateNum == 1)
		{
			m_arrOneLoopPlayRef.push_back(iStartGroupRef);
		}
	}
}

bool SoundPlayList::Init(SoundInstanceTemplate* pSoundInstanceTemplate)
{
	if(!pSoundInstanceTemplate)
		return false;
	m_pSoundInstanceTemplate = pSoundInstanceTemplate;
	if(m_pSoundInstanceTemplate->GetAudioInstanceNum() == 0)
		return false;
	return true;
}

bool SoundPlayList::GetFirstIndex(int& index, int& iLoopCount)
{
	if(!m_pSoundInstanceTemplate)
		return false;
	SoundDef* pSoundDef = m_pSoundInstanceTemplate->GetSoundDef();
	if(!pSoundDef)
		return false;
	int iAudioInstanceNum = m_pSoundInstanceTemplate->GetAudioInstanceNum();
	if(0 == iAudioInstanceNum)
		return false;

	calOneLoopRefArray();

	m_iPlayIndex = 0;
	index = m_arrOneLoopPlayRef[0];

	m_bHasNextIndex = true;

	SOUND_DEF_PROPERTY soundDefProp;
	pSoundDef->GetProperty(soundDefProp);	

	SOUND_INSTANCE_PROPERTY soundInsProp;
	m_pSoundInstanceTemplate->GetProperty(soundInsProp);

	bool bHasInterval = false;
	if(soundDefProp.iIntervalTime != 0 || soundDefProp.iIntervalTimeRandomization != 0 ||
		soundInsProp.iIntervalTime != 0 || soundInsProp.iIntervalTimeRandomization != 0)
		bHasInterval = true;


	if(soundInsProp.playNum == PLAY_NUM_ONE_AT_A_TIME)
	{
		if(soundDefProp.playMode == PLAY_MODE_SEQUENTIAL_EVENT_RESTART && !bHasInterval)
		{
			m_bHasNextIndex = false;
			iLoopCount = soundInsProp.iLoopCount;
			return true;
		}
	}
	if(iAudioInstanceNum == 1 && !bHasInterval)
	{
		m_bHasNextIndex = false;
		iLoopCount = soundInsProp.iLoopCount;
		return true;
	}
	iLoopCount = 1;
	m_iPlayIndex += 1;
	m_iCurIndex = m_arrOneLoopPlayRef[0];
	return true;
}

bool SoundPlayList::GetNextIndex(int& index, int& iLoopCount)
{
	if(!m_pSoundInstanceTemplate)
		return false;
	int iAudioInstanceNum = m_pSoundInstanceTemplate->GetAudioInstanceNum();
	if(0 == iAudioInstanceNum)
		return false;
	if(!m_bHasNextIndex)
		return false;

	SOUND_INSTANCE_PROPERTY soundInsProp;
	m_pSoundInstanceTemplate->GetProperty(soundInsProp);		

	
	if(soundInsProp.playNum == PLAY_NUM_ONE_AT_A_TIME)
	{
		if(soundInsProp.iLoopCount == m_iPlayIndex)
			return false;
	}
	else
	{
		if(soundInsProp.iLoopCount * iAudioInstanceNum == m_iPlayIndex)
			return false;
	}

	int iRemain = m_iPlayIndex%iAudioInstanceNum;
	if(iRemain == 0)
		calOneLoopRefArray();
	index = m_arrOneLoopPlayRef[iRemain];
	iLoopCount = 1;
	m_iPlayIndex += 1;
	m_iCurIndex = index;
	return true;
}

bool SoundPlayList::GetCurPlayIndex(int& idx) const
{
	idx = m_iCurIndex;
	return true;
}