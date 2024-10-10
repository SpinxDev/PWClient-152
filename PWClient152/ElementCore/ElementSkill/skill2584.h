#ifndef __CPPGEN_GNET_SKILL2584
#define __CPPGEN_GNET_SKILL2584
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2584:public Skill
    {
      public:
        enum
        { SKILL_ID = 2584 };
          Skill2584 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2584Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (0.2 * (225 + 13.5 * skill->GetLevel ()));
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
                return 1468;
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
                skill->GetPlayer ()->SetDecmp (0.8 * (225 + 13.5 * skill->GetLevel ()));
                skill->SetPlus (3.3 * skill->GetLevel () * skill->GetLevel () + 196.5 * skill->GetLevel () + 2349.9);
                skill->SetRatio (0.85 + 0.085 * skill->GetLevel ());
                skill->SetWaterdamage ((skill->GetPlayer ()->GetBalls () == 15 ? 1.2 : 1) * skill->GetMagicattack ());
                skill->GetPlayer ()->SetAddball (1);
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
      Skill2584Stub ():SkillStub (2584)
        {
            cls = 11;
            name = L"À×¼«±ù±©";
            nativename = "À×¼«±ù±©";
            icon = "À×¼«±ù±©.dds";
            max_level = 10;
            type = 1;
            apcost = 30;
            arrowcost = 0;
            apgain = 0;
            attr = 2;
            rank = 5;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1019;
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
            effect = "ÔÂÏÉ_À×¼«±ù±©_»÷ÖÐ.sgc";
            range.type = 0;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2583, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2584Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (225 + 13.5 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1468;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 18000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 53, 56, 59, 62, 65, 68, 71, 74, 77, 80 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 40334, 46336, 53070, 60320, 68750, 78520, 92310, 129060, 186390, 264000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 3380, 3680, 3980, 8980, 14980, 41980, 91980, 151980, 251980, 371980 };
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
                               225 + 13.5 * skill->GetLevel (),
                               85 + 8.5 * skill->GetLevel (), 3.3 * skill->GetLevel () * skill->GetLevel () + 196.5 * skill->GetLevel () + 2349.9);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
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
