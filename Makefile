output = oneko

animals = neko dog bsd sakura tomoyo koko

srcs = oneko.c $(animals:%=animals/%.c)

# animals/tora.o  - alternate color for neko, uses same mask bitmaps as neko, need to fix our macros

animals/%_mask.xbm: animals/%.png
	convert $< -alpha extract -negate $@

animals/%_bitmap.xbm: animals/%.png
	convert $< -alpha off $@

libs = c m X11 Xext Xfixes

CFLAGS += -Wno-overflow -Wno-parentheses -std=c11 -pedantic -D_DEFAULT_SOURCE -g

include .Nice.mk
