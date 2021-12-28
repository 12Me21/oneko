#include "../animal.h"

#include "tomoyo_bitmap.xbm"
#include "tomoyo_mask.xbm"

static Animal x = {
	.name = "tomoyo",
	.speed = 10, .idle = 6,
	.bitmap_width = 32, .bitmap_height = 32,
	.time = 125000L,
	.off_x = 32, .off_y = 32,
	.frame_list = FRAME_DEFS(tomoyo),
};

AUTORUN {
	define_animal(&x);
}
