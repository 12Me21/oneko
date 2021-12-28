#include "../animal.h"

#include "koko_bitmap.xbm"
#include "koko_mask.xbm"

static Animal x = {
	.name = "koko",
	.speed = 13, .idle = 13,
	.bitmap_width = 32, .bitmap_height = 32,
	.time = 125000L,
	.frame_list = FRAME_DEFS(koko),
};

AUTORUN {
	define_animal(&x);
}
