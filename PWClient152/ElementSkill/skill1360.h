#ifndef __CPPGEN_GNET_SKILL1360
#define __CPPGEN_GNET_SKILL1360
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1360:public Skill
    {
      public:
        enum
        { SKILL_ID = 1360 };
          Skill1360 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1360Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (10 + 5 * skill->GetLevel ());
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
      Skill1360Stub ():SkillStub (1360)
        {
            cls = 8;
            name = L"åÐÒ£¾÷";
            nativename = "åÐÒ£¾÷";
            icon = "åÐÒ£¾÷.dds";
            max_level = 10;
            type = 2;
            apcost = 0;
            arrowcost = 0;
            apgain = 10;
            attr = 0;
            rank = 1;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 1;
            showorder = 1133;
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
            effect = "½£Áé_åÐÒ£¾÷.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1360Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (10 + 5 * skill->GetLevel ());
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
            static int array[10] = { 9, 14, 19, 24, 29, 34, 39, 44, 49, 54 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 800, 1920, 3680, 6160, 9440, 13840, 19680, 27360, 37600, 51200 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 110, 250, 440, 680, 1090, 1540, 1990, 2480, 2980, 3480 };
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
            return _snwprintf (buffer, length, format, skill->GetLevel (), 10 + 5 * skill->GetLevel (), 40 + 5 * skill->GetLevel ());

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
            skill->GetVictim ()->SetTime (15000);
            skill->GetVictim ()->SetRatio (0.4 + 0.05 * skill->GetLevel ());
            skill->GetVictim ()->SetSpeedup (1);
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
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
