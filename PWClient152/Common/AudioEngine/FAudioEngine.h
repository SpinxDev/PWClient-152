#pragma once

#include <vector.h>
#include <set>
#include <ctype.h>
#include <cmath>
#include "FExportDecl.h"

#pragma warning(disable:4996)

namespace FMOD
{
	class ChannelGroup;
	class Channel;
}

namespace AudioEngine
{
	typedef unsigned int INITFLAGS;

	class Audio;
	typedef abase::vector<Audio*> AudioList;

	class AudioGroup;
	typedef abase::vector<AudioGroup*> AudioGroupList;

	class AudioBankArchive;
	typedef abase::vector<AudioBankArchive*> AudioBankArchiveList;

	class AudioInstance;
	typedef abase::vector<AudioInstance*> AudioInstanceList;

	class SoundDef;
	typedef abase::vector<SoundDef*> SoundDefList;

	class SoundDefArchive;
	typedef abase::vector<SoundDefArchive*> SoundDefArchiveList;

	class EventGroup;
	typedef abase::vector<EventGroup*> EventGroupList;

	class EventProject;
	typedef abase::vector<EventProject*> EventProjectList;

	class Event;
	typedef abase::vector<Event*> EventList;

	class EventLayer;
	typedef abase::vector<EventLayer*> EventLayerList;

	typedef std::set<EventLayer*> EventLayerSet;

	class Effect;
	typedef abase::vector<Effect*> EffectList;

	class Reverb;
	typedef abase::vector<Reverb*> ReverbList;

	class EventParameter;
	typedef abase::vector<EventParameter*> EventParameterList;

	typedef EventParameter EventInstanceParameter;
	typedef abase::vector<EventInstanceParameter*> EventInstanceParameterList;

	class SoundInstance;
	typedef abase::vector<SoundInstance*> SoundInstanceList;
	typedef std::set<SoundInstance*> SoundInstanceSet;

	class SoundInstanceTemplate;
	typedef abase::vector<SoundInstanceTemplate*> SoundInstanceTemplateList;

	class AudioBank;
	typedef abase::vector<AudioBank*> AudioBankList;

	class EventLayerInstance;
	typedef abase::vector<EventLayerInstance*> EventLayerInstanceList;
	typedef std::set<EventLayerInstance*> EventLayerInstanceSet;

	class EventInstance;
	typedef abase::vector<EventInstance*> EventInstanceList;

	typedef abase::vector<FMOD::Channel*> ChannelList;

	typedef enum
	{
		MODE_2D,
		MODE_3D
	}EVENT_MODE;

	typedef enum 
	{
		ROLLOFF_INVERSE,
		ROLLOFF_LINEAR,
		ROLLOFF_LINEAR_SQUARE,
		ROLLOFF_CUSTOM
	}EVENT_3D_ROLLOFF;

	typedef enum
	{
		MPB_STEAL_OLDEST,
		MPB_STEAL_NEWEST,
		MPB_STEAL_QUIETEST,
		MPB_JUST_FAIL
	}MAX_PLAYBACK_BEHAVIOR;

	struct EVENT_PROPERTY
	{
		bool				bOneShot;
		float				fVolume;
		float				fVolumeRandomization;
		float				fPitch;
		float				fPitchRandomization;
		int					iFadeInTime;
		int					iFadeInCurveType;
		int					iFadeOutTime;
		int					iFadeOutCurveType;
		EVENT_MODE			mode;
		EVENT_3D_ROLLOFF	rolloff;
		float				fMinDistance;
		float				fMaxDistance;
		float				f3DConeInsideAngle;
		float				f3DConeOutsideAngle;
		float				f3DConeOutsideVolume;
		float				f3DSpeakerSpread;
		unsigned int		uiMaxPlaybacks;
		MAX_PLAYBACK_BEHAVIOR maxPlaybacksBehavior;
		int					iMinIntervalTime;
		int					iPriority;
	};

