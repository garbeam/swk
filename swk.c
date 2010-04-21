/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "swk.h"

// move into SwkWindow* ?
static int running = 0;

int
swk_init(SwkWindow *w) {
	w->_e.win = w;
	swk_focus_first(w);
	if(w->r.w == 0 || w->r.h == 0) {
		w->r.w = 640;
		w->r.h = 480;
	}
	if(swk_gi_init(w)) {
		running = 1;
		swk_update(w);
	}
	return running;
}

void
swk_update(SwkWindow *w) {
	w->_e.type = EExpose;
	if(swk_gi_update(w)) {
		SwkBox *b = w->boxes;
		swk_fit(w);
		swk_gi_clear();
		for(;b->cb; b++) {
			w->_e.box = b;
			b->cb(&w->_e);
		}
		swk_gi_flip();
	} else running = 0;
}

void
swk_exit(void) {
	running = 0;
}

void
swk_loop(SwkWindow *w) {
	SwkEvent *e;
	do {
		if((e = swk_next_event(w)))
			swk_handle_event(e);
	} while(!e || e->type != EQuit);
}

static void swk_fit_row(SwkWindow *w, SwkBox *a, SwkBox *b, int y) {
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
swk_fit(SwkWindow *w) {
	int y = 0;
	SwkBox *b, *b2;
	for(b=b2=w->boxes; b->cb; b++) {
		if(b->r.w==-1 && b->r.h==-1) {
			swk_fit_row(w, b2, b, y);
			y += (int)(size_t)b->data;
			b2 = b+1;
		}
	}
	swk_fit_row(w, b2, b, y);
}

int
swk_has_event(SwkWindow *w) {
	return swk_gi_has_event(w);
}

SwkEvent *
swk_next_event(SwkWindow *w) {
	if(running)
		return swk_gi_event(w, 1);
	w->_e.type = EQuit;
	w->_e.win = w;
	return &w->_e;
}

void
swk_handle_event(SwkEvent *e) {
	SwkBox *b;
	switch(e->type) {
	case EKey:
		// TODO: ^F ullscreen? handle ^Y and ^P to copypasta box->text
		// ^A focus first widget, ^E focus last widget ?
		if(e->data.key.modmask == 2) {
			switch(e->data.key.keycode) {
			case 8:
				swk_focus_first(e->win);
				break;
			case 10:
				swk_focus_next(e->win);
				break;
			case 11:
				swk_focus_prev(e->win);
				break;
			case 12:
				e->type = EClick;
				break;
			}
		} else
		if(e->data.key.keycode == 9) { // TAB
			if(e->data.key.modmask)
				swk_focus_prev(e->win);
			else swk_focus_next(e->win);
			swk_update(e->win);
		} else
		if(e->data.key.keycode == 13) { // ENTER
			e->box = e->win->box;
			e->type = EClick;
		} else
		if(e->data.key.keycode == 27) { // ESC
			e->box = e->win->box;
			e->type = EQuit;
			swk_exit();
		}
		// send key to focused box
		e->box = e->win->box;
		if(e->win->box)
			e->win->box->cb(e);
		swk_update(e->win);
		break;
	case EMotion:
		for(b=e->win->boxes; b->cb; b++) {
			if(SWK_HIT(b->r, e->data.motion)) {
				e->win->box = e->box = b;
				b->cb(e);
				swk_update(e->win);
				break;
			}
		}
		break;
	case EClick:
		for(b=e->win->boxes; b->cb; b++) {
			if(SWK_HIT(b->r, e->data.click.point)) {
				e->box = e->win->box = b;
				e->box->cb(e);
				swk_update(e->win);
			}
		}
		break;
	case EExpose:
		swk_update(e->win);
		break;
	case EQuit:
		swk_gi_exit();
		break;
	default:
		break;
	}
}

void
swk_focus_first(SwkWindow *w) {
	w->box = w->boxes;
	while(w->box->cb == swk_filler)
		w->box++;
	if(w->box->cb == NULL)
		w->box = w->boxes;
}

void
swk_focus_next(SwkWindow *w) {
	w->box++;
	if(w->box->cb == NULL)
		w->box = w->boxes;
	while(w->box->cb == swk_filler)
		w->box++;
	if(w->box->cb == NULL)
		swk_focus_first(w);
}

void
swk_focus_prev(SwkWindow *w) {
	if(w->box == w->boxes) {
		while(w->box->cb)
			w->box++;
		w->box--;
	} else {
		w->box--;
		while(w->box->cb == swk_filler) {
			w->box--;
			if(w->box < w->boxes) {
				w->box = w->boxes;
				swk_focus_prev(w);
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
		if(e->win->box == e->box)
			swk_gi_line(r.x, r.y+1, r.w, 0, ColorHI);
		swk_gi_text(r, e->box->text);
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
			ptr = (char*)malloc(strlen(e->box->text)+2);
			strcpy(ptr, e->box->text);
			if(e->box->data)
				free(e->box->text);
			if((len = strlen (ptr))>0)
				ptr[len-1] = '\0';
			e->box->text = e->box->data = ptr;
		} else {
			if(key>=' '&&key<='~') {
				ptr = (char*)malloc(strlen(e->box->text)+2);
				sprintf(ptr, "%s%c", e->box->text, e->data.key.keycode);
				if(e->box->data)
					free(e->box->text);
				e->box->text = e->box->data = ptr;
			}
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
		if(e->win->box == e->box)
			swk_gi_rect(r, ColorHI);
		else swk_gi_rect(r, ColorFG);
		r.x++;
		swk_gi_text(r, e->box->text);
		break;
	default:
		break;
	}
}

void
swk_filler(SwkEvent *e) {
	/* empty widget */
}
