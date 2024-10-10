#ifndef _ROTLIST_H_
#define _ROTLIST_H_

template <class T>
class RotList
{
public:
	explicit RotList(int nBufSz)
	{
		m_nBufSz	= nBufSz;
		m_pData		= new T[nBufSz];
		m_nHead		= nBufSz - 1;
		m_nCount	= 0;
	}
	~RotList() { delete[] m_pData; }

protected:
	int		m_nBufSz;
	T*		m_pData;
	int		m_nHead;
	int		m_nCount;

protected:
	int	_get_next()	{ return m_nHead == m_nBufSz - 1 ? 0 : m_nHead + 1; }
	int _get_prev() { return m_nHead == 0 ? m_nBufSz - 1 : m_nHead - 1;	}
	int _get_offset(int nOff) const
	{
		nOff += m_nHead - m_nCount + 1;
		if (nOff < 0) nOff += m_nBufSz;
		return nOff;
	}

public:
	T& AddData(const T& data)
	{
		m_nHead = _get_next();
		if (m_nCount < m_nBufSz) m_nCount++;
		m_pData[m_nHead] = data;
		return m_pData[m_nHead];
	}
	int GetDataCount() const { return m_nCount;	}
	int	GetBufSize () const { return m_nBufSz; }
	void RemoveOldData(int nCount) { m_nCount -= nCount; }
	T& operator[] (int nIndex) { return m_pData[_get_offset(nIndex)]; }
	const T& operator[] (int nIndex) const { return m_pData[_get_offset(nIndex)]; }
	void RemoveAll() { m_nCount = 0; }
	void RemoveHead() { m_nHead = _get_prev(); m_nCount--; }
};


#endif