	typedef enum
	{
		EFFECT_TYPE_NONE,
		EFFECT_TYPE_VOLUME,		
		EFFECT_TYPE_PAN,
		EFFECT_TYPE_SURROUND_PAN,
		EFFECT_TYPE_PARAMEQ,
		EFFECT_TYPE_COMPRESSOR,
		EFFECT_TYPE_ECHO,
		EFFECT_TYPE_PITCH,
		EFFECT_TYPE_OCCLUSION
	}EFFECT_TYPE;


	typedef enum
	{
		STOP_MODE_CUTOFF,				// 停止时立即停止
		STOP_MODE_PLAYTOEND				// 停止时播放完当前音频再停止
	}STOP_MODE;

	typedef enum
	{
		PLAY_NUM_ONE_AT_A_TIME,
		PLAY_NUM_ALL
	}PLAY_NUM;

	typedef enum
	{
		START_MODE_IMMEDIATE,				// 立即开始播放
		START_MODE_WAIT_FOR_PREVIOUS		// 等待前一个播放完成再开始播放
	}START_MODE;

	struct SOUND_INSTANCE_PROPERTY
	{
		float		fStartPosition;			// 开始点[0,1]，针对EventParameter
		float		fLength;				// 长度[0,1]，针对EventParameter
		START_MODE	startMode;				// 开始模式
		STOP_MODE	stopMode;				// 循环模式
		int			iLoopCount;				// 循环次数，默认为1
		PLAY_NUM	playNum;				// 每次播放文件个数，一个还是全部都按照播放模式播放X遍
		int			iFadeInTime;
		int			iFadeInCurveType;
		int			iFadeOutTime;
		int			iFadeOutCurveType;
		int			iPreIntervalTime;			// 前置时间间隔[0,1200]s，默认0
		int			iPreIntervalTimeRandomization;	// 前置时间间隔随机[0,1200]s，默认0
		int			iIntervalTime;			// 时间间隔[0,1200]s，默认0
		int			iIntervalTimeRandomization;	// 时间间隔随机[0,1200]s，默认0

		SOUND_INSTANCE_PROPERTY()
		{
			memset(this, 0, sizeof(SOUND_INSTANCE_PROPERTY));
			fLength = 1.0f;
			iLoopCount = 1;
		}
	};

