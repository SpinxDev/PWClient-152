#ifndef __CPPGEN_GNET_SKILL306
#define __CPPGEN_GNET_SKILL306
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill306:public Skill
    {
      public:
        enum
        { SKILL_ID = 306 };
          Skill306 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill306Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (0.2 * (59.4 + 27 * skill->GetLevel ()));
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
                skill->GetPlayer ()->SetDecmp (0.8 * (59.4 + 27 * skill->GetLevel ()));
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
      Skill306Stub ():SkillStub (306)
        {
            cls = 3;
            name = L"æ£º¨ ı";
            nativename = "æ£º¨ ı";
            icon = "æ£º¨ ı.dds";
            max_level = 10;
            type = 2;
            apcost = 0;
            arrowcost = 0;
            apgain = 5;
            attr = 0;
            rank = 1;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1608;
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
            effect = "æ£º¨ ı.sgc";
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
        virtual ~ Skill306Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (59.4 + 27 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1000;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 1000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 16, 21, 26, 31, 36, 41, 46, 51, 56, 59 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 3200, 5800, 9200, 13900, 20000, 28100, 38900, 53200, 72400, 87000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 320, 530, 820, 1270, 1720, 2180, 2680, 3180, 3680, 3980 };
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
                               16.5 + 0.9 * skill->GetLevel (), 59.4 + 27 * skill->GetLevel (), 10 + 5 * skill->GetLevel (), 2 * skill->GetLevel ());

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
            skill->GetVictim ()->SetTime (600000);
            skill->GetVictim ()->SetRatio (0.1 + 0.05 * skill->GetLevel ());
            skill->GetVictim ()->SetValue (0.02 * skill->GetLevel ());
            skill->GetVictim ()->SetRetort2 (1);
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
            return 4;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1);
        }
#endif
    };
}
#endif
