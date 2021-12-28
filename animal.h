#pragma once

#define LEN(x) (sizeof(x)/sizeof((x)[0]))

typedef unsigned char* Bytes;

typedef struct Frame {
	unsigned char* bits;
	unsigned char* mask;
	//int width, height;
} Frame;

struct Frames {
	Frame
		mati2, jare2, kaki1, kaki2,
		mati3, sleep1, sleep2, awake,
		up1, up2, down1, down2,
		left1, left2, right1, right2,
		upleft1, upleft2, upright1, upright2,
		dwleft1, dwleft2, dwright1, dwright2,
		utogi1, utogi2, dtogi1, dtogi2,
		ltogi1, ltogi2, rtogi1, rtogi2;
};

enum {FRAME_COUNT = sizeof(struct Frames)/sizeof(Frame)};

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
	
	union {
		Frame frame_list[FRAME_COUNT];
		struct Frames frames;
	};
} Animal;

#define FRAME_DEFS(name) {{(unsigned char*)name##_bitmap_bits, (unsigned char*)name##_mask_bits}}

#define AUTORUN __attribute__((constructor)) static void init(void)

void define_animal(Animal* x);
