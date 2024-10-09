#include "EC_CountryConfig.h"
#include "EC_Global.h"
#include "EC_Game.h"
#include "elementdataman.h"
#include <A3DMacros.h>
#include <A2DSprite.h>

CECCountryConfig & CECCountryConfig::Instance()
{
	static CECCountryConfig s_instance;
	return s_instance;
}

CECCountryConfig::CECCountryConfig()
: m_pConfig(NULL)
{
	::ZeroMemory(m_sprites, sizeof(m_sprites));
	::ZeroMemory(m_stateSprites, sizeof(m_stateSprites));
}

void CECCountryConfig::Init()
{
	if (!g_pGame)	return;
	elementdataman *pDataMan = g_pGame->GetElementDataMan();
	if (!pDataMan)	return;
	
	//	查找第一个 COUNTRY_CONFIG
	DATA_TYPE dt = DT_INVALID;
	ID_SPACE space = ID_SPACE_CONFIG;
	unsigned int id = pDataMan->get_first_data_id(space, dt);
	while (id > 0)
	{
		if (dt == DT_COUNTRY_CONFIG) break;
		id = pDataMan->get_next_data_id(space, dt);
	}

	if (id > 0)
	{
		const COUNTRY_CONFIG * pConfig = static_cast<const COUNTRY_CONFIG *>(pDataMan->get_data_ptr(id, space, dt));
		int i(0);
		for (i = 0; i < COUNTRY_COUNT; ++ i)
		{
			m_names[i] = pConfig->country[i].name;
			m_names[i].TrimLeft();
			m_names[i].TrimRight();

			A2DSprite *pNewSprite = new A2DSprite;
			if (pNewSprite->Init(g_pGame->GetA3DDevice(), pConfig->country[i].file_icon, 0))
			{
				pNewSprite->SetLinearFilter(true);
				m_sprites[i] = pNewSprite;
			}
			else delete pNewSprite;
		}
		for (i = 0; i < STRONGHOLD_STATE_COUNT; ++ i)
		{
			const char *szFile = NULL;
			switch (i)
			{
			case SHS_ATTACKER:		szFile = "InGame\\攻方占领.tga"; break;
			case SHS_ATTACKER_HALF:	szFile = "InGame\\中立偏攻方.tga"; break;
			case SHS_NEUTRAL:		szFile = "InGame\\中立.tga"; break;
			case SHS_DEFENDER_HALF:	szFile = "InGame\\中立偏守方.tga"; break;
			case SHS_DEFENDER:		szFile = "InGame\\守方占领.tga"; break;
			default: ASSERT(false); break;
			}
			if (szFile)
			{
				A2DSprite *pNewSprite = new A2DSprite;
				if (pNewSprite->Init(g_pGame->GetA3DDevice(), szFile, 0))
				{					
					pNewSprite->SetLocalCenterPos(
						pNewSprite->GetWidth() / 2,
						pNewSprite->GetHeight() / 2);
					pNewSprite->SetLinearFilter(true);
					m_stateSprites[i] = pNewSprite;
				}
				else delete pNewSprite;
			}
		}

		m_pConfig = pConfig;
	}
}

void CECCountryConfig::Destroy()
{
	int i(0);
	for (i = 0; i < COUNTRY_COUNT; ++ i)
		A3DRELEASE(m_sprites[i]);
	for (i = 0; i < STRONGHOLD_STATE_COUNT; ++ i)
		A3DRELEASE(m_stateSprites[i]);
}

A2DSprite * CECCountryConfig::GetIcon(int idCountry)
{
	return ValidateID(idCountry) ? m_sprites[idCountry-1] : NULL;
}


const ACString *CECCountryConfig::GetName(int idCountry)const
{
	return ValidateID(idCountry) ? &m_names[idCountry-1] : NULL;
}

A3DCOLOR CECCountryConfig::GetColor(int idCountry)const
{
	return ValidateID(idCountry) ? m_pConfig->country[idCountry - 1].color : 0;
}

const float * CECCountryConfig::GetFlagGoal(bool bAttack)const
{
	return bAttack ? m_pConfig->attack_flag_goal : m_pConfig->defence_flag_goal;
}

const float * CECCountryConfig::GetStrongHoldPos(int index)const
{
	return ValidateStrongHold(index) ? m_pConfig->stronghold[index].mine_pos : NULL;
}

A2DSprite * CECCountryConfig::GetStrongHoldStateIcon(int state)const
{
	return ValidateStrongHoldState(state) ? m_stateSprites[state] : NULL;
}