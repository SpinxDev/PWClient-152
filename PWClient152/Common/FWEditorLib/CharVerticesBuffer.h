#ifndef _CHARVERTICESBUFFER_H_
#define _CHARVERTICESBUFFER_H_


template<class T>
class APtrList;

// used for contain data that used in conjunction 
// by CCharCurve/CCharContour/CCharOutline
class CCharVerticesBuffer  
{
public:
	int m_nVerticesCount;
	double (* m_paVertices)[2];
private:
	int ComputeVerticesArraySize() const;
	bool BuildVerticesCountFromOtherVerticesList(APtrList<CCharVerticesBuffer *> * pListVerticesBuffer);
	bool BuildVerticesArrayFromOtherVerticesList(APtrList<CCharVerticesBuffer *> * pListVerticesBuffer);
public:
	bool SetVerticesAt(int nIndex, double *p);
	void ReversVertices();
	bool IsCreated() const;
	int GetBufferSize() const;
	int GetVerticesCount() const{ return m_nVerticesCount; }
	void Destroy();
	bool Create(APtrList<CCharVerticesBuffer *> * pListVerticesBuffer);
	bool Create(int nVerticesCount);
	CCharVerticesBuffer();
	~CCharVerticesBuffer();
};

#endif 
