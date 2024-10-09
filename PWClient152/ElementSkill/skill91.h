#ifndef __CPPGEN_GNET_SKILL91
#define __CPPGEN_GNET_SKILL91
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill91:public Skill
    {
      public:
        enum
        { SKILL_ID = 91 };
          Skill91 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill91Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 800;
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
                skill->GetPlayer ()->SetDecmp (0.2 * (200 + 25 * skill->GetLevel ()));
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
                skill->GetPlayer ()->SetDecmp (0.8 * (200 + 25 * skill->GetLevel ()));
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
      Skill91Stub ():SkillStub (91)
        {
            cls = 1;
            name = L"ËªÈÐ";
            nativename = "ËªÈÐ";
            icon = "ËªÈÐ.dds";
            max_level = 10;
            type = 2;
            apcost = 0;
            arrowcost = 0;
            apgain = 10;
            attr = 4;
            rank = 4;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1214;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (0);
            }
            {
                restrict_weapons.push_back (292);
            }
            effect = "ËªÈÐ.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(90, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill91Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (200 + 25 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1000;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 3000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 39, 43, 47, 51, 55, 59, 63, 67, 71, 75 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 6150, 8000, 10350, 13300, 17025, 21750, 27750, 35500, 45250, 66500 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 1990, 2380, 2780, 3180, 3580, 3980, 10980, 31980, 91980, 171980 };
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
            return (float) (19.5 + 0.9 * skill->GetLevel ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (), 19.5 + 0.9 * skill->GetLevel (), 200 + 25 * skill->GetLevel (), 10 + 2 * skill->GetLevel ());

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
            skill->GetVictim ()->SetTime (900000);
            skill->GetVictim ()->SetRatio (0.1 + 0.02 * skill->GetLevel ());
            skill->GetVictim ()->SetIceblade (1);
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
            return 8;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1);
        }
#endif
    };
}
#endif
