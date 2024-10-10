/*
 * FILE: A3DModelSoftKeyFrame.cpp
 *
 * DESCRIPTION: 
 *
 * CREATED BY: YangLiu, 2012/11/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
 */

#include "StdAfx.h"
#ifdef A3D_PHYSX

#include "A3DModelSoftKeyFrame.h"

///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Reference to External variables and functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local Types and Variables and Global variables
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Local functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Implement SoftKeyFrameConfigDesc
//	
///////////////////////////////////////////////////////////////////////////

SoftKeyFrameConfigDesc::SoftKeyFrameConfigDesc()
{
	SetDefGeneralJointInfo();
}

SoftKeyFrameConfigDesc::SoftKeyFrameConfigDesc(const AString& strName)
{
	SetName(strName);
	SetDefGeneralJointInfo();
}

SoftKeyFrameConfigDesc::SoftKeyFrameConfigDesc(const SoftKeyFrameConfigDesc& rhs)
{
	m_strConfigName = rhs.m_strConfigName;
	m_JointValue = rhs.m_JointValue;
	m_strFixActorBones = rhs.m_strFixActorBones;
	m_strBestForceBones = rhs.m_strBestForceBones;
	m_bIsModified = true;
}

bool SoftKeyFrameConfigDesc::operator == (const SoftKeyFrameConfigDesc& rhs) const
{
	if (m_strConfigName != rhs.m_strConfigName)
		return false;

	size_t nCount = m_JointValue.size();
	size_t nRCount = rhs.m_JointValue.size();
	if (nCount != nRCount)
		return false;

	float diff = 0.0f;
	size_t i = 0, j = 0;
	for (i = 0; i < nCount; ++i)
	{
		for (j = 0; j < nRCount; ++j)
		{
			if (m_JointValue[i].strName == rhs.m_JointValue[j].strName)
			{
				diff = abs(m_JointValue[i].mSpring - rhs.m_JointValue[j].mSpring);
				if (!(diff < 0.00001f))
					return false;
				diff = abs(m_JointValue[i].mDamping - rhs.m_JointValue[j].mDamping);
				if (!(diff < 0.00001f))
					return false;
				break;
			}
		}
		if (j < nRCount)
			continue;
		else
			return false;
	}

	nCount = m_strFixActorBones.size();
	nRCount = rhs.m_strFixActorBones.size();
	if (nCount != nRCount)
		return false;
	for (i = 0; i < nCount; ++i)
	{
		for (j = 0; j < nRCount; ++j)
		{
			if (m_strFixActorBones[i] == rhs.m_strFixActorBones[j])
				break;
		}
		if (j < nRCount)
			continue;
		else
			return false;
	}

	nCount = m_strBestForceBones.size();
	nRCount = rhs.m_strBestForceBones.size();
	if (nCount != nRCount)
		return false;
	for (i = 0; i < nCount; ++i)
	{
		for (j = 0; j < nRCount; ++j)
		{
			if (m_strBestForceBones[i] == rhs.m_strBestForceBones[j])
				break;
		}
		if (j < nRCount)
			continue;
		else
			return false;
	}
	return true;
}

bool SoftKeyFrameConfigDesc::Save(NxStream* pNxStream) const
{
	pNxStream->storeDword(GetVersion());
	WriteString(pNxStream, m_strConfigName);

	size_t nCount = m_JointValue.size();
	pNxStream->storeDword(nCount);
	for (size_t i = 0; i < nCount; ++i)
	{
		WriteString(pNxStream, m_JointValue[i].strName);
		pNxStream->storeFloat(m_JointValue[i].mSpring);
		pNxStream->storeFloat(m_JointValue[i].mDamping);
	}

	nCount = m_strFixActorBones.size();
	pNxStream->storeDword(nCount);
	for (size_t i = 0; i < nCount; ++i)
		WriteString(pNxStream, m_strFixActorBones[i]);

	nCount = m_strBestForceBones.size();
	pNxStream->storeDword(nCount);
	for (size_t i = 0; i < nCount; ++i)
		WriteString(pNxStream, m_strBestForceBones[i]);
	return true;
}

