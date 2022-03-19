// MIT License
// 
// Copyright (c) 2022 Kjetil Hvalstrand

struct c8bit
{
	int w;
	int h;
	char *data;
};

union argb
{
	uint32 data;
	struct
	{
		char a;
		char r;
		char g;
		char b;
	};
};

struct c8bit *alloc_c8bit(int w,int h);
void free_c8bit( struct c8bit *c8bit );
void c8bit_setPixel( struct c8bit *c8bit, int x,int y, char c );
char c8bit_getPixel( struct c8bit *c8bit, int x,int y );

struct BitMap *load_iff( const char *name, BOOL alpha );
struct c8bit *load_iff_8bit( const char *name, BOOL alpha );


