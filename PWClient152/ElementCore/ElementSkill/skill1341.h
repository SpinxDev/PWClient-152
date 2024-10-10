#ifndef __CPPGEN_GNET_SKILL1341
#define __CPPGEN_GNET_SKILL1341
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1341:public Skill
    {
      public:
        enum
        { SKILL_ID = 1341 };
          Skill1341 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1341Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
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
                skill->GetPlayer ()->SetDecmp (25 + 1.5 * skill->GetLevel ());
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
      Skill1341Stub ():SkillStub (1341)
        {
            cls = 8;
            name = L"剑心通明";
            nativename = "剑心通明";
            icon = "剑心通明.dds";
            max_level = 10;
            type = 2;
            apcost = 0;
            arrowcost = 0;
            apgain = 10;
            attr = 1;
            rank = 2;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 1;
            showorder = 1105;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (1);
            }
            effect = "剑灵_剑心通明.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(1360, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1341Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (25 + 1.5 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 60000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 19, 24, 29, 34, 39, 44, 49, 54, 59, 64 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 4600, 7700, 11800, 17300, 24600, 34200, 47000, 64000, 87000, 118000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 440, 680, 1090, 1540, 1990, 2480, 2980, 3480, 3980, 12980 };
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
            return (float) (0);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), 25 + 1.5 * skill->GetLevel (), 40 + 4 * skill->GetLevel ());

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
            skill->GetVictim ()->SetTime (7000);
            skill->GetVictim ()->SetRatio (0.4 + 0.04 * skill->GetLevel ());
            skill->GetVictim ()->SetFastpray (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (0);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 0;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (0);
        }
#endif
    };
}
#endif
