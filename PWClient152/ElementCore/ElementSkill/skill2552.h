#ifndef __CPPGEN_GNET_SKILL2552
#define __CPPGEN_GNET_SKILL2552
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2552:public Skill
    {
      public:
        enum
        { SKILL_ID = 2552 };
          Skill2552 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2552Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 3000;
            }
            bool Quit (Skill * skill) const
            {
                return false;
            }
            bool Loop (Skill * skill) const
            {
                return false;
            }
            bool Bypass (Skill * skill) const
            {
                return false;
            }
            void Calculate (Skill * skill) const
            {
                skill->GetPlayer ()->SetPray (1);
            }
            bool Interrupt (Skill * skill) const
            {
                return false;
            }
            bool Cancel (Skill * skill) const
            {
                return 1;
            }
            bool Skip (Skill * skill) const
            {
                return 0;
            }
        };
#endif
#ifdef _SKILL_SERVER
        class State2:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 602;
            }
            bool Quit (Skill * skill) const
            {
                return false;
            }
            bool Loop (Skill * skill) const
            {
                return false;
            }
            bool Bypass (Skill * skill) const
            {
                return false;
            }
            void Calculate (Skill * skill) const
            {
                skill->GetPlayer ()->SetDecmp (105.75 + 9 * skill->GetLevel ());
                skill->SetPlus ((skill->GetCharging () / 3000) *
                                (5.8 * skill->GetLevel () * skill->GetLevel () + 247.5 * skill->GetLevel () + 2108.1));
                skill->SetRatio ((skill->GetCharging () / 3000) * (1.55 + 0.155 * skill->GetLevel ()));
                skill->SetDamage (skill->GetAttack ());
                skill->GetPlayer ()->SetPerform (1);
            }
            bool Interrupt (Skill * skill) const
            {
                return false;
            }
            bool Cancel (Skill * skill) const
            {
                return 0;
            }
            bool Skip (Skill * skill) const
            {
                return 0;
            }
        };
#endif
#ifdef _SKILL_SERVER
        class State3:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 0;
            }
            bool Quit (Skill * skill) const
            {
                return false;
            }
            bool Loop (Skill * skill) const
            {
                return false;
            }
            bool Bypass (Skill * skill) const
            {
                return false;
            }
            void Calculate (Skill * skill) const
            {
            }
            bool Interrupt (Skill * skill) const
            {
                return false;
            }
            bool Cancel (Skill * skill) const
            {
                return 0;
            }
            bool Skip (Skill * skill) const
            {
                return 0;
            }
        };
#endif
      Skill2552Stub ():SkillStub (2552)
        {
            cls = 10;
            name = L"∞¡’∂";
            nativename = "∞¡’∂";
            icon = "∞¡’∂.dds";
            max_level = 10;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 10;
            attr = 1;
            rank = 5;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 3;
            showorder = 1005;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 3;
            {
                restrict_weapons.push_back (44878);
            }
            effect = "“π”∞_∞¡’∂_ª˜÷–.sgc";
            range.type = 2;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2549, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2552Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (105.75 + 9 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 602;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 25000 - 1000 * skill->GetLevel ();
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 51, 55, 59, 63, 67, 71, 75, 79, 83, 87 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 36708, 44265, 53070, 63270, 75260, 92310, 146300, 235410, 369180, 567490 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 3180, 3580, 3980, 10980, 31980, 91980, 171980, 331980, 611980, 931980 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (12);
        }
        float GetAttackdistance (Skill * skill) const
        {
            return (float) (0);
        }
        float GetAngle (Skill * skill) const
        {
            return (float) (1 - 0.0111111 * (0));
        }
        float GetPraydistance (Skill * skill) const
        {
            return (float) (skill->GetPlayer ()->GetRange ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               105.75 + 9 * skill->GetLevel (),
                               25 - 1.0 * skill->GetLevel (),
                               155 + 15.5 * skill->GetLevel (), 5.8 * skill->GetLevel () * skill->GetLevel () + 247.5 * skill->GetLevel () + 2108.1);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (2000);
            skill->GetVictim ()->SetDizzy (1);
            skill->GetVictim ()->SetProbability (1.0 * -1);
            skill->GetVictim ()->SetTime (2601 + 7000 * skill->GetT0 () / 3000);
            skill->GetVictim ()->SetWeapondisabled (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (10);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (2.3 + 0.05 * skill->GetLevel ());
        }
        float GetTalent0 (PlayerWrapper * player) const
        {
            return (float) (player->GetCharging ());
        }
#endif
    };
}
#endif
