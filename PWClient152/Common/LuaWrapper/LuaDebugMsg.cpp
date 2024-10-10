/********************************************************************
	created:	2007/08/17
	author:		liudong
	
	purpose:	lua debug msg
	Copyright (C) 2007 - All Rights Reserved
*********************************************************************/

#include "LD_NameValue.h"
#include "LuaDebugMsg.h"
#include <stdio.h>

LuaDebugMsg::LuaDebugMsg(){
	data=NULL;
}

LuaDebugMsg::~LuaDebugMsg(){
	Release();
}

void LuaDebugMsg::Release(){
	if (data) {
		delete[] data;
		data= NULL;
	} 
}

LuaDebugMsg& LuaDebugMsg::operator=(const LuaDebugMsg& inMsg){
	header= inMsg.header;
	Release();
	data = new char[header.bodyLength];
	memcpy(data,inMsg.data,header.bodyLength);
	return *this;
}


//-------------------both app and debugger send-----------------
void LDM_SendBuffer::FromLDM(LuaDebugMsg& msg){
	Release();
	memcpy(name,msg.data,sizeof(name));
	bufLen= msg.header.bodyLength-sizeof(name);
	buf= new char[bufLen];
	memcpy(buf,msg.data+sizeof(name),bufLen);
}

void LDM_SendBuffer::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_SEND_BUFFER;
	msg.header.bodyLength= sizeof(name)+bufLen;
	msg.data= new char[msg.header.bodyLength]; 
    memcpy(msg.data,name,sizeof(name));
	memcpy(msg.data+sizeof(name),buf,bufLen);
}


LDM_SendBuffer::LDM_SendBuffer(){
	buf   = NULL;
	bufLen= 0;
}

LDM_SendBuffer::~LDM_SendBuffer(){
    Release();
}

void LDM_SendBuffer::Release(){
    if(buf){
		delete[] buf;
		buf = NULL;
	    bufLen= 0;	
	}	
}

void LDM_AppExit::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_APP_EXIT;
	msg.header.bodyLength= 0;
}



//-----------------------app send-----------------------------------

void LDM_ReportError::FromLDM(LuaDebugMsg& msg){
	memcpy(name,msg.data,sizeof(name));
	memcpy(&line,msg.data+sizeof(name),sizeof(int));
	memcpy(info,msg.data+sizeof(name)+sizeof(int),sizeof(info));
}

void LDM_ReportError::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_REPORT_ERROR;
	msg.header.bodyLength= sizeof(name)+sizeof(int)+sizeof(info);
	msg.data= new char[msg.header.bodyLength]; 
	memcpy(msg.data,name,sizeof(name));
	memcpy(msg.data+sizeof(name),&line,sizeof(int));
	memcpy(msg.data+sizeof(name)+sizeof(int),info,sizeof(info));
}

bool LDM_ReportError::FromLuaError(const _TCHAR * err)
{
	const _TCHAR * p0;
	p0 = err;

	//expect [
	while (*p0 != '\0')
	{
		if (*p0 == '[')
		{
			break;
		}
		p0++;
	}
	if (*p0 == '\0')
	{
		return false;
	}

	//expect left "
	while (*p0 != '\0')
	{
		if (*p0 == '\"')
		{
			break;
		}
		p0++;
	}
	if (*p0 == '\0')
	{
		return false;
	}

	p0++; //skip left "

	//expect right "
	_TCHAR * p1;
	p1 = name;
	while (*p0 != '\0')
	{
		if (*p0 == '\"')
		{
			break;
		}
		*p1 = *p0;
		p0++;
		p1++;
	}
	*p1 = '\0';
	if (*p0 == '\0')
	{
		return false;
	}
	
	//expect :
	_TCHAR line_str[DEBUGGER_TOKEN_LEN];
	while (*p0 != '\0')
	{
		if (*p0 == ':')
		{
			break;
		}
		p0++;
	}
	if (*p0 == '\0')
	{
		return false;
	}
	p0++;  //skip :

	//expect next :
	p1 = line_str;
	while (*p0 != '\0')
	{
		if (*p0 == ':')
		{
			break;
		}
		*p1 = *p0;
		p0++;
		p1++;
	}
	*p1 = '\0';
	if (*p0 == '\0')
	{
		return false;
	}
	line = _ttoi(line_str);
	p0++;  //skip :

	//fill error info
	p1 = info;
	while (*p0 != '\0')
	{
		*p1 = *p0;
		p0++;
		p1++;
	}
	*p1 = '\0';

	return true;
}


