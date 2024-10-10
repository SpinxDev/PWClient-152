#ifndef __CPPGEN_GNET_SKILL2566
#define __CPPGEN_GNET_SKILL2566
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2566:public Skill
    {
      public:
        enum
        { SKILL_ID = 2566 };
          Skill2566 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2566Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (0.2 * (68.4 + 7.2 * skill->GetLevel ()));
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
                return 1101;
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
                skill->GetPlayer ()->SetDecmp (0.8 * (68.4 + 7.2 * skill->GetLevel ()));
                skill->SetPlus (5.4 * skill->GetLevel () * skill->GetLevel () + 205.2 * skill->GetLevel () + 1465.2);
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
      Skill2566Stub ():SkillStub (2566)
        {
            cls = 10;
            name = L"列星";
            nativename = "列星";
            icon = "列星.dds";
            max_level = 10;
            type = 1;
            apcost = 30;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 4;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1018;
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
            effect = "夜影_列星_击中.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2564, 1));
            combosk_preskill = 2564;
            combosk_interval = 6000;
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2566Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (68.4 + 7.2 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1101;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 6000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 42, 46, 50, 54, 58, 62, 66, 70, 74, 78 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 23400, 28786, 35000, 42240, 50716, 60320, 71820, 85000, 129060, 209380 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 2280, 2680, 3080, 3480, 3880, 8980, 22980, 71980, 151980, 291980 };
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
                               68.4 + 7.2 * skill->GetLevel (),
                               5.4 * skill->GetLevel () * skill->GetLevel () + 205.2 * skill->GetLevel () + 1465.2, 10 + 2 * skill->GetLevel ());

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
            skill->GetVictim ()->SetTime (8101);
            skill->GetVictim ()->SetRatio (0.1 + 0.02 * skill->GetLevel ());
            skill->GetVictim ()->SetDecdefence (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (skill->GetPlayer ()->GetRange () + 1.35 * 5.5);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (2.4 + skill->GetLevel () * 0.06);
        }
#endif
    };
}
#endif
