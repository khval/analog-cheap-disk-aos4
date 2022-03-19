// MIT License
// 
// Copyright (c) 2022 Kjetil Hvalstrand

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#define __USE_INLINE__

#include <proto/exec.h>
#include <proto/dos.h>
#include <proto/intuition.h>
#include <proto/graphics.h>
#include <proto/layers.h>
#include <proto/diskfont.h>

#include "init.h"

struct Library			*LayersBase = NULL;
struct LayersIFace		*ILayers = NULL;

struct Library			*IntuitionBase = NULL;
struct IntuitionIFace		*IIntuition = NULL;

struct Library			*GraphicsBase = NULL;
struct GraphicsIFace		*IGraphics = NULL;

struct Library			*DataTypesBase = NULL;
struct DataTypesIFace	*IDataTypes = NULL;

struct Library			*PTReplayBase = NULL;
struct PTReplayIFace	*IPTReplay = NULL;


BOOL open_lib( const char *name, int ver , const char *iname, int iver, struct Library **base, struct Interface **interface);
void close_lib_all( struct Library **Base, struct Interface **I );


bool startup()
{
	if ( ! open_lib( "layers.library", 51L , "main", 1, &LayersBase, (struct Interface **) &ILayers ) ) return FALSE;
	if ( ! open_lib( "intuition.library", 51L , "main", 1, &IntuitionBase, (struct Interface **) &IIntuition  ) ) return FALSE;
	if ( ! open_lib( "graphics.library", 54L , "main", 1, &GraphicsBase, (struct Interface **) &IGraphics  ) ) return FALSE;
	if ( ! open_lib( "datatypes.library", 53 , "main", 1, &DataTypesBase, (struct Interface **) &IDataTypes  ) ) return FALSE;
	if ( ! open_lib( "ptreplay.library", 7 , "main", 1, &PTReplayBase, (struct Interface **) &IPTReplay  ) ) return FALSE;

	return TRUE;
}

void shutdhown()
{
	close_lib(DataTypes);
	close_lib(Layers);
	close_lib(Intuition);
	close_lib(Graphics);
}

BOOL open_lib( const char *name, int ver , const char *iname, int iver, struct Library **base, struct Interface **interface)
{
	*interface = NULL;
	*base = OpenLibrary( name , ver);

	if (*base)
	{
		 *interface = GetInterface( *base,  iname , iver, TAG_END );
		if (!*interface) Printf("Unable to getInterface %s for %s %ld!\n",iname,name,ver);
	}
	else
	{
	   	Printf("Unable to open the %s %ld!\n",name,ver);
	}
	return (*interface) ? TRUE : FALSE;
}

void close_lib_all( struct Library **Base, struct Interface **I )
{
	if (*Base) CloseLibrary(*Base); *Base = 0;
	if (*I) DropInterface((struct Interface*) *I); *I = 0;
}