	typedef struct REVERB_PROPERTIES
	{                                   /*       MIN    MAX    DEFAULT DESCRIPTION */
		int          Instance;          /* [w]   0      3      0       Environment Instance.                                                 (SUPPORTED:SFX(4 instances) and Wii (3 instances)) */
		int          Environment;       /* [r/w] -1     25     -1      Sets all listener properties.  -1 = OFF.                              (SUPPORTED:SFX(-1 only)/PSP) */
		float        EnvDiffusion;      /* [r/w] 0.0    1.0    1.0     Environment diffusion                                                 (SUPPORTED:WII) */
		int          Room;              /* [r/w] -10000 0      -1000   Room effect level (at mid frequencies)                                (SUPPORTED:SFX/WII/PSP) */
		int          RoomHF;            /* [r/w] -10000 0      -100    Relative room effect level at high frequencies                        (SUPPORTED:SFX) */
		int          RoomLF;            /* [r/w] -10000 0      0       Relative room effect level at low frequencies                         (SUPPORTED:SFX) */
		float        DecayTime;         /* [r/w] 0.1    20.0   1.49    Reverberation decay time at mid frequencies                           (SUPPORTED:SFX/WII) */
		float        DecayHFRatio;      /* [r/w] 0.1    2.0    0.83    High-frequency to mid-frequency decay time ratio                      (SUPPORTED:SFX) */
		float        DecayLFRatio;      /* [r/w] 0.1    2.0    1.0     Low-frequency to mid-frequency decay time ratio                       (SUPPORTED:---) */
		int          Reflections;       /* [r/w] -10000 1000   -2602   Early reflections level relative to room effect                       (SUPPORTED:SFX/WII) */
		float        ReflectionsDelay;  /* [r/w] 0.0    0.3    0.007   Initial reflection delay time                                         (SUPPORTED:SFX) */
		int          Reverb;            /* [r/w] -10000 2000   200     Late reverberation level relative to room effect                      (SUPPORTED:SFX) */
		float        ReverbDelay;       /* [r/w] 0.0    0.1    0.011   Late reverberation delay time relative to initial reflection          (SUPPORTED:SFX/WII) */
		float        ModulationTime;    /* [r/w] 0.04   4.0    0.25    Modulation time                                                       (SUPPORTED:---) */
		float        ModulationDepth;   /* [r/w] 0.0    1.0    0.0     Modulation depth                                                      (SUPPORTED:WII) */
		float        HFReference;       /* [r/w] 1000.0 20000  5000.0  Reference high frequency (hz)                                         (SUPPORTED:SFX) */
		float        LFReference;       /* [r/w] 20.0   1000.0 250.0   Reference low frequency (hz)                                          (SUPPORTED:SFX) */
		float        Diffusion;         /* [r/w] 0.0    100.0  100.0   Value that controls the echo density in the late reverberation decay. (SUPPORTED:SFX) */
		float        Density;           /* [r/w] 0.0    100.0  100.0   Value that controls the modal density in the late reverberation decay (SUPPORTED:SFX) */
		unsigned int Flags;             /* [r/w] FMOD_REVERB_FLAGS - modifies the behavior of above properties                               (SUPPORTED:WII) */
	} REVERB_PROPERTY;

/*
[DEFINE] 
[
[NAME] 
REVERB_PRESETS

[DESCRIPTION]   
A set of predefined environment PARAMETERS, created by Creative Labs
These are used to initialize an REVERB_PROPERTIES structure statically.
ie 
REVERB_PROPERTIES prop = REVERB_PRESET_GENERIC;

[REMARKS]

[PLATFORMS]
Win32, Win64, Linux, Linux64, Macintosh, Xbox360, PlayStation 2, PlayStation Portable, PlayStation 3, Wii, iPhone

[SEE_ALSO]
System::setReverbProperties
]
*/
	/*                                    Inst Env  Diffus  Room   RoomHF  RmLF DecTm   DecHF  DecLF   Refl  RefDel   Revb  RevDel  ModTm  ModDp   HFRef    LFRef   Diffus  Densty  FLAGS */
#define REVERB_PRESET_OFF              {  0, -1,  1.00f, -10000, -10000, 0,   1.00f,  1.00f, 1.0f,  -2602, 0.007f,   200, 0.011f, 0.25f, 0.000f, 5000.0f, 250.0f,   0.0f,   0.0f, 0x33f }
#define REVERB_PRESET_GENERIC          {  0,  0,  1.00f, -1000,  -100,   0,   1.49f,  0.83f, 1.0f,  -2602, 0.007f,   200, 0.011f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_PADDEDCELL       {  0,  1,  1.00f, -1000,  -6000,  0,   0.17f,  0.10f, 1.0f,  -1204, 0.001f,   207, 0.002f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_ROOM             {  0,  2,  1.00f, -1000,  -454,   0,   0.40f,  0.83f, 1.0f,  -1646, 0.002f,    53, 0.003f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_BATHROOM         {  0,  3,  1.00f, -1000,  -1200,  0,   1.49f,  0.54f, 1.0f,   -370, 0.007f,  1030, 0.011f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f,  60.0f, 0x3f }
#define REVERB_PRESET_LIVINGROOM       {  0,  4,  1.00f, -1000,  -6000,  0,   0.50f,  0.10f, 1.0f,  -1376, 0.003f, -1104, 0.004f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_STONEROOM        {  0,  5,  1.00f, -1000,  -300,   0,   2.31f,  0.64f, 1.0f,   -711, 0.012f,    83, 0.017f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_AUDITORIUM       {  0,  6,  1.00f, -1000,  -476,   0,   4.32f,  0.59f, 1.0f,   -789, 0.020f,  -289, 0.030f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_CONCERTHALL      {  0,  7,  1.00f, -1000,  -500,   0,   3.92f,  0.70f, 1.0f,  -1230, 0.020f,    -2, 0.029f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_CAVE             {  0,  8,  1.00f, -1000,  0,      0,   2.91f,  1.30f, 1.0f,   -602, 0.015f,  -302, 0.022f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x1f }
#define REVERB_PRESET_ARENA            {  0,  9,  1.00f, -1000,  -698,   0,   7.24f,  0.33f, 1.0f,  -1166, 0.020f,    16, 0.030f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_HANGAR           {  0,  10, 1.00f, -1000,  -1000,  0,   10.05f, 0.23f, 1.0f,   -602, 0.020f,   198, 0.030f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_CARPETTEDHALLWAY {  0,  11, 1.00f, -1000,  -4000,  0,   0.30f,  0.10f, 1.0f,  -1831, 0.002f, -1630, 0.030f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_HALLWAY          {  0,  12, 1.00f, -1000,  -300,   0,   1.49f,  0.59f, 1.0f,  -1219, 0.007f,   441, 0.011f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_STONECORRIDOR    {  0,  13, 1.00f, -1000,  -237,   0,   2.70f,  0.79f, 1.0f,  -1214, 0.013f,   395, 0.020f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_ALLEY            {  0,  14, 0.30f, -1000,  -270,   0,   1.49f,  0.86f, 1.0f,  -1204, 0.007f,    -4, 0.011f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_FOREST           {  0,  15, 0.30f, -1000,  -3300,  0,   1.49f,  0.54f, 1.0f,  -2560, 0.162f,  -229, 0.088f, 0.25f, 0.000f, 5000.0f, 250.0f,  79.0f, 100.0f, 0x3f }
#define REVERB_PRESET_CITY             {  0,  16, 0.50f, -1000,  -800,   0,   1.49f,  0.67f, 1.0f,  -2273, 0.007f, -1691, 0.011f, 0.25f, 0.000f, 5000.0f, 250.0f,  50.0f, 100.0f, 0x3f }
#define REVERB_PRESET_MOUNTAINS        {  0,  17, 0.27f, -1000,  -2500,  0,   1.49f,  0.21f, 1.0f,  -2780, 0.300f, -1434, 0.100f, 0.25f, 0.000f, 5000.0f, 250.0f,  27.0f, 100.0f, 0x1f }
#define REVERB_PRESET_QUARRY           {  0,  18, 1.00f, -1000,  -1000,  0,   1.49f,  0.83f, 1.0f, -10000, 0.061f,   500, 0.025f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }
#define REVERB_PRESET_PLAIN            {  0,  19, 0.21f, -1000,  -2000,  0,   1.49f,  0.50f, 1.0f,  -2466, 0.179f, -1926, 0.100f, 0.25f, 0.000f, 5000.0f, 250.0f,  21.0f, 100.0f, 0x3f }
#define REVERB_PRESET_PARKINGLOT       {  0,  20, 1.00f, -1000,  0,      0,   1.65f,  1.50f, 1.0f,  -1363, 0.008f, -1153, 0.012f, 0.25f, 0.000f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x1f }
#define REVERB_PRESET_SEWERPIPE        {  0,  21, 0.80f, -1000,  -1000,  0,   2.81f,  0.14f, 1.0f,    429, 0.014f,  1023, 0.021f, 0.25f, 0.000f, 5000.0f, 250.0f,  80.0f,  60.0f, 0x3f }
#define REVERB_PRESET_UNDERWATER       {  0,  22, 1.00f, -1000,  -4000,  0,   1.49f,  0.10f, 1.0f,   -449, 0.007f,  1700, 0.011f, 1.18f, 0.348f, 5000.0f, 250.0f, 100.0f, 100.0f, 0x3f }

