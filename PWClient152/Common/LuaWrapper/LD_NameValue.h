/********************************************************************
	created:	2007/08/31
	author:		liudong
	
	purpose:    types of tree to store variables
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#pragma once
#include <tchar.h>

#ifndef MAX_PATH 
#define MAX_PATH 260
#endif

#ifndef NULL 
#define NULL 0
#endif

//return type
enum LD_ResultType{
    LD_Result_TRUE,
	LD_Result_ERROR,
	LD_Result_ERROR_EXIST,
	LD_Result_REF
};

class LD_TreeNode;

//variable
class LD_NameValue  
{
public:
	LD_NameValue();
	LD_NameValue  operator=(LD_NameValue& nameValue);
	bool  operator==(LD_NameValue& nameValue);
	bool  operator!=(LD_NameValue& nameValue);

	_TCHAR m_Name[MAX_PATH];
	_TCHAR m_Value[MAX_PATH];
	_TCHAR m_Type[MAX_PATH];
	_TCHAR m_VarAddress[MAX_PATH];
};


//nodes of list to store variables(table)
class LD_NameValueNode{
public:
	LD_NameValueNode(){
		m_IsValid     = true;
		m_pAttachNode = NULL;
		m_pPrev       = NULL;
		m_pNext       = NULL;
		m_ID          = 0;
	}

//	LD_NameValue       m_NameValue;
	LD_NameValueNode*  m_pPrev;
	LD_NameValueNode*  m_pNext;
	//whether the variable(TreeNodes) pointed to is valid.
	bool               m_IsValid;
	//the highest level TreeNode in the tree with value of this 
	LD_TreeNode*       m_pAttachNode;
	//the id of the variable ,useful when the tree is serialized
	int                m_ID;
};

class LD_TreeRoot;

//node of tree, store variables and there position.
class LD_TreeNode
{
public:
	LD_TreeNode(LD_TreeRoot* pRoot);
	~LD_TreeNode();
	LD_TreeNode  operator=(LD_TreeNode& nameValue);

	//nameValue will be copied inside the function
	//ppTreeNode: return the new child node
	//return:
	//LD_Result_TRUE       :succeed add a new child.
	//LD_Result_REF        :succeed add a new child,but it is a reference,
	//                      don't have to add child to it again.
	//LD_Result_ERROR      :failed to add a new child.
	//LD_Result_ERROR_EXIST:failed to add a new child,for the variable has
	//                      already exit in this position.
	LD_ResultType AddChild(LD_NameValue& nameValue,LD_TreeNode** ppTreeNode=NULL);

	//get node from the tree, if failed return false£¬
	//if succeed return true¡£
	bool GetNameValue(LD_NameValue& nameValue);

	//update the value
//	bool SetNameValue(LD_NameValue& nameValue);

	//return the id of the variable in the node,
	//if no variable exit,like the root, return -1;
	int  GetNameValueID();

	LD_TreeNode* GetParent();

	LD_TreeNode* GetFirstChild();
	int          GetChildNum();//return the number of its children,not include grandsons.

	LD_TreeNode* GetNextSibling();
	LD_TreeNode* GetPrevSibling();
	int          GetSiblingNum();//return the number of its sibling, include self.
		
	//remove invalid nodes,
	//if this node is invalid ,this function will delete the node self and return true.
	//if some of its children(grandson) is invalid, they will be deleted and return true.
	//if return false, there is no node invalid.
	//just delete nodes in the tree, do not change variables list.
	bool         RemoveInvalidateNodes();

	//remove immediate child with the same name as nameValue.  
	void         RemoveChild(LD_NameValue& nameValue);

public:
	LD_NameValueNode* m_pNameValueNode;
	LD_TreeRoot*      m_pTreeRoot;

	LD_TreeNode*  m_pFirstChild;
	LD_TreeNode*  m_pParent;
	LD_TreeNode*  m_pPrevSibling;
	LD_TreeNode*  m_pNextSibling;

	//record current variable's name and value.
	LD_NameValue  m_NameValue;
};


//root of tree
class LD_TreeRoot:public LD_TreeNode{
public:
	LD_TreeRoot();
	~LD_TreeRoot();

	//Return the highest level tree node that include the variable. 
	//Only table variables can be found.
	LD_TreeNode*   GetTreeNode(LD_NameValue&  nameValue);
	
	//remove node with the variable of nameValue from the tree
//	void          Remove(LD_NameValue&  nameValue);

	//find variable from the list,only variables of type table is stored in the list. 
    LD_NameValueNode* GetNameValueNode(LD_NameValue&  nameValue);

	//return variable according to the id,called when serialize and deserialize a tree.
	//if no variable is found , return NULL.
	LD_NameValueNode* GetNameValueNode( int ID);
	
	//cleat variables, if IsAll is false remove invalid varibles,else remove all variables.
	void         Clear(bool IsAll=true);

	//reset the nodes that the table variables attached to
	//call this function after all variables are added into the tree.
	void         ResetTreeNode();

	//add NULL at tail to let user input variable.
	void AddNULLAtTail();

private:
	//called in ResetTreeNode(),used when referenced nodes are moved.
	void MoveNode(LD_TreeNode* pTo,LD_TreeNode* pFrom);

	//called in ResetTreeNode(),try to make variables in LDTreeNode valid,
	//if succeed return true, if failed return false¡£
    bool  DealWithInvalideNode(LD_TreeNode* pOldNode);
	
	//update the value of the variable
	//nameValue :input the variable to be changed to.
	//return    :if this variable is not found,return LD_Result_ERROR£¬
	//           if succeed ,return LD_Result_TRUE
//	LD_ResultType UpdateValue(LD_NameValue& nameValue);

public:
	LD_NameValueNode*  m_pNameValueRoot;
};