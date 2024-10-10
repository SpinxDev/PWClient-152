#ifndef __CPPGEN_GNET_SKILL1151
#define __CPPGEN_GNET_SKILL1151
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1151:public Skill
    {
      public:
        enum
        { SKILL_ID = 1151 };
          Skill1151 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1151Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 1200;
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
                skill->GetPlayer ()->SetDecmp (0.2 * (10 + 6.5 * skill->GetLevel ()));
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
                skill->GetPlayer ()->SetDecmp (0.8 * (10 + 6.5 * skill->GetLevel ()));
                skill->SetPlus (6.1 * skill->GetLevel () * skill->GetLevel () + 82 * skill->GetLevel () - 50.4);
                skill->SetRatio (0.5 + 0.05 * skill->GetLevel ());
                skill->SetWaterdamage (skill->GetMagicattack ());
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
      Skill1151Stub ():SkillStub (1151)
        {
            cls = 2;
            name = L"543523";
            nativename = "543523";
            icon = "±ù·â.dds";
            max_level = 10;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 5;
            attr = 4;
            rank = 1;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 0;
            allow_land = 0;
            allow_air = 0;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 1;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 2;
            {
                restrict_weapons.push_back (25333);
            }
            {
                restrict_weapons.push_back (0);
            }
            effect = "ÁÒ»ð·û.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(1137, 2));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill1151Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (10 + 6.5 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1000;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 6000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 16, 21, 26, 31, 36, 41, 46, 51, 56, 61 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 3200, 5800, 9200, 13900, 20000, 28100, 38900, 53200, 72400, 98300 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 3800, 7000, 12000, 18000, 26000, 37000, 51000, 70000, 96000, 130000 };
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
            return (float) (19.5 + 0.9 * skill->GetLevel ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               19.5 + 0.9 * skill->GetLevel (),
                               10 + 5 * skill->GetLevel (),
                               50 + 5 * skill->GetLevel (),
                               4.3 * skill->GetLevel () * skill->GetLevel () + 57.4 * skill->GetLevel () - 38,
                               7 * skill->GetLevel () * skill->GetLevel () + 174.4 * skill->GetLevel () + 676.4, 4 + 0.4 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 150 * skill->GetLevel ();
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (15000);
            skill->GetVictim ()->SetAmount (7 * skill->GetLevel () * skill->GetLevel () + 174.4 * skill->GetLevel () + 676.4);
            skill->GetVictim ()->SetBleeding (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (4000 + 400 * skill->GetLevel ());
            skill->GetVictim ()->SetRatio (50);
            skill->GetVictim ()->SetSlow (1);
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
            return 27;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (2 + 0.1 * skill->GetLevel ());
        }
#endif
    };
}
#endif
