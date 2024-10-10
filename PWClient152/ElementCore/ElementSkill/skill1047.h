#ifndef __CPPGEN_GNET_SKILL1047
#define __CPPGEN_GNET_SKILL1047
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill1047:public Skill
    {
      public:
        enum
        { SKILL_ID = 1047 };
          Skill1047 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill1047Stub:public SkillStub
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
                skill->GetPlayer ()->SetDecelfmp (130 + (skill->GetLevel () - 1) * 4);
                skill->GetPlayer ()->SetDecelfap (500);
                skill->SetEarthdamage (0 + (skill->GetLevel () - 1) * 51 * (1 + skill->GetPlayer ()->GetElfstr () * 0.01));
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
      Skill1047Stub ():SkillStub (1047)
        {
            cls = 258;
            name = L"灵气爆发";
            nativename = "灵气爆发";
            icon = "灵气爆发.dds";
            max_level = 10;
            type = 2;
            apcost = 500000;
            arrowcost = 0;
            apgain = 0;
            attr = 7;
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
            effect = "灵气爆发.sgc";
            range.type = 5;
            doenchant = true;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 0));
#ifdef _SKILL_SERVER
            statestub.push_back (new State1 ());
#endif
        }
        virtual ~ Skill1047Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (130 + (skill->GetLevel () - 1) * 4);
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
            static int array[10] = { 7000555, 7000560, 7000565, 7000570, 7000575, 7000580, 7000585, 7000590, 7000595, 7000599 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 13620, 18480, 25000, 34000, 53200, 88000, 141000, 221200, 317200, 500000 };
            return array[skill->GetLevel () - 1];
        }
        float GetRadius (Skill * skill) const
        {
            return (float) (7);
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
            return (float) (8);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), 130 + (skill->GetLevel () - 1) * 4, 500, 3 * skill->GetLevel ());

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
            skill->GetVictim ()->SetValue (100 + 3 * skill->GetLevel () + skill->GetT0 () * 0.5);
            skill->GetVictim ()->SetAp (1);
            return true;
        }
        bool TakeEffect (Skill * skill) const
        {;
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (10);
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
