#pragma once

#include "FAudioEngine.h"
#include "FGuid.h"
#include "FExportDecl.h"

#include <string>


class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	typedef enum
	{
		PLAY_MODE_SEQUENTIAL_EVENT_RESTART,	// 事件重新发生时，从第一个文件开始播放
		PLAY_MODE_SEQUENTIAL,				// 事件重新发生时，从当前文件开始播放
		PLAY_MODE_RANDOM,					// 随机播放，允许重复
		PLAY_MODE_RANDOM_NOREPEAT			// 随机播放，不允许重复
	}PLAY_MODE;	

	struct SOUND_DEF_PROPERTY
	{
		PLAY_MODE	playMode;				// 播放模式
		float		fVolume;				// 音量调节[-60,0]dB，默认0
		float		fVolumeRandomization;	// 音量随机[-60,0]dB，默认0
		float		fPitch;					// 音高调节[-24,24]半音，默认0
		float		fPitchRandomization;	// 音高随机[-24,24]半音，默认0
		int			iIntervalTime;			// 时间间隔[0,1200]s，默认0
		int			iIntervalTimeRandomization;	// 时间间隔[0,1200]s，默认0
		void*		userdata;				// 用户数据
	};

	class EventProject;

	class _EXPORT_DLL_ SoundDef
	{
		friend class SoundDefManager;
		friend class SoundDefArchive;
	public:
		SoundDef(void);
		~SoundDef(void);
	public:
		bool					Init(EventProject* pEventProject);
		const char*				GetName() const { return m_strName.c_str(); }
		void					SetName(const char* szName) { m_strName = szName; }
		const char*				GetNote() const { return m_strNote.c_str(); }
		void					SetNote(const char* szNote) { m_strNote = szNote; }
		void					GetProperty(SOUND_DEF_PROPERTY& prop) const { prop = m_SoundDefProperty; }
		void					SetProperty(SOUND_DEF_PROPERTY prop) { m_SoundDefProperty = prop; }
		AudioGroup*				GetAudioGroupByIndex(int idx) const;
		int						GetAudioGroupNum() const { return (int)m_listAudioGroup.size(); }
		AudioGroup*				AddAudioGroup();
		bool					MoveUpAudioGroup(AudioGroup* pAudioGroup);
		bool					MoveDownAudioGroup(AudioGroup* pAudioGroup);
		bool					RemoveAudioGroup(AudioGroup* pAudioGroup);
		const Guid&				GetGuid() const { return m_Guid; }
		bool					Load(AFileImage* pFile);
		bool					Save(AFile* pFile);
		bool					LoadXML(TiXmlElement* root, bool bPreset = false);
		bool					SaveXML(TiXmlElement* pParent, bool bPreset = false);
		void					SetParentArchive(SoundDefArchive* pParentArchive) { m_pParentArchive = pParentArchive; }
		SoundDefArchive*		GetParentArchive() const { return m_pParentArchive; }
	protected:
		void					release();
	protected:
		AudioGroupList			m_listAudioGroup;
		SOUND_DEF_PROPERTY		m_SoundDefProperty;
		std::string				m_strName;
		std::string				m_strNote;
		Guid					m_Guid;
		EventProject*			m_pEventProject;
		SoundDefArchive*		m_pParentArchive;
	};
}