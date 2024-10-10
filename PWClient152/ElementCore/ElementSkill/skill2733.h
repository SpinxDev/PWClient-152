#ifndef __CPPGEN_GNET_SKILL2733
#define __CPPGEN_GNET_SKILL2733
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2733:public Skill
    {
      public:
        enum
        { SKILL_ID = 2733 };
          Skill2733 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2733Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecmp (0.2 * 117);
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
                skill->GetPlayer ()->SetDecmp (0.8 * 117);
                skill->SetPlus (4119);
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
      Skill2733Stub ():SkillStub (2733)
        {
            cls = 10;
            name = L"狂·双合";
            nativename = "狂·双合";
            icon = "魔双合.dds";
            max_level = 1;
            type = 1;
            apcost = 20;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 30;
            eventflag = 0;
            is_senior = 1;
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
            pre_skills.push_back (std::pair < ID, int >(2549, 10));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
            statestub.push_back (new State2 ());
            statestub.push_back (new State3 ());
#endif
        }
        virtual ~ Skill2733Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (117);
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
            static int array[10] = { 89 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 1000000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 46284 };
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
            return _snwprintf (buffer, length, format, 0.15 * 4119);

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
            skill->GetVictim ()->SetValue (2848);
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
            return (float) (2.2);
        }
        float GetTalent2 (PlayerWrapper * player) const
        {
            return (float) (player->GetMaxhp ());
        }
#endif
    };
}
#endif
