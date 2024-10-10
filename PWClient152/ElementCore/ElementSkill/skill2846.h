#ifndef __CPPGEN_GNET_SKILL2846
#define __CPPGEN_GNET_SKILL2846
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2846:public Skill
    {
      public:
        enum
        { SKILL_ID = 2846 };
          Skill2846 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2846Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (119);
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
                return 1402;
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
                skill->GetPlayer ()->SetDecmp (475);
                skill->SetPlus (5694);
                skill->SetRatio (2.3);
                skill->SetGolddamage (0.5 * skill->GetMagicattack ());
                skill->SetDamage ((skill->GetPlayer ()->GetBalls () == 15 ? 1 : 0) * skill->GetMagicattack ());
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
      Skill2846Stub ():SkillStub (2846)
        {
            cls = 11;
            name = L"�񡤵��µӻ�";
            nativename = "�񡤵��µӻ�";
            icon = "ħ���µӻ�.dds";
            max_level = 1;
            type = 1;
            apcost = 100;
            arrowcost = 0;
            apgain = 0;
            attr = 2;
            rank = 6;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1026;
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
            effect = "����_���µӻ�_����.sgc";
            range.type = 3;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            combosk_preskill = 2804;
            combosk_interval = 30000;
            is_inherent = true;
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2846Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (594);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1402;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 30000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 0 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 0 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 3980, 8980, 14980, 41980, 91980, 151980, 251980, 371980, 611980, 851980 };
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
            return (float) (1 - 0.0111111 * (0));
        }
        float GetPraydistance (Skill * skill) const
        {
            return (float) (27 + skill->GetPlayer ()->GetPrayrangeplus ());
        }
        bool CheckComboSkExtraCondition (Skill * skill) const
        {
            return (bool) (skill->GetPlayer ()->GetComboarg ()->GetValue (0) >= 75);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               16.5 + 0.9 * skill->GetLevel (),
                               336 + 18 * skill->GetLevel (),
                               5 + 0.5 * skill->GetLevel (),
                               120 + 11 * skill->GetLevel (),
                               0.5 * 5.5 * skill->GetLevel () * skill->GetLevel () + 0.5 * 347.4 * skill->GetLevel () + 0.5 * 4562.9,
                               120 + 11 * skill->GetLevel (),
                               0.5 * 5.5 * skill->GetLevel () * skill->GetLevel () + 0.5 * 347.4 * skill->GetLevel () + 0.5 * 4562.9);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * skill->GetT0 () == 3 ? 100 : 0);
            skill->GetVictim ()->SetTime (5402);
            skill->GetVictim ()->SetShowicon (0);
            skill->GetVictim ()->SetBefrozen (1);
            return true;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 10);
            skill->GetVictim ()->SetTime (100);
            skill->GetVictim ()->SetRatio (0.1);
            skill->GetVictim ()->SetAmount (skill->GetT0 () == 11 ? 1 : (skill->GetT0 () == 7 ? 2 : 10));
            skill->GetVictim ()->SetValue (2625);
            skill->GetVictim ()->SetAurabless2 (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (38 + skill->GetPlayer ()->GetPrayrangeplus ());
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
        float GetTalent0 (PlayerWrapper * player) const
        {
            return (float) (player->GetBalls ());
        }
        float GetTalent1 (PlayerWrapper * player) const
        {
            return (float) (player->GetMagicattack ());
        }
#endif
    };
}
#endif