void LDM_DebugStart::FromLDM(LuaDebugMsg& msg){
	memcpy(name,msg.data,sizeof(name));
}

void LDM_DebugStart::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_DEBUG_START;
	msg.header.bodyLength= sizeof(name);
	msg.data = new char[msg.header.bodyLength];
	memcpy(msg.data,name,sizeof(name));
}

LDM_Break::LDM_Break(){
	lineNum  =-1;
	stackLen = 0;
	callStack= NULL;
}

LDM_Break::~LDM_Break(){
	Release();
}

void LDM_Break::Release(){
	if (callStack) {
		delete[] callStack;
		callStack = NULL;
	}
	stackLen =0;
}

void LDM_Break::FromLDM(LuaDebugMsg& msg){
	Release();
	int*  pInt = (int*)msg.data;
	lineNum    = *pInt;
	++pInt;

	char* pChar= (char*)pInt;
	memcpy(name,pChar,sizeof(name));
	pChar+= sizeof(name);

	stackLen = msg.header.bodyLength - sizeof(int) - sizeof(name);
	stackLen/=sizeof(_TCHAR);
	callStack = new _TCHAR[stackLen];
	memcpy(callStack,pChar,stackLen*sizeof(_TCHAR));
}

void LDM_Break::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_BREAK;
	msg.header.bodyLength= sizeof(int) + sizeof(name) + stackLen*sizeof(_TCHAR);
	msg.data= new char[msg.header.bodyLength];
	char* pData= msg.data;
	memcpy(pData,&lineNum,sizeof(int));
	pData+= sizeof(int);
	memcpy(pData,name,sizeof(name));
	pData+= sizeof(name);
	memcpy(pData,callStack,stackLen*sizeof(_TCHAR));
}

void LDM_ShowBufferLine::FromLDM(LuaDebugMsg& msg){
	char* pChar= msg.data;
	memcpy(&line,pChar,sizeof(int));
	pChar+= sizeof(int);
	memcpy(name,pChar,sizeof(name));
}

void  LDM_ShowBufferLine::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_SHOW_BUFFER_LINE;
	msg.header.bodyLength= sizeof(int)+ sizeof(name);
	msg.data = new char[msg.header.bodyLength];

	char* pChar= msg.data;
	memcpy(pChar,&line,sizeof(int));
	pChar+= sizeof(int);
	memcpy(pChar,name,sizeof(name));
}

void LDM_RTError::ToLDM(LuaDebugMsg& msg){
	msg.header.msgType   = LDM_RT_ERROR ;
	msg.header.bodyLength= 0;
}

//////////////////////////////////////////////////////////////////////////
//    local variables and functions, only active in this file
//////////////////////////////////////////////////////////////////////////
//public buffer
class PublicBuffer{
public:
	PublicBuffer();
	~PublicBuffer();
	void ResetSize();

	int   defaultLength;
	int   length;
	char* pBuf;
};

PublicBuffer::PublicBuffer(){
	defaultLength= 3*1024*1024;//3M
	length       = defaultLength;
	pBuf         = new char[length];
}

PublicBuffer::~PublicBuffer(){
	if (pBuf) {
		delete[] pBuf;
		pBuf = NULL;
	}
	length =0;
}

void PublicBuffer::ResetSize(){
    int   newLength= length+defaultLength;
	char* pNewBuf  = new char[newLength];
	memcpy(pNewBuf,pBuf,length);
    delete[] pBuf;
	length = newLength;
	pBuf   = pNewBuf;
}

static PublicBuffer publicBuf; 

