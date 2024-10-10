#include "stdafx.h"
#include "MyFTCache.h"


extern "C"
FT_Error MyFTCFaceRequest(FTC_FaceID  face_id,
						  FT_Library  library,
						  FT_Pointer  request_data,
						  FT_Face*    aface )
{
	My_FaceID myFaceID = (My_FaceID) face_id;

	FT_Error error = 0;
	if (myFaceID->file_length == 0)
	{
		error = FT_New_Face(
			library, 
			myFaceID->file_path, 
			myFaceID->face_index, 
			aface);
	}
	else
	{
		error = FT_New_Memory_Face(
			library, 
			(const FT_Byte *) myFaceID->file_data, 
			myFaceID->file_length,
			myFaceID->face_index,
			aface);
	}

	if (!error)
		(*aface)->generic.data = (void *)myFaceID->font_style;
	return error;
}


