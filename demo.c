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
#include <proto/ptreplay.h>

#include "init.h"
#include "EngineTimer.h"
#include "comp.h"
#include "iff.h"

struct Window *win = NULL;

struct TimerContext tc;

struct BitMap *loadPngImage(char *name, int *outWidth, int *outHeight, BOOL *outHasAlpha);

bool open_window();
void close_window();
void draw_comp();

bool quit = false;

struct BitMap *abc_bm;
struct BitMap *scroll_bm;
struct BitMap *present_bm;
struct BitMap *title_bm;
struct BitMap *bobble_bm;

struct c8bit *menu_8bit;

int bobble_w, bobble_h;

struct BitMap *disp_bm;
int disp_w, disp_h;

struct BitMap *bm;
int bm_w, bm_h;

BOOL bm_alpha;

char *PTtexte;
extern char *Texte;
extern char *abc;

char lookup[256];

void draw_frame();

struct BitMap *LoadIff( const char *name );

int mx, my;

extern int selY;
extern int mod_status;

extern void intro_render1();
extern void intro_render2();
extern void intro_mouseClick( int Code );
extern void main_render();
extern void main_mouseClick( int Code );

extern void reset_scroll();

struct part parts[]=
{
	{intro_render1,intro_mouseClick},
	{intro_render2,intro_mouseClick},
	{main_render,main_mouseClick},
	{NULL, NULL }
};

struct part *part;

char *modfiles[] = 
	{
		"mod.status",
		"mod.adel",
		"mod.compote",
		"mod.banzai",
		"mod.milk",
		"mod.jambon",
		"mod.atom",
		"mod.computer",
		"mod.track",
		NULL
	};

struct Module *Module1 = NULL;

void load_mod(char *name)
{
	char namepath[100];

	if (Module1) PTStop(Module1);
	if (Module1) PTUnloadModule( Module1 );

	sprintf(namepath,"media/%s",name);

	Module1 = PTLoadModule(namepath);

	if (! Module1 )
	{
		printf("Failed to load module %s\n",namepath);
	}

	if (Module1) PTPlay(Module1);
}

void demo()
{
	int ww,wh;
	ULONG win_mask;
	ULONG sig;
	struct IntuiMessage *msg;

	win_mask = win -> UserPort ? 1 << win -> UserPort ->mp_SigBit : 0;
	do
	{
		sig = Wait( win_mask | tc.timer_mask | SIGBREAKF_CTRL_C );

		if (sig & tc.timer_mask)
		{
			WaitTOF();
			reset_timer( tc.timer_io );
			draw_comp( disp_bm, disp_w, disp_h, win );
			part -> render();

/*
			if (Module1)
			{
				int pos = PTSongPos(Module1);
				int pat = PTSongPattern(Module1, pos);			

				char *ptr = PTPatternData(Module1, pat , PTPatternPos(Module1) );

//				printf("Song pos: %d, pattern %d, pattern pos %d\n", pos, pat, PTPatternPos(Module1) );
			}
*/
		}

		if (sig & win_mask)
		{
			while (msg = (struct IntuiMessage *) GetMsg( win -> UserPort ))
			{
				switch (msg -> Class)
				{
					case IDCMP_MOUSEMOVE:
						ww = win -> Width - win -> BorderLeft - win -> BorderRight;
						wh = win -> Height - win -> BorderTop - win -> BorderBottom;
						mx = disp_w * (msg -> MouseX - win -> BorderLeft) / ww;
						my = disp_h * (msg -> MouseY - win -> BorderTop) / wh;
						break;

					case IDCMP_MOUSEBUTTONS:
						part -> mouseClick( msg -> Code );
						break;

					case IDCMP_RAWKEY:
						if (msg -> Code == 69) quit = true;
						break;

					case IDCMP_CLOSEWINDOW:
						quit = true;
						break;

					case IDCMP_GADGETUP:
						break;

					case IDCMP_ACTIVEWINDOW:
						break;

					case IDCMP_INACTIVEWINDOW:
						break;
				}

				ReplyMsg( (struct Message *) msg );
			}
		}
		
	} while ( !quit );
}

struct bobble bobbles[10];

void reset_bobble( struct bobble *b)
{
	b -> x = rand() % 85 + 30;
	b -> y = 480;
}

void init_bobbles()
{
	struct bobble tmp;
	int n,nn; 

	for (n = 0; n< 10; n++)
	{
		bobbles[n].x = rand() % 85 + 30;
		bobbles[n].y = rand() % 480;
		bobbles[n].speed = (rand() % 4) + 1;
		bobbles[n].f = (rand() % 5) +1;
	}

	for (n = 0; n< 10; n++)
	{
		for (nn = 1; nn< 10; nn++)
		{
			if (bobbles[nn-1].speed > bobbles[nn].speed)
			{
				tmp = bobbles[nn-1];
				bobbles[nn-1] = bobbles[nn];
				bobbles[nn] = tmp;
			}
		}
	}
}

int y= 480;

char lookup[256];

extern struct BitMap *load_iff( const char *name, BOOL alpha );

struct filesgfx
{
	void **ptr_gfx;
	void *(*fn) (const char *, BOOL opt);
	const char *name;
	BOOL opt;
};

const char *failed_to_load = "failed to load '%s'\n";

