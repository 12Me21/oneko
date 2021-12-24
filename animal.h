#pragma once

#define LEN(x) (sizeof(x)/sizeof((x)[0]))

typedef unsigned char* Bytes;

typedef struct Frame {
	unsigned char* bits;
	unsigned char* mask;
	int width, height;
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

enum {FRAME_COUNT = sizeof(struct Frames)/sizeof(Bytes)};

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

#define FRAME_DEF(filename) {(unsigned char*)filename##_bits, (unsigned char*)filename##_mask_bits, filename##_width, filename##_height}

#define FRAME_DEFS(name) {FRAME_DEF(mati2##name), FRAME_DEF(jare2##name), FRAME_DEF(kaki1##name), FRAME_DEF(kaki2##name), FRAME_DEF(mati3##name), FRAME_DEF(sleep1##name), FRAME_DEF(sleep2##name), FRAME_DEF(awake##name), FRAME_DEF(up1##name), FRAME_DEF(up2##name), FRAME_DEF(down1##name), FRAME_DEF(down2##name), FRAME_DEF(left1##name), FRAME_DEF(left2##name), FRAME_DEF(right1##name), FRAME_DEF(right2##name), FRAME_DEF(upleft1##name), FRAME_DEF(upleft2##name), FRAME_DEF(upright1##name), FRAME_DEF(upright2##name), FRAME_DEF(dwleft1##name), FRAME_DEF(dwleft2##name), FRAME_DEF(dwright1##name), FRAME_DEF(dwright2##name), FRAME_DEF(utogi1##name), FRAME_DEF(utogi2##name), FRAME_DEF(dtogi1##name), FRAME_DEF(dtogi2##name), FRAME_DEF(ltogi1##name), FRAME_DEF(ltogi2##name), FRAME_DEF(rtogi1##name), FRAME_DEF(rtogi2##name)}

#define AUTORUN __attribute__((constructor)) static void init(void)

void define_animal(Animal* x);
