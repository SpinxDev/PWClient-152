#ifndef __CPPGEN_GNET_SKILL330
#define __CPPGEN_GNET_SKILL330
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill330:public Skill
    {
      public:
        enum
        { SKILL_ID = 330 };
          Skill330 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill330Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 1500;
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
                return skill->GetRand () < 10;
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
                skill->GetPlayer ()->SetDecmp (-5 + 10 * skill->GetLevel ());
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
      Skill330Stub ():SkillStub (330)
        {
            cls = 3;
            name = L"治疗宠物";
            nativename = "治疗宠物";
            icon = "治疗宠物.dds";
            max_level = 10;
            type = 10;
            apcost = 0;
            arrowcost = 0;
            apgain = 8;
            attr = 0;
            rank = 0;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1626;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 3;
            {
                restrict_weapons.push_back (292);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "宠物治疗.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill330Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (-5 + 10 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1000;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 3000 - 200 * skill->GetLevel ();
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 3, 13, 23, 33, 43, 53, 63, 73, 83, 93 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 40, 840, 2800, 6440, 12800, 24080, 44400, 86000, 234400, 560800 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 11525 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 10, 220, 630, 1450, 2380, 3380, 10980, 131980, 611980, 1651980 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (0);
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
            return (float) (16.5 + 0.9 * skill->GetLevel ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               16.5 + 0.9 * skill->GetLevel (),
                               -5 + 10 * skill->GetLevel (),
                               3 - 0.2 * skill->GetLevel (), -10 + 55 * skill->GetLevel (), 10 + 2 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return -10 + 25 * skill->GetLevel ();
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetValue (-10 + 55 * skill->GetLevel () + skill->GetMagicdamage () * (0.1 + 0.02 * skill->GetLevel ()));
            skill->GetVictim ()->SetHeal (1);
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
        float GetHitrate (Skill * skill) const
        {
            return (float) (1);
        }
#endif
    };
}
#endif
