#include "stdafx.h"
#include "MyRaster.h"
#include "ftglyph.h"

struct raster_user_data
{
	int outline_width;
	FT_BitmapGlyph glyph;
};


#define MAX_WHITE_COLOR	0

extern "C"
void my_span_outline(int       y,
					 int       count,
					 FT_Span*  spans,
					 void*     user )
{
	int i;
	raster_user_data *data = (raster_user_data *)user;
	FT_Bitmap bitmap = data->glyph->bitmap;
	unsigned char *line = bitmap.buffer + (data->glyph->bitmap.rows - y - 1) * bitmap.pitch; // current line of buffer

	for (i = 0; i < count; i++)
	{
		if (spans->coverage > MAX_WHITE_COLOR)
		{
			line[spans->x] = 255;
			line[spans->x + spans->len - 1] = 255; 
		}
		spans++;
	}
}
