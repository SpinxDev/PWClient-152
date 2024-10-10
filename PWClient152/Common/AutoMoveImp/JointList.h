/********************************************************************
	created:	2008/05/28
	author:		kuiwu
	
	purpose:	joint list
	Copyright (C) 2008 - All Rights Reserved
*********************************************************************/

#ifndef _JOINT_LIST_H_
#define _JOINT_LIST_H_

#include <vector.h>
using namespace abase;
namespace AutoMove
{

struct Joint 
{
	int layer1;
	int layer2;
	A3DPOINT2 ptMap;
};

class CJointList
{
public:
	CJointList(){};
	~CJointList(){};
	void Release()
	{
		m_aJoints.clear();
	}
	void Add(const Joint& joint)
	{
		m_aJoints.push_back(joint);
	}
	void Del(int index)
	{
		abase::vector<Joint>::iterator it;
		it = m_aJoints.begin() + index;
		m_aJoints.erase(it);
	}
	int GetCount() const
	{
		return (int)m_aJoints.size();
	}
	const Joint * GetJoint(int index) const
	{
		return &m_aJoints[index];
	}

	bool Check(const Joint& joint)
	{
		const int repulsion = 4;
		int i;
		for (i = 0; i < (int)m_aJoints.size(); ++i)
		{
			const Joint& curJoint = m_aJoints[i];
			if ((curJoint.layer1 == joint.layer1 && curJoint.layer2 == joint.layer2)
				||(curJoint.layer1 == joint.layer2 && curJoint.layer2 == joint.layer1))
			{
				A3DPOINT2 diff = curJoint.ptMap - joint.ptMap;
				if (abs(diff.x) < repulsion  && abs(diff.y) < repulsion)
				{
					return false;
				}
			}
		}

		return true;
	}
private:
	
private:
	vector<Joint> m_aJoints;	
};


}

#endif