const char *bobble_name = "media/boble-transparent.png";
const char *bm_name = "media/CheapDisk-analog-logo-clean.png";

bool load_media()
{
	struct filesgfx *fgfx;
	struct filesgfx filesgfx[]=
		{
			{(void**)&menu_8bit, (void *(*) (const char *, BOOL)) load_iff_8bit,"media/IFF.Fond", FALSE},
			{(void**)&present_bm, (void *(*) (const char *, BOOL)) load_iff,"media/IFF.Present", TRUE},
			{(void**)&title_bm,(void *(*) (const char *, BOOL)) load_iff,"media/IFF.Nounours", TRUE},
			{(void**)&abc_bm,(void *(*) (const char *, BOOL)) load_iff,"media/IFF.Fonte8", TRUE},
			{NULL,NULL,NULL, TRUE}
		};
	
	for ( fgfx=filesgfx ; fgfx -> name ; fgfx ++ )
	{
		*(fgfx -> ptr_gfx) = fgfx -> fn( fgfx -> name, fgfx -> opt );
		if ( ! *(fgfx -> ptr_gfx) )
		{
			printf(failed_to_load,fgfx -> name);
			return false;
		}
	}

	bobble_bm = loadPngImage( bobble_name, &bobble_w, &bobble_h, &bm_alpha );
	bm = loadPngImage( bm_name, &bm_w, &bm_h, &bm_alpha );

	if (!bobble_bm)
	{
		printf(failed_to_load, bobble_name );
		return false;
	}

	if (!bm)
	{
		printf(failed_to_load, bm_name );
		return false;
	}

	scroll_bm = AllocBitMapTags( 640,100, 24, BMATags_PixelFormat, PIXF_A8R8G8B8 , BMATags_Displayable, TRUE, TAG_END );
	if (!scroll_bm) return false;

	disp_w = 640;
	disp_h = 480;
	disp_bm = AllocBitMapTags( disp_w,disp_h, 24, BMATags_PixelFormat, PIXF_A8R8G8B8 , BMATags_Displayable, TRUE, TAG_END );
	if (!disp_bm) return false;

	return true;
}

#define safefree(fn,data) if (data) fn(data); data=NULL;

void unload_media()
{
	safefree(free_c8bit,menu_8bit);
	safefree(FreeBitMap,present_bm);
	safefree(FreeBitMap,title_bm);
	safefree(FreeBitMap,abc_bm);
	safefree(FreeBitMap,scroll_bm);
	safefree(FreeBitMap,disp_bm);
	safefree(FreeBitMap,bobble_bm);
	safefree(FreeBitMap,bm);
	safefree(PTUnloadModule,Module1);
}

void init_lookup();

int main()
{


	if ( ! startup() )
	{
		shutdown();
		return 0;
	}

	PTtexte = Texte;
	init_lookup();

	if (load_media())
	{
		struct RastPort rp;
		InitRastPort(&rp);

		reset_scroll();

		rp.BitMap = disp_bm;
		RectFillColor( &rp, 0, 0, 640, 480, 0xFF000000 );

		if (open_window() && open_timer_context( &tc ))
		{
			init_bobbles();

			part = parts + 0;

			draw_comp( disp_bm, disp_w, disp_h, win );

			selY = 124 + 0;
			load_mod(modfiles[0]);
			mod_status = is_playing;

			demo();
		}
	}

	unload_media();

	close_timer_context( &tc );
	close_window();

	shutdown();
	return 0;
}


bool open_window()
{
	win = OpenWindowTags( NULL, 
			WA_Left,60,

			WA_InnerWidth, 640,
			WA_InnerHeight, 480,
			WA_DragBar, TRUE,
			WA_CloseGadget, TRUE,

			WA_MinWidth,	320,
		 	WA_MinHeight,	200,	

			WA_MaxWidth,~0,
			WA_MaxHeight,	~0,

			WA_IDCMP, 
				IDCMP_GADGETUP |
				IDCMP_CLOSEWINDOW |
				IDCMP_MOUSEBUTTONS | 
				IDCMP_MOUSEMOVE |
				IDCMP_INTUITICKS |
				IDCMP_RAWKEY |
//				IDCMP_VANILLAKEY |
				IDCMP_GADGETUP|
				IDCMP_ACTIVEWINDOW |
				IDCMP_INACTIVEWINDOW
			,
			WA_Flags, WFLG_ACTIVATE,
			WA_RMBTrap, true,
			WA_ReportMouse, true,
			WA_DragBar, true,
			WA_DepthGadget, true,
			WA_SizeGadget, TRUE,
			WA_SizeBBottom, TRUE,
			TAG_END);

	return win ? true : false;
}

void close_window()
{
	if (win) CloseWindow(win);
	win = NULL;
}

char *abc = "abcdefghijklmnopqrstuvwxyz0123456789.,'!:-?>";

void init_lookup()
{
	int i;
	char d[2] = {0,0};
	char *p;

	for (i=0;i<256;i++) lookup[i]=255;

	for (p=abc;*p;p++)
	{
		i = p- abc;

		if ((*p>='a')&&(*p<='z'))
		{
			lookup[*p] = i;
			lookup[*p-'a'+'A'] = i;
		}
		else
		{
			lookup[*p] = i;
		}
	}
}

