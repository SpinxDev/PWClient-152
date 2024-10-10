#pragma once

#include <vector>
#include <set>

namespace AudioEngine
{
	class SoundInstanceTemplate;

	/*
	typedef std::vector<int> PlayIndexArray;
	typedef std::set<int> IndexSet;

	class SoundPlayList
	{
	public:
		SoundPlayList(void);
		~SoundPlayList(void);
	public:
		bool				GenerateList(SoundInstanceTemplate* pSoundInstanceTemplate);
		int					GetPlayListNum() const { return (int)m_arrPlayIndex.size(); }
		int					GetPlayIndexByIndex(int idx) const;
		bool				GetNextPlayIndex(int& idx);
		bool				GetCurPlayIndex(int& idx) const;
		int					GetCurIndex() const { return m_iCurIndex; }
	protected:
		int					getAudioGroupStartIndex(int iGroupIdx) const;
		int					getNextPlayIndex() const;
		int					getNoRepeatPlayIndex() const;
	protected:
		PlayIndexArray		m_arrPlayIndex;
		PlayIndexArray		m_arrLastPlayIndex;
		SoundInstanceTemplate*	m_pSoundInstanceTemplate;
		mutable IndexSet	m_setNoRepeatIndex;
		int					m_iCurIndex;
		int					m_iLastCurIndex;		// 上次播放停止时的播放位置
	};
	*/

	typedef enum
	{
		SPLPM_SEQUENTIAL,
		SPLPM_RANDOM,
		SPLPM_RANDOM_NOREPEAT
	}SPL_PLAY_MODE;

	typedef std::vector<int> RefArray;
	typedef std::set<int> RefSet;

	class SoundPlayList
	{
	public:
		SoundPlayList();
		~SoundPlayList();
	public:
		bool					Init(SoundInstanceTemplate* pSoundInstanceTemplate);
		bool					GetFirstIndex(int& index, int& iLoopCount);
		bool					GetNextIndex(int& index, int& iLoopCount);
		bool					GetCurPlayIndex(int& idx) const;
	protected:
		RefArray				calRefArrayByPlayMode(int iNum, SPL_PLAY_MODE playMode);
		void					calOneLoopRefArray();
		int						getAudioGroupStartRef(int iGroupIndex);
	protected:
		SoundInstanceTemplate*	m_pSoundInstanceTemplate;
		RefArray				m_arrOneLoopPlayRef;
		int						m_iPlayIndex;
		int						m_iCurIndex;
		bool					m_bHasNextIndex;
	};
}
