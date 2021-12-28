#include "../animal.h"

#include "neko_bitmap.xbm"
#include "neko_mask.xbm"

static Animal x = {
	.name = "neko",
	.speed = 13, .idle = 6,
	.bitmap_width = 32, .bitmap_height = 32,
	.time = 125000L,
	.frame_list = FRAME_DEFS(neko),
};

AUTORUN {
	define_animal(&x);
}
