#ifndef _FT_MY_RASTER
#define _FT_MY_RASTER

#include <ft2build.h>
#include FT_FREETYPE_H 
#include <freetype/ftoutln.h>

extern "C"
void my_span_outline(  int       y,
					   int       count,
					   FT_Span*  spans,
					   void*     user );


#endif
