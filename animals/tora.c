#include "../animal.h"

#include "gen/tora_bitmap.xbm"
#include "gen/tora_mask.xbm"

static Animal x = {
	.name = "tora",
	.speed = 16, .idle = 6,
	.time = 125000L,
	FRAME_DEFS(tora),
};

AUTORUN {
	define_animal(&x);
}
