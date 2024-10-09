#ifndef _CHARCONTOUR_H_
#define _CHARCONTOUR_H_

class CCharVerticesBuffer;
class CCharCurve;
template<class T>
class APtrList;
class A3DVECTOR3;
struct tagTTPOLYGONHEADER;
typedef struct tagTTPOLYGONHEADER TTPOLYGONHEADER;
typedef unsigned char BYTE;

// convert windows provided char glyph info to 
// a contour in CCharVerticesBuffer format, using
// CCharCurve as its helper.
class CCharContour  
{
	bool m_bClockwise;
	CCharVerticesBuffer * m_pCharVerticesBuffer;
public:
	void Simplify(float fSimplifyFactor, float fMinLenPerCurve);
	void ReversVertices();
	void HideDuplicatedVertex();
	void ComputeClockwise();
	bool CreateFromFont(TTPOLYGONHEADER *pPolyHeader, BYTE *& pPos);
	void Destroy();
	
	CCharContour();
	~CCharContour();

	const CCharVerticesBuffer * GetCharVerticesBuffer() const { return m_pCharVerticesBuffer; }
	CCharVerticesBuffer * GetCharVerticesBuffer() { return m_pCharVerticesBuffer; }
	bool IsClockwise() const { return m_bClockwise; }
	bool IsAntiClockwise() const { return !IsClockwise(); }
	bool BuildVerticesBufferFromCurveList(APtrList<CCharCurve *> *pListCurve);
protected:
	void ConvertFromCurveListToVerticesBufferList(APtrList<CCharVerticesBuffer *> *pListVerticesBuffer, APtrList<CCharCurve *> *pListCurve);
	void ClearCurveList(APtrList<CCharCurve *> *pListCurve, bool bDeleteList);
	float GetCosAngle(const A3DVECTOR3& v1, const A3DVECTOR3 &v2);
	bool BuildCurveListFromFont(TTPOLYGONHEADER *pPolyHeader, BYTE *&pPos, APtrList<CCharCurve *> *pListCurve);
};

#endif

