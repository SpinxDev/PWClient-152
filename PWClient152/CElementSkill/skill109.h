#ifndef __CPPGEN_GNET_SKILL109
#define __CPPGEN_GNET_SKILL109
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill109:public Skill
    {
      public:
        enum
        { SKILL_ID = 109 };
          Skill109 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill109Stub:public SkillStub
    {
      public:
        Skill109Stub ():SkillStub (109)
        {
            cls = 4;
            name = L"野性回复";
            nativename = "野性回复";
            icon = "野性回复.dds";
            max_level = 10;
            type = 5;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 0;
            eventflag = 1;
            posdouble = 0;
            time_type = 0;
            showorder = 1510;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (9);
            }
            effect = "";
            range.type = 0;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(0, 1));
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill109Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (18 + skill->GetLevel ());
        }
        int GetExecutetime (Skill * skill) const
        {
            return 1500;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 4000;
        }
        int GetRequiredLevel (Skill * skill) const
        {
            static int array[10] = { 6, 11, 16, 21, 26, 31, 36, 41, 46, 51 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 320, 1200, 2560, 4640, 7360, 11120, 16000, 22480, 31120, 42560 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 50, 160, 320, 530, 820, 1270, 1720, 2180, 2680, 3180 };
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
            return (float) (1 - 0.0111111 * (20 + 2.5 * skill->GetLevel ()));
        }
        float GetPraydistance (Skill * skill) const
        {
            return (float) (skill->GetPlayer ()->GetRange () + 3 + 0.2 * skill->GetLevel ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, skill->GetLevel (), 2 * skill->GetLevel ());

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool TakeEffect (Skill * skill) const
        {
            skill->GetPlayer ()->SetInchpgen (4 * skill->GetLevel ());
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (25);
        }
        int GetAttackspeed (Skill * skill) const
        {
            return 0;
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
