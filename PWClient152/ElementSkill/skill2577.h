#ifndef __CPPGEN_GNET_SKILL2577
#define __CPPGEN_GNET_SKILL2577
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2577:public Skill
    {
      public:
        enum
        { SKILL_ID = 2577 };
          Skill2577 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2577Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 401;
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
                skill->GetPlayer ()->SetDecmp (0.2 * (313.2 + 16.2 * skill->GetLevel ()));
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
                skill->GetPlayer ()->SetDecmp (0.8 * (313.2 + 16.2 * skill->GetLevel ()));
                skill->GetPlayer ()->SetAddball (5);
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
      Skill2577Stub ():SkillStub (2577)
        {
            cls = 11;
            name = L"±ÀÀ×¼Àìë";
            nativename = "±ÀÀ×¼Àìë";
            icon = "±ÀÀ×¼Àìë.dds";
            max_level = 10;
            type = 3;
            apcost = 100;
            arrowcost = 0;
            apgain = 0;
            attr = 2;
            rank = 6;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1011;
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
            effect = "ÔÂÏÉÄÐ_±ÀÀ×¼Àìë_»÷ÖÐ.sgc";
            range.type = 0;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2576, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2577Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (313.2 + 16.2 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1001;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 30000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 61, 64, 67, 70, 73, 76, 79, 82, 85, 88 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 57997, 66080, 75260, 85000, 113950, 165200, 235410, 330460, 458250, 629680 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 6980, 12980, 31980, 71980, 131980, 211980, 331980, 531980, 771980, 1011980 };
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
                               313.2 + 16.2 * skill->GetLevel (),
                               8 + 0.4 * skill->GetLevel (),
                               0.25 * 3.6 * skill->GetLevel () * skill->GetLevel () + 0.25 * 232.9 * skill->GetLevel () + 0.25 * 3149.2);

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
            skill->GetVictim ()->SetTime (1000);
            skill->GetVictim ()->SetAmount (1);
            skill->GetVictim ()->SetToxic (1);
            return true;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetTime (13001);
            skill->GetVictim ()->SetRatio (skill->GetLevel ());
            skill->GetVictim ()->SetAmount (1);
            skill->GetVictim ()->SetValue (45990);
            skill->GetVictim ()->SetSummonplantpet (1);
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
