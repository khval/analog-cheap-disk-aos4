// MIT License
// 
// Copyright (c) 2022 Kjetil Hvalstrand

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>


#define __USE_INLINE__

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/graphics.h>
#include <proto/intuition.h>
#include <proto/datatypes.h>

#include <datatypes/pictureclass.h>

#define xAspect bmh_XAspect
#define yAspect bmh_YAspect

extern int scaleX;
extern int scaleY;

#include "iff.h"

union argb palette[256];

extern int bitplaines;

void SetColorCR(int c,struct ColorRegister *cr)
{
	palette[c].a = 255;
	palette[c].r = cr -> red;
	palette[c].g = cr -> green;
	palette[c].b = cr -> blue;
}

#define only_info 0

void setPixel( struct BitMap *bitmap, int x, int y, char color )
{
	struct RastPort rp;
	InitRastPort(&rp);
	rp.BitMap = bitmap;

	 WritePixelColor(&rp,x,y,palette[color].data);
}

struct BitMap *load_iff( const char *name, BOOL alpha )
{
	struct DataType *dto = NULL;
	struct BitMapHeader *bm_header;
	struct BitMap *dt_bitmap;
	struct ColorRegister *cr;
	ULONG modeid = 0; 
	ULONG colors;
	ULONG depth;
	ULONG bformat;
	ULONG mode;
	struct BitMap *ret_bm = NULL;

	// new screen if screen number is set, or if current screen is not open.

	if(dto = (struct DataType *) NewDTObject( name, DTA_GroupID, GID_PICTURE, TAG_DONE))
	{
		SetDTAttrs ( (Object*) dto, NULL,NULL,	PDTA_DestMode, (ULONG) PMODE_V43,TAG_DONE);
		DoDTMethod ( (Object*) dto,NULL,NULL,DTM_PROCLAYOUT,NULL,TRUE); 
		GetDTAttrs ( (Object*) dto,PDTA_BitMapHeader, (ULONG *) &bm_header, 	
					PDTA_BitMap, (ULONG) &dt_bitmap, 
					PDTA_ColorRegisters, &cr,
					PDTA_NumColors, &colors,
					PDTA_ModeID, &modeid,
					TAG_DONE);

		bformat = GetBitMapAttr(dt_bitmap,BMA_PIXELFORMAT);

		depth = GetBitMapAttr(dt_bitmap,BMA_DEPTH);
	
		{
			struct RastPort rp;
			int c,x,y;
			InitRastPort(&rp);
			rp.BitMap = dt_bitmap;

			if ((bformat==PIXF_NONE) || (bformat==PIXF_CLUT))
			{
				ret_bm = AllocBitMapTags( bm_header -> bmh_Width,bm_header -> bmh_Height, 24, BMATags_PixelFormat, PIXF_A8R8G8B8 , BMATags_Displayable, TRUE, TAG_END );

				if (ret_bm)
				{
					if (cr) for (c=0;c<colors;c++) SetColorCR(c,cr + c);
					if (alpha) palette[0].data = 0x00000000;


					for (y=0;y<bm_header -> bmh_Height;y++)
					{
						for (x=0;x<bm_header -> bmh_Width;x++) setPixel( ret_bm, x,y, ReadPixel(&rp,x,y));
					}
				}
			}
		}

		DisposeDTObject((Object*) dto);
	}

	return ret_bm;
}

struct c8bit *alloc_c8bit(int w,int h)
{
	struct c8bit *ret;

	ret = (struct c8bit *) malloc(sizeof(struct c8bit));
	if (ret)
	{
		ret -> w = w;
		ret -> h = h;
		ret -> data = malloc( w * h );
		if (ret -> data) return ret;
		free(ret);
	}
	return NULL;
}

void free_c8bit( struct c8bit *c8bit )
{
	free(c8bit);
}

void c8bit_setPixel( struct c8bit *c8bit, int x,int y, char c )
{
	c8bit -> data[ c8bit -> w * y + x ] = c;
}

char c8bit_getPixel( struct c8bit *c8bit, int x,int y )
{
	return c8bit -> data[ c8bit -> w * y + x ];
}

struct c8bit *load_iff_8bit( const char *name, BOOL alpha )
{
	struct DataType *dto = NULL;
	struct BitMapHeader *bm_header;
	struct BitMap *dt_bitmap;
	struct ColorRegister *cr;
	ULONG modeid = 0; 
	ULONG colors;
	ULONG depth;
	ULONG bformat;
	ULONG mode;
	struct c8bit *ret_bm = NULL;

	// new screen if screen number is set, or if current screen is not open.

	if(dto = (struct DataType *) NewDTObject( name, DTA_GroupID, GID_PICTURE, TAG_DONE))
	{
		SetDTAttrs ( (Object*) dto, NULL,NULL,	PDTA_DestMode, (ULONG) PMODE_V43,TAG_DONE);
		DoDTMethod ( (Object*) dto,NULL,NULL,DTM_PROCLAYOUT,NULL,TRUE); 
		GetDTAttrs ( (Object*) dto,PDTA_BitMapHeader, (ULONG *) &bm_header, 	
					PDTA_BitMap, (ULONG) &dt_bitmap, 
					PDTA_ColorRegisters, &cr,
					PDTA_NumColors, &colors,
					PDTA_ModeID, &modeid,
					TAG_DONE);

		bformat = GetBitMapAttr(dt_bitmap,BMA_PIXELFORMAT);

		depth = GetBitMapAttr(dt_bitmap,BMA_DEPTH);
	
		{
			struct RastPort rp;
			int c,x,y;
			InitRastPort(&rp);
			rp.BitMap = dt_bitmap;

			if ((bformat==PIXF_NONE) || (bformat==PIXF_CLUT))
			{
				ret_bm = alloc_c8bit( bm_header -> bmh_Width,bm_header -> bmh_Height );

				if (ret_bm)
				{
					if (cr) for (c=0;c<colors;c++) SetColorCR(c,cr + c);

					for (y=0;y<bm_header -> bmh_Height;y++)
					{
						for (x=0;x<bm_header -> bmh_Width;x++) c8bit_setPixel( ret_bm, x,y, ReadPixel(&rp,x,y));
					}
				}
			}
		}

		DisposeDTObject((Object*) dto);
	}

	return ret_bm;
}