//to avoid the buffer overflow .
bool MyMemcpy(void* pBuf,const void* pValue,size_t bytes){
    int msgLength= (char*)pBuf-publicBuf.pBuf+bytes;
	if (msgLength > publicBuf.length ) {
		//::MessageBox(0,_T("the varaible to send is too big,larger than 3M!"),0,0);
		//here should call PublicBuffer::ResetSize(),then call TreeToLDM() again.
		return false;
	}
    memcpy(pBuf,pValue,bytes);
	return true;
}

//recursive, to store each node of a tree.
bool SerializeTree(char*& pBuf,LD_TreeNode* pNode){
	//store the id of the variable in the current node.
	int valueID=pNode->GetNameValueID();
	if(!MyMemcpy(pBuf,&valueID,sizeof(int))) {return false;}
	pBuf+= sizeof(int);

	//store the the variable in the current node.
	int strBytes= _tcslen(pNode->m_NameValue.m_Name)*sizeof(_TCHAR);
	if(!MyMemcpy(pBuf,&strBytes,sizeof(int))) {return false;}
 	pBuf+= sizeof(int);
	if(!MyMemcpy(pBuf,pNode->m_NameValue.m_Name,strBytes)) {return false;}
	pBuf+=strBytes;

	strBytes= _tcslen(pNode->m_NameValue.m_Value)*sizeof(_TCHAR);
	if(!MyMemcpy(pBuf,&strBytes,sizeof(int))) {return false;}
 	pBuf+= sizeof(int);
	if(!MyMemcpy(pBuf,pNode->m_NameValue.m_Value,strBytes)) {return false;}
	pBuf+=strBytes;

	strBytes= _tcslen(pNode->m_NameValue.m_Type)*sizeof(_TCHAR);
	if(!MyMemcpy(pBuf,&strBytes,sizeof(int))) {return false;}
 	pBuf+= sizeof(int);
	if(!MyMemcpy(pBuf,pNode->m_NameValue.m_Type,strBytes)) {return false;}
	pBuf+=strBytes;

	strBytes= _tcslen(pNode->m_NameValue.m_VarAddress)*sizeof(_TCHAR);
	if(!MyMemcpy(pBuf,&strBytes,sizeof(int))) {return false;}
 	pBuf+= sizeof(int);
	if(!MyMemcpy(pBuf,pNode->m_NameValue.m_VarAddress,strBytes)) {return false;}
	pBuf+=strBytes;

	//store the number of children
    int childNum= pNode->GetChildNum();
	if(!MyMemcpy(pBuf,&childNum,sizeof(int))) {return false;}
 	pBuf+= sizeof(int);

	//store each child node
    LD_TreeNode* pChild= pNode->GetFirstChild();
	while (pChild) {
		if(!SerializeTree(pBuf,pChild)){
			return false;
		}
		pChild= pChild->GetNextSibling();
	}
	return true;
}

//recursive, to make a tree from the serialized data.
void DeserializeTree(LD_TreeNode* pNode,char*& pBuf){
	//get the id of the variable in the current node.
	int valueID;
	memcpy(&valueID,pBuf,sizeof(int));
	pBuf+= sizeof(int);
	pNode->m_pNameValueNode = pNode->m_pTreeRoot->GetNameValueNode(valueID);
	if (pNode->m_pNameValueNode && !pNode->m_pNameValueNode->m_pAttachNode) {
		pNode->m_pNameValueNode->m_pAttachNode= pNode;
	}
	//get the the variable in the current node.
	int strBytes;
	memcpy(&strBytes,pBuf,sizeof(int));
	pBuf+= sizeof(int);
	memcpy(pNode->m_NameValue.m_Name,pBuf,strBytes);
	pNode->m_NameValue.m_Name[strBytes/sizeof(_TCHAR)]=_T('\0');
	pBuf+=strBytes;

	strBytes;
	memcpy(&strBytes,pBuf,sizeof(int));
	pBuf+= sizeof(int);
	memcpy(pNode->m_NameValue.m_Value,pBuf,strBytes);
	pNode->m_NameValue.m_Value[strBytes/sizeof(_TCHAR)]=_T('\0');
	pBuf+=strBytes;

	strBytes;
	memcpy(&strBytes,pBuf,sizeof(int));
	pBuf+= sizeof(int);
	memcpy(pNode->m_NameValue.m_Type,pBuf,strBytes);
	pNode->m_NameValue.m_Type[strBytes/sizeof(_TCHAR)]=_T('\0');
	pBuf+=strBytes;

	strBytes;
	memcpy(&strBytes,pBuf,sizeof(int));
	pBuf+= sizeof(int);
	memcpy(pNode->m_NameValue.m_VarAddress,pBuf,strBytes);
	pNode->m_NameValue.m_VarAddress[strBytes/sizeof(_TCHAR)]=_T('\0');
	pBuf+=strBytes;

	//get the number of children
    int childNum;
	memcpy(&childNum,pBuf,sizeof(int));
	pBuf+= sizeof(int);

	//get each child node
    LD_TreeNode* pChild    = NULL;
	LD_TreeNode* pLastChild= NULL;
	for (int i=0;i<childNum;++i) {
		pChild= new LD_TreeNode(pNode->m_pTreeRoot);
		DeserializeTree(pChild,pBuf);
		pChild->m_pParent = pNode;
		if (!pNode->m_pFirstChild){
			pNode->m_pFirstChild = pChild;
		}else{
			pLastChild->m_pNextSibling = pChild;
			pChild->m_pPrevSibling = pLastChild;
		}
		pLastChild = pChild;
	}
}

