#include "../animal.h"

#include "bsd_bitmap.xbm"
#include "bsd_mask.xbm"

static Animal x = {
	.name = "bsd_daemon",
	.speed = 16, .idle = 6,
	.bitmap_width = 32, .bitmap_height = 32,
	.time = 300000L,
	.off_x = 22, .off_y = 20,
	.frame_list = FRAME_DEFS(bsd),
};

AUTORUN {
	define_animal(&x);
}
