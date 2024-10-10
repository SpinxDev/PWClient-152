#ifndef __CPPGEN_GNET_SKILL105
#define __CPPGEN_GNET_SKILL105
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill105:public Skill
    {
      public:
        enum
        { SKILL_ID = 105 };
          Skill105 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill105Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
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
                skill->GetPlayer ()->SetDecmp (0.2 * (26.4 + 5.5 * skill->GetLevel ()));
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
                return 2000;
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
                skill->GetPlayer ()->SetDecmp (0.8 * (26.4 + 5.5 * skill->GetLevel ()));
                skill->SetPlus (0);
                skill->SetRatio (1 + 0.1 * skill->GetLevel ());
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
      Skill105Stub ():SkillStub (105)
        {
            cls = 4;
            name = L"·ç»ðÂÖ";
            nativename = "·ç»ðÂÖ";
            icon = "·ç»ðÂÖ.dds";
            max_level = 10;
            type = 1;
            apcost = 100;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 3;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1506;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (9);
            }
            effect = "·ç»ðÂÖ.sgc";
            range.type = 1;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(12, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill105Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (26.4 + 5.5 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 2000;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 15000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 29, 34, 39, 44, 49, 54, 59, 64, 69, 74 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 11800, 17300, 24600, 34200, 47000, 64000, 87000, 118000, 160000, 239000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 1090, 1540, 1990, 2480, 2980, 3480, 3980, 12980, 51980, 151980 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (3.5);
        }
        float GetAttackdistance (Skill * skill) const
        {
            return (float) (8 + 0.4 * skill->GetLevel ());
        }
        float GetAngle (Skill * skill) const
        {
            return (float) (1 - 0.0111111 * (20 + 2.5 * skill->GetLevel ()));
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
                               26.4 + 5.5 * skill->GetLevel (),
                               8 + 0.4 * skill->GetLevel (),
                               100 + 10 * skill->GetLevel (), 4.4 * skill->GetLevel () * skill->GetLevel () + 161.3 * skill->GetLevel () + 725);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 300 * skill->GetLevel ();
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (15000);
            skill->GetVictim ()->SetAmount (4.4 * skill->GetLevel () * skill->GetLevel () + 161.3 * skill->GetLevel () + 725);
            skill->GetVictim ()->SetBurning (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (8);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 15;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.5 + 0.1 * skill->GetLevel ());
        }
#endif
    };
}
#endif
