#ifndef __CPPGEN_GNET_SKILL2549
#define __CPPGEN_GNET_SKILL2549
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2549:public Skill
    {
      public:
        enum
        { SKILL_ID = 2549 };
          Skill2549 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2549Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (0.2 * (14.4 + 9 * skill->GetLevel ()));
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
                return 934;
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
                skill->GetPlayer ()->SetDecmp (0.8 * (14.4 + 9 * skill->GetLevel ()));
                skill->SetPlus (6.8 * skill->GetLevel () * skill->GetLevel () + 126 * skill->GetLevel () + 299);
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
      Skill2549Stub ():SkillStub (2549)
        {
            cls = 10;
            name = L"双合";
            nativename = "双合";
            icon = "双合.dds";
            max_level = 10;
            type = 1;
            apcost = 20;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 1;
            eventflag = 0;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1004;
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
            effect = "夜影_双合_击中.sgc";
            range.type = 0;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2548, 1));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2549Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (14.4 + 9 * skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 934;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 6000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 13, 18, 23, 28, 33, 38, 43, 48, 53, 58 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 2100, 4100, 6790, 10028, 14007, 18860, 24640, 31752, 40334, 50716 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 220, 400, 630, 1000, 1450, 1900, 2380, 2880, 3380, 3880 };
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
                               14.4 + 9 * skill->GetLevel (),
                               6.8 * skill->GetLevel () * skill->GetLevel () + 126 * skill->GetLevel () + 299,
                               0.15 * (6.8 * skill->GetLevel () * skill->GetLevel () + 126 * skill->GetLevel () + 299));

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * ((skill->GetPlayer ()->GetHp () + 0.1) / (skill->GetPlayer ()->GetMaxhp () + 0.1)) >
                                                 ((skill->GetT0 () + 0.1) / (skill->GetT2 () + 0.1)) ? 0 : 100);
            skill->GetVictim ()->SetTime (1000);
            skill->GetVictim ()->SetAmount (0.15 *
                                            (skill->GetT1 () + 6.8 * skill->GetLevel () * skill->GetLevel () + 126 * skill->GetLevel () + 299));
            skill->GetVictim ()->SetBleeding (1);
            return true;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 10);
            skill->GetVictim ()->SetTime (100);
            skill->GetVictim ()->SetRatio (0.1);
            skill->GetVictim ()->SetAmount (skill->GetTarget ()->GetValid () ==
                                            1 ? (((skill->GetTarget ()->GetHp () + 0.1) / (skill->GetTarget ()->GetMaxhp () + 0.1)) >
                                                 ((skill->GetT0 () + 0.1) / (skill->GetT2 () + 0.1)) ? 1 : 0) : 0);
            skill->GetVictim ()->SetValue (2615);
            skill->GetVictim ()->SetAurabless2 (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (skill->GetPlayer ()->GetRange () + 1.2 * 5.5);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.4 + skill->GetLevel () * 0.08);
        }
        float GetTalent0 (PlayerWrapper * player) const
        {
            return (float) (player->GetHp ());
        }
        float GetTalent1 (PlayerWrapper * player) const
        {
            return (float) (player->GetAttack ());
        }
        float GetTalent2 (PlayerWrapper * player) const
        {
            return (float) (player->GetMaxhp ());
        }
#endif
    };
}
#endif
