/********************************************************************
	created:	2007/08/31
	author:		liudong
	
	purpose:    types of tree to store variables
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#include "LD_NameValue.h"
#include <stdio.h>
#include <windows.h>

//////////////////////////////////////////////////////////////////////
// LD_NameValue
//////////////////////////////////////////////////////////////////////
LD_NameValue::LD_NameValue(){
	ZeroMemory(m_Name,sizeof(m_Name));
	ZeroMemory(m_Value,sizeof(m_Value));
	ZeroMemory(m_Type,sizeof(m_Type));
	ZeroMemory(m_VarAddress,sizeof(m_VarAddress));
}

LD_NameValue LD_NameValue::operator =(LD_NameValue& nameValue){
	memcpy(m_Name,nameValue.m_Name,sizeof(m_Name));
	memcpy(m_Value,nameValue.m_Value,sizeof(m_Value));
	memcpy(m_Type,nameValue.m_Type,sizeof(m_Type));
	memcpy(m_VarAddress,nameValue.m_VarAddress,sizeof(m_VarAddress));

	return *this;
}

bool LD_NameValue::operator ==(LD_NameValue& nameValue){
    if (!_tcscmp(m_Name,nameValue.m_Name) &&
		!_tcscmp(m_Type,nameValue.m_Type)&&
		!_tcscmp(m_VarAddress,nameValue.m_VarAddress))
	{
		return true;
    }
	return false;
}

bool LD_NameValue::operator !=(LD_NameValue& nameValue){
	if( (*this)==nameValue){
		return false;
	}else{
		return true;
	}
}

//////////////////////////////////////////////////////////////////////
//  LD_TreeNode
//////////////////////////////////////////////////////////////////////

LD_TreeNode::LD_TreeNode(LD_TreeRoot* pRoot){
	m_pFirstChild  = NULL;
	m_pParent      = NULL;
	m_pPrevSibling = NULL;
	m_pNextSibling = NULL;
	m_pNameValueNode= NULL;
	m_pTreeRoot = pRoot;
}

LD_TreeNode  LD_TreeNode::operator=(LD_TreeNode& nameValue){
	m_pFirstChild  = nameValue.m_pFirstChild;
	m_pParent      = nameValue.m_pParent;
	m_pPrevSibling = nameValue.m_pPrevSibling;
	m_pNextSibling = nameValue.m_pNextSibling;
	m_pNameValueNode= nameValue.m_pNameValueNode;
	m_NameValue    = nameValue.m_NameValue;
	m_pTreeRoot     = nameValue.m_pTreeRoot;
	return *this;
}

LD_TreeNode::~LD_TreeNode()
{
	if (m_pFirstChild) {
		//delete children
		while (m_pFirstChild) {
			LD_TreeNode* pNextChild= m_pFirstChild->GetNextSibling();
			//set the child's father,sibling to null , to simple delete the child.
			m_pFirstChild->m_pParent = NULL;
			m_pFirstChild->m_pPrevSibling = NULL;
			m_pFirstChild->m_pNextSibling = NULL;
			delete m_pFirstChild;
			m_pFirstChild = pNextChild;
		}
	}

	if ( m_pParent && (this==m_pParent->m_pFirstChild) ) {
		m_pParent->m_pFirstChild= m_pNextSibling;
	}
	if (m_pPrevSibling) {
		m_pPrevSibling->m_pNextSibling = m_pNextSibling;
	}
	if (m_pNextSibling) {
		m_pNextSibling->m_pPrevSibling = m_pPrevSibling;
	}

	if (m_pNameValueNode && m_pNameValueNode->m_pAttachNode == this) {
		m_pNameValueNode->m_pAttachNode= NULL;
		m_pNameValueNode->m_IsValid= false;
	}
	//delete table variable in the list. //liudong
}

bool  LD_TreeNode::RemoveInvalidateNodes(){
	if (!m_pNameValueNode->m_IsValid) {
        delete this;
		return true;
	}

	bool returnValue= false;
	LD_TreeNode*  pOldChild= m_pFirstChild;
	while (pOldChild) {
		LD_TreeNode* pChild= pOldChild->GetNextSibling();
		if(pOldChild->RemoveInvalidateNodes()){
			returnValue= true;
		}
		pOldChild= pChild;
	}
	return returnValue;
}

void  LD_TreeNode::RemoveChild(LD_NameValue& nameValue){
	LD_TreeNode*  pOldChild= m_pFirstChild;
	while (pOldChild) {
		LD_TreeNode* pNextChild= pOldChild->GetNextSibling();
        if (!_tcscmp(nameValue.m_Name,pOldChild->m_NameValue.m_Name)) {
			if (m_pFirstChild == pOldChild) {
				m_pFirstChild= pOldChild->m_pNextSibling;
			}
			if (pOldChild->m_pNextSibling) {
				pOldChild->m_pNextSibling->m_pPrevSibling= pOldChild->m_pPrevSibling;
			}
			if (pOldChild->m_pPrevSibling) {
				pOldChild->m_pPrevSibling->m_pNextSibling= pOldChild->m_pNextSibling;
			}
			delete pOldChild;
			m_pTreeRoot->ResetTreeNode();
			break;
		}
		pOldChild= pNextChild;
	}
}

LD_ResultType LD_TreeNode::AddChild(LD_NameValue& nameValue,LD_TreeNode** ppTreeNode){
	if (ppTreeNode) {
	    *ppTreeNode = NULL;
	}
	if (!m_pTreeRoot) {
		return LD_Result_ERROR;
	}

	//the real parent node of this node.
	LD_TreeNode* pParentNode = this;
	if (m_pNameValueNode) {
		pParentNode = m_pNameValueNode->m_pAttachNode;
	}
	
	//to prevent adding variable to the same directory twice.
	LD_TreeNode* pOldChild= pParentNode->GetFirstChild();
	while (pOldChild) {
		if (nameValue == pOldChild->m_NameValue) {
			return LD_Result_ERROR_EXIST ;
		}
		pOldChild = pOldChild->GetNextSibling();
	}

	if (_tcscmp(_T("table"),nameValue.m_Type)) {
		//insert new variable that is not a table.
		LD_TreeNode* pChild  = new LD_TreeNode(m_pTreeRoot);
		pChild->m_NameValue  = nameValue;
		pChild->m_pParent    = pParentNode;
		pChild->m_pNextSibling= pParentNode->m_pFirstChild ;
		if (pParentNode->m_pFirstChild) {
			pParentNode->m_pFirstChild->m_pPrevSibling= pChild;
		}
		pParentNode->m_pFirstChild = pChild;
		if (ppTreeNode) {
	        *ppTreeNode = pChild;
		}
		return LD_Result_TRUE;
	}else{
		//insert table variable
		//try to find reference of the variable
		LD_NameValueNode* pNameValueNode= m_pTreeRoot->GetNameValueNode(nameValue);
		
		// this variable occurred for the first time, insert it into the tree.
		if (!pNameValueNode) {
			//store the table variable in the list
			pNameValueNode= new LD_NameValueNode();
	        //pNameValueNode->m_NameValue   = nameValue;
			if (m_pTreeRoot->m_pNameValueRoot){
				pNameValueNode->m_pNext = m_pTreeRoot->m_pNameValueRoot;
				m_pTreeRoot->m_pNameValueRoot->m_pPrev = pNameValueNode;
			}
			m_pTreeRoot->m_pNameValueRoot= pNameValueNode;

			//insert it in the tree
			LD_TreeNode* pChild      = new LD_TreeNode(m_pTreeRoot);
			pChild->m_NameValue      = nameValue;
			pChild->m_pNameValueNode = pNameValueNode;
			pChild->m_pParent        = pParentNode;
			pChild->m_pNextSibling   = pParentNode->m_pFirstChild ;
			if (pParentNode->m_pFirstChild) {
				pParentNode->m_pFirstChild->m_pPrevSibling= pChild;
			}
			pParentNode->m_pFirstChild = pChild;
			
			//attach the variable to the first node it occurred.
			pNameValueNode->m_pAttachNode = pChild;
			if (ppTreeNode) {
				*ppTreeNode = pChild;
			}
			return LD_Result_TRUE;
		}else {
			//the variable already eixt, here is a reference
			LD_TreeNode* pChild  = new LD_TreeNode(m_pTreeRoot);
			pChild->m_NameValue      = nameValue;
			pChild->m_pNameValueNode = pNameValueNode;
			pChild->m_pParent        = pParentNode;
			pChild->m_pNextSibling   = pParentNode->m_pFirstChild ;
			if (pParentNode->m_pFirstChild) {
				pParentNode->m_pFirstChild->m_pPrevSibling= pChild;
			}
			pParentNode->m_pFirstChild = pChild;
			if (ppTreeNode) {
				*ppTreeNode = pChild;
			}
			return LD_Result_REF;
		}
	}
}

bool LD_TreeNode::GetNameValue(LD_NameValue& nameValue){
	nameValue= m_NameValue;
	return true;
}

/*
//if it was a table in the past, may be delete the table or delete a reference.
//if added a new table ,should add table in the list or add a reference 
bool LD_TreeNode::SetNameValue(LD_NameValue& nameValue){
    if (m_NameValue == nameValue) {
		return true;
    }else if (!strcmp("table",nameValue.m_Type)) {
		m_NameValue= nameValue;
		return true;
    }
	
}*/

