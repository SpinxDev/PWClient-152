#ifndef __CPPGEN_GNET_SKILL71
#define __CPPGEN_GNET_SKILL71
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill71:public Skill
    {
      public:
        enum
        { SKILL_ID = 71 };
          Skill71 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill71Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 300;
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
                skill->GetPlayer ()->SetDecmp (0.2 * (94.5 + 8.4 * skill->GetLevel ()));
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
                skill->GetPlayer ()->SetDecmp (0.8 * (94.5 + 8.4 * skill->GetLevel ()));
                skill->SetPlus (1.3 * skill->GetLevel () * skill->GetLevel () + 123.5 * skill->GetLevel () + 1077.7);
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
      Skill71Stub ():SkillStub (71)
        {
            cls = 0;
            name = L"ÁÑÑÒÑ×Õð";
            nativename = "ÁÑÑÒÑ×Õð";
            icon = "°ÔÍõ±©Å­.dds";
            max_level = 10;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 10;
            attr = 1;
            rank = 5;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1119;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 1;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (9);
            }
            effect = "°ÔÍõ±©Å­.sgc";
            range.type = 2;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(70, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill71Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (94.5 + 8.4 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1400;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 8000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 49, 53, 57, 61, 65, 69, 73, 77, 81, 85 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 47000, 60200, 76900, 98300, 125000, 160000, 215000, 327000, 484000, 705000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 2980, 3380, 3780, 6980, 14980, 51980, 131980, 251980, 451980, 771980 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (10);
        }
        float GetAttackdistance (Skill * skill) const
        {
            return (float) (0);
        }
        float GetAngle (Skill * skill) const
        {
            return (float) (1 - 0.0111111 * (20 + 3 * skill->GetLevel ()));
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
                               94.5 + 8.4 * skill->GetLevel (),
                               1.3 * skill->GetLevel () * skill->GetLevel () + 123.5 * skill->GetLevel () + 1077.7,
                               30 + 2 * skill->GetLevel (), 4 + 0.4 * skill->GetLevel ());

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
            skill->GetVictim ()->SetTime (4000 + 400 * skill->GetLevel ());
            skill->GetVictim ()->SetRatio (0.3 + 0.02 * skill->GetLevel ());
            skill->GetVictim ()->SetSlow (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (0);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 11;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.5 + 0.1 * skill->GetLevel ());
        }
#endif
    };
}
#endif
