#include "../animal.h"

#include "gen/<name>_bitmap.xbm"
#include "gen/<name>_mask.xbm"

static Animal x = {
	.name = "<name>",
	.speed = 13,
	.idle = 6,
	.time = 125000L,
	
	// 0,0 means the window will be centered above your cursor
	.off_x = 0,
	.off_y = 0,
	
	FRAME_DEFS(<name>),
};

AUTORUN {
	define_animal(&x);
}
