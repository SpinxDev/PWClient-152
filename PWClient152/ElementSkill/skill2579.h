#ifndef __CPPGEN_GNET_SKILL2579
#define __CPPGEN_GNET_SKILL2579
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2579:public Skill
    {
      public:
        enum
        { SKILL_ID = 2579 };
          Skill2579 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2579Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 1001;
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
                skill->GetPlayer ()->SetDecmp (0.2 * (-3 + 15 * skill->GetLevel ()));
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
                return 1001;
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
                skill->GetPlayer ()->SetDecmp (0.8 * (-3 + 15 * skill->GetLevel ()));
                skill->SetPlus (5.3 * skill->GetLevel () * skill->GetLevel () + 79.6 * skill->GetLevel () + 98.3);
                skill->SetRatio (0.6 + 0.06 * skill->GetLevel ());
                skill->GetPlayer ()->SetAddball (1);
                skill->SetWaterdamage ((skill->GetPlayer ()->GetForm () == 1 ? 1.2 : 0.9) * skill->GetMagicattack ());
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
      Skill2579Stub ():SkillStub (2579)
        {
            cls = 11;
            name = L"月诗引";
            nativename = "月诗引";
            icon = "月诗引.dds";
            max_level = 10;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 10;
            attr = 4;
            rank = 0;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1014;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 3;
            {
                restrict_weapons.push_back (44879);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "月仙_月诗引_击中.sgc";
            range.type = 0;
            doenchant = false;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            is_movingcast = true;
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2579Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (-3 + 15 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1001;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 1000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 4, 9, 14, 19, 24, 29, 34, 39, 44, 49 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 200, 1000, 2400, 4600, 7392, 10738, 14878, 19926, 25992, 33370 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 20, 110, 250, 440, 680, 1090, 1540, 1990, 2480, 2980 };
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
            return (float) (16.5 + 0.9 * skill->GetLevel () + skill->GetPlayer ()->GetPrayrangeplus ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               16.5 + 0.9 * skill->GetLevel (),
                               -3 + 15 * skill->GetLevel (),
                               0.9 * 60 + 0.9 * 6 * skill->GetLevel (),
                               0.9 * 5.3 * skill->GetLevel () * skill->GetLevel () + 0.9 * 79.6 * skill->GetLevel () + 0.9 * 98.3);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (6001);
            skill->GetVictim ()->SetRatio (0.2);
            skill->GetVictim ()->SetFastpray (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (25 + skill->GetLevel () + skill->GetPlayer ()->GetPrayrangeplus ());
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
