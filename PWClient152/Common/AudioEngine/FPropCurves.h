#pragma once

#include "FExportDecl.h"
#include "FCurve.h"
#include <vector>
#include <string>

class TiXmlElement;
class AFileImage;
class AFile;

namespace AudioEngine
{
	typedef struct _CVPOINT
	{
		float x;
		float y;
	}CVPOINT;

	typedef std::vector<CVPOINT> CVPointArray;
	typedef std::vector<CURVE_TYPE> CurveArray;

	class _EXPORT_DLL_ PropCurves
	{
	public:
		PropCurves(void);
		~PropCurves(void);
	public:
 		void				Init(float fDefaultY, float fMinY, float fMaxY);
		void				SetName(const char* szName) { m_strName = szName; }
		const char*			GetName() const { return m_strName.c_str(); }
		void				SetUnit(const char* szUnit) { m_strUnit = szUnit; }
		const char*			GetUnit() const { return m_strUnit.c_str(); }
		void				SetDefault(float fDefualt);
		float				GetDefault() const;
		float				GetMin() const;
		float				GetMax() const;
 		bool				AddPoint(CVPOINT pt);
 		bool				DeletePoint(int idx);
 		void				SetPoint(int idx, CVPOINT pt);
 		float				GetValue(float x) const;
 		int					GetCurveNum() const;
		void				SetCurve(int idx, CURVE_TYPE type);
 		CURVE_TYPE			GetCurveByIndex(int idx) const;
 		int					GetPointNum() const;
 		CVPOINT				GetPointByIndex(int idx) const;
		void				MoveWholeCurve(float delta);
		void				MoveCurve(int idx, float delta);
		bool				Load(AFileImage* pFile);
		bool				Save(AFile* pFile);
		bool				LoadXML(TiXmlElement* root);
		bool				SaveXML(TiXmlElement* pParent);
	protected:
		void				release();
	protected:
		float				m_fDefaultY;
		float				m_fMinY;
		float				m_fMaxY;
		CVPointArray			m_arrPoint;
		CurveArray			m_arrCurve;
		std::string			m_strName;
		std::string			m_strUnit;
	};
}