//store the tree in the message,only change msg.header.bodyLength ºÍ msg.data
//msg  : output the message that is filled with data.
//pRoot: input the tree to be send. 
void TreeToLDM(LuaDebugMsg& msg,LD_TreeRoot* pRoot){
	if (!pRoot) {
		return;
	}
	pRoot->ResetTreeNode();
	//public buffer
    char* pBuf= publicBuf.pBuf;

	//store number of the variables
	int valueNum=0;
	LD_NameValueNode*  pNameValueNow = pRoot->m_pNameValueRoot;
	while (pNameValueNow) {
		pNameValueNow->m_ID = valueNum;
		++valueNum;
		pNameValueNow = pNameValueNow->m_pNext;
	}
	memcpy(pBuf,&valueNum,sizeof(int));
	pBuf+= sizeof(int);

	/*
	//store each variable
	pNameValueNow = pRoot->m_pNameValueRoot;
	while (pNameValueNow) {
		int strLength= sizeof(pNameValueNow->m_NameValue.m_Name);
		memcpy(pBuf,pNameValueNow->m_NameValue.m_Name,strLength);
		pBuf+= strLength;

	    strLength= sizeof(pNameValueNow->m_NameValue.m_Value );
		memcpy(pBuf,pNameValueNow->m_NameValue.m_Value,strLength);
		pBuf+= strLength;

		strLength= sizeof(pNameValueNow->m_NameValue.m_Type);
		memcpy(pBuf,pNameValueNow->m_NameValue.m_Type,strLength);
		pBuf+= strLength;

		pNameValueNow = pNameValueNow->m_pNext;
	}
	*/
    
	//store the tree
	if (!SerializeTree(pBuf,pRoot)) {
		LD_TreeRoot newRoot;
		LD_NameValue nameValue;
		_stprintf(nameValue.m_Name,_T("var_overflow"));
		_stprintf(nameValue.m_Value,_T("the varaible to send is too big,larger than 3M!"));
		_stprintf(nameValue.m_Type,_T("error"));
		newRoot.AddChild(nameValue);
		TreeToLDM(msg,&newRoot);
		return;
	}

	int msgLength= pBuf-publicBuf.pBuf;
//	if (msgLength > publicBuf.length ) {
//		::MessageBox(0,_T("the varaible to send is too big,larger than 3M!"),0,0);
//		//here should call PublicBuffer::ResetSize(),then call TreeToLDM() again.
// 	}
	
	msg.header.bodyLength = msgLength;
	msg.data = new char[msgLength];
	memcpy(msg.data,publicBuf.pBuf,msgLength);
}

