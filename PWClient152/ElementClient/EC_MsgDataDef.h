
#pragma once

enum DataType
{
	MSG_DATABASE = 0,
	MSG_TARGETPOS,
	MSG_TARGETNPC,
};

class MsgDataBase
{
protected:
	int m_imsgType;
public:
	MsgDataBase(int type)
	{
		m_imsgType = type;
	}
	virtual ~MsgDataBase() {}
	virtual MsgDataBase* Clone() {	return new MsgDataBase(*this);	}

	int GetDataType() const { return m_imsgType; }
};

class MsgDataAutoMove : public MsgDataBase
{
	int		m_iNPCId;
	int		m_iTaskId;
public:
	MsgDataAutoMove(int type, int id, int taskid) : MsgDataBase(type)
	{
		m_iNPCId = id;
		m_iTaskId = taskid;
	}
	virtual ~MsgDataAutoMove() {}
	virtual MsgDataBase* Clone() {	return new MsgDataAutoMove(*this);	}
	
	int GetTargetID()const
	{
		return m_iNPCId;
	}

	int GetTaskID()const
	{
		return m_iTaskId;
	}
};