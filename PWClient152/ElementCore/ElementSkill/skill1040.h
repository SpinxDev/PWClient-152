#ifndef __CPPGEN_GNET_SKILL1040
#define __CPPGEN_GNET_SKILL1040
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1040:public Skill
    {
      public:
        enum
        { SKILL_ID = 1040 };
          Skill1040 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1040Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (140 + (skill->GetLevel () - 1) * 3);
                skill->GetPlayer ()->SetDecelfap (1055 + 99 * (skill->GetLevel () - 1));
                skill->SetWooddamage ((0.7 * (177 + (skill->GetLevel () - 1) * 50 + skill->GetT1 () * 3)) * 6.7);
                skill->SetEarthdamage ((0.3 * (177 + (skill->GetLevel () - 1) * 50 + skill->GetT1 () * 3)) * 6.7);
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
      Skill1040Stub ():SkillStub (1040)
        {
            cls = 258;
            name = L"¾£¼¬Ö®Å­";
            nativename = "¾£¼¬Ö®Å­";
            icon = "¾£¼¬Ö®Å­.dds";
            max_level = 10;
            type = 1;
            apcost = 1055099;
            arrowcost = 0;
            apgain = 0;
            attr = 6;
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
            effect = "¾£¼¬Ö®Å­.sgc";
            range.type = 3;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1040Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (140 + (skill->GetLevel () - 1) * 3);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 60000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 6000760, 6000765, 6000770, 6000775, 6000780, 6000785, 6000790, 6000795, 6000799, 6000799 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 18480, 25000, 34000, 53200, 88000, 141000, 221200, 317200, 500000, 500000 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (12);
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
                               140 + (skill->GetLevel () - 1) * 3, 1055 + 99 * (skill->GetLevel () - 1), (177 + 51 * (skill->GetLevel () - 1)) * 6.7);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetBreak (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (26);
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