int  LD_TreeNode::GetNameValueID(){
    if (m_pNameValueNode) {
		return m_pNameValueNode->m_ID;
    }else{
		return -1;
    }
}

int LD_TreeNode::GetChildNum(){
	LD_TreeNode* pChild=GetFirstChild();
	int num ;
	if (pChild) {
		num= pChild->GetSiblingNum();
	}else{
		num=0;
	}
	return num;
}

LD_TreeNode* LD_TreeNode::GetFirstChild(){
    return m_pFirstChild;
}

LD_TreeNode* LD_TreeNode::GetNextSibling(){
	return m_pNextSibling ;
}

LD_TreeNode* LD_TreeNode::GetPrevSibling(){
	return m_pPrevSibling ;
}

LD_TreeNode* LD_TreeNode::GetParent(){
	return m_pParent ;
}

int LD_TreeNode::GetSiblingNum(){
	int num=1;//self

	//back
	LD_TreeNode* pSibling= GetNextSibling();
	while (pSibling) {
		++num;
		pSibling= pSibling->GetNextSibling();
	}

	//prev
	pSibling= GetPrevSibling();
	while (pSibling) {
		++num;
		pSibling= pSibling->GetPrevSibling();
	}

	return num;
}


//////////////////////////////////////////////////////////////////////
//  LD_TreeRoot
//////////////////////////////////////////////////////////////////////

