#ifndef __CPPGEN_GNET_SKILL2370
#define __CPPGEN_GNET_SKILL2370
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2370:public Skill
    {
      public:
        enum
        { SKILL_ID = 2370 };
          Skill2370 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2370Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (33);
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
                skill->GetPlayer ()->SetDecmp (132);
                skill->SetPlus (5115);
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
      Skill2370Stub ():SkillStub (2370)
        {
            cls = 0;
            name = L"Ðþ¡¤ÅüÐÇÕ¶ÔÂ";
            nativename = "Ðþ¡¤ÅüÐÇÕ¶ÔÂ";
            icon = "Ðþ¡¤ÅüÐÇÕ¶ÔÂ.dds";
            max_level = 1;
            type = 1;
            apcost = 0;
            arrowcost = 0;
            apgain = 15;
            attr = 1;
            rank = 21;
            eventflag = 0;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1116;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 1;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (5);
            }
            {
                restrict_weapons.push_back (1);
            }
            effect = "1Á÷ÐÇ¸ÏÔÂ.sgc";
            range.type = 1;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(402, 1));
            pre_skills.push_back (std::pair < ID, int >(418, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2370Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (165);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 800;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 6000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 100 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 1000000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 42131 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 1000000 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (3.5);
        }
        float GetAttackdistance (Skill * skill) const
        {
            return (float) (15);
        }
        float GetAngle (Skill * skill) const
        {
            return (float) (1 - 0.0111111 * (0));
        }
        float GetPraydistance (Skill * skill) const
        {
            return (float) (8);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format);
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
            skill->GetVictim ()->SetValue (18);
            skill->GetVictim ()->SetRepel (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetBreak (1);
            skill->GetVictim ()->SetProbability (1.0 * skill->GetT1 ());
            skill->GetVictim ()->SetTime (6000);
            skill->GetVictim ()->SetDizzy (1);
            return true;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * skill->GetT0 ());
            skill->GetVictim ()->SetTime (1000);
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->SetAmount (35);
            skill->GetVictim ()->SetValue (0);
            skill->GetVictim ()->SetApgencont2 (1);
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
            return 6;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.7);
        }
        float GetTalent0 (PlayerWrapper * player) const
        {
            return (float) (player->GetCurweapon () == 1 ? 100 : 0);
        }
        float GetTalent1 (PlayerWrapper * player) const
        {
            return (float) (player->GetCurweapon () == 5 ? 33 : 0);
        }
#endif
    };
}
#endif
