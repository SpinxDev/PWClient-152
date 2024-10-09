// Filename	: WikiGuideData.h
// Creator	: Feng Ning
// Date		: 2010/09/27

#pragma once

#include "AAssist.h"
#include "vector.h"

struct WikiGuideEqu
{
	int LevelMin;
	int LevelMax;
	
	int Prof;
	int Items[20];
	
	void Empty()
	{
		memset(this, 0, sizeof(WikiGuideEqu));
	}
};

struct WikiGuideArea
{
	int LevelMin;
	int LevelMax;
	
	int ID;
	ACString Name;
	ACString Desc;
	
	void Empty()
	{
		ID = 0;
		LevelMax = 0;
		LevelMin = 0;
		Name.Empty();
		Desc.Empty();
	}
};

struct WikiGuideFeature
{
	int LevelMin;
	int LevelMax;
	
	int LevelStart;
	ACString Name;
	ACString Type;
	ACString Area;
	ACString Npc;
	ACString Award;
	ACString Desc;
	
	void Empty()
	{
		LevelMax = 0;
		LevelMin = 0;
		LevelStart = 0;
		
		Name.Empty();
		Type.Empty();
		Area.Empty();
		Npc.Empty();
		Award.Empty();
		Desc.Empty();
	}
};

class A3DVECTOR3;
class WikiGuideData
{
public:
	WikiGuideData(){};
	const abase::vector<WikiGuideArea>& Area() const;
	const abase::vector<WikiGuideEqu>& Equ() const;
	const abase::vector<WikiGuideFeature>& Feature() const;

	bool GetEntry(int areaid, A3DVECTOR3* pVec) const;

public:
	static bool Init();

private:
	WikiGuideData(const WikiGuideData&);
	WikiGuideData& operator=(const WikiGuideData&);
};
