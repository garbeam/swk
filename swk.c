/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "swk.h"
#include "config.h"

int
swk_init(SwkWindow *w) {
	w->_e.win = w;
	if (w->box == NULL)
		swk_focus_first(w);
	if(w->r.w == 0 || w->r.h == 0) {
		w->r.w = WINWIDTH;
		w->r.h = WINHEIGHT;
	}
	if(swk_gi_init(w)) {
		w->running = 1;
		swk_update(w);
	}
	return w->running;
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
	} else w->running = 0;
}

void
swk_exit(SwkWindow *w) {
	w->running = 0;
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
			x += winc;
		}
	}
}

static int
countrows(SwkBox *b) {
	int row = 0;
	for(; b->cb; b++) {
		if(b->r.w==-1&&b->r.h==-1)
			row += (int)(size_t)b->data;
		else row += b->r.h;
	}
	return row;
}

void
swk_fit(SwkWindow *w) {
	int x, y = 0;
	SwkBox *b, *b2;
	for(b=b2=w->boxes; b->cb; b++) {
		if(b->r.w==-1 && b->r.h==-1) {
			x = (int)(size_t)b->data;
			if (x>0) {
				swk_fit_row(w, b2, b, y);
				y += (int)(size_t)b->data;
				b2 = b+1;
			} else {
				swk_fit_row(w, b2, b, y);
				b2 = b+1;
				y += 1+(w->r.h-countrows(b2));
				if (y<0) {
					fprintf(stderr, "overflow: must scroll\n");
					y=0;
				}
			}
		}
	}
	swk_fit_row(w, b2, b, y);
}

int
swk_has_event(SwkWindow *w) {
	return swk_gi_has_event(w);
}

void
swk_focus_activate(SwkWindow *w) {
	w->_e.box = w->box;
	w->_e.type = EClick;
}

SwkEvent *
swk_next_event(SwkWindow *w) {
	if(w->running)
		return swk_gi_event(w, 1);
	w->_e.type = EQuit;
	w->_e.win = w;
	return &w->_e;
}

void
swk_handle_event(SwkEvent *e) {
	int i;
	SwkBox *b;
	switch(e->type) {
	case EKey:
		for(i=0; keys[i].cb; i++) {
			if (e->data.key.modmask == keys[i].modmask
			&&  e->data.key.keycode == keys[i].keycode) {
				keys[i].cb(e->win);
				break;
			}
		}
		/* XXX: this must be implemented in app? */
		if (e->data.key.keycode==27) {
			e->box = e->win->box;
			e->type = EQuit;
			swk_exit(e->win);
			break;
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

/* -- widgets -- */

void
swk_label(SwkEvent *e) {
	Rect r;
	switch(e->type) {
	case EExpose:
		r = e->box->r;
		swk_gi_text(r, e->box->text);
		if(e->win->box == e->box)
			swk_gi_line(r.x, r.y+1, r.w, 0, ColorHI);
		break;
	default:
		break;
	}
}

void
swk_password(SwkEvent *e) {
	int len;
	Rect r;
	char *str, *ptr;
	switch(e->type) {
	case EExpose:
		r = e->box->r;
		if(e->win->box == e->box)
			swk_gi_line(r.x, r.y+1, r.w, 0, ColorHI);
		len = strlen(e->box->text);
		if (len>0) {
			ptr = str = malloc(len+1);
			for(;len--;ptr++)
				*ptr='*';
			*ptr='\0';
			swk_gi_text(r, str);
			free(str);
		}
		break;
	case EClick:
		printf("password: %s\n", e->box->text);
	default:
		swk_entry(e);
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
		r.x++;
		swk_gi_text(r, e->box->text);
		r.x--;
		if(e->win->box == e->box)
			swk_gi_rect(r, ColorHI);
		else swk_gi_rect(r, ColorFG);
		break;
	default:
		break;
	}
}

void
swk_filler(SwkEvent *e) {
	/* empty widget */
}

void
swk_option(SwkEvent *e) {
	Rect r;
	SwkBox **b = (SwkBox**)e->box->data;
	switch(e->type) {
	case EClick:
		if (b==(void*)0) e->box->data = (void*)1;
		else if (b==(void*)1) e->box->data = (void*)0;
		else *b = (e->box==*b)?NULL:e->box;
		break;
	case EExpose:
		r = e->box->r;
		if(e->win->box == e->box)
			swk_gi_line(r.x, r.y+1, r.w, 0, ColorHI);
		r.w = r.h = 1;
		if (b==(void*)1) swk_gi_fill(r, ColorHI, 1);
		else if (b==(void*)0) swk_gi_fill(r, ColorFG, 1);
		else if (e->box==*b) swk_gi_fill(r, ColorHI, 1);
		else swk_gi_fill(r, ColorFG, 1);
		r = e->box->r;
		r.x += 2;
		swk_gi_text(r, e->box->text);
		break;
	default:
		break;
	}
}

void
swk_separator(SwkEvent *e) {
	Rect r;
	switch(e->type) {
	case EExpose:
		r = e->box->r;
		if(e->win->box == e->box)
			swk_gi_line(r.x, r.y+1, r.w, 0, ColorHI);
		else swk_gi_line(r.x, r.y+1, r.w, 0, ColorFG);
		break;
	default:
		break;
	}
}
