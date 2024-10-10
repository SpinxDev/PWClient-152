#pragma once

#include "FAudioEngine.h"
#include "FExportDecl.h"
#include <string>

namespace FMOD
{
	class Reverb;
	class Geometry;
}

namespace AudioEngine
{
	class EventSystem;

	enum
	{
		REVERB_SHAPE_NONE,
		REVERB_SHAPE_CUBIOD,
		REVERB_SHAPE_SPHERE,
		REVERB_SHAPE_POLYGON,
	};

	class _EXPORT_DLL_ Reverb
	{
	public:
		Reverb(void);
		~Reverb(void);
	public:
		bool						Init(EventSystem* pEventSystem);
		void						GetProperty(REVERB_PROPERTY& prop) const;
		bool						SetProperty(REVERB_PROPERTY prop);
		bool						SetActive(bool bActive);
		bool						Set3DAttributes(const VECTOR& pos, float fMinDistance, float fMaxDistance);
		bool						SetShapeSphere(const VECTOR& center, float radius);
		bool						SetShapeCubiod(const VECTOR& center, float length, float width, float hight);
		bool						SetShapePolygon(VECTOR* pPoints, int iPtNum, float fBottom, float fTop);
		const char*					GetName() const;
		void						SetName(const char* szName);
	protected:
		void						release();
	protected:
		FMOD::Reverb*				m_pReverb;
		FMOD::Geometry*				m_pGeometry;
		EventSystem*				m_pEventSystem;
		REVERB_PROPERTY				m_ReverbProperty;
		std::string					m_strName;
		int							m_iShape;
	};
}
