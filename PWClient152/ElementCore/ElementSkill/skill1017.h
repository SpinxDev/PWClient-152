#ifndef __CPPGEN_GNET_SKILL1017
#define __CPPGEN_GNET_SKILL1017
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1017:public Skill
    {
      public:
        enum
        { SKILL_ID = 1017 };
          Skill1017 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1017Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (55 + (skill->GetLevel () - 1) * 2);
                skill->GetPlayer ()->SetDecelfap (250 + 18 * (skill->GetLevel () - 1));
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
      Skill1017Stub ():SkillStub (1017)
        {
            cls = 258;
            name = L"´óµØ²¨ÎÆ.ËÀÍö";
            nativename = "´óµØ²¨ÎÆ.ËÀÍö";
            icon = "´óµØ²¨ÎÆËÀÍö.dds";
            max_level = 10;
            type = 3;
            apcost = 250018;
            arrowcost = 0;
            apgain = 0;
            attr = 6;
            rank = 0;
            eventflag = 0;
            time_type = 1;
            showorder = 0;
            allow_land = 1;
            allow_air = 0;
            allow_water = 0;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            effect = "´óµØ²¨ÎÆËÀÍö.sgc";
            range.type = 2;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1017Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (55 + (skill->GetLevel () - 1) * 2);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 1000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 22120, 22125, 22130, 22135, 22140, 22145, 22150, 22155, 22160, 22165 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 1040, 1680, 2560, 3720, 5260, 7300, 10000, 13620, 18480, 25000 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (16);
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
                               55 + (skill->GetLevel () - 1) * 2, 250 + 18 * (skill->GetLevel () - 1), 50 + 5 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 50 + 5 * skill->GetLevel ());
            skill->GetVictim ()->SetTime (60000 - skill->GetT0 () * 200);
            skill->GetVictim ()->SetShowicon (1);
            skill->GetVictim ()->SetAmount (20 + skill->GetT0 () * 1);
            skill->GetVictim ()->SetValue (1);
            skill->GetVictim ()->SetApleakcont (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (40);
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
