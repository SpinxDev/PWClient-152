#ifndef __CPPGEN_GNET_SKILL1021
#define __CPPGEN_GNET_SKILL1021
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1021:public Skill
    {
      public:
        enum
        { SKILL_ID = 1021 };
          Skill1021 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1021Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (70);
                skill->GetPlayer ()->SetDecelfap (400);
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
      Skill1021Stub ():SkillStub (1021)
        {
            cls = 258;
            name = L"À§¸¿";
            nativename = "À§¸¿";
            icon = "À§¸¿.dds";
            max_level = 10;
            type = 3;
            apcost = 400000;
            arrowcost = 0;
            apgain = 0;
            attr = 7;
            rank = 0;
            eventflag = 0;
            clslimit = 0x40;
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
            effect = "À§¸¿.sgc";
            range.type = 0;
            doenchant = true;
            dobless = true;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1021Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (70);
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
            static int array[10] = { 30330, 30335, 30340, 30345, 30350, 30355, 30360, 30365, 30370, 30375 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 2560, 3720, 5260, 7300, 10000, 13620, 18480, 25000, 34000, 53200 };
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
            return (float) (6);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), 3 + skill->GetLevel () * 0.4);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 10;
        }
        bool StateAttack (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * 100);
            skill->GetVictim ()->SetTime (3000 + skill->GetLevel () * 400);
            skill->GetVictim ()->SetFix (1);
            return true;
        }
        bool BlessMe (Skill * skill) const
        {
            skill->GetVictim ()->SetProbability (1.0 * skill->GetT1 () * 0.25);
            skill->GetVictim ()->SetTime (5000);
            skill->GetVictim ()->SetImmuneslowdizzy (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (7);
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
