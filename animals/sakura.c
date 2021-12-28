#include "../animal.h"

#include "sakura_bitmap.xbm"
#include "sakura_mask.xbm"

static Animal x = {
	.name = "sakura",
	.speed = 13, .idle = 6,
	.bitmap_width = 32, .bitmap_height = 32,
	.time = 125000L,
	.frame_list = FRAME_DEFS(sakura),
};

AUTORUN {
	define_animal(&x);
}
