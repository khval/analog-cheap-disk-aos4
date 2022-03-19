// MIT License
// 
// Copyright (c) 2022 Kjetil Hvalstrand


#define __USE_INLINE__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include "init.h"
#include "EngineTimer.h"
#include "comp.h"

extern struct BitMap *present_bm;
extern struct BitMap *title_bm;
extern struct BitMap *disp_bm;

static int t = 0;

show = 0;

extern struct part *part;

void intro_render1()
{
//	part +=2;

	t += 1;

	if (t>30)
	{
		struct RastPort rp;
		InitRastPort(&rp);	

		rp.BitMap = disp_bm;
		RectFillColor( &rp, 0, 0, 640, 480, 0xF000000 );

		show ++;

		if (show<4)
		{
			 composite( present_bm,0,80,640,30*show, disp_bm,70,100,640,60*show-1);
		}
		else part ++;

		t = 0;
	}
}

static int y = 480;

void intro_render2()
{
	struct RastPort rp;
	InitRastPort(&rp);	

	rp.BitMap = disp_bm;
	RectFillColor( &rp, 0, 0, 640, 480, 0xF000000 );

	if (y>-1)
	{
		composite( title_bm,0,0,200,250, disp_bm,275, y,400,250*2);
	}
	else
	{
		composite( title_bm,0,y < 0 ? (-y+1)/2 : 0,200,250, disp_bm,275, y&1,400,250*2);
	}

	y--;

	if (y<-365) part++;

}

extern struct part parts[];

void intro_mouseClick(  int Code )
{
	if ( Code & IECODE_UP_PREFIX )
	{
		part = parts + 2;
	}
}

