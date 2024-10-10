#ifndef _FT_CACHE
#define _FT_CACHE

#include <ft2build.h>
#include FT_FREETYPE_H 
#include <freetype/ftcache.h>

// if file_length == 0, file_path is valid, else file_data is valid
typedef struct
{
	union
	{
		char file_path[MAX_PATH];
		void* file_data;
	};
	int file_length;
	int face_index;
	int font_style;
} My_FaceID_Rec, *My_FaceID;

extern "C"
FT_Error MyFTCFaceRequest(FTC_FaceID  face_id,
						  FT_Library  library,
						  FT_Pointer  request_data,
						  FT_Face*    aface );


#endif