	/* PlayStation Portable Only presets */
#define REVERB_PRESET_PSP_ROOM         {  0,  1,  0,     0,      0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f,     0, 0.000f, 0.00f, 0.000f, 0000.0f,   0.0f,  0.0f,    0.0f, 0x31f }
#define REVERB_PRESET_PSP_STUDIO_A     {  0,  2,  0,     0,      0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f,     0, 0.000f, 0.00f, 0.000f, 0000.0f,   0.0f,  0.0f,    0.0f, 0x31f }
#define REVERB_PRESET_PSP_STUDIO_B     {  0,  3,  0,     0,      0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f,     0, 0.000f, 0.00f, 0.000f, 0000.0f,   0.0f,  0.0f,    0.0f, 0x31f }
#define REVERB_PRESET_PSP_STUDIO_C     {  0,  4,  0,     0,      0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f,     0, 0.000f, 0.00f, 0.000f, 0000.0f,   0.0f,  0.0f,    0.0f, 0x31f }
#define REVERB_PRESET_PSP_HALL         {  0,  5,  0,     0,      0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f,     0, 0.000f, 0.00f, 0.000f, 0000.0f,   0.0f,  0.0f,    0.0f, 0x31f }
#define REVERB_PRESET_PSP_SPACE        {  0,  6,  0,     0,      0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f,     0, 0.000f, 0.00f, 0.000f, 0000.0f,   0.0f,  0.0f,    0.0f, 0x31f }
#define REVERB_PRESET_PSP_ECHO         {  0,  7,  0,     0,      0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f,     0, 0.000f, 0.00f, 0.000f, 0000.0f,   0.0f,  0.0f,    0.0f, 0x31f }
#define REVERB_PRESET_PSP_DELAY        {  0,  8,  0,     0,      0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f,     0, 0.000f, 0.00f, 0.000f, 0000.0f,   0.0f,  0.0f,    0.0f, 0x31f }
#define REVERB_PRESET_PSP_PIPE         {  0,  9,  0,     0,      0,      0,   0.0f,   0.0f,  0.0f,     0,  0.000f,     0, 0.000f, 0.00f, 0.000f, 0000.0f,   0.0f,  0.0f,    0.0f, 0x31f }
	/* [DEFINE_END] */

