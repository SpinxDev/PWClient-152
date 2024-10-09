#ifndef __CPPGEN_GNET_SKILL1004
#define __CPPGEN_GNET_SKILL1004
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1004:public Skill
    {
      public:
        enum
        { SKILL_ID = 1004 };
          Skill1004 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1004Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (30 + (skill->GetLevel () - 1));
                skill->GetPlayer ()->SetDecelfap (15 + 15 * (skill->GetLevel () - 1));
                skill->SetFiredamage ((0.6 * (5 + (skill->GetLevel () - 1) * 13)) * 11);
                skill->SetDamage ((0.4 * (5 + (skill->GetLevel () - 1) * 13)) * 11);
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
      Skill1004Stub ():SkillStub (1004)
        {
            cls = 258;
            name = L"¡“—Êµ∂";
            nativename = "¡“—Êµ∂";
            icon = "¡“—Êµ∂.dds";
            max_level = 10;
            type = 1;
            apcost = 15015;
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
            long_range = 2;
            restrict_corpse = 0;
            allow_forms = 1;
            effect = "¡“—Êµ∂.sgc";
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
        virtual ~ Skill1004Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (30 + (skill->GetLevel () - 1));
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 8000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 1001005, 1001010, 1001015, 1001020, 1001025, 1001030, 1001035, 1001040, 1001045, 1001050 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 30, 120, 280, 520, 840, 1280, 1860, 2630, 3650, 5000 };
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
                               skill->GetLevel (),
                               30 + (skill->GetLevel () - 1),
                               15 * skill->GetLevel (),
                               0.6 * (5 + (skill->GetLevel () - 1) * 13) * 11, 0.4 * (5 + (skill->GetLevel () - 1) * 13) * 11);

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
