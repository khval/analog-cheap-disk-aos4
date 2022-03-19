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

extern int mx,my;
extern struct BitMap *bm;
extern struct BitMap *disp_bm;
extern struct BitMap *abc_bm;
extern struct BitMap *scroll_bm;
extern struct BitMap *bobble_bm;
extern int bobble_w, bobble_h;

extern struct c8bit *menu_8bit;

extern char *Texte,*PTtexte ;
extern char *Texteinfo;

extern char lookup[256];
extern char *modfiles[];
extern struct bobble bobbles[10];

extern struct Module *Module1;

static int t = 0;
static int sf = 0;

int mod_status = is_stopped;

void reset_scroll()
{

	struct RastPort rp;
	InitRastPort(&rp);
	rp.BitMap = scroll_bm;

	if (scroll_bm) RectFillColor( &rp, 0, 0, 640, 100, 0xFF000077 );
}

void scroll(struct BitMap *bm,int s)
{
	composite( bm,s,50*sf,640-s,50, bm,0,50*(1-sf),640-s-1,49 );
	sf = 1- sf;
}

char pixels[320*200];

void sp(int x,int y,char c)
{
	pixels[320*y+x] = c;
}


uint32 pal[]={0xff000000,0xfff0f0f0,0xffff0000,0xffc0c0f0,0xffb0b0f0,0xff90a0f0,0xff8080f0,0xff6070f0,0xff5050d0,0xff4040c0,0xff3030a0,0xff202090,0xff101070,0xff101060,0xff000040,0xff000030,0xfff0f0c0,0xffe0e0a0,0xffd0d090,0xffc0b080,0xffb0a070,0xffb09060,0xffa08050,0xff906040,0xff805030,0xff704020,0xff603020,0xff502010,0xff501010,0xff401000,0xff300000,0xff200000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000,0x00000000};

int selY = 0;

void c2t()
{

	int x,y;
	uint32 bpr;
	char *base;

	APTR lock = LockBitMapTags(bm,
		LBM_BytesPerRow, &bpr,
		LBM_BaseAddress, &base,
		TAG_END);

	if (lock)
	{
		pal[2]=pal[8];

		for (y=124;y<210;y++)
		{
			if (y==selY) pal[2]=0xFF005577;
			if (y==selY+10) pal[2]=pal[8];

			for (x=180;x<315;x++)
			{
				((uint32 *) (base + bpr * (y-6))) [x+13] = pal[ c8bit_getPixel(menu_8bit,x,y) ];
			}
		}

		UnlockBitMap( lock );
	}
}

#define item_in(items,value,mi,ma) (items) * ( (value) - (mi) ) /  ( (ma)-(mi) )

void main_mouseClick( int Code )
{
	// 335,196,589,347 music..

	if ( Code & IECODE_UP_PREFIX )
	{
		if (between(335,mx,589) && between(196,my,347))
		{
			int y;
			y = 9 * (my - 196) / (347-196);

			if (y<9) 
			{
				selY = 124 + y*9;
				load_mod(modfiles[y]);
				mod_status = is_playing;
			}
		}

		if (between(335,mx,588) && between(357,my,371))
		{
			int b = item_in(4,mx,335,588);

			switch (b)
			{
				case 0:	// play				
						if (Module1)
						{
							PTPlay(Module1);
							mod_status = is_playing;
						}
						break;

				case 1:	// pause
						if (Module1)
						{
							switch (mod_status)
							{
								case is_playing: 
										PTPause(Module1); 
										mod_status = is_paused; break;

								case is_paused: 
										PTResume(Module1); 
										mod_status = is_playing; break;
							}
						}
						break;

				case 3:	// info
						reset_scroll();
						PTtexte = Texteinfo;
						break;
			}
		}
	}
}

