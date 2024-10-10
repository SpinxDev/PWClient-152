#ifndef __CPPGEN_GNET_SKILL2574
#define __CPPGEN_GNET_SKILL2574
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2574:public Skill
    {
      public:
        enum
        { SKILL_ID = 2574 };
          Skill2574 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2574Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 1450;
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
                skill->GetPlayer ()->SetDecmp (0.2 * (126 + 22.5 * skill->GetLevel ()));
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
                return 1002;
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
                skill->GetPlayer ()->SetDecmp (0.8 * (126 + 22.5 * skill->GetLevel ()));
                skill->SetPlus (7.3 * skill->GetLevel () * skill->GetLevel () + 194.4 * skill->GetLevel () + 859.5);
                skill->SetRatio (0.85 + 0.085 * skill->GetLevel ());
                skill->GetPlayer ()->SetAddball (5);
                skill->SetGolddamage (skill->GetMagicattack ());
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
      Skill2574Stub ():SkillStub (2574)
        {
            cls = 11;
            name = L"望月咏";
            nativename = "望月咏";
            icon = "望月咏.dds";
            max_level = 10;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 15;
            attr = 2;
            rank = 3;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1006;
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
            effect = "月仙_望月咏_击中.sgc";
            range.type = 1;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2573, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2574Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (126 + 22.5 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1002;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 10000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 33, 38, 43, 48, 53, 58, 63, 68, 73, 78 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 14007, 18860, 24640, 31752, 40334, 50716, 63270, 78520, 113950, 209380 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 1450, 1900, 2380, 2880, 3380, 3880, 10980, 41980, 131980, 291980 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (1.5 + 0.15 * skill->GetLevel ());
        }
        float GetAttackdistance (Skill * skill) const
        {
            return (float) (14 + 0.6 * skill->GetLevel ());
        }
        float GetAngle (Skill * skill) const
        {
            return (float) (1 - 0.0111111 * (0));
        }
        float GetPraydistance (Skill * skill) const
        {
            return (float) (12 + 0.6 * skill->GetLevel ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               12 + 0.6 * skill->GetLevel (),
                               126 + 22.5 * skill->GetLevel (),
                               14 + 0.6 * skill->GetLevel (),
                               1.5 + 0.15 * skill->GetLevel (),
                               85 + 8.5 * skill->GetLevel (), 7.3 * skill->GetLevel () * skill->GetLevel () + 194.4 * skill->GetLevel () + 859.5);

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
            return (float) (10);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
