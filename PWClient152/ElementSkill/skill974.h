#ifndef __CPPGEN_GNET_SKILL974
#define __CPPGEN_GNET_SKILL974
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill974:public Skill
    {
      public:
        enum
        { SKILL_ID = 974 };
          Skill974 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill974Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (30 + (skill->GetLevel () - 1) * 2);
                skill->GetPlayer ()->SetDecelfap (100 + 10 * (skill->GetLevel () - 1));
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
      Skill974Stub ():SkillStub (974)
        {
            cls = 258;
            name = L"²ÐÉñ¾÷";
            nativename = "²ÐÉñ¾÷";
            icon = "²ÐÉñ¾÷.dds";
            max_level = 10;
            type = 3;
            apcost = 100010;
            arrowcost = 0;
            apgain = 0;
            attr = 3;
            rank = 0;
            eventflag = 0;
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
            effect = "²ÐÉñ¾÷.sgc";
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
        virtual ~ Skill974Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (30 + (skill->GetLevel () - 1) * 2);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 20000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 200005, 200010, 200015, 200020, 200025, 200030, 200035, 200040, 200045, 200050 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 60, 240, 560, 1040, 1680, 2560, 3720, 5260, 7300, 10000 };
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
            return (float) (25);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (), 30 + (skill->GetLevel () - 1) * 2, 100 + 10 * (skill->GetLevel () - 1), 10 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 3;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetValue (-skill->GetPlayer ()->GetAp () * (0.05 + skill->GetT0 () * 0.002));
            skill->GetVictim ()->SetAp (1);
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (4000);
            skill->GetVictim ()->SetAmount (skill->GetPlayer ()->GetAp () * (0.05 + skill->GetT0 () * 0.002) * 10 * skill->GetLevel ());
            skill->GetVictim ()->SetToxic (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (30);
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
