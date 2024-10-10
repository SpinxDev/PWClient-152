#ifndef __CPPGEN_GNET_SKILL966
#define __CPPGEN_GNET_SKILL966
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill966:public Skill
    {
      public:
        enum
        { SKILL_ID = 966 };
          Skill966 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill966Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (75 + (skill->GetLevel () - 1) * 3);
                skill->GetPlayer ()->SetDecelfap (500 + 99 * (skill->GetLevel () - 1));
                skill->SetGolddamage ((25 + (skill->GetLevel () - 1) * 29 + skill->GetT0 () * 2) * 3.7);
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
      Skill966Stub ():SkillStub (966)
        {
            cls = 258;
            name = L"±¼À×»÷";
            nativename = "±¼À×»÷";
            icon = "±¼À×»÷.dds";
            max_level = 10;
            type = 1;
            apcost = 500099;
            arrowcost = 0;
            apgain = 0;
            attr = 2;
            rank = 0;
            eventflag = 0;
            posdouble = 0x02;
            clslimit = 0xff7;
            time_type = 1;
            showorder = 0;
            allow_land = 0;
            allow_air = 1;
            allow_water = 0;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 2;
            restrict_corpse = 0;
            allow_forms = 1;
            effect = "±¼À×»÷.sgc";
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
        virtual ~ Skill966Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (75 + (skill->GetLevel () - 1) * 3);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 10000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 7000031, 7000036, 7000041, 7000046, 7000051, 7000056, 7000061, 7000066, 7000071, 7000076 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 2780, 4000, 5620, 7780, 10640, 14480, 19660, 26600, 36200, 59000 };
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
                               75 + (skill->GetLevel () - 1) * 3, 500 + 99 * (skill->GetLevel () - 1), (25 + (skill->GetLevel () - 1) * 29) * 3.7);

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
