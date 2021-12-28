#include "../animal.h"

#include "gen/tomoyo_bitmap.xbm"
#include "gen/tomoyo_mask.xbm"

static Animal x = {
	.name = "tomoyo",
	.speed = 10, .idle = 6,
	.bitmap_width = 32, .bitmap_height = 32,
	.time = 125000L,
	.off_x = 32, .off_y = 32,
	FRAME_DEFS(tomoyo),
};

AUTORUN {
	define_animal(&x);
}
