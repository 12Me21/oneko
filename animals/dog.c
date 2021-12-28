#include "../animal.h"

#include "dog_bitmap.xbm"
#include "dog_mask.xbm"

static Animal x = {
	.name = "dog",
	.speed = 10, .idle = 6,
	.bitmap_width = 32, .bitmap_height = 32,
	.time = 125000L,
	.frame_list = FRAME_DEFS(dog),
};

AUTORUN {
	define_animal(&x);
}
