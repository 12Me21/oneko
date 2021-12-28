output = oneko

animals = neko dog bsd sakura tomoyo tora koko

srcs = oneko.c $(animals:%=animals/%.c)

libs = c m
pkgs = x11 xext xfixes

CFLAGS += -g
CFLAGS += -std=c11 -pedantic -Wno-overflow -Wno-parentheses



include .Nice.mk



# it took me so long to figure this out
# you need the part before the first colon
# otherwise this will not work
$(animals:%=$(junkdir)/animals/%.c.o) : $(junkdir)/animals/%.c.o : animals/%_mask.xbm animals/%_bitmap.xbm

%_mask.xbm: %.png
	convert $< -alpha extract -negate $@

%_bitmap.xbm: %.png
	convert $< -alpha off $@
