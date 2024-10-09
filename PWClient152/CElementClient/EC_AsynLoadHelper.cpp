// Filename	: EC_AsynLoadHelper.cpp
// Creator	: Xu Wenbin
// Date		: 2014/11/28

#include "EC_AsynLoadHelper.h"

#include <windows.h>

#include <algorithm>

//	class CECAsynLoadIDFactory
CECAsynLoadIDFactory::CECAsynLoadIDFactory()
: m_nextLoadCounter(1)
{
}

CECAsynLoadID CECAsynLoadIDFactory::Generate(int loadType){
	return CECAsynLoadID(loadType, m_nextLoadCounter++, ::GetTickCount());
}

//	class CECAsynLoadCommand
CECAsynLoadCommand::~CECAsynLoadCommand(){
	delete m_pContent;
	m_pContent = NULL;
}

void CECAsynLoadCommand::UpdateSortWeight(){
	if (m_pContent){
		m_pContent->UpdateSortWeight();
	}
}

bool CECAsynLoadCommand::ThreadLoad(){
	return m_pContent && m_pContent->ThreadLoad();
}

//	class CECAsynLoadCommandArray

CECAsynLoadCommandArray::CECAsynLoadCommandArray(){
}

CECAsynLoadCommandArray::CECAsynLoadCommandArray(const CommandArray &commandArray){
	Assign(commandArray);
}

void CECAsynLoadCommandArray::Assign(const CommandArray &commandArray){
	Clear();
	m_commandArray = commandArray;
}

void CECAsynLoadCommandArray::Append(CECAsynLoadCommand *pCommand){
	if (!pCommand){
		assert(false);
		return;
	}
	m_commandArray.push_back(pCommand);
}

void CECAsynLoadCommandArray::Clear(){
	for (Iterator it = Begin(); it != End(); ++ it){
		CECAsynLoadCommand *pCommand = *it;
		delete pCommand;
	}
	m_commandArray.clear();
}

int CECAsynLoadCommandArray::Count()const{
	return (int)m_commandArray.size();
}

bool CECAsynLoadCommandArray::IsEmpty()const{
	return m_commandArray.empty();
}

CECAsynLoadCommand *CECAsynLoadCommandArray::GetAt(int index){
	return index >= 0 && index < Count() ? m_commandArray[index] : NULL;
}

CECAsynLoadCommandArray::Iterator CECAsynLoadCommandArray::Begin(){
	return m_commandArray.begin();
}

CECAsynLoadCommandArray::Iterator CECAsynLoadCommandArray::End(){
	return m_commandArray.end();
}

CECAsynLoadCommandArray::Iterator CECAsynLoadCommandArray::Find(const CECAsynLoadID &id){
	Iterator result;
	for (result = Begin(); result != End(); ++ result){
		CECAsynLoadCommand * pCommand = *result;
		if (pCommand->GetID() == id){
			break;
		}
	}
	return result;
}

CECAsynLoadCommandArray::Iterator CECAsynLoadCommandArray::Find(int loadType, const CECAsynLoadContent *pContent){
	Iterator result;
	for (result = Begin(); result != End(); ++ result){
		CECAsynLoadCommand * pCommand = *result;
		if (pCommand->GetID().GetLoadType() == loadType &&
			*pCommand->GetContent() == *pContent){
			break;
		}
	}
	return result;
}

void CECAsynLoadCommandArray::Remove(Iterator it){
	if (it == End()){
		return;
	}
	if (CECAsynLoadCommand * pCommand = *it){
		assert(Find(pCommand->GetID()) == it);
		m_commandArray.erase(it);
		delete pCommand;
	}
	assert(false);
}

int CECAsynLoadCommandArray::RemoveIf(CECAsynLoadCommandMatcher *pMatcher){
	int result(0);
	if (pMatcher){
		int i(0);
		while (i < Count()){
			CECAsynLoadCommand *pCommand = m_commandArray[i];
			if (pMatcher->IsMatch(pCommand)){
				delete pCommand;
				++ result;
				m_commandArray.erase(Begin()+i);
			}else{
				++ i;
			}
		}
	}
	return result;
}

void CECAsynLoadCommandArray::UpdateSortWeight(){
	for (Iterator it = Begin(); it != End(); ++ it){
		if (CECAsynLoadCommand *pCommand = *it){
			pCommand->UpdateSortWeight();
		}
	}
}

static bool SortCECAsynLoadCommand(const CECAsynLoadCommand *lhs, const CECAsynLoadCommand *rhs){
	if (!lhs){
		return true;
	}
	if (!rhs){
		return false;
	}
	return lhs->GetContent()->GetSortWeight() < rhs->GetContent()->GetSortWeight();
}

CECAsynLoadCommandArray CECAsynLoadCommandArray::FetchNearest(int count){
	if (count <= 0 || IsEmpty()){
		return CECAsynLoadCommandArray();
	}
	CECAsynLoadCommandArray result;
	if (count >= Count()){
		result.Assign(m_commandArray);
		m_commandArray.clear();
	}else{		
		std::partial_sort(Begin(), Begin()+count, End(), SortCECAsynLoadCommand);
		result.Assign(CommandArray(Begin(), Begin()+count));
		m_commandArray.erase(Begin(), Begin()+count);
	}
	return result;
}

CECAsynLoadCommandArray CECAsynLoadCommandArray::FetchAll(){
	CECAsynLoadCommandArray result = m_commandArray;
	m_commandArray.clear();
	return result;
}

//	class CECAsynLoadCenter
CECAsynLoadCenter::CECAsynLoadCenter(){
}

CECAsynLoadCenter::~CECAsynLoadCenter(){
	Clear();
}

void CECAsynLoadCenter::Clear(){
	m_commandArray.Clear();
}

bool CECAsynLoadCenter::Append(int loadType, CECAsynLoadContent *pContent, CECAsynLoadID &id){
	if (!pContent){
		assert(false);
		return false;
	}
	CECAsynLoadCommand *pCommand = new CECAsynLoadCommand(m_idFactory.Generate(loadType), pContent);
	if (!pCommand){
		assert(false);
		return false;
	}
	m_commandArray.Append(pCommand);
	id = pCommand->GetID();
	return true;
}