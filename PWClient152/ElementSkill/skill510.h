#ifndef __CPPGEN_GNET_SKILL510
#define __CPPGEN_GNET_SKILL510
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill510:public Skill
    {
      public:
        enum
        { SKILL_ID = 510 };
          Skill510 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill510Stub:public SkillStub
    {
      public:
        Skill510Stub ():SkillStub (510)
        {
            cls = 4;
            name = L"真·野性回复";
            nativename = "真·野性回复";
            icon = "野性回复1.dds";
            max_level = 1;
            type = 5;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 20;
            eventflag = 1;
            is_senior = 1;
            clslimit = 0;
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
            pre_skills.push_back (std::pair < ID, int >(109, 10));
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill510Stub ()
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
            static int array[10] = { 89 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 1000000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 9581 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 1000000 };
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
            return _snwprintf (buffer, length, format, 25);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool TakeEffect (Skill * skill) const
        {
            skill->GetPlayer ()->SetInchpgen (50);
            skill->GetPlayer ()->SetIncmpgen (20);
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
