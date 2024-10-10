#ifndef __CPPGEN_GNET_SKILL2374
#define __CPPGEN_GNET_SKILL2374
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2374:public Skill
    {
      public:
        enum
        { SKILL_ID = 2374 };
          Skill2374 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2374Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
        {
          public:
            int GetTime (Skill * skill) const
            {
                return 400;
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
                skill->GetPlayer ()->SetDecmp (28);
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
                return 1500;
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
                skill->GetPlayer ()->SetDecmp (112);
                skill->SetPlus (5834);
                skill->SetRatio (1.05);
                skill->SetDamage (skill->GetAttack ());
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
      Skill2374Stub ():SkillStub (2374)
        {
            cls = 0;
            name = L"Ðþ¡¤ÉýÁúÆÆ";
            nativename = "Ðþ¡¤ÉýÁúÆÆ";
            icon = "Ðþ¡¤ÉýÁúÆÆ.DDS";
            max_level = 1;
            type = 1;
            apcost = 100;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 20;
            eventflag = 0;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1117;
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
            {
                restrict_weapons.push_back (1);
            }
            {
                restrict_weapons.push_back (5);
            }
            effect = "1°ÔÍõÁú·É.sgc";
            range.type = 0;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(406, 1));
            pre_skills.push_back (std::pair < ID, int >(414, 1));
            pre_skills.push_back (std::pair < ID, int >(398, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2374Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (140);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1500;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 15000;
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
            static int array[10] = { 42135 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 1000000 };
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
            return _snwprintf (buffer, length, format);
        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 3000;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (5000 + skill->GetT0 ());
            skill->GetVictim ()->SetValue (2);
            skill->GetVictim ()->SetPalsy (1);
            skill->GetVictim ()->SetProbability (1.0 * skill->GetT2 ());
            skill->GetVictim ()->SetTime (11000);
            skill->GetVictim ()->SetRatio (skill->GetPlayer ()->GetCls () == -1 ? 0.5 : 1);
            skill->GetVictim ()->SetSlowpray (1);
            skill->GetVictim ()->SetProbability (1.0 * skill->GetT1 () > 0 ? 100 : 0);
            skill->GetVictim ()->SetTime (6000);
            skill->GetVictim ()->SetAmount (skill->GetT1 ());
            skill->GetVictim ()->SetBleeding (1);
            return true;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 35);
            skill->GetVictim ()->SetTime (1500);
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->SetAmount (100);
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
            return (float) (19.7);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 13;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.7);
        }
        float GetTalent0 (PlayerWrapper * player) const
        {
            return (float) (player->GetCurweapon () == 9 ? 2000 : 0);
        }
        float GetTalent1 (PlayerWrapper * player) const
        {
            return (float) (player->GetCurweapon () == 5 ? 6 * player->GetEquipattack () : 0);
        }
        float GetTalent2 (PlayerWrapper * player) const
        {
            return (float) (player->GetCurweapon () == 1 ? 100 : 0);
        }
#endif
    };
}
#endif
