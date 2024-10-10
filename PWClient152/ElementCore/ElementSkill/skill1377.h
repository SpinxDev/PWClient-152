#ifndef __CPPGEN_GNET_SKILL1377
#define __CPPGEN_GNET_SKILL1377
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1377:public Skill
    {
      public:
        enum
        { SKILL_ID = 1377 };
          Skill1377 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1377Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 1000;
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
                skill->GetPlayer ()->SetDecmp (0.2 * (245 + 28 * skill->GetLevel ()));
                skill->GetPlayer ()->SetPray (1);
            }
            bool Interrupt (Skill * skill) const
            {
                return skill->GetRand () < 25 - 2 * skill->GetLevel ();
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
                return 500;
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
                skill->GetPlayer ()->SetDecmp (0.8 * (245 + 28 * skill->GetLevel ()));
                skill->SetPlus (2.1 * skill->GetLevel () * skill->GetLevel () + 127.9 * skill->GetLevel () + 936.2);
                skill->SetRatio (0.5 + 0.05 * skill->GetLevel ());
                skill->SetWooddamage (skill->GetMagicattack ());
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
      Skill1377Stub ():SkillStub (1377)
        {
            cls = 9;
            name = L"乱舞清风";
            nativename = "乱舞清风";
            icon = "长风破.dds";
            max_level = 10;
            type = 1;
            apcost = 30;
            arrowcost = 0;
            apgain = 0;
            attr = 3;
            rank = 4;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1208;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (292);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "魅灵_东风咒.sgc";
            range.type = 2;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(1376, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill1377Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (245 + 28 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 500;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 8000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 39, 43, 47, 51, 55, 59, 63, 67, 71, 75 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 24600, 32000, 41400, 53200, 68100, 87000, 111000, 142000, 181000, 266000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 1990, 2380, 2780, 3180, 3580, 3980, 10980, 31980, 91980, 171980 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (10 + 0.4 * skill->GetLevel ());
        }
        float GetAttackdistance (Skill * skill) const
        {
            return (float) (4 + 0.3 * skill->GetLevel ());
        }
        float GetAngle (Skill * skill) const
        {
            return (float) (1 - 0.0111111 * (60));
        }
        float GetPraydistance (Skill * skill) const
        {
            return (float) (8 + 0.4 * skill->GetLevel ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               8 + 0.4 * skill->GetLevel (),
                               245 + 28 * skill->GetLevel (),
                               10 + 0.4 * skill->GetLevel (),
                               50 + 5 * skill->GetLevel (), 2.1 * skill->GetLevel () * skill->GetLevel () + 127.9 * skill->GetLevel () + 936.2);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 47);
            skill->GetVictim ()->SetTime (3000 + 200 * skill->GetLevel ());
            skill->GetVictim ()->SetSealed (1);
            skill->GetVictim ()->SetProbability (1.0 * 47);
            skill->GetVictim ()->SetTime (3000 + 200 * skill->GetLevel ());
            skill->GetVictim ()->SetFix (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (25 + skill->GetLevel ());
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 11;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1);
        }
#endif
    };
}
#endif
