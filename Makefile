oneko: oneko.o animals/neko.o

LDLIBS += -lc -lm -lX11 -lXext -lXfixes
CFLAGS += -Wno-overflow -Wno-parentheses -std=c11 -pedantic -D_DEFAULT_SOURCE