	typedef struct VECTOR 
	{
		VECTOR(){}
		VECTOR(float x, float y, float z)
		{
			this->x = x;
			this->y = y;
			this->z = z;
		}
		VECTOR(const VECTOR& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
		}
		VECTOR(float f)
		{
			x = y = z = f;
		}
		bool operator == (const VECTOR& v)
		{
			if(x == v.x && y == v.y && z == v.z)
				return true;
			return false;
		}
		float x,y,z;
	}VECTOR;

#define INIT_NORMAL                     0x00000000 /* All platforms - Initialize normally */
#define INIT_STREAM_FROM_UPDATE         0x00000001 /* All platforms - No stream thread is created internally.  Streams are driven from System::update.  Mainly used with non-realtime outputs. */
#define INIT_3D_RIGHTHANDED             0x00000002 /* All platforms - FMOD will treat +X as right, +Y as up and +Z as backwards (towards you). */
#define INIT_SOFTWARE_DISABLE           0x00000004 /* All platforms - Disable software mixer to save memory.  Anything created with FMOD_SOFTWARE will fail and DSP will not work. */
#define INIT_SOFTWARE_OCCLUSION         0x00000008 /* All platforms - All FMOD_SOFTWARE with FMOD_3D based voices will add a software lowpass filter effect into the DSP chain which is automatically used when Channel::set3DOcclusion is used or the geometry API. */
#define INIT_SOFTWARE_HRTF              0x00000010 /* All platforms - All FMOD_SOFTWARE with FMOD_3D based voices will add a software lowpass filter effect into the DSP chain which causes sounds to sound duller when the sound goes behind the listener.  Use System::setAdvancedSettings to adjust cutoff frequency. */
#define INIT_SOFTWARE_REVERB_LOWMEM     0x00000040 /* All platforms - SFX reverb is run using 22/24khz delay buffers, halving the memory required. */
#define INIT_ENABLE_PROFILE             0x00000020 /* All platforms - Enable TCP/IP based host which allows FMOD Designer or FMOD Profiler to connect to it, and view memory, CPU and the DSP network graph in real-time. */
#define INIT_VOL0_BECOMES_VIRTUAL       0x00000080 /* All platforms - Any sounds that are 0 volume will go virtual and not be processed except for having their positions updated virtually.  Use System::setAdvancedSettings to adjust what volume besides zero to switch to virtual at. */
#define INIT_WASAPI_EXCLUSIVE           0x00000100 /* Win32 Vista only - for WASAPI output - Enable exclusive access to hardware, lower latency at the expense of excluding other applications from accessing the audio hardware. */
#define INIT_DSOUND_HRTFNONE            0x00000200 /* Win32 only - for DirectSound output - FMOD_HARDWARE | FMOD_3D buffers use simple stereo panning/doppler/attenuation when 3D hardware acceleration is not present. */
#define INIT_DSOUND_HRTFLIGHT           0x00000400 /* Win32 only - for DirectSound output - FMOD_HARDWARE | FMOD_3D buffers use a slightly higher quality algorithm when 3D hardware acceleration is not present. */
#define INIT_DSOUND_HRTFFULL            0x00000800 /* Win32 only - for DirectSound output - FMOD_HARDWARE | FMOD_3D buffers use full quality 3D playback when 3d hardware acceleration is not present. */
#define INIT_PS2_DISABLECORE0REVERB     0x00010000 /* PS2 only - Disable reverb on CORE 0 to regain 256k SRAM. */
#define INIT_PS2_DISABLECORE1REVERB     0x00020000 /* PS2 only - Disable reverb on CORE 1 to regain 256k SRAM. */
#define INIT_PS2_DONTUSESCRATCHPAD      0x00040000 /* PS2 only - Disable FMOD's usage of the scratchpad. */
#define INIT_PS2_SWAPDMACHANNELS        0x00080000 /* PS2 only - Changes FMOD from using SPU DMA channel 0 for software mixing, and 1 for sound data upload/file streaming, to 1 and 0 respectively. */
#define INIT_PS3_PREFERDTS              0x00800000 /* PS3 only - Prefer DTS over Dolby Digital if both are supported. Note: 8 and 6 channel LPCM is always preferred over both DTS and Dolby Digital. */
#define INIT_PS3_FORCE2CHLPCM           0x01000000 /* PS3 only - Force PS3 system output mode to 2 channel LPCM. */
#define INIT_XBOX_REMOVEHEADROOM        0x00100000 /* Xbox only - By default DirectSound attenuates all sound by 6db to avoid clipping/distortion.  CAUTION.  If you use this flag you are responsible for the final mix to make sure clipping / distortion doesn't happen. */
#define INIT_SYSTEM_MUSICMUTENOTPAUSE   0x00200000 /* Xbox 360 / PS3 - The "music" channelgroup which by default pauses when custom 360 dashboard / PS3 BGM music is played, can be changed to mute (therefore continues playing) instead of pausing, by using this flag. */
#define INIT_SYNCMIXERWITHUPDATE        0x00400000 /* Win32/Wii/PS3/Xbox/Xbox 360 - FMOD Mixer thread is woken up to do a mix when System::update is called rather than waking periodically on its own timer. */
#define INIT_DTS_NEURALSURROUND         0x02000000 /* Win32/Mac/Linux/Solaris - Use DTS Neural surround downmixing from 7.1 if speakermode set to FMOD_SPEAKERMODE_STEREO or FMOD_SPEAKERMODE_5POINT1.  Internal DSP structure will be set to 7.1. */
#define INIT_GEOMETRY_USECLOSEST        0x04000000 /* All platforms - With the geometry engine, only process the closest polygon rather than accumulating all polygons the sound to listener line intersects. */
}


bool GF_StopChannelGroup(FMOD::ChannelGroup* pChannelGroup);


bool _EXPORT_DLL_ GF_CompareNoCase(const char * string1, size_t string1_length, const char * string2, size_t string2_length);

// Lp(sound pressure level) = 20Lg(ratio)dB
float _EXPORT_DLL_ GF_ConvertVolumeFromDB2Ratio(float dB);

// Lp(sound pressure level) = 20Lg(ratio)dB
float _EXPORT_DLL_ GF_ConvertVolumeFromRatio2DB(float fVolume);

// Ratio = pow(2, tone/6)
float _EXPORT_DLL_ GF_ConvertPitchFromTone2Ratio(float tone);


template<class T>
T _EXPORT_DLL_ GF_Clamp(T min, T max, T value)
{
	if(value < min)
		value = min;
	if(value > max)
		value = max;
	return value;
}