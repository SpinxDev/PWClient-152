#ifndef __CPPGEN_GNET_SKILL54
#define __CPPGEN_GNET_SKILL54
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill54:public Skill
    {
      public:
        enum
        { SKILL_ID = 54 };
          Skill54 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill54Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (0.2 * (5.6 + 7 * skill->GetLevel ()));
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
                skill->GetPlayer ()->SetDecmp (0.8 * (5.6 + 7 * skill->GetLevel ()));
                skill->SetPlus (2.9 * skill->GetLevel () * skill->GetLevel () + 89.9 * skill->GetLevel () + 174.6);
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
      Skill54Stub ():SkillStub (54)
        {
            cls = 0;
            name = L"流水";
            nativename = "流水";
            icon = "流水.dds";
            max_level = 10;
            type = 1;
            apcost = 30;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 1;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1103;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 1;
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
            effect = "流水.sgc";
            range.type = 0;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill54Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (5.6 + 7 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1000;
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
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 0 };
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
                               5.6 + 7 * skill->GetLevel (), 2.9 * skill->GetLevel () * skill->GetLevel () + 89.9 * skill->GetLevel () + 174.6);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 400 * skill->GetLevel ();
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (14.1);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 12;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (2 + 0.1 * skill->GetLevel ());
        }
#endif
    };
}
#endif
