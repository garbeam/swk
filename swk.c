/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include "swk.h"

static int running = 0;
static SwkWindow *w = NULL;

int
swk_init(SwkWindow* window) {
	w = window;
	w->box = w->boxes;
	if (w->r.w == 0 || w->r.h == 0) {
		w->r.w = 640;
		w->r.h = 480;
	}
	if (swk_gi_init(w))
		running = 1;
	return running;
}

void
swk_update() {
	if (!swk_gi_update(w))
		running = 0;
	else swk_gi_flip();
}

void
swk_exit() {
	running = 0;
}

void
swk_loop() {
	SwkEvent *e;
	do {
		if ((e = swk_event(1)))
			swk_event_handle(e);
	} while (!e || e->type != EQuit);
}

void
swk_fit() {
	SwkBox *b;
	for(b=w->boxes; b->cb; b++)
		printf("Handler: %p text: \"%s\"\n", b->cb, b->text);
}

SwkEvent *
swk_event(int dowait) {
	static SwkEvent ev;
	if (running)
		return swk_gi_event();
	ev.type = EQuit;
	return &ev;
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
		swk_update();
		break;
	case EQuit:
		swk_gi_exit();
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
