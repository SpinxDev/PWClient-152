#ifndef __CPPGEN_GNET_SKILL2568
#define __CPPGEN_GNET_SKILL2568
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2568:public Skill
    {
      public:
        enum
        { SKILL_ID = 2568 };
          Skill2568 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2568Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 67;
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
                skill->GetPlayer ()->SetDecmp (87.75 + 7.8 * skill->GetLevel ());
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
                return 601;
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
                skill->SetSection (1);
                skill->SetPlus (5.6 * skill->GetLevel () * skill->GetLevel () + 234.2 * skill->GetLevel () + 1924.8);
                skill->SetRatio (0.3 + 0.03 * skill->GetLevel ());
                skill->SetDamage (0.4 * skill->GetAttack ());
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
                return 868;
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
                skill->SetSection (2);
                skill->SetPlus (5.6 * skill->GetLevel () * skill->GetLevel () + 234.2 * skill->GetLevel () + 1924.8);
                skill->SetRatio (0.3 + 0.03 * skill->GetLevel ());
                skill->SetDamage (0.6 * skill->GetAttack ());
                skill->GetPlayer ()->SetPerform (0);
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
        class State5:public SkillStub::State
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
      Skill2568Stub ():SkillStub (2568)
        {
            cls = 10;
            name = L"²ÔÄñ";
            nativename = "²ÔÄñ";
            icon = "²ÔÄñ.dds";
            max_level = 10;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 15;
            attr = 7;
            rank = 5;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1015;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 1;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 3;
            {
                restrict_weapons.push_back (44878);
            }
            effect = "";
            range.type = 0;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2563, 1));
            combosk_preskill = 2563;
            combosk_interval = 6000;
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
            statestub.push_back (new State4 ());
            statestub.push_back (new State5 ());
#endif
        }
        virtual ~ Skill2568Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (87.75 + 7.8 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return -1;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 30000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 49, 53, 57, 61, 65, 69, 73, 77, 81, 85 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 33370, 40334, 48447, 57997, 68750, 81600, 113950, 186390, 295240, 458250 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 2980, 3380, 3780, 6980, 14980, 51980, 131980, 251980, 451980, 771980 };
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
                               87.75 + 7.8 * skill->GetLevel (),
                               30 + 3 * skill->GetLevel (), 5.6 * skill->GetLevel () * skill->GetLevel () + 234.2 * skill->GetLevel () + 1924.8);

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
            skill->GetVictim ()->SetTime (3000);
            skill->GetVictim ()->SetValue (2);
            skill->GetVictim ()->SetPalsy (1);
            return true;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (1469);
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->SetInvincible (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (skill->GetPlayer ()->GetRange () + 1.7 * 5.5);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (2.3 + 0.05 * skill->GetLevel ());
        }
#endif
    };
}
#endif
