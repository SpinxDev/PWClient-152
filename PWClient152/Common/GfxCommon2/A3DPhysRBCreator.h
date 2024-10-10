/*
* FILE: A3DPhysRBCreator.h
*
* DESCRIPTION: 帮助构造物理RigidBody对象描述结构的类，包括不可破碎的，可以简单破碎的，和高级破碎三种类型
*		编辑存储一些额外信息，用于构造出APhysXSkeletonRBObjectDesc等对象
*
* CREATED BY: Zhangyachuan, 2009/07/22
*
* HISTORY: 
*
* Copyright (c) 2009 Archosaur Studio, All Rights Reserved.
*/

#ifndef _A3DPHYSRBCREATOR_H_
#define _A3DPHYSRBCREATOR_H_


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

class APhysXGraph;

enum PHYS_RBDESC_DATA_TYPE 
{
	NON_BREAKABLE = 0,			//	None of breakable information (APhysXSkeletonRBObjectDesc)
	SIMPLE_BREAKABLE,			//	Represent for the APhysXSimpleBreakableSkeletonRBObjectDesc
	BREAKABLE,					//	Represent for the APhysXBreakableSkeletonRBObjectDesc

	RBDESC_MAX_NUM,
};

struct PhysRBCreatorInfo
{
	PHYS_RBDESC_DATA_TYPE m_Type;
	float m_fBreakLimit;		// this is only valid when m_Type equals SIMPLE_BREAKABLE / BREAKABLE
};

///////////////////////////////////////////////////////////////////////////
//	
//	Declare of Global functions
//	
///////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DPhysRBCreator
//	
///////////////////////////////////////////////////////////////////////////

class A3DPhysRBCreator
{

public:		//	Types

public:		//	Constructor and Destructor

	A3DPhysRBCreator(void);
	virtual ~A3DPhysRBCreator(void);

public:		//	Attributes

public:		//	Operations

	static A3DPhysRBCreator* CreatePhysRBCreator(unsigned int uRBType);
	static A3DPhysRBCreator* CreatePhysRBInfoFromStream(NxStream* ps);
	static void StorePhysRBInfoToStream(NxStream* ps, unsigned int iBreakableType);

	virtual unsigned int GetRBType() const = 0;
	virtual bool GetCreatorInfo(PhysRBCreatorInfo* pRBCreatorInfo) = 0;
	
	virtual bool Load(NxStream* ps) = 0;
	virtual bool Save(NxStream* ps) = 0;

	virtual A3DPhysRBCreator* Clone() const = 0;
	
	//	Create a breakable rb-object desc by using the original desc data
	virtual APhysXSkeletonRBObjectDesc* CreateSkeletonRBObjectDesc() = 0;

	//	Post processing for the skeleton rb-object desc
	virtual void PostProcessSkeletonRBObjectDesc(APhysXSkeletonRBObjectDesc* pDesc) {  }


protected:	//	Attributes

protected:	//	Operations

	//	Disable copy-ctor & operator =
	A3DPhysRBCreator(const A3DPhysRBCreator&);
	A3DPhysRBCreator& operator = (const A3DPhysRBCreator&);

};



///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DPhysNoneBreakableRBCreator
//	
///////////////////////////////////////////////////////////////////////////


class A3DPhysNoneBreakableRBCreator : public A3DPhysRBCreator
{

public:		//	Types

public:		//	Constructor and Destructor

	A3DPhysNoneBreakableRBCreator(void);
	virtual ~A3DPhysNoneBreakableRBCreator(void);

public:		//	Attributes

public:		//	Operations

protected:	//	Attributes

protected:	//	Operations

	virtual unsigned int GetRBType() const { return NON_BREAKABLE; }

	virtual bool Load(NxStream* ps);
	virtual bool Save(NxStream* ps);

	virtual A3DPhysRBCreator* Clone() const;

	//	Create a breakable rb-object desc by using the original desc data
	virtual APhysXSkeletonRBObjectDesc* CreateSkeletonRBObjectDesc();
	virtual bool GetCreatorInfo(PhysRBCreatorInfo* pRBCreatorInfo);

