/*
 * FILE: A3DGFXCollector.h
 *
 * DESCRIPTION: Cache GFX object. Avoid massively initialize and finalize.
 *
 * CREATED BY: Cui Ming 2002-1-10
 *
 * HISTORY:
 *
 * Copyright (c) 2001 Archosaur Studio, All Rights Reserved.	
 */

#ifndef __A3DGFXCOLLECTOR_H_
#define __A3DGFXCOLLECTOR_H_

#define DEFAULT_GFX_NUMBER_IN_CACHE 10

class A3DGFXCollector;
class A3DGraphicsFX;
class A3DGFXCollectorElement
{
public:
	void Init(const char * filename);
	~A3DGFXCollectorElement();
	inline int  AddGFXObject(A3DGraphicsFX *gfx)
	{
		if(m_gfxcount >= m_capability)
		{
			A3DGraphicsFX **p;
			int capability;
			if(m_capability < 100) 
				capability = m_capability * 2; 
			else 
				capability = m_capability * 3/2;
			p = (A3DGraphicsFX **)realloc(m_gfxtab,capability * sizeof(A3DGraphicsFX *));
			if(!p) return -1;
			m_capability = capability;
			m_gfxtab = p;
		}
		m_gfxtab[m_gfxcount] = gfx;
		m_gfxcount ++;
		return 0;
	}

	inline A3DGraphicsFX * GetGFXObject()
	{
		if(!m_gfxcount) return NULL;
		m_gfxcount --;
		return m_gfxtab[m_gfxcount];
	}

	int Release();

protected:
	char	m_keyfile[_MAX_PATH];
	int		m_gfxcount;
	int		m_capability;
	A3DGraphicsFX **m_gfxtab;
	friend class A3DGFXCollector;
};

class A3DGFXCollector
{
public:
	A3DGFXCollector(int capability);
	~A3DGFXCollector();

	int Release();//$$

	int PushBack(const char * filename,A3DGraphicsFX * gfx);
	A3DGraphicsFX * PopUp(const char * filename);
protected:
	int			m_elecount;
	int			m_capability;
	A3DGFXCollectorElement *m_eletab;
};
#endif
