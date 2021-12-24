#include "../animal.h"

#include "../bitmaps/neko/neko.include"
#include "../bitmasks/neko/neko.mask.include"

static Animal x = {
	.name = "neko",
	.speed = 13, .idle = 6,
	.bitmap_width = 32, .bitmap_height = 32,
	.time = 125000L,
	.frame_list = FRAME_DEFS(),
};

AUTORUN {
	define_animal(&x);
}
