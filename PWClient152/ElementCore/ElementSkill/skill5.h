#ifndef __CPPGEN_GNET_SKILL5
#define __CPPGEN_GNET_SKILL5
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill5:public Skill
    {
      public:
        enum
        { SKILL_ID = 5 };
          Skill5 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill5Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 200;
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
                skill->GetPlayer ()->SetDecmp (0.2 * (1.7 + 8.4 * skill->GetLevel ()));
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
                skill->GetPlayer ()->SetDecmp (0.8 * (1.7 + 8.4 * skill->GetLevel ()));
                skill->SetPlus (1.8 * skill->GetLevel () * skill->GetLevel () + 66.9 * skill->GetLevel () + 123.7);
                skill->SetRatio (0);
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
      Skill5Stub ():SkillStub (5)
        {
            cls = 0;
            name = L"龙现";
            nativename = "龙现";
            icon = "龙现.dds";
            max_level = 10;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 10;
            attr = 1;
            rank = 0;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
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
            effect = "龙现.sgc";
            range.type = 0;
            doenchant = false;
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
        virtual ~ Skill5Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (1.7 + 8.4 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 500;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 6000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 6, 11, 16, 21, 26, 31, 36, 41, 46, 51 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 400, 1500, 3200, 5800, 9200, 13900, 20000, 28100, 38900, 53200 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 50, 160, 320, 530, 820, 1270, 1720, 2180, 2680, 3180 };
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
            return (float) (7 + skill->GetLevel () * 0.3);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               7 + skill->GetLevel () * 0.3,
                               1.7 + 8.4 * skill->GetLevel (), 1.8 * skill->GetLevel () * skill->GetLevel () + 66.9 * skill->GetLevel () + 123.7);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 200 * skill->GetLevel ();
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (12 + 0.5 * skill->GetLevel ());
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 12;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.5 + 0.05 * skill->GetLevel ());
        }
#endif
    };
}
#endif
