#pragma once

#define LEN(x) (sizeof(x)/sizeof((x)[0]))

enum {
	BITMAP_WIDTH = 32,
	BITMAP_HEIGHT = 32,
};

typedef unsigned char Frame[BITMAP_WIDTH*BITMAP_HEIGHT/8];

enum {FRAME_COUNT = 32};

//typedef Frame Frames[FRAME_COUNT];

typedef struct Animal {
	char* name;
	int speed;
	int idle;
	int bitmap_width, bitmap_height;
	long time;
	int off_x, off_y;
	
	/*Bytes cursor;
	Bytes mask;
	int cursor_width, cursor_height;
	int cursor_x_hot, cursor_y_hot;*/
	
	Frame* bitmaps;
	Frame* masks;
} Animal;

#define FRAME_DEFS(name) .bitmaps = (Frame*)name##_bitmap_bits, .masks = (Frame*)name##_mask_bits

#define AUTORUN __attribute__((constructor)) static void init(void)

void define_animal(Animal* x);
