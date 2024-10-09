// Filename	: WikiGuideData.cpp
// Creator	: Feng Ning
// Date		: 2010/09/27

#pragma once

#include "WikiGuideData.h"
#include "WikiDataReader.h"
#include "AMemory.h"
#include "A3DVector.h"
#include "AChar.h"
#include "globaldataman.h"

#include <typeinfo>

#define new A_DEBUG_NEW

//------------------------------------------------------------------------
//
// Global Data for Guide
//
//------------------------------------------------------------------------
abase::vector<WikiGuideArea> global_wikiguide_area;
abase::vector<WikiGuideEqu> global_wikiguide_equ;
abase::vector<WikiGuideFeature> global_wikiguide_feature;

const abase::vector<WikiGuideArea>& WikiGuideData::Area() const
{
	return global_wikiguide_area;
}

const abase::vector<WikiGuideEqu>& WikiGuideData::Equ() const
{
	return global_wikiguide_equ;
}

const abase::vector<WikiGuideFeature>& WikiGuideData::Feature() const
{
	return global_wikiguide_feature;
}

bool WikiGuideData::GetEntry(int areaid, A3DVECTOR3* pVec) const
{
	if(!pVec)
	{
		return false;
	}

	const abase::vector<TRANS_TARGET>& aWayPoints = *globaldata_gettranstargets();
	for (size_t i=0; i < aWayPoints.size(); i++)
	{
		const TRANS_TARGET& entry = aWayPoints[i];
		if (entry.id != (int)areaid)
		{
			continue;
		}

		*pVec = entry.vecPos;
		return true;
	}

	return false;
}

bool WikiGuideData::Init()
{
	bool isUnicode = true;

	if(global_wikiguide_area.empty())
	{
		WikiDataReader reader(isUnicode);

		// for WikiGuideArea
		reader.Init(AS2AC(typeid(WikiGuideArea).name()), sizeof(WikiGuideArea));
		WikiGuideArea templArea;
		reader.Register(_AL("LevelMin"), &templArea, &templArea.LevelMin, WikiDataReader::TYPE_INTEGER);
		reader.Register(_AL("LevelMax"), &templArea, &templArea.LevelMax, WikiDataReader::TYPE_INTEGER);
		reader.Register(_AL("ID"), &templArea, &templArea.ID, WikiDataReader::TYPE_INTEGER);
		reader.Register(_AL("Areaname"), &templArea, &templArea.Name, WikiDataReader::TYPE_STRING);
		reader.Register(_AL("Description"), &templArea, &templArea.Desc, WikiDataReader::TYPE_STRING);
		
		reader.Open("configs\\guide_area.txt", true);
		while(!reader.IsEnd())
		{
			templArea.Empty();
			if(!reader.ReadObject(&templArea))
			{
				ASSERT(false);
				break;
			}
			global_wikiguide_area.push_back(templArea);
		}
		reader.Release();
	}
	
	if(global_wikiguide_equ.empty())
	{
		WikiDataReader reader(isUnicode);
		// for WikiGuideEqu
		reader.Init(AS2AC(typeid(WikiGuideEqu).name()), sizeof(WikiGuideEqu));
		WikiGuideEqu templEqu;
		reader.Register(_AL("LevelMin"), &templEqu, &templEqu.LevelMin, WikiDataReader::TYPE_INTEGER);
		reader.Register(_AL("LevelMax"), &templEqu, &templEqu.LevelMax, WikiDataReader::TYPE_INTEGER);
		reader.Register(_AL("Career"), &templEqu, &templEqu.Prof, WikiDataReader::TYPE_INTEGER);
		reader.Register(_AL("ID"), &templEqu, &templEqu.Items, WikiDataReader::TYPE_INTEGER, 
			sizeof(templEqu.Items) / sizeof(templEqu.Items[0]), sizeof(templEqu.Items[0]));
		
		reader.Open("configs\\guide_equ.txt", true);
		while(!reader.IsEnd())
		{
			templEqu.Empty();
			if(!reader.ReadObject(&templEqu))
			{
				ASSERT(false);
				break;
			}
			global_wikiguide_equ.push_back(templEqu);
		}
		reader.Release();
	}
	
	if(global_wikiguide_feature.empty())
	{
		WikiDataReader reader(isUnicode);
		// for WikiGuideFeature
		reader.Init(AS2AC(typeid(WikiGuideFeature).name()), sizeof(WikiGuideFeature));
		WikiGuideFeature templFeature;
		reader.Register(_AL("LevelMin"), &templFeature, &templFeature.LevelMin, WikiDataReader::TYPE_INTEGER);
		reader.Register(_AL("LevelMax"), &templFeature, &templFeature.LevelMax, WikiDataReader::TYPE_INTEGER);
		reader.Register(_AL("GetLevel"), &templFeature, &templFeature.LevelStart, WikiDataReader::TYPE_INTEGER);
		
		reader.Register(_AL("Name"), &templFeature, &templFeature.Name, WikiDataReader::TYPE_STRING);
		reader.Register(_AL("Type"), &templFeature, &templFeature.Type, WikiDataReader::TYPE_STRING);
		reader.Register(_AL("Area"), &templFeature, &templFeature.Area, WikiDataReader::TYPE_STRING);
		reader.Register(_AL("Npc"), &templFeature, &templFeature.Npc, WikiDataReader::TYPE_STRING);
		reader.Register(_AL("Prize"), &templFeature, &templFeature.Award, WikiDataReader::TYPE_STRING);
		reader.Register(_AL("Description"), &templFeature, &templFeature.Desc, WikiDataReader::TYPE_STRING);
		
		reader.Open("configs\\guide_feature.txt", true);
		while(!reader.IsEnd())
		{
			templFeature.Empty();
			if(!reader.ReadObject(&templFeature))
			{
				ASSERT(false);
				break;
			}
			global_wikiguide_feature.push_back(templFeature);
		}
		reader.Release();
	}

	return true;
}