#ifndef __CPPGEN_GNET_SKILL328
#define __CPPGEN_GNET_SKILL328
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill328:public Skill
    {
      public:
        enum
        { SKILL_ID = 328 };
          Skill328 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill328Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 6000;
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
                skill->GetPlayer ()->SetDecmp (150 * skill->GetLevel ());
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
      Skill328Stub ():SkillStub (328)
        {
            cls = 3;
            name = L"驯服宠物";
            nativename = "驯服宠物";
            icon = "驯服宠物.dds";
            max_level = 10;
            type = 3;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 0;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1625;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 2;
            allow_forms = 3;
            {
                restrict_weapons.push_back (292);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "驯服宠物.sgc";
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
        virtual ~ Skill328Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (150 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1000;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 25000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 7, 17, 27, 37, 47, 57, 67, 77, 87, 97 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 240, 1480, 4000, 8600, 16560, 30760, 56800, 130800, 338800, 718000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 11526 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 70, 360, 910, 1810, 2780, 3780, 31980, 251980, 931980, 2291980 };
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
            return (float) (25);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), 12, 150 * skill->GetLevel (), 150 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 1;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetEntrap (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (35);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1);
        }
#endif
    };
}
#endif
