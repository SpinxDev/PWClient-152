#ifndef __CPPGEN_GNET_SKILL245
#define __CPPGEN_GNET_SKILL245
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill245:public Skill
    {
      public:
        enum
        { SKILL_ID = 245 };
          Skill245 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill245Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (0.2 * (6 + 7.5 * skill->GetLevel ()));
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
                return 600;
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
                skill->GetPlayer ()->SetDecmp (0.8 * (6 + 7.5 * skill->GetLevel ()));
                skill->SetPlus (3.3 * skill->GetLevel () * skill->GetLevel () + 88.6 * skill->GetLevel () + 165.1);
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
      Skill245Stub ():SkillStub (245)
        {
            cls = 6;
            name = L"º®±ùÖ®Ê¸";
            nativename = "º®±ùÖ®Ê¸";
            icon = "º®±ùÖ®Ê¸.dds";
            max_level = 10;
            type = 1;
            apcost = 25;
            arrowcost = 1;
            apgain = 0;
            attr = 1;
            rank = 1;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1313;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 1;
            long_range = 1;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (13);
            }
            effect = "º®±ùÖ®Ê¸.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(244, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill245Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (6 + 7.5 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 600;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 3000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 9, 14, 19, 24, 29, 34, 39, 44, 49, 54 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 1000, 2400, 4600, 7700, 11800, 17300, 24600, 34200, 47000, 64000 };
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
            return (float) (skill->GetPlayer ()->GetRange ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               6 + 7.5 * skill->GetLevel (),
                               3.3 * skill->GetLevel () * skill->GetLevel () + 88.6 * skill->GetLevel () + 165.1, 3 + 0.2 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 100 * skill->GetLevel ();
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (4000 + 150 * skill->GetLevel ());
            skill->GetVictim ()->SetRatio (0.5);
            skill->GetVictim ()->SetSlow (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (skill->GetPlayer ()->GetRange () + 5);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 21;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1 + 0.05 * skill->GetLevel ());
        }
#endif
    };
}
#endif
