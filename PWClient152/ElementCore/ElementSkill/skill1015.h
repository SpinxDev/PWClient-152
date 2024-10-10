#ifndef __CPPGEN_GNET_SKILL1015
#define __CPPGEN_GNET_SKILL1015
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1015:public Skill
    {
      public:
        enum
        { SKILL_ID = 1015 };
          Skill1015 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1015Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (100 - skill->GetPlayer ()->GetElflevel () * 0.2);
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
      Skill1015Stub ():SkillStub (1015)
        {
            cls = 258;
            name = L"地火术";
            nativename = "地火术";
            icon = "地火术.dds";
            max_level = 1;
            type = 3;
            apcost = 200000;
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
            effect = "地火术.sgc";
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
        virtual ~ Skill1015Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (100 - skill->GetPlayer ()->GetElflevel () * 0.2);
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
            static int array[10] = { 0 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 0 };
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
            return _snwprintf (buffer, length, format, 4 * skill->GetLevel (), 3 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 1;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetTime (5000 + skill->GetT1 () * 25);
            skill->GetVictim ()->SetRatio (0.04 * skill->GetLevel ());
            skill->GetVictim ()->SetValue (0.03 * skill->GetLevel ());
            skill->GetVictim ()->SetCharred (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (27);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 28;
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
