#include <stdlib.h>
#include "draw.h"

static DrawBuffer buf;

int
draw_init(int w, int h) {
	buf.pixels = malloc(sizeof(int)*w*h);
	buf.w = w;
	buf.h = h;
	return swk_gi_init(w, h);
}

void
swk_exit() {
	free (buf.pixels);
	swk_gi_exit();
}

/* TODO: move to gi_sdl.c */
void
draw_line(int x, int y, int w, int h) {
}

void
draw_box(int x, int y, int w, int h) {
}

void
draw_rect(int x, int y, int w, int h) {
}

void
draw_text(int x, int y, const char *text) {
}
