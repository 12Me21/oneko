output = oneko

animals = neko dog bsd sakura tomoyo tora koko

srcs = oneko.c $(animals:%=animals/%.c)

libs = c m
pkgs = x11 xext xfixes

CFLAGS += -g
CFLAGS += -std=c11 -pedantic -Wno-overflow -Wno-parentheses



include .Nice.mk



animals/gen/%_mask.xbm: animals/%.png
	@mkdir -p $(@D)
	convert $< -alpha extract -negate $@

animals/gen/%_bitmap.xbm: animals/%.png
	@mkdir -p $(@D)
	convert $< -alpha off $@

# it took me so long to figure this out
# you need the part before the first colon
# otherwise this will not work
$(animals:%=$(junkdir)/animals/%.c.o) : $(junkdir)/animals/%.c.o : animals/gen/%_mask.xbm animals/gen/%_bitmap.xbm
