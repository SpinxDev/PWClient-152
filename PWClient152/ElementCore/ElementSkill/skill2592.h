#ifndef __CPPGEN_GNET_SKILL2592
#define __CPPGEN_GNET_SKILL2592
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2592:public Skill
    {
      public:
        enum
        { SKILL_ID = 2592 };
          Skill2592 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2592Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 1034;
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
                return 50;
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
                skill->GetPlayer ()->SetPerform (2);
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
                return 1268;
            }
            bool Quit (Skill * skill) const
            {
                return false;
            }
            bool Loop (Skill * skill) const
            {
                return skill->GetPlayer ()->GetMp () > 168 + 9 * skill->GetLevel ();
            }
            bool Bypass (Skill * skill) const
            {
                return false;
            }
            void Calculate (Skill * skill) const
            {
                skill->GetPlayer ()->SetDecmp (168 + 9 * skill->GetLevel ());
                skill->SetPlus (4.4 * skill->GetLevel () * skill->GetLevel () + 277.1 * skill->GetLevel () + 3639.2);
                skill->SetRatio (0.75 + 0.075 * skill->GetLevel ());
                skill->SetWaterdamage (0.22 * skill->GetMagicattack ());
                skill->SetGolddamage (0.22 * skill->GetMagicattack ());
                skill->GetPlayer ()->SetCombat (1);
                skill->GetPlayer ()->SetPerform (0);
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
        class State4:public SkillStub::State
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
      Skill2592Stub ():SkillStub (2592)
        {
            cls = 11;
            name = L"晓月银光";
            nativename = "晓月银光";
            icon = "晓月银光.dds";
            max_level = 10;
            type = 1;
            apcost = 150;
            arrowcost = 0;
            apgain = 0;
            attr = 2;
            rank = 6;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 2;
            showorder = 1025;
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
            effect = "月仙_晓月银光_击中.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2591, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
            statestub.push_back (new State4 ());
#endif
        }
        virtual ~ Skill2592Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (168 + 9 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return -1;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 5000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 59, 62, 65, 68, 71, 74, 77, 80, 83, 86 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 53070, 60320, 68750, 78520, 92310, 129060, 186390, 264000, 369180, 510180 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 3980, 8980, 14980, 41980, 91980, 151980, 251980, 371980, 611980, 851980 };
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
                               168 + 9 * skill->GetLevel (),
                               0.44 * 75 + 0.44 * 7.5 * skill->GetLevel (),
                               0.44 * 4.4 * skill->GetLevel () * skill->GetLevel () + 0.44 * 277.1 * skill->GetLevel () + 0.44 * 3639.2);

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
            skill->GetVictim ()->SetTime (5000);
            skill->GetVictim ()->SetRatio (0.8);
            skill->GetVictim ()->SetSlow (1);
            skill->GetVictim ()->SetProbability (1.0 * -1);
            skill->GetVictim ()->SetTime (5000);
            skill->GetVictim ()->SetRatio (0.3);
            skill->GetVictim ()->SetSlowfly (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (4000);
            skill->GetVictim ()->SetRatio (0.01);
            skill->GetVictim ()->SetAmount (15);
            skill->GetVictim ()->SetValue (2);
            skill->GetVictim ()->SetInchurt5 (1);
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