bool SoftKeyFrameConfigDesc::Load(NxStream* pNxStream)
{
	int nVer = pNxStream->readDword();
	int nPrefixRead = nVer & 0xff000000;
	int nPrefixNum = GetVersion() & 0xff000000;
	if (nPrefixRead != nPrefixNum)
		return false;
	ReadString(pNxStream, m_strConfigName);

	m_JointValue.clear();
	size_t nCount = pNxStream->readDword();
	for (size_t i = 0; i < nCount; ++i)
	{
		JointInfo ji;
		ReadString(pNxStream, ji.strName);
		ji.mSpring = pNxStream->readFloat();
		ji.mDamping = pNxStream->readFloat();
		m_JointValue.push_back(ji);
	}

	m_strFixActorBones.clear();
	AString strRead;
	nCount = pNxStream->readDword();
	for (size_t i = 0; i < nCount; ++i)
	{
		ReadString(pNxStream, strRead);
		m_strFixActorBones.push_back(strRead);
	}

	m_strBestForceBones.clear();
	nCount = pNxStream->readDword();
	for (size_t i = 0; i < nCount; ++i)
	{
		ReadString(pNxStream, strRead);
		m_strBestForceBones.push_back(strRead);
	}
	m_bIsModified = true;
	return true;
}

bool SoftKeyFrameConfigDesc::operator != (const SoftKeyFrameConfigDesc& rhs) const
{
	return !(*this == rhs);
}

void SoftKeyFrameConfigDesc::SetDefGeneralJointInfo()
{
	if (0 < m_JointValue.size())
	{
		m_JointValue[0].mSpring = 100.0f;
		m_JointValue[0].mDamping = 1.0f;
	}
	else
	{
		JointInfo jntVal;
		jntVal.strName = DefJointName();
		jntVal.mSpring = 100.0f;
		jntVal.mDamping = 1.0f;
		m_JointValue.push_back(jntVal);
	}
	m_bIsModified = true;
}

void SoftKeyFrameConfigDesc::UpdateJointInfo(const JointInfo& ji)
{
	m_bIsModified = true;
	if (ji.strName.IsEmpty() || ji.strName == DefJointName())
	{
		m_JointValue[0].mSpring = ji.mSpring;
		m_JointValue[0].mDamping = ji.mDamping;
		return;
	}

	size_t nCount = m_JointValue.size();
	for (size_t i = 1; i < nCount; ++i)
	{
		if (ji.strName == m_JointValue[i].strName)
		{
			m_JointValue[i].mSpring = ji.mSpring;
			m_JointValue[i].mDamping = ji.mDamping;
			return;
		}
	}
	m_JointValue.push_back(ji);
}

bool SoftKeyFrameConfigDesc::GetJointInfo(JointInfo& outValue, const char* pName) const
{
	size_t nCount = m_JointValue.size();
	if (0 == nCount)
		return false;

	if ((0 == pName) || (0 == pName[0]))
	{
		outValue = m_JointValue[0];
		return true;
	}

	for (size_t i = 0; i < nCount; ++i)
	{
		if (!m_JointValue[i].strName.CompareNoCase(pName))
		{
			outValue = m_JointValue[i];
			return true;
		}
	}

	return false;
}

void SoftKeyFrameConfigDesc::RemoveJointInfo(const char* pName)
{
	if ((0 == pName) || (0 == pName[0]))
		return;

	size_t nCount = m_JointValue.size();
	// do not remove general joint info
	for (size_t i = 1; i < nCount; ++i)
	{
		if (!m_JointValue[i].strName.CompareNoCase(pName))
		{
			if (i != nCount - 1)
				m_JointValue[i] = m_JointValue[nCount - 1];
			m_JointValue.pop_back();
			m_bIsModified = true;
			return;
		}
	}
}