//create a tree from a message
//pRoot: output the tree that is created
//msg  : input the message that contains the data of the tree.
void LDMToTree(LD_TreeRoot* pRoot,LuaDebugMsg& msg){
	if (!pRoot) {
		return;
	}
	pRoot->Clear();
	char* pBuf= msg.data;

	//get the total number of variables
	int valueNum;
	memcpy(&valueNum,pBuf,sizeof(int));
	pBuf+=sizeof(int);

	//get each variable and create nodes in pRoot.
	LD_NameValueNode*  pNameValueNow;
	LD_NameValueNode*  pNameValueLast;
	for (int i=0;i<valueNum;++i) {
		pNameValueNow = new LD_NameValueNode();
		pNameValueNow->m_ID = i;
		if (!pRoot->m_pNameValueRoot) {
			pRoot->m_pNameValueRoot = pNameValueNow;
		}else{
			pNameValueLast->m_pNext = pNameValueNow;
			pNameValueNow->m_pPrev  = pNameValueLast;
		}
		pNameValueLast = pNameValueNow;
	}

	//create the tree.
	DeserializeTree(pRoot,pBuf);
	
	pRoot->ResetTreeNode();
}
//////////////////////////////////////////////////////////////////////////
//    end:     local variables and functions, only active in this file
//////////////////////////////////////////////////////////////////////////


void LDM_SendAllGlobalVar::FromLDM(LuaDebugMsg& msg){
	if (m_pRoot) {
	    LDMToTree(m_pRoot,msg);
	}
}

void LDM_SendAllGlobalVar::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_SEND_ALL_GLOBAL_VAR;
	if (m_pRoot) {
		TreeToLDM(msg,m_pRoot);
	}
}

void LDM_SendAllLocalVar::FromLDM(LuaDebugMsg& msg){
	if (m_pRoot) {
	    LDMToTree(m_pRoot,msg);
	}
}

void LDM_SendAllLocalVar::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_SEND_ALL_LOCAL_VAR;
	if (m_pRoot) {
		TreeToLDM(msg,m_pRoot);
	}	
}

void LDM_SendWatchVar::FromLDM(LuaDebugMsg& msg){
	if (m_pRoot) {
	    LDMToTree(m_pRoot,msg);
	}
}

void LDM_SendWatchVar::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_SEND_WATCH_VAR;
	if (m_pRoot) {
		TreeToLDM(msg,m_pRoot);
	}	
}



//------------------------debugger send------------------------------

void  LDM_StepOver::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_STEP_OVER;
	msg.header.bodyLength= 0;
}

void  LDM_StepInto::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_STEP_INTO;
	msg.header.bodyLength= 0;
}

void  LDM_StepOut::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_STEP_OUT;
	msg.header.bodyLength= 0;
}

void LDM_RunToCursor::FromLDM(LuaDebugMsg& msg){
	char* pChar= msg.data;
	memcpy(&line,pChar,sizeof(int));
	pChar+= sizeof(int);
	memcpy(name,pChar,sizeof(name));
}

void  LDM_RunToCursor::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_RUN_TO_CURSOR;
	msg.header.bodyLength= sizeof(int)+ sizeof(name);
	msg.data = new char[msg.header.bodyLength];

	char* pChar= msg.data;
	memcpy(pChar,&line,sizeof(int));
	pChar+= sizeof(int);
	memcpy(pChar,name,sizeof(name));
}


WatchVarNameNode::WatchVarNameNode(){
    pNext = NULL;
}

WatchVarNameNode::~WatchVarNameNode(){
	if (pNext) {
		delete pNext;
	}
	pNext = NULL;
}

LDM_RequestWatchVar::LDM_RequestWatchVar(){
    m_pHead = NULL;
	m_NameNum =0;
}

LDM_RequestWatchVar::~LDM_RequestWatchVar(){
    Release();
}

void LDM_RequestWatchVar::Release(){
    if (m_pHead) {
		delete m_pHead;
		m_pHead = NULL;
    }
	m_NameNum =0;
}

void LDM_RequestWatchVar::FromLDM(LuaDebugMsg& msg){
	Release();
	const int NAME_LENGTH= DEBUGGER_TOKEN_LEN*sizeof(_TCHAR);
	m_NameNum = msg.header.bodyLength / NAME_LENGTH;
	if (m_NameNum<=0) {
		return;
	}

	WatchVarNameNode* pNodeLast;
    char* pChar=msg.data;
	for (int i=0;i<m_NameNum;++i) {
		WatchVarNameNode* pNodeNow=new WatchVarNameNode();
		memcpy(pNodeNow->name,pChar,NAME_LENGTH);
		if (!m_pHead) {
			m_pHead = pNodeNow;
		}else{
            pNodeLast->pNext = pNodeNow;
		}
		pNodeLast = pNodeNow;
		pChar+= NAME_LENGTH;
	}
	
}

