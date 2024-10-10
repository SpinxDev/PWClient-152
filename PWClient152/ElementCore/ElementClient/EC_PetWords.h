// Filename	: EC_PetWords.h
// Creator	: Xu Wenbin
// Date		: 2010/11/04

#pragma once

#include <AAssist.h>
#include <ABaseDef.h>
#include <hashmap.h>
#include <AWScriptFile.h>

//
//	宠物有话说（客户端行为）
//
class CECPetWords
{
public:

	CECPetWords();

	//	说话的类型
	enum TYPE_WORDS
	{
		TW_SUMMON,			//	召唤出生时
		TW_REST,			//	播放休闲动作
		TW_PASSIVE,			//	攻击模式改为手动模式
		TW_DEFENSIVE,		//	攻击模式改为防守模式
		TW_OFFENSIVE,		//	攻击模式改为攻击模式
		TW_STOP,			//	移动模式改为原地待命
		TW_FOLLOW,			//	移动模型改为跟随主人
		TW_FIGHT,			//	进入战斗
		TW_RECALL,			//	被玩家召回
		TW_DEAD,			//	被杀死时
		TW_DISAPPEAR,		//	将要消失时（时限到等）
		TW_SACRIFICE,		//	牺牲（玩家使用斗转星移技能时）
		TW_MAX
	};

	//	重新初始化
	void Reset();

	//	加载宠物说话内容
	bool LoadWords(const char *szFile);

	//	查询说话内容（无返回NULL）
	const ACHAR * GetWords(int idPet, TYPE_WORDS type);

private:

	bool ReadType(AWScriptFile &ScriptFile);
	bool ReadContent(AWScriptFile &ScriptFile);

	struct WordsID
	{
		int			id;			//	宠物ID
		TYPE_WORDS	type;		//	说话时机

		bool operator == (const WordsID &rhs)const { return this->id == rhs.id && this->type == rhs.type; }
	};

	struct _words_hash_function
	{
		unsigned long operator()(const WordsID &rhs)const{ return rhs.id; }
	};

	enum {MAX_WORDSCOUNT = 3};			//	同一宠物同一情景下说话种类的最大值
	typedef abase::vector<ACString>	Words;
	typedef abase::hash_map<WordsID, Words, _words_hash_function> WordsMap;
	WordsMap	m_words;				//	所有宠物的喊话情景及内容

	float		m_wordsProp[TW_MAX];	//	各喊话情景下喊话发生的概率
};