void SoftKeyFrameConfigDesc::RemoveAllJointInfo()
{
	m_JointValue.clear();
	SetDefGeneralJointInfo();
	m_bIsModified = true;
}

int SoftKeyFrameConfigDesc::GetNBJointInfo() const
{
	return int(m_JointValue.size());
}

bool SoftKeyFrameConfigDesc::GetJointInfo(JointInfo& outValue, int index) const
{
	size_t nCount = m_JointValue.size();
	if ((0 <= index) && (index < nCount))
	{
		outValue = m_JointValue[index];
		return true;
	}
	return false;
}

void SoftKeyFrameConfigDesc::AddFixActorBone(const char* pBone)
{
	if ((0 == pBone) || (0 == pBone[0]))
		return;

	size_t nCount = m_strFixActorBones.size();
	for (size_t i = 0; i < nCount; ++i)
	{
		if (!m_strFixActorBones[i].CompareNoCase(pBone))
			return;
	}

	m_strFixActorBones.push_back(AString(pBone));
	m_bIsModified = true;
}

void SoftKeyFrameConfigDesc::RemoveFixActorBone(const char* pBone)
{
	if ((0 == pBone) || (0 == pBone[0]))
		return;

	size_t nCount = m_strFixActorBones.size();
	for (size_t i = 0; i < nCount; ++i)
	{
		if (!m_strFixActorBones[i].CompareNoCase(pBone))
		{
			if (i != nCount - 1)
				m_strFixActorBones[i] = m_strFixActorBones[nCount - 1];
			m_strFixActorBones.pop_back();
			m_bIsModified = true;
			return;
		}
	}
}

void SoftKeyFrameConfigDesc::RemoveAllFixActorBones()
{
	m_strFixActorBones.clear();
	m_bIsModified = true;
}

int SoftKeyFrameConfigDesc::GetNBFixActorBones() const
{
	return int(m_strFixActorBones.size());
}

bool SoftKeyFrameConfigDesc::GetFixActorBone(int index, AString& outBone) const
{
	size_t nCount = m_strFixActorBones.size();
	if ((0 <= index) && (index < nCount))
	{
		outBone = m_strFixActorBones[index];
		return true;
	}
	return false;
}

void SoftKeyFrameConfigDesc::AddBestForceBone(const char* pBone)
{
	if ((0 == pBone) || (0 == pBone[0]))
		return;

	size_t nCount = m_strBestForceBones.size();
	for (size_t i = 0; i < nCount; ++i)
	{
		if (!m_strBestForceBones[i].CompareNoCase(pBone))
			return;
	}

	m_strBestForceBones.push_back(AString(pBone));
}

void SoftKeyFrameConfigDesc::RemoveBestForceBone(const char* pBone)
{
	if ((0 == pBone) || (0 == pBone[0]))
		return;

	size_t nCount = m_strBestForceBones.size();
	for (size_t i = 0; i < nCount; ++i)
	{
		if (!m_strBestForceBones[i].CompareNoCase(pBone))
		{
			if (i != nCount - 1)
				m_strBestForceBones[i] = m_strBestForceBones[nCount - 1];
			m_strBestForceBones.pop_back();
			return;
		}
	}

}

void SoftKeyFrameConfigDesc::RemoveAllBestForceBones()
{
	m_strBestForceBones.clear();
}

int  SoftKeyFrameConfigDesc::GetNBBestForceBones() const
{
	return int(m_strBestForceBones.size());
}

bool SoftKeyFrameConfigDesc::GetBestForceBone(int index, AString& outBone) const
{
	size_t nCount = m_strBestForceBones.size();
	if ((0 <= index) && (index < nCount))
	{
		outBone = m_strBestForceBones[index];
		return true;
	}
	return false;
}

#endif	//	A3D_PHYSX
