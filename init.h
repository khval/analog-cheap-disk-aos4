// MIT License
// 
// Copyright (c) 2022 Kjetil Hvalstrand


enum
{
	is_stopped = 0,
	is_playing,
	is_paused
};

struct part
{
	void (*render) ();
	void (*mouseClick) (int Code);
};

struct bobble
{
	int x;
	int y;
	int speed;
	double f;
};

bool startup();
void shutdhown();

#define close_lib(name) close_lib_all( &(name ## Base), (struct Interface **) &(I ## name) )

#define between(mi,v,ma) ((v>=mi)&&(v<=ma))

