#include "../animal.h"

#include "gen/koko_bitmap.xbm"
#include "gen/koko_mask.xbm"

static Animal x = {
	.name = "koko",
	.speed = 13, .idle = 13,
	.bitmap_width = 32, .bitmap_height = 32,
	.time = 125000L,
	FRAME_DEFS(koko),
};

AUTORUN {
	define_animal(&x);
}
