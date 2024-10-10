// FFT.h: interface for the CFFT class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_FFT_H__AC81A6E0_650B_11D3_88F1_A26870CFF4B8__INCLUDED_)
#define AFX_FFT_H__AC81A6E0_650B_11D3_88F1_A26870CFF4B8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct _tagComplex
{
	float	a;//Real
	float	b;//
} COMPLEX, * PCOMPLEX;

class CFFT  
{
private:
	int *			m_pInverseIndex;
	COMPLEX	*		m_pBuffer;
	COMPLEX	*		m_pW;
	COMPLEX	*		m_pWInverse;
	int				m_N;
	int				m_M;

protected:
	bool SetComplexBuffer(COMPLEX * pBuffer);
	bool GetComplexBuffer(COMPLEX * pBuffer);

public:						
	CFFT();
	virtual ~CFFT();
	bool Init(int nLength);
	bool Release();

	bool FFT();
	bool GetBuffer(float * pOutBuffer, int nBufferUnitNum);
	bool SetBuffer(float * pInBuffer, int nBufferUnitNum);

	bool IFFT();
	inline COMPLEX CMul(COMPLEX x, COMPLEX y)
	{
		COMPLEX res;
		res.a = x.a * y.a - x.b * y.b;
		res.b = x.a * y.b + x.b * y.a;
		return res;
	}
};

#endif // !defined(AFX_FFT_H__AC81A6E0_650B_11D3_88F1_A26870CFF4B8__INCLUDED_)
