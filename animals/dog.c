#include "../animal.h"

#include "gen/dog_bitmap.xbm"
#include "gen/dog_mask.xbm"

static Animal x = {
	.name = "dog",
	.speed = 10, .idle = 6,
	.time = 125000L,
	FRAME_DEFS(dog),
};

AUTORUN {
	define_animal(&x);
}
