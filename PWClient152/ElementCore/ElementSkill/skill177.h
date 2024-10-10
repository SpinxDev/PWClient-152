#ifndef __CPPGEN_GNET_SKILL177
#define __CPPGEN_GNET_SKILL177
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill177:public Skill
    {
      public:
        enum
        { SKILL_ID = 177 };
          Skill177 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill177Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (20);
                skill->GetPlayer ()->SetPerform (1);
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
      Skill177Stub ():SkillStub (177)
        {
            cls = 0;
            name = L"易筋经";
            nativename = "易筋经";
            icon = "易筋经.dds";
            max_level = 10;
            type = 2;
            apcost = 0;
            arrowcost = 0;
            apgain = 10;
            attr = 1;
            rank = 1;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 1;
            showorder = 1131;
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
                restrict_weapons.push_back (1);
            }
            {
                restrict_weapons.push_back (182);
            }
            {
                restrict_weapons.push_back (5);
            }
            {
                restrict_weapons.push_back (292);
            }
            {
                restrict_weapons.push_back (9);
            }
            effect = "易筋经.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill177Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (20);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 10000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 16, 21, 26, 31, 36, 41, 46, 51, 56, 61 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 2560, 4640, 7360, 11120, 16000, 22480, 31120, 42560, 57920, 78640 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 320, 530, 820, 1270, 1720, 2180, 2680, 3180, 3680, 6980 };
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
            return _snwprintf (buffer, length, format, skill->GetLevel (), 20 + 10 * skill->GetLevel (), 20 + 10 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetTime (30000);
            skill->GetVictim ()->SetRatio (0.2 + 0.1 * skill->GetLevel ());
            skill->GetVictim ()->SetAmount (0.2 + 0.1 * skill->GetLevel ());
            skill->GetVictim ()->SetYijin (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (5);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (0);
        }
#endif
    };
}
#endif
