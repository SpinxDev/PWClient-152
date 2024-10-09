#ifndef __CPPGEN_GNET_SKILL99
#define __CPPGEN_GNET_SKILL99
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill99:public Skill
    {
      public:
        enum
        { SKILL_ID = 99 };
          Skill99 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill99Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 5000;
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
                skill->GetPlayer ()->SetDecmp (0.2 * (336 + 18 * skill->GetLevel ()));
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
                return 1400;
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
                skill->GetPlayer ()->SetDecmp (0.8 * (336 + 18 * skill->GetLevel ()));
                skill->SetPlus (5.4 * skill->GetLevel () * skill->GetLevel () + 339.8 * skill->GetLevel () + 4462.4);
                skill->SetRatio (2.5 + 0.25 * skill->GetLevel ());
                skill->SetEarthdamage (skill->GetMagicattack ());
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
      Skill99Stub ():SkillStub (99)
        {
            cls = 1;
            name = L"̩ɽѹ��";
            nativename = "̩ɽѹ��";
            icon = "̩ɽѹ��.dds";
            max_level = 10;
            type = 1;
            apcost = 200;
            arrowcost = 0;
            apgain = 0;
            attr = 6;
            rank = 6;
            eventflag = 0;
            posdouble = 0;
            time_type = 0;
            showorder = 1224;
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
            effect = "̩ɽѹ��.sgc";
            range.type = 3;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(184, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill99Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (336 + 18 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1400;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 30000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 59, 62, 65, 68, 71, 74, 77, 80, 83, 86 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 87000, 104000, 125000, 151000, 181000, 239000, 327000, 440000, 586000, 773000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 3980, 8980, 14980, 41980, 91980, 151980, 251980, 371980, 611980, 851980 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (8 + 0.4 * skill->GetLevel ());
        }
        float GetAttackdistance (Skill * skill) const
        {
            return (float) (4 + 0.4 * skill->GetLevel ());
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
                               skill->GetLevel (),
                               19.5 + 0.9 * skill->GetLevel (),
                               336 + 18 * skill->GetLevel (),
                               8 + 0.4 * skill->GetLevel (),
                               250 + 25 * skill->GetLevel (),
                               5.4 * skill->GetLevel () * skill->GetLevel () + 339.8 * skill->GetLevel () + 4462.4,
                               85 + skill->GetLevel (), 3 + 0.3 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 1000 * skill->GetLevel ();
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 85 + skill->GetLevel ());
            skill->GetVictim ()->SetTime (4000 + 300 * skill->GetLevel ());
            skill->GetVictim ()->SetDizzy (1);
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
            return 31;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1);
        }
#endif
    };
}
#endif
