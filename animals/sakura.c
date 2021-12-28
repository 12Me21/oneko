#include "../animal.h"

#include "gen/sakura_bitmap.xbm"
#include "gen/sakura_mask.xbm"

static Animal x = {
	.name = "sakura",
	.speed = 13, .idle = 6,
	.time = 125000L,
	FRAME_DEFS(sakura),
};

AUTORUN {
	define_animal(&x);
}
