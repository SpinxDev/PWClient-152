#pragma once

#include "ExpTypes.h"
#include <ABaseDef.h>
#include <AAssist.h>

class A2DSprite;

class CECCountryConfig
{
public:
	static CECCountryConfig & Instance();

	void Init();
	void Destroy();

	const COUNTRY_CONFIG * GetConfig()const{ return m_pConfig; }
	A2DSprite * GetIcon(int idCountry);
	const ACString * GetName(int idCountry)const;
	DWORD GetColor(int idCountry)const;
	const float * GetFlagGoal(bool bAttack)const;
	const float * GetStrongHoldPos(int index)const;

	enum {COUNTRY_COUNT = sizeof(((COUNTRY_CONFIG *)NULL)->country) / sizeof(((COUNTRY_CONFIG *)NULL)->country[0])};
	enum {STRONGHOLD_COUNT = sizeof(((COUNTRY_CONFIG *)NULL)->stronghold) / sizeof(((COUNTRY_CONFIG *)NULL)->stronghold[0])};
	enum {STRONGHOLD_STATE_COUNT = sizeof(((COUNTRY_CONFIG *)NULL)->stronghold[0].state) / sizeof(((COUNTRY_CONFIG *)NULL)->stronghold[0].state[0])};

	enum StrongHoldState{
		SHS_ATTACKER,			//	攻方占领
		SHS_ATTACKER_HALF,		//	攻方部分占领
		SHS_NEUTRAL,			//	中立
		SHS_DEFENDER_HALF,		//	守方部分占领
		SHS_DEFENDER,			//	守方占领
		SHS_MAX,
	};
	A2DSprite * GetStrongHoldStateIcon(int state)const;

	static bool ValidateID(int idCountry){ return idCountry > 0 && idCountry <= COUNTRY_COUNT; }
	static bool ValidateStrongHold(int index){ return index >= 0 && index < STRONGHOLD_COUNT; }
	static bool ValidateStrongHoldState(int state){ return state >= 0 && state < SHS_MAX; }
	static bool IsAttackerStrongHold(int state){ return state == SHS_ATTACKER || state == SHS_ATTACKER_HALF; }
	static bool IsDefenderStrongHold(int state){ return state == SHS_DEFENDER || state == SHS_DEFENDER_HALF; }

private:
	CECCountryConfig();
	CECCountryConfig(const CECCountryConfig &);
	CECCountryConfig & operator=(const CECCountryConfig &);

	const COUNTRY_CONFIG *m_pConfig;
	A2DSprite * m_sprites[COUNTRY_COUNT];
	ACString m_names[COUNTRY_COUNT];

	A2DSprite * m_stateSprites[STRONGHOLD_STATE_COUNT];
};