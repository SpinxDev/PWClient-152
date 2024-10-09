// Filename	: EC_CastSkillWhenMove.h
// Creator	: Xu Wenbin
// Date		: 2014/8/1

#ifndef _ELEMENTCLIENT_EC_CASTSKILLWHENMOVE_H_
#define _ELEMENTCLIENT_EC_CASTSKILLWHENMOVE_H_

class CECPlayer;
class CECCastSkillWhenMove{
	CECCastSkillWhenMove();
	CECCastSkillWhenMove(const CECCastSkillWhenMove &);
	CECCastSkillWhenMove & operator = (const CECCastSkillWhenMove &);

	struct PlayerInfo{
		int	profession;
		int	gender;
		int	shapeType;
		int	shapeID;
		PlayerInfo(int profession, int gender, int shapeType, int shapeID);
		PlayerInfo(CECPlayer *pPlayer);
	};
	bool HasModelSupport(const PlayerInfo &player)const;
	bool IsSkillSupported(int idSkill, const PlayerInfo &player)const;
	bool HasActionSupport(int idSkill, const PlayerInfo &player)const;
	bool GetUpperBodyBonesName(const PlayerInfo &player, int &bonesNameCount, const char ** & szBonesName)const;
	bool GetLowerBodyBonesName(const PlayerInfo &player, int &bonesNameCount, const char ** & szBonesName)const;

public:
	static CECCastSkillWhenMove &Instance();
	bool HasModelSupport(CECPlayer *pPlayer)const;
	bool IsSkillSupported(int idSkill, CECPlayer *pPlayer)const;
	bool HasActionSupport(int idSkill, CECPlayer *pPlayer)const;
	bool GetUpperBodyBonesName(CECPlayer *pPlayer, int &bonesNameCount, const char ** & szBonesName)const;
	bool GetLowerBodyBonesName(CECPlayer *pPlayer, int &bonesNameCount, const char ** & szBonesName)const;
};

#endif	//	_ELEMENTCLIENT_EC_CASTSKILLWHENMOVE_H_