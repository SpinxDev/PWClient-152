// Filename	: EC_Optimize.h
// Creator	: Xu Wenbin
// Date		: 2013/4/18
#pragma once

#include <basetsd.h>
#include <windows.h>

class CECOptimize
{
  public:
	typedef __int64 clientid_t;
    struct GFX
    {
		bool	bExcludeHost;

		bool	bHidePlayerCast;
		bool	bHideNPCCast;
		bool	bHidePlayerAttack;
		bool	bHideNPCAttack;

        bool	bHidePlayerFly;
        bool	bHideNPCFly;
        bool	bHidePlayerHit;
        bool	bHideNPCHit;
        bool	bHidePlayerState;
        bool	bHideNPCState;

		bool	bHideWeaponStone;
		bool	bHideArmorStone;
		bool	bHideSuite;

        GFX();

		bool CanShowCast(clientid_t idHost, int cid)const;
		bool CanShowAttack(clientid_t idHost, int cid)const;
		bool CanShowFly(clientid_t idHost)const;
		bool CanShowHit(clientid_t idHost)const;
		bool CanShowState(clientid_t idHost, int cid)const;
		bool CanShowWeaponStone(clientid_t idHost, int cid)const;
		bool CanShowArmorStone(clientid_t idHost, int cid)const;
		bool CanShowSuite(clientid_t idHost, int cid)const;
    };
    const GFX & GetGFX()const
    {
        return m_gfx;
    }
    void SetGFX(const GFX &gfx);

	void OnEndGameState();

    bool GetVirtualSize(SIZE_T &);
	bool GetPrivateBytes(SIZE_T &);
    
    static CECOptimize & Instance();

	~CECOptimize();

  private:
    CECOptimize();
    
	//	not implemented
	CECOptimize(const CECOptimize &);
    CECOptimize & operator=(const CECOptimize &);
    
    GFX		m_gfx;
	HMODULE	m_hNTDLL;
};
