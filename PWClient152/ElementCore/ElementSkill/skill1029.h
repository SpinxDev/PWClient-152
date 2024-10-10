#ifndef __CPPGEN_GNET_SKILL1029
#define __CPPGEN_GNET_SKILL1029
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1029:public Skill
    {
      public:
        enum
        { SKILL_ID = 1029 };
          Skill1029 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1029Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (85 + (skill->GetLevel () - 1) * 3);
                skill->GetPlayer ()->SetDecelfap (600 + 99 * (skill->GetLevel () - 1));
                skill->SetDamage ((39 + (skill->GetLevel () - 1) * 28 + skill->GetT0 () * 3) * 6.1);
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
      Skill1029Stub ():SkillStub (1029)
        {
            cls = 258;
            name = L"µØÁÑ×¦";
            nativename = "µØÁÑ×¦";
            icon = "µØÁÑ×¦.dds";
            max_level = 10;
            type = 1;
            apcost = 600099;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 0;
            eventflag = 0;
            posdouble = 0x01;
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
            effect = "µØÁÑ×¦.sgc";
            range.type = 1;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1029Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (85 + (skill->GetLevel () - 1) * 3);
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
            static int array[10] = { 4331, 4336, 4341, 4346, 4351, 4356, 4361, 4366, 4371, 4376 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 2780, 4000, 5620, 7780, 10640, 14480, 19660, 26600, 36200, 59000 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (3.5);
        }
        float GetAttackdistance (Skill * skill) const
        {
            return (float) (13);
        }
        float GetAngle (Skill * skill) const
        {
            return (float) (1 - 0.0111111 * (0));
        }
        float GetPraydistance (Skill * skill) const
        {
            return (float) (5 + 0.8 * skill->GetLevel ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format,
                               skill->GetLevel (),
                               5 + 0.8 * skill->GetLevel (),
                               85 + (skill->GetLevel () - 1) * 3, 600 + 99 * (skill->GetLevel () - 1), (39 + (skill->GetLevel () - 1) * 28) * 6.1);

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
