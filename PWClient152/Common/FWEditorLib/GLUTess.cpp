// GLUTess.cpp: implementation of the CGLUTess class.
//
//////////////////////////////////////////////////////////////////////

#include "GLUTess.h"
#include "GLUContour.h"
#include "GLUPolygon.h"
#include "CodeTemplate.h"


#define new A_DEBUG_NEW




CGLUTess::CGLUTess()
{
	m_pListVertices = new APtrList<GLdouble *>;
	m_pListCombinedVertices = new APtrList<GLdouble *>;
	m_pTessObject = NULL;
	m_bTessSuccess = true;
}

CGLUTess::~CGLUTess()
{
	Destroy();
	SAFE_DELETE(m_pListVertices);
	SAFE_DELETE(m_pListCombinedVertices);
}

bool CGLUTess::CreateFromGLUPolygon(const CGLUPolygon *pGLUPolygon)
{
	ASSERT(pGLUPolygon);

	if (pGLUPolygon->GetGLUContourList()->GetCount() == 0)
		return true;

	if (!InitGLUTess())
		return false;

	m_bTessSuccess = true;

	// begin polygon
	gluTessBeginPolygon(m_pTessObject, this);
	
	// for each contour
	ALISTPOSITION pos = pGLUPolygon->GetGLUContourList()->GetHeadPosition();
	while (pos)
	{
		CGLUContour *pGLUContour = pGLUPolygon->GetGLUContourList()->GetNext(pos);

		gluTessBeginContour(m_pTessObject);
		
		for (int i = 0; i < pGLUContour->GetVerticesCount(); i++)
		{
			gluTessVertex(m_pTessObject, pGLUContour->m_paGLUVertices[i], pGLUContour->m_paGLUVertices[i]);
		}
		
		gluTessEndContour(m_pTessObject);
	}

	try
	{
		// end polygon
		gluTessEndPolygon(m_pTessObject);
	}
	catch(...)
	{
		m_bTessSuccess = false;
	}

	UninitGLUTess();

	if (!m_bTessSuccess)
	{
		Destroy();
		return false;
	}

	return true;
}

void CGLUTess::Destroy()
{
	// test
//	ALISTPOSITION pos = m_pListVertices->GetHeadPosition();
//	while (pos)
//	{
//		GLdouble *p = m_pListVertices->GetNext(pos);
//		SAFE_DELETE_ARRAY(p);
//	}
	// end test
	m_pListVertices->RemoveAll();

	ALISTPOSITION pos = m_pListCombinedVertices->GetHeadPosition();
	while (pos)
	{
		GLdouble *p = m_pListCombinedVertices->GetNext(pos);
		SAFE_DELETE_ARRAY(p);
	}
	m_pListCombinedVertices->RemoveAll();

}

bool CGLUTess::InitGLUTess()
{
	ASSERT(NULL == m_pTessObject);

	if (!(m_pTessObject = gluNewTess()))
		return false;

	gluTessCallback(m_pTessObject, GLU_TESS_ERROR_DATA, (void (CALLBACK *)())TessErrorDataCallback);
	gluTessCallback(m_pTessObject, GLU_TESS_BEGIN_DATA, (void (CALLBACK *)())BeginDataCallback);
	gluTessCallback(m_pTessObject, GLU_TESS_END_DATA , (void (CALLBACK *)())EndDataCallback);
	gluTessCallback(m_pTessObject, GLU_TESS_VERTEX_DATA, (void (CALLBACK *)())VertexDataCallback);
	gluTessCallback(m_pTessObject, GLU_TESS_COMBINE_DATA, (void (CALLBACK *)())CombineDataCallback);
	gluTessCallback(m_pTessObject, GLU_TESS_EDGE_FLAG_DATA, (void (CALLBACK *)())EdgeFlagDataCallback);

	gluTessProperty(m_pTessObject, GLU_TESS_WINDING_RULE , GLU_TESS_WINDING_ODD);
	gluTessProperty(m_pTessObject, GLU_TESS_BOUNDARY_ONLY, false);
	gluTessProperty(m_pTessObject, GLU_TESS_TOLERANCE, true);

	return true;
}

void CGLUTess::UninitGLUTess()
{
	gluDeleteTess(m_pTessObject);
	m_pTessObject = NULL;
}

//////////////////////////////////////////////////////////////////////////
// static callback functions 

void CALLBACK CGLUTess::TessErrorDataCallback(GLenum errno, void *polygon_data)
{
	ASSERT(polygon_data);

	CGLUTess *pThis = (CGLUTess *)polygon_data;

	pThis->m_bTessSuccess = false;
}

void CALLBACK CGLUTess::BeginDataCallback(GLenum type, void *polygon_data)
{
	ASSERT(polygon_data);

	CGLUTess *pThis = (CGLUTess *)polygon_data;
	
}

void CGLUTess::EndDataCallback(void *polygon_data)
{
	ASSERT(polygon_data);

	CGLUTess *pThis = (CGLUTess *)polygon_data;

}

void CALLBACK CGLUTess::VertexDataCallback(void *vertex_data, void *polygon_data)
{
	ASSERT(polygon_data);

	CGLUTess *pThis = (CGLUTess *)polygon_data;

	pThis->m_pListVertices->AddTail((GLdouble *) vertex_data);
}

void CALLBACK CGLUTess::CombineDataCallback(GLdouble coords[], void *vertex_data[], GLfloat weight[], void **outData, void * polygon_data)
{
	GLdouble *pNewVertex = new GLdouble[3];
	memcpy(pNewVertex, coords, sizeof(GLdouble) * 3);
	*outData = pNewVertex;
	
	CGLUTess *pThis = (CGLUTess *)polygon_data;
	pThis->m_pListCombinedVertices->AddTail(pNewVertex);
}

void CALLBACK CGLUTess::EdgeFlagDataCallback(GLboolean flag, void *polygon_data)
{
	ASSERT(polygon_data);

	CGLUTess *pThis = (CGLUTess *)polygon_data;

}

