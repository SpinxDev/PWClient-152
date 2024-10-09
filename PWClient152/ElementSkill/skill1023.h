#ifndef __CPPGEN_GNET_SKILL1023
#define __CPPGEN_GNET_SKILL1023
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1023:public Skill
    {
      public:
        enum
        { SKILL_ID = 1023 };
          Skill1023 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1023Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfap (340 + 99 * (skill->GetLevel () - 1));
                skill->SetDamage (skill->GetPlayer ()->GetElfmp () * 20 * (1 + skill->GetT0 () * 0.008) +
                                  (60 + (skill->GetLevel () - 1) * 10) * (1 + skill->GetT0 () * 0.008) * 9.3);
                skill->GetPlayer ()->SetDecelfmp (skill->GetPlayer ()->GetElfmp ());
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
      Skill1023Stub ():SkillStub (1023)
        {
            cls = 258;
            name = L"Èâ²«ÕßÖ®Å­";
            nativename = "Èâ²«ÕßÖ®Å­";
            icon = "À§Éñ¾÷.dds";
            max_level = 10;
            type = 1;
            apcost = 340099;
            arrowcost = 0;
            apgain = 0;
            attr = 7;
            rank = 0;
            eventflag = 0;
            posdouble = 0x02;
            clslimit = 0x11;
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
            effect = "À§Éñ¾÷.sgc";
            range.type = 0;
            doenchant = false;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1023Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (1);
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
            static int array[10] = { 3030025, 3030030, 3030035, 3030040, 3030045, 3030050, 3030055, 3030060, 3030065, 3030070 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 1680, 2560, 3720, 5260, 7300, 10000, 13620, 18480, 25000, 34000 };
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
            return (float) (15);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               340 + 99 * (skill->GetLevel () - 1),
                               (60 + 10 * (skill->GetLevel () - 1)) * 9.3, 50 + skill->GetLevel () * 5, 500 + skill->GetLevel () * 200);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 1;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 50 + 5 * skill->GetLevel ());
            skill->GetVictim ()->SetTime (8000 + skill->GetT0 () * 42);
            skill->GetVictim ()->SetShowicon (1);
            skill->GetVictim ()->SetAmount (500 + skill->GetLevel () * 200 + skill->GetT0 () * 10);
            skill->GetVictim ()->SetValue (0);
            skill->GetVictim ()->SetAbsorbmagicdamage (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (16);
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
