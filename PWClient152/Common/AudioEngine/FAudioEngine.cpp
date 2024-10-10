#include "FAudioEngine.h"

#include <fmod.hpp>

bool GF_StopChannelGroup(FMOD::ChannelGroup* pChannelGroup)
{	

	if(FMOD_OK != pChannelGroup->stop())
		return false;

	int iSubGroupNum;
	if(FMOD_OK != pChannelGroup->getNumGroups(&iSubGroupNum))
		return false;
	for (int i=0; i<iSubGroupNum; ++i)
	{
		FMOD::ChannelGroup* pSubChannelGroup = 0;
		if(FMOD_OK != pChannelGroup->getGroup(i, &pSubChannelGroup))
			continue;
		if(!pSubChannelGroup)
			continue;
		if(!GF_StopChannelGroup(pSubChannelGroup))
			return false;
	}

	int iChannelNum = 0;
	if (FMOD_OK != pChannelGroup->getNumChannels(&iChannelNum))
		return false;
	for (int i=0; i<iChannelNum; ++i)
	{
		FMOD::Channel* pChannel = 0;
		FMOD_RESULT r = pChannelGroup->getChannel(i, &pChannel);
		if(FMOD_OK != r)
			continue;
		if(!pChannel)
			continue;
		if(FMOD_OK != pChannel->stop())
			continue;
	}
	
	return true;
}

bool GF_CompareNoCase(const char * string1, size_t string1_length, const char * string2, size_t string2_length)
{
	if(string1_length!=string2_length)
	{
		return false;
	}

	for(size_t i=0; i<string1_length; i++)
	{
		if(toupper(*(string1+i))!=toupper(*(string2+i)))
		{
			return false;
		}
	}

	return true;
}

// Lp(sound pressure level) = 20Lg(ratio)dB
float GF_ConvertVolumeFromDB2Ratio(float dB)
{
	return pow(10, dB/20.0f);
}

float GF_ConvertVolumeFromRatio2DB(float fVolume)
{
	return log10(fVolume)*20.0f;
}

// Ratio = pow(2, tone/6)
float GF_ConvertPitchFromTone2Ratio(float tone)
{
	return pow(2.0f, tone/6.0f);
}