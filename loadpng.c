// MIT License
// 
// Copyright (c) 2022 Kjetil Hvalstrand


#include <png.h>

#define __USE_INLINE__

#include <proto/graphics.h>

uint32 argb_reverse(char *ptr)
{
	uint32 ret;
	ret = (ptr[3] << 24) | (ptr[0] << 16) | (ptr[1] << 8) | ptr[2];
	return ret;
}

uint32 rgb_to_argb(char *ptr)
{
	uint32 ret;
	ret = 0xFF000000 | (ptr[0] << 16) | (ptr[1] << 8) | ptr[2];
	return ret;
}

struct BitMap *loadPngImage(char *name, int *outWidth, int *outHeight, BOOL *outHasAlpha)
{
	png_structp png_ptr;
	png_info *info_ptr;
	png_bytepp row_pointers;
	unsigned int sig_read = 0;
	int color_type, interlace_type;
	int srcBytesPerPixel;
	int x,y;
	int dx;
	struct BitMap *bm = NULL;
	uint32 *to_row;
	APTR plock;
	char *Memory;
	ULONG BytesPerRow;
	FILE *fp;

	uint32 (*to_argb) (char *ptr);

	if ((fp = fopen(name, "rb")) == NULL)   return NULL;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

	info_ptr=png_create_info_struct(png_ptr);

	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, NULL, NULL);
		return NULL;
	}

	png_init_io(png_ptr, fp);
	png_set_sig_bytes(png_ptr, sig_read);
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, NULL);

	if (setjmp(png_jmpbuf(png_ptr)))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
		fclose(fp);
		return NULL;
	}

 	*outWidth = png_get_image_width(png_ptr,info_ptr);
	*outHeight =  png_get_image_height(png_ptr,info_ptr);

	color_type = png_get_color_type(png_ptr,info_ptr);

	switch ( color_type )
	{
		case PNG_COLOR_TYPE_RGBA:
			*outHasAlpha = TRUE;
			to_argb = argb_reverse;
			srcBytesPerPixel = 4;
			break;

		case PNG_COLOR_TYPE_RGB:
			*outHasAlpha = FALSE;
			to_argb = rgb_to_argb;
			srcBytesPerPixel = 3;
			break;

		default:

			png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
			fclose(fp);
			return NULL;
	}

	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);

	row_pointers = png_get_rows(png_ptr, info_ptr);

 	if (bm = AllocBitMapTags( *outWidth,*outHeight, 24, 
			BMATags_PixelFormat, PIXF_A8R8G8B8 ,
			BMATags_Displayable, TRUE,
			TAG_END ))
	{
		plock = LockBitMapTags( bm,
				LBM_BaseAddress, &Memory,
				LBM_BytesPerRow, &BytesPerRow,
				TAG_END  );

		if (plock)
		{
			for ( y = 0; y < *outHeight; y++)
			{
				to_row	= (uint32 *) ((char *) Memory + ( y * BytesPerRow ) );

				for (x = 0 ; x < (*outWidth); x++)
				{
					to_row[x] = to_argb( (char *) row_pointers[y]+x*srcBytesPerPixel );
				}
			}

			UnlockBitMap( plock );
		}
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, NULL);
	fclose(fp);
	return bm;
}

