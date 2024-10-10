/*
 * FILE: A3DModelCollector.h
 *
 * DESCRIPTION: Cache Model object. Avoid massively initialize and finalize.
 *
 * CREATED BY: Cui Ming 2002-1-10 in A3DModelCollector
 *
 * HISTORY:
 *		Modified by Hedi to make it be compatible with model management
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef _A3DMODELCOLLECTOR_H_
#define _A3DMODELCOLLECTOR_H_

#define DEFAULT_MODEL_NUMBER_IN_CACHE 10

class A3DModelCollector;
class A3DModel;
class A3DModelCollectorElement
{
public:
	bool Init(const char * filename);
	~A3DModelCollectorElement();
	inline int  AddModelObject(A3DModel *pModel)
	{
		if(m_modcount >= m_capability)
		{
			A3DModel **p;
			int capability;
			if(m_capability < 100) 
				capability = m_capability * 2; 
			else 
				capability = m_capability * 3/2;
			p = (A3DModel **)realloc(m_modtab,capability * sizeof(A3DModel *));
			if(!p) return -1;
			m_capability = capability;
			m_modtab = p;
		}
		m_modtab[m_modcount] = pModel;
		m_modcount ++;
		return 0;
	}

	inline A3DModel * GetModelObject()
	{
		if(!m_modcount) return NULL;
		m_modcount --;
		return m_modtab[m_modcount];
	}

	int Release();

protected:
	char	m_keyfile[_MAX_PATH];
	int		m_modcount;
	int		m_capability;
	A3DModel **m_modtab;
	friend class A3DModelCollector;
};

class A3DModelCollector
{
public:
	A3DModelCollector(int capability);
	~A3DModelCollector();

	int Release();//$$

	int PushBack(const char * filename,A3DModel * pModel);
	A3DModel * PopUp(const char * filename);
protected:
	int			m_elecount;
	int			m_capability;
	A3DModelCollectorElement *m_eletab;
};
#endif