void LDM_RequestWatchVar::ToLDM(LuaDebugMsg& msg){
	m_NameNum = 0;
	WatchVarNameNode* pNodeNow= m_pHead;
	while (pNodeNow) {
		++m_NameNum;
		pNodeNow= pNodeNow->pNext;
	} 
	const int NAME_LENGTH= DEBUGGER_TOKEN_LEN*sizeof(_TCHAR);
    
	msg.Release();
	msg.header.msgType   = LDM_REQUEST_WATCH_VAR;
	msg.header.bodyLength= m_NameNum*NAME_LENGTH;
	if (msg.header.bodyLength>0) {
		msg.data = new char[msg.header.bodyLength];
		char* pChar= msg.data;
		pNodeNow= m_pHead;
		while (pNodeNow) {
			memcpy(pChar,pNodeNow->name,NAME_LENGTH);
			pChar+= NAME_LENGTH;
			pNodeNow= pNodeNow->pNext;
		}
	}else{
		msg.data = NULL;
	}

}

void LDM_RequestAllGlobalVar::FromLDM(LuaDebugMsg& msg){
    
}

void LDM_RequestAllGlobalVar::ToLDM(LuaDebugMsg& msg){
    msg.Release();
	msg.header.msgType   = LDM_REQUEST_ALL_GLOBAL_VAR;
	msg.header.bodyLength= 0;
}

void LDM_RequestAllLocalVar::FromLDM(LuaDebugMsg& msg){
    
}

void LDM_RequestAllLocalVar::ToLDM(LuaDebugMsg& msg){
    msg.Release();
	msg.header.msgType   = LDM_REQUEST_ALL_LOCAL_VAR;
	msg.header.bodyLength= 0;
}

void  LDM_StackChange::FromLDM(LuaDebugMsg& msg){
	char* pChar=msg.data;
	memcpy(&level,pChar,sizeof(int));
}

void  LDM_StackChange::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_STACK_CHANGE;
	msg.header.bodyLength= sizeof(int);
	msg.data = new char[msg.header.bodyLength];
	char* pChar=msg.data;
	memcpy(pChar,&level,sizeof(int));
}

void LDM_DebuggerExit::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_DEBUGGER_EXIT;
	msg.header.bodyLength= 0;
}

void LDM_RequestBuffer::FromLDM(LuaDebugMsg& msg){
	memcpy(name,msg.data,sizeof(name));
}

void LDM_RequestBuffer::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_REQUEST_BUFFER;
	msg.header.bodyLength= sizeof(name);
	msg.data= new char[msg.header.bodyLength]; 
	memcpy(msg.data,name,sizeof(name));
}

void LDM_Go::ToLDM(LuaDebugMsg& msg){
    msg.Release();
	msg.header.msgType   = LDM_GO;
	msg.header.bodyLength= 0;
}

void LDM_BreakPoint::FromLDM(LuaDebugMsg& msg){
	char* pChar= msg.data;
	memcpy(&isAdd,pChar,sizeof(bool));
	pChar+= sizeof(bool);
	memcpy(&lineNum,pChar,sizeof(int));
	pChar+= sizeof(int);
	memcpy(name,pChar,sizeof(name));
}

void LDM_BreakPoint::ToLDM(LuaDebugMsg& msg){
	msg.Release();
	msg.header.msgType   = LDM_BREAK_POINT ;
	msg.header.bodyLength= sizeof(bool)+ sizeof(int)+ sizeof(name);
	msg.data = new char[msg.header.bodyLength];

	char* pChar= msg.data;
	memcpy(pChar,&isAdd,sizeof(bool));
	pChar+= sizeof(bool);
	memcpy(pChar,&lineNum,sizeof(int));
	pChar+= sizeof(int);
	memcpy(pChar,name,sizeof(name));
}


