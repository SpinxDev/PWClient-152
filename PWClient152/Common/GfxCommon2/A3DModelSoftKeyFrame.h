/*
 * FILE: A3DModelSoftKeyFrame.h
 *
 * DESCRIPTION: 
 *
 * CREATED BY: YangLiu, 2012/11/23
 *
 * HISTORY: 
 *
 * Copyright (c) 2012 Archosaur Studio, All Rights Reserved.
 */

#ifndef _A3DMODELSOFTKEYFRAME_H_
#define _A3DMODELSOFTKEYFRAME_H_


///////////////////////////////////////////////////////////////////////////
//	
//	Define and Macro
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Types and Global variables
//	
///////////////////////////////////////////////////////////////////////////

class NxStream;

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////

struct JointInfo
{
	JointInfo() : mSpring(100.0f), mDamping(1.0f)
	{

	}

	AString	strName;
	float mSpring;
	float mDamping;
};

class SoftKeyFrameConfigDesc
{
public:
	static int GetVersion() { return 0xDD000001; }
	static const char* DefJointName() { return "General"; }

public:
	SoftKeyFrameConfigDesc();
	SoftKeyFrameConfigDesc(const AString& strName);
	SoftKeyFrameConfigDesc(const SoftKeyFrameConfigDesc& rhs);

	bool Save(NxStream* pNxStream) const;
	bool Load(NxStream* pNxStream);

	bool operator == (const SoftKeyFrameConfigDesc& rhs) const;
	bool operator != (const SoftKeyFrameConfigDesc& rhs) const;

	bool IsModified() const { return m_bIsModified; }
	void ClearModifiedFlag() { m_bIsModified = false; }

	const AString& GetName() const { return m_strConfigName; }

	// add new item if the input name is not exist
	// update joint info if the input name is exist
	// update general joint info if the input name is empty or DefJointName() 
	void UpdateJointInfo(const JointInfo& ji);
	// return general joint info if the input name is empty or DefJointName()
	bool GetJointInfo(JointInfo& outValue, const char* pName = 0) const;
	void RemoveJointInfo(const char* pName);
	// remove all joint info except general joint(reset default general joint value)
	void RemoveAllJointInfo();
	int  GetNBJointInfo() const;
	bool GetJointInfo(JointInfo& outValue, int index) const;

	void AddFixActorBone(const char* pBone);
	void RemoveFixActorBone(const char* pBone);
	void RemoveAllFixActorBones();
	int  GetNBFixActorBones() const;
	bool GetFixActorBone(int index, AString& outBone) const;

	void AddBestForceBone(const char* pBone);
	void RemoveBestForceBone(const char* pBone);
	void RemoveAllBestForceBones();
	int  GetNBBestForceBones() const;
	bool GetBestForceBone(int index, AString& outBone) const;

private:
	friend class A3DModelPhysSyncData;
	void SetName(const AString& str) { m_strConfigName = str; }
	void SetDefGeneralJointInfo();

private:
	bool m_bIsModified;
	AString m_strConfigName;
	abase::vector<JointInfo> m_JointValue;    // the first element is a general configuration.
	abase::vector<AString> m_strFixActorBones;
	abase::vector<AString> m_strBestForceBones;
};

class SoftKeyFrameDescNotify
{
public:
	virtual void onReleaseDesc(SoftKeyFrameConfigDesc& skfDesc) = 0;

protected:
	virtual ~SoftKeyFrameDescNotify(){};
};


///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////

#endif	//	_A3DMODELSOFTKEYFRAME_H_
