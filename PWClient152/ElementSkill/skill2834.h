#ifndef __CPPGEN_GNET_SKILL2834
#define __CPPGEN_GNET_SKILL2834
namespace GNET
{
#ifdef _SKILL_SERVER
    class Skill2834:public Skill
    {
      public:
        enum
        { SKILL_ID = 2834 };
          Skill2834 ():Skill (SKILL_ID)
        {
        }
    };
#endif
    class Skill2834Stub:public SkillStub
    {
      public:
        Skill2834Stub ():SkillStub (2834)
        {
            cls = 11;
            name = L"¿ñ¡¤ëÊ×åÑªÂö";
            nativename = "¿ñ¡¤ëÊ×åÑªÂö";
            icon = "Ä§¼¼ÄÜ¼õÉË.dds";
            max_level = 1;
            type = 5;
            apcost = 0;
            arrowcost = 0;
            apgain = 0;
            attr = 0;
            rank = 31;
            eventflag = 1;
            is_senior = 1;
            posdouble = 0;
            clslimit = 0;
            time_type = 0;
            showorder = 1103;
            allow_land = 1;
            allow_air = 1;
            allow_water = 1;
            allow_ride = 0;
            auto_attack = 0;
            long_range = 0;
            restrict_corpse = 0;
            allow_forms = 0;
            effect = "";
            range.type = 0;
            doenchant = false;
            dobless = false;
            commoncooldown = 0;
            commoncooldowntime = 0;
            pre_skills.push_back (std::pair < ID, int >(2691, 10));
#ifdef _SKILL_SERVER
#endif
        }
        virtual ~ Skill2834Stub ()
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
            static int array[10] = { 99 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredSp (Skill * skill) const
        {
            static int array[10] = { 2000000 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredItem (Skill * skill) const
        {
            static int array[10] = { 46389 };
            return array[skill->GetLevel () - 1];
        }
        int GetRequiredMoney (Skill * skill) const
        {
            static int array[10] = { 2000000 };
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
            return (float) (0);
        }
#ifdef _SKILL_CLIENT
        int GetIntroduction (Skill * skill, wchar_t * buffer, int length, wchar_t * format) const
        {
            return _snwprintf (buffer, length, format);
        }
#endif
#ifdef _SKILL_SERVER
        int GetEnmity (Skill * skill) const
        {
            return 0;
        }
        bool TakeEffect (Skill * skill) const
        {
            skill->GetPlayer ()->SetIncnearnormaldmgreduce (0.05);
            skill->GetPlayer ()->SetIncnearskilldmgreduce (0.02);
            skill->GetPlayer ()->SetInccrit (1);
            return true;
        }
        float GetEffectdistance (Skill * skill) const
        {
            return (float) (0);
        }
        float GetHitrate (Skill * skill) const
        {
            return (float) (1.0);
        }
#endif
    };
}
#endif
