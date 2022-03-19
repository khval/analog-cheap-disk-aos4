// MIT License
// 
// Copyright (c) 2022 Kjetil Hvalstrand

#define __USE_INLINE__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <proto/exec.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>

#include "comp.h"

void set_target_hookData( struct BitMap *bitmap, int bm_w,int bm_h,struct Window *win, struct Rectangle *rect, CompositeHookData *hookData )
{
	rect -> MinX = win->BorderLeft;
 	rect -> MinY = win->BorderTop;
 	rect -> MaxX = win->Width - win->BorderRight - 1;
 	rect -> MaxY = win->Height - win->BorderBottom - 1;

 	float destWidth = rect->MaxX - rect->MinX + 1;
 	float destHeight = rect->MaxY - rect->MinY + 1;
 	float scaleX = (destWidth + 0.5f) / bm_w;
 	float scaleY = (destHeight + 0.5f) / bm_h;

	hookData->srcBitMap = bitmap;
	hookData->srcWidth = bm_w;
	hookData->srcHeight = bm_h;
	hookData->offsetX = win->BorderLeft;
	hookData->offsetY = win->BorderTop;
	hookData->scaleX = COMP_FLOAT_TO_FIX(scaleX);
	hookData->scaleY = COMP_FLOAT_TO_FIX(scaleY);
	hookData->retCode = COMPERR_Success;
}

static ULONG compositeHookFunc(	struct Hook *hook, struct RastPort *rastPort, struct BackFillMessage *msg)
 {

	CompositeHookData *hookData = (CompositeHookData*)hook->h_Data;

	hookData->retCode = CompositeTags(
		COMPOSITE_Src, 
			hookData->srcBitMap, 
			rastPort->BitMap,
		COMPTAG_SrcWidth,   hookData->srcWidth,
		COMPTAG_SrcHeight,  hookData->srcHeight,
		COMPTAG_ScaleX, 	hookData->scaleX,
		COMPTAG_ScaleY, 	hookData->scaleY,
		COMPTAG_OffsetX,    msg->Bounds.MinX - (msg->OffsetX - hookData->offsetX),
		COMPTAG_OffsetY,    msg->Bounds.MinY - (msg->OffsetY - hookData->offsetY),
		COMPTAG_DestX,      msg->Bounds.MinX,
		COMPTAG_DestY,      msg->Bounds.MinY,
		COMPTAG_DestWidth,  msg->Bounds.MaxX - msg->Bounds.MinX + 1,
		COMPTAG_DestHeight, msg->Bounds.MaxY - msg->Bounds.MinY + 1,
		COMPTAG_Flags,      COMPFLAG_SrcFilter | COMPFLAG_IgnoreDestAlpha | COMPFLAG_HardwareOnly,
		TAG_END);

	return 0;
}

void draw_comp( struct BitMap *bm, int bm_w, int bm_h , struct Window *win )
{
	struct Hook hook;
	CompositeHookData hookData;
	struct Rectangle rect;
	struct BackFillMessage bmsg;

	if (bm)
	{
		set_target_hookData( bm, bm_w, bm_h, win, &rect, &hookData );
		hook.h_Entry = (HOOKFUNC) compositeHookFunc;
		hook.h_Data = &hookData;

		LockLayer( 0, win -> RPort -> Layer );
		DoHookClipRects(&hook, win->RPort, &rect);
		UnlockLayer( win -> RPort -> Layer );
	}
}


void composite( 
	struct BitMap *source_bm, int source_x,int source_y,int source_w,int source_h,
	struct BitMap *dest_BitMap,int dx, int dy, int dw, int dh )
{
	CompositeTags( COMPOSITE_Src_Over_Dest, source_bm, dest_BitMap,
			COMPTAG_SrcX, source_x,
			COMPTAG_SrcY, source_y,
			COMPTAG_SrcWidth, source_w,
			COMPTAG_SrcHeight, source_h, 
			COMPTAG_OffsetX, dx,
			COMPTAG_OffsetY, dy,
			COMPTAG_DestX, dx,
			COMPTAG_DestY, dy,
			COMPTAG_DestWidth, dw,
			COMPTAG_DestHeight, dh,
			COMPTAG_ScaleX, COMP_FLOAT_TO_FIX((float) (dw+1) / (float) source_w) ,
			COMPTAG_ScaleY, COMP_FLOAT_TO_FIX((float) (dh+1) / (float) source_h) ,
			TAG_END	 );
}



#if 0

void draw_gfx()
{
	if (bm)
	{
		BltBitMapRastPort(bm, bm_w, bm_h,
				win -> RPort, 0, 0, bm_w, bm_h,
				0x0C0);
	}
}

#endif

