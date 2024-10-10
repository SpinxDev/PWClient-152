#ifndef _FWGLUTESS_H_
#define _FWGLUTESS_H_

#include <wtypes.h>
#include <wingdi.h>
#include <gl/gl.h>
#include <gl/glu.h>

template<class T>
class APtrList;
class CGLUPolygon;

typedef APtrList<GLdouble *> AGLUVertexList;

class CGLUTess  
{
	APtrList<GLdouble *> * m_pListVertices;
	APtrList<GLdouble *> * m_pListCombinedVertices;
	
	// for tess operation
	GLUtesselator * m_pTessObject;
	bool m_bTessSuccess;

public:
	const APtrList<GLdouble *> * GetVertexList() const { return m_pListVertices; }
	APtrList<GLdouble *> * GetVertexList() { return m_pListVertices; }
	void Destroy();
	bool CreateFromGLUPolygon(const CGLUPolygon *pGLUPolygon);
	CGLUTess();
	virtual ~CGLUTess();

protected:
	bool InitGLUTess();
private:
	void UninitGLUTess();
	static void CALLBACK EdgeFlagDataCallback(GLboolean flag, void * polygon_data);
	static void CALLBACK CombineDataCallback(GLdouble coords[3], void * vertex_data[4], GLfloat weight[4], void **outData, void * polygon_data);
	static void CALLBACK VertexDataCallback(void * vertex_data, void * polygon_data);
	static void CALLBACK EndDataCallback(void * polygon_data);
	static void CALLBACK BeginDataCallback(GLenum type, void * polygon_data);
	static void CALLBACK TessErrorDataCallback(GLenum errno, void * polygon_data);
};

#endif 