#include "../animal.h"

#include "tora_bitmap.xbm"
#include "tora_mask.xbm"

static Animal x = {
	.name = "tora",
	.speed = 16, .idle = 6,
	.bitmap_width = 32, .bitmap_height = 32,
	.time = 125000L,
	.frame_list = FRAME_DEFS(tora),
};

AUTORUN {
	define_animal(&x);
}
