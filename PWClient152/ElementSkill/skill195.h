#ifndef __CPPGEN_GNET_SKILL195
#define __CPPGEN_GNET_SKILL195
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill195:public Skill
    {
      public:
        enum
        { SKILL_ID = 195 };
          Skill195 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill195Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (0.2 * (-1.6 + 4 * skill->GetLevel ()));
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
                return 800;
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
                skill->GetPlayer ()->SetDecmp (0.8 * (-1.6 + 4 * skill->GetLevel ()));
                skill->SetPlus (0);
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
      Skill195Stub ():SkillStub (195)
        {
            cls = 4;
            name = L"Ω …±";
            nativename = "Ω …±";
            icon = "Ω …±.dds";
            max_level = 10;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 5;
            attr = 1;
            rank = 0;
            eventflag = 4;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1502;
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
            effect = "Ω …±.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(102, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill195Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (-1.6 + 4 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 800;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 8000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 3, 8, 13, 18, 23, 28, 33, 38, 43, 48 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 100, 800, 2100, 4100, 7000, 10900, 16100, 23000, 32000, 44100 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 10, 90, 220, 400, 630, 1000, 1450, 1900, 2380, 2880 };
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
                               -1.6 + 4 * skill->GetLevel (), 4.8 * skill->GetLevel () * skill->GetLevel () + 103 * skill->GetLevel () + 72.5);

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
            skill->GetVictim ()->SetTime (15000);
            skill->GetVictim ()->SetAmount (4.8 * skill->GetLevel () * skill->GetLevel () + 103 * skill->GetLevel () + 72.5);
            skill->GetVictim ()->SetBleeding (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (12.5);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 3;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.2 + 0.05 * skill->GetLevel ());
        }
#endif
    };
}
#endif