void main_render()
{
	int i;
	int y;
	int dx;
	int n; 
	int b_w,b_h;
	
	c2t();	

	composite( bm,30,15,320,255, disp_bm,0,0,640,480 );
	composite( scroll_bm,2,50*sf,640,50, disp_bm,254,480-30,639,50);

	scroll(scroll_bm,2);

	if (t == 7)
	{
		i = lookup[*PTtexte];

		if (i != 256) composite( abc_bm,8*i,134,8,23, scroll_bm,500,0,(sf*30)+15,46 );

		t = 0;

		PTtexte++;
		if (*PTtexte == 0) PTtexte = Texte;
	}
	else t++;

	for (n = 0; n< 10; n++)
	{
		dx = bobbles[n].x + sin(bobbles[n].f)* (10 * bobbles[n].speed / 4);

		b_w = bobble_w*bobbles[n].speed/4;
		b_h = bobble_h*bobbles[n].speed/4;

		y= bobbles[n].y;
	
		composite( bobble_bm,0,y<0 ? -y : 0,bobble_w,bobble_h, disp_bm,dx-(b_w/2), y<0 ? 0 : y,b_w,b_h );
		bobbles[n].y-=bobbles[n].speed;
		if (bobbles[n].y<-50)
		{
			reset_bobble( bobbles + n );
		}

		bobbles[n].f+= 0.05;
	}
}


char *Texte = "                  "
	"RITCHY AND EREKOSE PRESENT TO YOU THIS LITTLE"
	" PRODUCTION FROM THE MOST"
	" WELLKNOWN GROUP POPULAR FOR HIS AMAZING, SMOKING, DRINKING,"
	" FUC...HUHO...HER...HOPLAING, EVERYTHINGS-ING GUYS !! YES, "
	" ANOTHER UNBELIEVABLE PRODUCTION IS NOW UNDER YOUR EYES FROM THIS"
	" GROUP CALLED........> PARLAFOX...HU, NO.....> BRISTOL....HU, SHIT."
	".....AH YEAH ! KANGOUROU BROTHERS !......AH, SHIT...> GOLANA !"
	".......NO.........AH THAT'S IT......A N A L O G !!    YYEEESS !"
	" I FOUND IT !!!! YYYEEEEEEEAAAAHHHHHH !! YAHOU, YIPIE !! YARGLA !"
	"..............HEY !....WHAT ?........WE ALL ALLREADY NOTICE THAT...."
	"...AH ? REALLY ??.............OK OK....."
	"WELL, AS I (EREKOSE) REALLY DONT KNOW WHAT TO SAY, LET'S WRITE SOME"
	" BULLSHIT....SO, THIS LITTLE MUSIC DISK WAS DONE IN ONE NIGHT, AT"
	" RITCHY'S HOME, JUST AFTER THE TRADIONNAL SPAGHETTI-YOGURT DINER."
	" ALL CODE WAS DONE BY...!! RITCHY !!...ALL MUSIC BY EREKOSE (ME) AND MY"
	" FRIEND MONTY>US, BUT OLD MEMBER OF ANALOG...AND AT LAST BUT NOT LEAST"
	" ALL GFX BY THE WONDERFULL...!!! BAOBAB !!!..."
	"AND NOW FOR SOMETHING COMPLETLY DIFFERENT....A COW WITH TWO NOSES..."
	"NO,NO,NO....A MEN WITH THREE EYES.......NO,NO,NO...."
	" JE TIENS ICI A SIGNALER QUE TOUTES RESSEMBLANCE AVEC DES NOMS"
	" OU DES GROUPES CONNUS NE SERAIT QUE PUREMENT VOLONTAIRE."
	" POP-QUIZ: WHO LIKE MAC DONALDS ? "
	" ANSWER: NOT ME !!! "
	"AND NOW ? WELL, I THINK I'LL JUST GIVE A LITTLE HELLO TO ALL MY OLD"
	"FRIENDS AND CONTACT I LEFT FOR SOME TIMES (I'M NOW IN THE ARMY AND"
	" ITS REALLY BORING !!!! NEVER DO THE ARMY IF YA BE ABLE TO !!)."
	" SO, BIG KISS TO ALL MEMBERS OF ANALOG, MONTY AND VANESSA, ZUUL(PC),"  
	" ALEX>MVT, BROSS>TSB, JOGEIR, KAEL, KARSTEN (INDEP), BAD CAT>IBB,"
	" , STEPHANE, RACKLER>LEGO, MR KING, ZINKFLOYD>TRSI, WHIRLY>DELIGHT,"
	" MANITOU>MOSQUITO, SHUN>LIQUID, SHED>CRYO, SANDMAN>EXILE, CORTEX, KILLER>ARKHAM,"
	" SNOOPY>DEFORM.............TO ALL OF YOU, REALLY SORRY FOR MY FUCKING"
	" BIG DELAY, BUT I REALLY DONT REACH TO BE A GOOD SWAPPER....I'M TOO"
	" LAZY FOR THAT..................GOOD BYE !................."
	"SCROLL TEXT RESTART...............5.........4.........3........1....."
	" HEY !!! WHERE IS THE 2 ????.......JUST KIDDING !.....2......1....."
	" HAVE A NICE DAY !......THANK YOU !!!!!!!!......................";


