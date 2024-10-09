// File		: EC_ProfConfig.h
// Creator	: Xu Wenbin
// Date		: 2013/9/10

#pragma once

class CECProfConfig
{
private:
	CECProfConfig();
	CECProfConfig(const CECProfConfig &);
	CECProfConfig & operator == (const CECProfConfig &);

public:
	static CECProfConfig & Instance();

	bool IsProfession(int prof)const;
	bool IsGender(int gender)const;
	bool IsRace(int race)const;
	bool IsExist(int prof, int gender)const;
	int	 GetCounterpartGender(int gender)const;

	bool CanShowOnCreate(int prof, int gender)const;
	int  GetRaceShowOrder(int race)const;
	int	 GetRaceByProfession(int prof)const;
	int  GetProfessionShowOrderInRace(int prof)const;

	bool ContainsAllProfession(unsigned int mask)const;
	unsigned int GetAllProfessionMask()const;

	int  GetMaxBodyID(int prof)const;
};