	//	Disable copy-ctor & operator =
	A3DPhysNoneBreakableRBCreator(const A3DPhysNoneBreakableRBCreator&);
	A3DPhysNoneBreakableRBCreator& operator = (const A3DPhysNoneBreakableRBCreator&);

};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DPhysSimpleBreakableRBCreator
//	
///////////////////////////////////////////////////////////////////////////


class A3DPhysSimpleBreakableRBCreator : public A3DPhysRBCreator
{

public:		//	Types

public:		//	Constructor and Destructor

	A3DPhysSimpleBreakableRBCreator(void);
	virtual ~A3DPhysSimpleBreakableRBCreator(void);

public:		//	Attributes

public:		//	Operations

	float GetBreakLimit() const { return m_fBreakLimit; }
	void SetBreakLimit(float fBreakLimit) { m_fBreakLimit = fBreakLimit; }
	bool GetIsDynamic() const { return m_bIsDynamic; }
	void SetIsDynamic(bool bIsDynamic) { m_bIsDynamic = bIsDynamic; }

protected:	//	Attributes

	static unsigned int s_SimpleBreakVer;

	float m_fBreakLimit;
	bool m_bIsDynamic;

protected:	//	Operations

	virtual unsigned int GetRBType() const { return SIMPLE_BREAKABLE; }

	virtual bool Load(NxStream* ps);
	virtual bool Save(NxStream* ps);

	virtual A3DPhysRBCreator* Clone() const;

	//	Create a breakable rb-object desc by using the original desc data
	virtual APhysXSkeletonRBObjectDesc* CreateSkeletonRBObjectDesc();
	virtual bool GetCreatorInfo(PhysRBCreatorInfo* pRBCreatorInfo);
	
	//	Disable copy-ctor & operator =
	A3DPhysSimpleBreakableRBCreator(const A3DPhysSimpleBreakableRBCreator&);
	A3DPhysSimpleBreakableRBCreator& operator = (const A3DPhysSimpleBreakableRBCreator&);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Class A3DPhysBreakableRBCreator
//	
///////////////////////////////////////////////////////////////////////////

class A3DPhysBreakableRBCreator : public A3DPhysRBCreator
{

public:		//	Types

public:		//	Constructor and Destructor

	A3DPhysBreakableRBCreator(void);
	virtual ~A3DPhysBreakableRBCreator(void);

public:		//	Attributes

public:		//	Operations

	float GetActorBreakLimit() const;
	void SetActorBreakLimit(float fActorBreakLimit);
	float GetBreakFallOffFactor() const;
	void SetBreakFallOffFactor(float fBreakFallOffFactor);

protected:	//	Attributes

	static unsigned int s_BreakVer;

	float m_fActorBreakLimit;
	float m_fBreakFallOffFactor;

	APhysXGraph* m_pActorNeighborGraph;
	APhysXGraph* m_pActorSupportGraph;

protected:	//	Operations

	virtual unsigned int GetRBType() const { return BREAKABLE; }

	virtual bool Load(NxStream* ps);
	virtual bool Save(NxStream* ps);

	virtual A3DPhysRBCreator* Clone() const;

	//	Create a breakable rb-object desc by using the original desc data
	virtual APhysXSkeletonRBObjectDesc* CreateSkeletonRBObjectDesc();
	//	Post processing for the skeleton rb-object desc
	virtual void PostProcessSkeletonRBObjectDesc(APhysXSkeletonRBObjectDesc* pDesc);
	virtual bool GetCreatorInfo(PhysRBCreatorInfo* pRBCreatorInfo);

	//	Disable copy-ctor & operator =
	A3DPhysBreakableRBCreator(const A3DPhysBreakableRBCreator&);
	A3DPhysBreakableRBCreator& operator = (const A3DPhysBreakableRBCreator&);
};

///////////////////////////////////////////////////////////////////////////
//	
//	Inline functions
//	
///////////////////////////////////////////////////////////////////////////


#endif	//	_A3DPHYSRBCREATOR_H_


