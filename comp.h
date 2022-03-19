// MIT License
// 
// Copyright (c) 2022 Kjetil Hvalstrand

typedef struct CompositeHookData_s {
	struct BitMap *srcBitMap; // The source bitmap
	int32 srcWidth, srcHeight; // The source dimensions
	int32 offsetX, offsetY; // The offsets to the destination area relative to the window's origin
	int32 scaleX, scaleY; // The scale factors
	uint32 retCode; // The return code from CompositeTags()
} CompositeHookData;

void draw_comp();

void composite( 
	struct BitMap *source_bm, int source_x,int source_y,int source_w,int source_h,
	struct BitMap *dest_BitMap,int dx, int dy, int dw, int dh );

