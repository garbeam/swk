/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include "swk.h"

static SwkWindow *w = NULL;

int
swk_init(SwkWindow* window) {
	w = window;
	if (w->r.w == 0 || w->r.h == 0) {
		w->r.w = 640;
		w->r.h = 480;
	}
	return draw_init(w->r.w, w->r.h);
}

void
swk_fit() {
	SwkBox *b;
	for(b=w->boxes; b->cb; b++)
		printf("Handler: %p text: \"%s\"\n", b->cb, b->text);
}

void
swk_event_handle(SwkEvent *e) {
	switch(e->type) {
	case EKey:
	case EClick:
		if (w->box && w->box->cb)
			w->box->cb(e);
		break;
	case EExpose:
		swk_fit();
		//swk_gi_flip();
		break;
	case EQuit:
		swk_exit();
		break;
	default:
		break;
	}
}

void
swk_focus_next() {
	w->box++;
	if (w->box->cb == NULL)
		w->box = w->boxes;
}

void
swk_focus_prev() {
	if (w->box == w->boxes) {
		while(w->box->cb)
			w->box++;
		w->box--;
	} else w->box--;
}

/* widgets */
void
swk_label(SwkEvent *e) {
}

void
swk_entry(SwkEvent *e) {
}

void
swk_button(SwkEvent *e) {
}

void
swk_filler(SwkEvent *e) {
	/* empty widget */
}
