#ifndef __CPPGEN_GNET_SKILL631
#define __CPPGEN_GNET_SKILL631
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill631:public Skill
    {
      public:
        enum
        { SKILL_ID = 631 };
          Skill631 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill631Stub:public SkillStub
    {
      public:
        Skill631Stub ():SkillStub (631)
        {
            cls = 6;
            name = L"狂·百步穿杨";
            nativename = "狂·百步穿杨";
            icon = "百步穿杨2.dds";
            max_level = 1;
            type = 5;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 1;
            rank = 30;
            eventflag = 2;
            is_senior = 1;
            clslimit = 0;
            time_type = 0;
            showorder = 1323;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 1;
            {
                restrict_weapons.push_back (13);
            }
            effect = "";
            range.type = 0;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(255, 10));
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill631Stub ()
        {
        }
        float GetMpcost (Skill * skill) const
        {
            return (float) (0);
        }
        int GetExecutetime (Skill * skill) const
        {
            return 0;
        }
        int GetCoolingtime (Skill * skill) const
        {
            return 0;
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
            static int array[10] = { 9704 };
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
            return (float) (1 - 0.0111111 * (0));
        }
        float GetPraydistance (Skill * skill) const
        {
            return (float) (skill->GetPlayer ()->GetRange ());
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format, 12);

        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool TakeEffect (Skill * skill) const
        {
            skill->GetPlayer ()->SetIncrange (2);
            skill->GetPlayer ()->SetInchitrate (0.1);
            skill->GetPlayer ()->SetInccrit (1);
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (skill->GetPlayer ()->GetRange () + 5);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
