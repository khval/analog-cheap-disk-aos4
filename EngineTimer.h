// MIT License
// 
// Copyright (c) 2022 Kjetil Hvalstrand

struct TimerContext
{
	struct MsgPort *timer_port;
	struct TimeRequest *timer_io;
	int timer_mask;
	bool its_open;
};

void reset_timer(struct TimeRequest *timer_io);
bool open_timer_context( struct TimerContext *tc );
void close_timer_context( struct TimerContext *tc );

