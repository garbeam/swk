/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "swk.h"

static int running = 0;
static SwkWindow *w = NULL;

int
swk_init(SwkWindow* window) {
	w = window;
	w->box = w->boxes;
	if(w->r.w == 0 || w->r.h == 0) {
		w->r.w = 640;
		w->r.h = 480;
	}
	if(swk_gi_init(w)) {
		running = 1;
		swk_update();
	}
	return running;
}

void
swk_update() {
	SwkEvent ev = { .type = EExpose };
	if(swk_gi_update(w)) {
		SwkBox *b = w->boxes;
		swk_fit();
		swk_gi_clear();
		for(;b->cb; b++) {
			ev.box = b;
			b->cb(&ev);
		}
		swk_gi_flip();
	} else running = 0;
}

void
swk_exit() {
	running = 0;
}

void
swk_loop() {
	SwkEvent *e;
	do {
		if((e = swk_event(1)))
			swk_event_handle(e);
	} while (!e || e->type != EQuit);
}

static void swk_fit_row(SwkBox *a, SwkBox *b, int y) {
	int count, x = 0;
	SwkBox *btmp;
	count = 0;
	for(btmp=a; btmp<b; btmp++)
		count++;
	if(count) {
		int winc = w->r.w / count;
		for(btmp=a; btmp<b; btmp++) {
			btmp->r.x = x;
			btmp->r.y = y;
			btmp->r.w = winc;
			btmp->r.h = 1;
			x+=winc;
		}
	}
}

void
swk_fit() {
	int y = 0;
	SwkBox *b, *b2;
	for(b=b2=w->boxes; b->cb; b++) {
		if(b->r.w==-1 && b->r.h==-1) {
			swk_fit_row(b2, b, y);
			y += (int)(size_t)b->data;
			b2 = b+1;
		}
	}
	swk_fit_row(b2, b, y);
}

SwkEvent *
swk_event(int dowait) {
	static SwkEvent ev;
	if(running)
		return swk_gi_event();
	ev.type = EQuit;
	return &ev;
}

void
swk_event_handle(SwkEvent *e) {
	SwkBox *b;
	switch(e->type) {
	case EKey:
		// TODO: handle ^Y and ^P to copypasta box->text
		if(e->data.key.keycode == 9) { // TAB
			if(e->data.key.modmask)
				swk_focus_prev();
			else swk_focus_next();
			swk_update();
		} else
		if(e->data.key.keycode == 13) { // ENTER
			e->box = w->box;
			e->type = EClick;
		}
		// send key to focused box
		e->box = w->box;
		if(w->box)
			w->box->cb(e);
		swk_update();
		break;
	case EMotion:
		for(b=w->boxes; b->cb; b++) {
			if(SWK_HIT(b->r, e->data.motion)) {
				w->box = e->box = b;
				b->cb(e);
				swk_update();
				break;
			}
		}
		break;
	case EClick:
		for(b=w->boxes; b->cb; b++) {
			if(SWK_HIT(b->r, e->data.click.point)) {
				e->box = w->box = b;
				e->box->cb(e);
				swk_update();
			}
		}
		break;
	case EExpose:
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
	if(w->box->cb == NULL)
		w->box = w->boxes;
	while(w->box->cb == swk_filler)
		w->box++;
	if(w->box->cb == NULL)
		w->box = w->boxes;
}

void
swk_focus_prev() {
	if(w->box == w->boxes) {
		while(w->box->cb)
			w->box++;
		w->box--;
	} else {
		w->box--;
		while (w->box->cb == swk_filler) {
			w->box--;
			if(w->box < w->boxes) {
				w->box = w->boxes;
				swk_focus_prev();
				return;
			}
		}
	}
}

/* widgets */
void
swk_label(SwkEvent *e) {
	Rect r;
	switch(e->type) {
	case EExpose:
		r = e->box->r;
		if(w->box == e->box)
			swk_gi_line(r.x, r.y+1, r.w, 0, ColorHI);
		swk_gi_text(r.x, r.y, e->box->text);
		break;
	default:
		break;
	}
}

void
swk_entry(SwkEvent *e) {
	int len, key;
	char *ptr;
	switch(e->type) {
	case EKey:
		key = e->data.key.keycode;
		if(key == 8) {
			ptr = strdup (e->box->text);
			if(e->box->data)
				free(e->box->text);
			if((len = strlen (ptr))>0)
				ptr[len-1] = '\0';
			e->box->text = e->box->data = ptr;
		} else {
			ptr = (char*)malloc(strlen(e->box->text)+2);
			sprintf(ptr, "%s%c", e->box->text, e->data.key.keycode);
			if(e->box->data)
				free(e->box->text);
			e->box->text = e->box->data = ptr;
		}
		break;
	default:
		swk_label(e);
		break;
	}
}

void
swk_button(SwkEvent *e) {
	Rect r;
	switch(e->type) {
	case EExpose:
		r = e->box->r;
		if(w->box == e->box)
			swk_gi_rect(r.x, r.y, r.w, r.h, ColorHI);
		else swk_gi_rect(r.x, r.y, r.w, r.h, ColorFG);
		swk_gi_text(r.x+1, r.y, e->box->text);
		break;
	default:
		break;
	}
}

void
swk_filler(SwkEvent *e) {
	/* empty widget */
}