LD_TreeRoot::LD_TreeRoot():LD_TreeNode(NULL){
		m_pNameValueRoot = NULL;
		m_pTreeRoot      = this;
}

LD_TreeRoot::~LD_TreeRoot(){
	Clear();
}

LD_TreeNode*   LD_TreeRoot::GetTreeNode(LD_NameValue&  nameValue){
	LD_NameValueNode* pNode= GetNameValueNode(nameValue);
	if (pNode) {
		return pNode->m_pAttachNode;
	}else{
		return NULL;
	}
	
}

/*
void LD_TreeRoot::Remove(LD_NameValue& nameValue){
	LD_NameValueNode* pNode= GetNameValueNode(nameValue);
	if (pNode) {
		pNode->m_IsValid= false;
		Clear(false);
	}
}
*/

void LD_TreeRoot::Clear(bool IsAll/* =true */){
	//delete all variable
	if(IsAll){
		//delete tree
		while (m_pFirstChild) {
			LD_TreeNode* pChild= m_pFirstChild->GetNextSibling();
			delete m_pFirstChild;
			m_pFirstChild= pChild;
		}
		
		//delete table variable list.
		while (m_pNameValueRoot) {
			LD_NameValueNode* pNextNameValueNode= m_pNameValueRoot->m_pNext;
			delete m_pNameValueRoot;
			m_pNameValueRoot= pNextNameValueNode;
		}
	}else{//delete invalid table
		//delete invalid nodes int the tree
		LD_TreeNode*  pOldChild= m_pFirstChild;
		while (pOldChild) {
			LD_TreeNode* pChild= pOldChild->GetNextSibling();
			pOldChild->RemoveInvalidateNodes();
			pOldChild= pChild;
		}

		//delete invalid node in the list
		LD_NameValueNode* pOldNameValueNode= m_pNameValueRoot;
		while (pOldNameValueNode) {
			LD_NameValueNode* pNextNameValueNode= pOldNameValueNode->m_pNext;
			if (!pOldNameValueNode->m_IsValid) {
				if (pOldNameValueNode->m_pPrev) {
					pOldNameValueNode->m_pPrev->m_pNext = pOldNameValueNode->m_pNext;
				}else{
					//current node is the root
					m_pNameValueRoot = pOldNameValueNode->m_pNext;
				}
				if (pOldNameValueNode->m_pNext) {
					pOldNameValueNode->m_pNext->m_pPrev = pOldNameValueNode->m_pPrev;
				}
				delete pOldNameValueNode;
			}
			pOldNameValueNode= pNextNameValueNode;
		}
	}
}

