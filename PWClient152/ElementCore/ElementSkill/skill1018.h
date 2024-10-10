#ifndef __CPPGEN_GNET_SKILL1018
#define __CPPGEN_GNET_SKILL1018
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1018:public Skill
    {
      public:
        enum
        { SKILL_ID = 1018 };
          Skill1018 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1018Stub:public SkillStub
    {
      public:
#ifdef _SKILL_SERVER
        class State1:public SkillStub::State
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
                skill->GetPlayer ()->SetDecelfmp (55 + (skill->GetLevel () - 1) * 4);
                skill->GetPlayer ()->SetDecelfap (200);
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
      Skill1018Stub ():SkillStub (1018)
        {
            cls = 258;
            name = L"²Êºç×£¸£";
            nativename = "²Êºç×£¸£";
            icon = "²Êºç×£¸£.dds";
            max_level = 10;
            type = 2;
            apcost = 200000;
            arrowcost = 0;
            apgain = 0;
            attr = 5;
            rank = 0;
            eventflag = 0;
            clslimit = 0x80;
            time_type = 1;
            showorder = 0;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            effect = "²Êºç×£¸£.sgc";
            range.type = 0;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1018Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (55 + (skill->GetLevel () - 1) * 4);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 30000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 1022020, 1022025, 1022030, 1022035, 1022040, 1022045, 1022050, 1022055, 1022060, 1022065 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 1040, 1680, 2560, 3720, 5260, 7300, 10000, 13620, 18480, 25000 };
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
            return (float) (30);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               55 + (skill->GetLevel () - 1) * 4,
                               10 + 2 * skill->GetLevel (), 10 + 2 * skill->GetLevel (), skill->GetLevel (), skill->GetLevel () * 2);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 5 + skill->GetLevel () + skill->GetT1 () * 0.5);
            skill->GetVictim ()->SetTime (8000);
            skill->GetVictim ()->SetRatio (0.1 + 0.02 * skill->GetLevel ());
            skill->GetVictim ()->SetShowicon (1);
            skill->GetVictim ()->SetIncattack2 (1);
            skill->GetVictim ()->SetProbability (1.0 * 5 + skill->GetLevel () + skill->GetT1 () * 0.5);
            skill->GetVictim ()->SetTime (8000);
            skill->GetVictim ()->SetRatio (0.1 + 0.02 * skill->GetLevel ());
            skill->GetVictim ()->SetShowicon (1);
            skill->GetVictim ()->SetIncmagic2 (1);
            skill->GetVictim ()->SetProbability (1.0 * skill->GetT1 () * 0.25);
            skill->GetVictim ()->SetTime (8000);
            skill->GetVictim ()->SetRatio (skill->GetLevel () * 0.02);
            skill->GetVictim ()->SetFastpray2 (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (8000);
            skill->GetVictim ()->SetValue (skill->GetLevel ());
            skill->GetVictim ()->SetIncsmite2 (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (33);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 20;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
        float GetTalent0 (PlayerWrapper * player) const
        {
            return (float) (player->GetElfstr ());
        }
        float GetTalent1 (PlayerWrapper * player) const
        {
            return (float) (player->GetElfagi ());
        }
#endif
    };
}
#endif
