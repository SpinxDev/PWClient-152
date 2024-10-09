#ifndef __CPPGEN_GNET_SKILL2573
#define __CPPGEN_GNET_SKILL2573
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2573:public Skill
    {
      public:
        enum
        { SKILL_ID = 2573 };
          Skill2573 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2573Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 1450;
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
                skill->GetPlayer ()->SetDecmp (0.2 * (90 + 22.5 * skill->GetLevel ()));
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
                return 1001;
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
                skill->GetPlayer ()->SetDecmp (0.8 * (90 + 22.5 * skill->GetLevel ()));
                skill->SetPlus (7.7 * skill->GetLevel () * skill->GetLevel () + 179.6 * skill->GetLevel () + 595.4);
                skill->SetRatio (0.85 + 0.085 * skill->GetLevel ());
                skill->GetPlayer ()->SetAddball (5);
                skill->SetGolddamage (skill->GetMagicattack ());
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
      Skill2573Stub ():SkillStub (2573)
        {
            cls = 11;
            name = L"√˘“Ù”Ω";
            nativename = "√˘“Ù”Ω";
            icon = "√˘“Ù”Ω.dds";
            max_level = 10;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 10;
            attr = 2;
            rank = 2;
            eventflag = 0;
            clslimit = 0;
            time_type = 0;
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
                restrict_weapons.push_back (44879);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "‘¬œ…_√˘“˜”Ω_ª˜÷–.sgc";
            range.type = 2;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2572, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2573Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (90 + 22.5 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1001;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 10000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 25, 30, 35, 40, 45, 50, 55, 60, 65, 70 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 7980, 11520, 15810, 21040, 27375, 35000, 44265, 55440, 68750, 85000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 0 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 730, 1180, 1630, 2080, 2580, 3080, 3580, 4980, 14980, 71980 };
            return array[skill->GetLevel () - 1];
        }
        int GetMaxAbility (Skill * skill) const
        {
            static int array[10] = { 0 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredRealmLevel (Skill * skill) const
        {
            static int array[10] = { 0 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (8 + 0.4 * skill->GetLevel ());
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
            return (float) (5 + 0.4 * skill->GetLevel ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               5 + 0.4 * skill->GetLevel (),
                               90 + 22.5 * skill->GetLevel (),
                               8 + 0.4 * skill->GetLevel (),
                               85 + 8.5 * skill->GetLevel (), 7.7 * skill->GetLevel () * skill->GetLevel () + 179.6 * skill->GetLevel () + 595.4);

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
            skill->GetVictim ()->SetTime (11001);
            skill->GetVictim ()->SetRatio (skill->GetT0 () == 11 ? 0.43 : 0.25);
            skill->GetVictim ()->SetSlowattack (1);
            skill->GetVictim ()->SetProbability (1.0 * -1);
            skill->GetVictim ()->SetTime (7001);
            skill->GetVictim ()->SetRatio (skill->GetT0 () == 11 ? 0.22 : 0.14);
            skill->GetVictim ()->SetSlowpray (1);
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
        int GetAttackspeed (Skill * skill) const
        {
            return 0;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
        float GetTalent0 (PlayerWrapper * player) const
        {
            return (float) (player->GetBalls ());
        }
#endif
    };
}
#endif