LD_NameValueNode* LD_TreeRoot::GetNameValueNode(LD_NameValue&  nameValue){
	LD_NameValueNode* pOldNameValueNode= m_pNameValueRoot;
	while (pOldNameValueNode) {
		LD_NameValueNode* pNextNameValueNode= pOldNameValueNode->m_pNext;
        //if (nameValue == pOldNameValueNode->m_NameValue) {
		//	return pOldNameValueNode;
        //}else  
		//{
		    //key is difference but value is to the same table.
		    //now the address should point to the value's table.
            
			if (!_tcscmp(_T("table"),nameValue.m_Type)&&
			      !_tcscmp(_T("table"),pOldNameValueNode->m_pAttachNode->m_NameValue.m_Type)&&
				  !_tcscmp(nameValue.m_VarAddress,pOldNameValueNode->m_pAttachNode->m_NameValue.m_VarAddress)
				  )
			{
				return pOldNameValueNode;
			}
        //}
		pOldNameValueNode= pNextNameValueNode;
	}
	return NULL;
}

LD_NameValueNode* LD_TreeRoot::GetNameValueNode( int ID){
	LD_NameValueNode* pOldNameValueNode= m_pNameValueRoot;
	while (pOldNameValueNode) {
		LD_NameValueNode* pNextNameValueNode= pOldNameValueNode->m_pNext;
        if (ID == pOldNameValueNode->m_ID ) {
			return pOldNameValueNode;
        }
		pOldNameValueNode= pNextNameValueNode;
	}
	return NULL;
}

// node to search tree nodes (wide search first)
class LD_ResetNode{
public:
	LD_ResetNode(){
		pNowNode     = NULL;
		pNextNode = NULL;
	}
	~LD_ResetNode(){
		if(pNextNode){
			delete pNextNode;
			pNextNode= NULL;
		}
	}
	LD_TreeNode*  pNowNode;
	LD_ResetNode* pNextNode;
};

void LD_TreeRoot::MoveNode(LD_TreeNode* pTo,LD_TreeNode* pFrom){
	if (pTo != pFrom) {
	    LD_TreeNode* pFromChild= pFrom->GetFirstChild();
		LD_TreeNode* pTmp= pTo->m_pFirstChild;
		while (pTmp && pTmp->m_pNextSibling) {
			pTmp= pTmp->m_pNextSibling;
		}
		if (!pTmp) {
			pTo->m_pFirstChild= pFromChild;
		}else{
			pTmp->m_pNextSibling= pFromChild;
			if (pFromChild) {
				pFromChild->m_pPrevSibling= pTmp;
			}
		}
		while (pFromChild) {
			pFromChild->m_pParent= pTo;
			pFromChild= pFromChild->GetNextSibling();
		}
		pFrom->m_pFirstChild               = NULL;
		pTo->m_pNameValueNode->m_pAttachNode = pTo;
	}
}

bool  LD_TreeRoot::DealWithInvalideNode(LD_TreeNode* pOldNode){
	if (!pOldNode->m_pNameValueNode->m_pAttachNode) {
		pOldNode->m_pNameValueNode->m_pAttachNode= pOldNode;
		pOldNode->m_pNameValueNode->m_IsValid     = true;//liudong
	}else if (pOldNode == pOldNode->m_pNameValueNode->m_pAttachNode) {
		pOldNode->m_pNameValueNode->m_IsValid     = true;
	}else{
		int levelOld=0;
		int levelNew=0;
		LD_TreeNode* pTmp= pOldNode;
		while (pTmp) {
			++levelOld;
			pTmp= pTmp->GetParent();
		}
		pTmp= pOldNode->m_pNameValueNode->m_pAttachNode;
		while (pTmp) {
			++levelNew;
			pTmp= pTmp->GetParent();
		}
		if (levelOld != levelNew) {
			//find a variable should be attached to a node in a higher level,
			//move the node's children's position and so on.
			MoveNode(pOldNode,pOldNode->m_pNameValueNode->m_pAttachNode);
			pOldNode->m_pNameValueNode->m_IsValid     = true;
		}
	}
	return pOldNode->m_pNameValueNode->m_IsValid;
}

