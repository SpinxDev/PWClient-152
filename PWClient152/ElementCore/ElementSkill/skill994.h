#ifndef __CPPGEN_GNET_SKILL994
#define __CPPGEN_GNET_SKILL994
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill994:public Skill
    {
      public:
        enum
        { SKILL_ID = 994 };
          Skill994 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill994Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfap (30 + 30 * (skill->GetLevel () - 1));
                skill->SetFiredamage (skill->GetPlayer ()->GetElfmp () * (1 + 5 * (skill->GetLevel () - 1)));
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
      Skill994Stub ():SkillStub (994)
        {
            cls = 258;
            name = L"Ñ×Á÷";
            nativename = "Ñ×Á÷";
            icon = "»ðÑ×ÆÆ.dds";
            max_level = 10;
            type = 1;
            apcost = 30030;
            arrowcost = 0;
            apgain = 0;
            attr = 5;
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
            effect = "»ðÑ×ÆÆ.sgc";
            range.type = 0;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill994Stub ()
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
            return 120000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 1001, 1005, 1010, 1015, 1020, 1025, 1030, 1035, 1040, 1045 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 10, 60, 240, 560, 1040, 1680, 2560, 3720, 5260, 7300 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (8);
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
            return (float) (18);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), 30 + 30 * (skill->GetLevel () - 1), 1 + 5 * (skill->GetLevel () - 1));

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (20);
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
