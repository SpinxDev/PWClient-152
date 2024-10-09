// Filename	: EC_PetWords.h
// Creator	: Xu Wenbin
// Date		: 2010/11/04

#pragma once

#include <AAssist.h>
#include <ABaseDef.h>
#include <hashmap.h>
#include <AWScriptFile.h>

//
//	�����л�˵���ͻ�����Ϊ��
//
class CECPetWords
{
public:

	CECPetWords();

	//	˵��������
	enum TYPE_WORDS
	{
		TW_SUMMON,			//	�ٻ�����ʱ
		TW_REST,			//	�������ж���
		TW_PASSIVE,			//	����ģʽ��Ϊ�ֶ�ģʽ
		TW_DEFENSIVE,		//	����ģʽ��Ϊ����ģʽ
		TW_OFFENSIVE,		//	����ģʽ��Ϊ����ģʽ
		TW_STOP,			//	�ƶ�ģʽ��Ϊԭ�ش���
		TW_FOLLOW,			//	�ƶ�ģ�͸�Ϊ��������
		TW_FIGHT,			//	����ս��
		TW_RECALL,			//	������ٻ�
		TW_DEAD,			//	��ɱ��ʱ
		TW_DISAPPEAR,		//	��Ҫ��ʧʱ��ʱ�޵��ȣ�
		TW_SACRIFICE,		//	���������ʹ�ö�ת���Ƽ���ʱ��
		TW_MAX
	};

	//	���³�ʼ��
	void Reset();

	//	���س���˵������
	bool LoadWords(const char *szFile);

	//	��ѯ˵�����ݣ��޷���NULL��
	const ACHAR * GetWords(int idPet, TYPE_WORDS type);

private:

	bool ReadType(AWScriptFile &ScriptFile);
	bool ReadContent(AWScriptFile &ScriptFile);

	struct WordsID
	{
		int			id;			//	����ID
		TYPE_WORDS	type;		//	˵��ʱ��

		bool operator == (const WordsID &rhs)const { return this->id == rhs.id && this->type == rhs.type; }
	};

	struct _words_hash_function
	{
		unsigned long operator()(const WordsID &rhs)const{ return rhs.id; }
	};

	enum {MAX_WORDSCOUNT = 3};			//	ͬһ����ͬһ�龰��˵����������ֵ
	typedef abase::vector<ACString>	Words;
	typedef abase::hash_map<WordsID, Words, _words_hash_function> WordsMap;
	WordsMap	m_words;				//	���г���ĺ����龰������

	float		m_wordsProp[TW_MAX];	//	�������龰�º��������ĸ���
};