void  LD_TreeRoot::ResetTreeNode(){
	//set all the variables in the list to be invalid,
	//after attached the variables to the node the node's are set valid.
	LD_NameValueNode* pOldNameValueNode= m_pNameValueRoot;
	while (pOldNameValueNode) {
		LD_NameValueNode* pNextNameValueNode= pOldNameValueNode->m_pNext;
        pOldNameValueNode->m_IsValid = false;
		pOldNameValueNode= pNextNameValueNode;
	}

//==============search the tree with wide order first ========================
	LD_ResetNode* pRootResetNode = NULL;
	LD_ResetNode* pLastResetNode = NULL;

	LD_TreeNode*  pOldChild= m_pFirstChild;
	while (pOldChild) {
		LD_TreeNode* pNextChild= pOldChild->GetNextSibling();
		if (_tcscmp(_T("table"),pOldChild->m_NameValue.m_Type)) {
			//no table variable
			pOldChild= pNextChild;
		    continue;
		}

		LD_ResetNode* pNowResetNode =new LD_ResetNode();
		pNowResetNode->pNowNode= pOldChild;
        if (!pLastResetNode) {
			pLastResetNode=pRootResetNode=pNowResetNode;
        }else{
		    pLastResetNode->pNextNode =pNowResetNode;
			pLastResetNode            =pNowResetNode;
        }
		if (!pOldChild->m_pNameValueNode->m_IsValid) {
			DealWithInvalideNode(pOldChild);
		}
		pOldChild= pNextChild;
	}

    LD_ResetNode* pHead=pRootResetNode;
	LD_ResetNode* pTail=pLastResetNode;

	LD_ResetNode* pNow=pHead;
	while (pNow) {
		LD_TreeNode*  pOldChild= pNow->pNowNode->GetFirstChild();
		while (pOldChild) {
			LD_TreeNode* pNextChild= pOldChild->GetNextSibling();
			if (_tcscmp(_T("table"),pOldChild->m_NameValue.m_Type)) {
				//no table variable
			    pOldChild= pNextChild;
				continue;
			}
            LD_ResetNode* pNowResetNode =new LD_ResetNode();
		    pNowResetNode->pNowNode     = pOldChild;
			pLastResetNode->pNextNode   = pNowResetNode;
			pLastResetNode              = pNowResetNode;	
			if (!pOldChild->m_pNameValueNode->m_IsValid) {
				DealWithInvalideNode(pOldChild);
			}
			pOldChild= pNextChild;
		}
		if (pNow == pTail) {
			//end search current directory,start search next directory
			//find current directory's begin and end position.
			pHead= pTail->pNextNode;
			pNow = pHead;
			pTail= pHead;
			while (pTail && pTail->pNextNode) {
				pTail= pTail->pNextNode;
			}
		}else{
			//search current directory
            pNow=pNow->pNextNode;
		}
	}
//==============search the tree with wide order first===================================
	
	//release
	if (pRootResetNode) {
		delete pRootResetNode;
	}
	
}

void LD_TreeRoot::AddNULLAtTail(){
	LD_TreeNode* pNode= GetFirstChild() ;
	if (pNode) {
		if (_tcscmp(_T(""),pNode->m_NameValue.m_Name)) {
			LD_NameValue nameValue;
			_stprintf(nameValue.m_Name,_T(""));
			AddChild(nameValue);
		}
	}else{
		LD_NameValue nameValue;
		_stprintf(nameValue.m_Name,_T(""));
		AddChild(nameValue);
	}
}


/*
//just for table
LD_ResultType LD_TreeRoot::UpdateValue(LD_NameValue& nameValue){
	LD_NameValueNode* pNode= GetNameValueNode(nameValue);
	if (pNode) {
		pNode->m_NameValue = nameValue;
		return LD_Result_TRUE;
	}
	return LD_Result_ERROR;
}
*/