char *Texteinfo =
		"   WELCOME IN THE INFOTEXT... "
		"  J'SUIS D'ACCORD AVEC SERGE!!!   "
		"PRESS ESC TO QUIT CHEAP DISK    "
		"CLICK ON NAME OF MUSIC TO CHOOSE ANOTHER ONE....INCREDIBLE"
		" NO ?    '"
		"CLICK ON PAUSE TO..........PAUSE ?.....YEAH !'"
		"CLICK ON PLAY TO REPLAY AFTER PAUSE..."
		"CLICK ON FILTER TO TOGGLE THE FILTER ON-OFF      "
		"CLICK ON INFO TO.......OH, YOU ALREADY DID IT !!!   "
		"CREDITS FOR THIS ONE-NIGHT MUSIC-FILE:   "
		"M68K CODE > RITCHY - PPC CODE > LIVEFORIT - FONT > PLASMA - "
		"GFX > BAOBAB - MORAL SUPPORT > WORGLUB...   "
		"THIS MUSICAL-DEMO HAS BEEN CODED IN ONE LONG NIGHT...  "
		"THANX TO THE COCA-COLA AND BEER, CHESTERFIELD AND ROYAL "
		"MENTHOL CIGARETTES, MARS ICECREAM AND CREME MONTBLANC "
		"A LA VANILLE...QU'ON A OUBLIE DE MANGER ! MAIS TANT MIEUX"
		" CA EN FERA PLUS POUR MOI!      "
		"CHRISTINE !!! Y'T'RESTE PAS QUE'QUES CAHUETTES ?  "
		"         ALLEZ CHRISTINE, QUOI !! FAIT PETER LES CAHUETTES !"
		"                    "
		"BON, ICI JE -RITCHY- PEUX TAPER QUELQUECHOSE CAR PERSONNE "
		"NE LE LIRA JAMAIS... JE VOUDRAIS DIRE QUE ANALOG EST "
		"VRAIMENT UN GROUPE SUPER ! UNE DEMO PAR AN, TOUT LE MONDE "
		"TE CRITIQUE ET DE PREFERENCE CEUX QUI NE FONT RIEN...    "
		"BISOUS A LAETI,FIFI,STEPH,...,PAT,THIERRY,DEUFRE,SPEEDMAN,"
		"DD FROM SPIDER!,7TH YEUX AUSSI,CHAOS,ACKERLIGHT,BAMIGA "
		"SECTOR ONE,THE KENT TEAM,RED SECTOR,PLASMA FORCE,UNIT A,"
		"DOC,WILD COPPER,REBELS,CBC,CASCADE,MEGAFORCE... "
		"AND N'OUBLIEZ PAS >>> ONLY AMIGA "
		"MAKES IT POSSIBLE!  BISOUS A TOI AUSSI PETIT LECTEUR DE "
		"SCROLLING,CAR ON T'EMBRASSE PAS SOUVENT ET C'EST PAS JUSTE."
		"THIS DEMO IS FREEWARE, IT MEANS THAT IS A >FREE SOFTWARE, "
		"SO IT'S FORBIDDEN DE LA VENDRE !   "
		"           ";

