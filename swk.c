/* See LICENSE file for copyright and license details. */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "swk.h"
#include "config.h"

static SwkWindow *w = NULL;
static int running = 0;
static __thread int rendering = 0;

int
swk_use(SwkWindow *win) {
	w = win = win->_e.win = win;
	if(win->box == NULL)
		swk_focus_first(w);
	if(w->r.w == 0 || w->r.h == 0) {
		w->r.w = WINWIDTH;
		w->r.h = WINHEIGHT;
	}
	if(!running && !swk_gi_init(w))
		return 0;
	running = 1;
	swk_update();
	return 1;
}

void
swk_update() {
	char text[8];
	int roy, oy, scroll = 0;
	if(rendering)
		return;
	rendering = 1;
	w->_e.type = EExpose;
	if(swk_gi_update(w)) {
		SwkBox *b = w->boxes;
		swk_fit(w);
		swk_gi_clear();
		roy = oy = 0;
		for(;b->cb; b++) {
			w->_e.box = b;
			if(b->r.w==-1 && b->r.h==-1 && ((int)(size_t)b->data)<0)
				roy = oy+1;
			if(b->scroll)
				scroll = b->scroll;
			if(roy && b->r.y < roy) {
				sprintf(text, "(%d)", scroll);
				Rect r = w->r;
				r.x = r.w-1;
				r.y = roy;
				r.w = 3;
				swk_gi_text(r, text);
				swk_gi_line(--r.x, roy, r.w, 0, ColorHI);
			}
			else b->cb(&w->_e);
			oy = b->r.y;
		}
		swk_gi_flip();
	}
	rendering = 0;
}

// TODO: enqueue events here instead of use a global variable?
void
swk_exit() {
	running = 0;
}

void
swk_loop() {
	SwkEvent *e;
	do {
		if((e = swk_next_event(w)))
			swk_handle_event(e);
	} while(!e || e->type != EQuit);
}

void
swk_fontsize_increase() {
	swk_gi_fontsize(1);
	swk_update(w);
}

void
swk_fontsize_decrease() {
	swk_gi_fontsize(-1);
	swk_update(w);
}

static void
setscrollbox(int delta) {
	SwkBox *r = NULL;
	SwkBox *b = w->boxes;
	for(; b->cb; b++) {
		if(IS_SCROLLBOX(b))
			r = b;
		if(w->box==b && r)
			break;
	}
	if(r) r->scroll += delta;
}

void
swk_scroll_up() {
	setscrollbox(2);
}

void
swk_scroll_down() {
	setscrollbox(-2);
}

static void
swk_fit_row(SwkBox *a, SwkBox *b, int y) {
	SwkBox *btmp;
	int count = 0, x = 0;
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
	int row = 7; // hacky value to center widgets
	for(; b->cb; b++)
		if(IS_SCROLLBOX(b))
			row += (int)(size_t)b->data;
	return row;
}

void
swk_fit() {
	SwkBox *b, *b2;
	int x, y = 0, skip = 0;
	for(b=b2=w->boxes; b->cb; b++) {
		if(b->r.w==-1 && b->r.h==-1) {
			x = (int)(size_t)b->data;
			swk_fit_row(b2, b, y);
			y += x-skip;
			// vertical align //
			if(x<0) y += w->r.h-countrows(b2);
			b2 = b+1;
		}
		y += b->scroll;
	}
	swk_fit_row(b2, b, y);
}

int
swk_has_event() { // XXX: remove this useless wrap
	return swk_gi_has_event(w);
}

void
swk_focus_activate() {
	w->_e.box = w->box;
	w->_e.type = EClick;
}

SwkEvent *
swk_next_event() {
	if(running)
		return swk_gi_event(w, 1);
	w->_e.type = EQuit;
	w->_e.win = w;
	return &w->_e;
}

void
swk_handle_event(SwkEvent *e) {
	int i;
	SwkBox *b;
	if(e->win->cb)
		e->win->cb(e);
	switch(e->type) {
	case EKey:
		for(i=0; keys[i].cb; i++) {
			if(e->data.key.modmask == keys[i].modmask
			&& e->data.key.keycode == keys[i].keycode) {
				keys[i].cb(e->win);
				break;
			}
		}
		/* XXX: this must be implemented in app? */
		if(e->data.key.keycode==27) {
			e->box = e->win->box;
			e->type = EQuit;
			swk_exit(e->win);
			break;
		}
		// send key to focused box
		e->box = e->win->box;
		if(e->win->box)
			e->win->box->cb(e);
		swk_update();
		break;
	case EMotion:
		for(b=e->win->boxes; b->cb; b++) {
			if(SWK_HIT(b->r, e->data.motion)) {
				e->win->box = e->box = b;
				b->cb(e);
				swk_update();
				break;
			}
		}
		break;
	case EClick:
		// TODO: do this needs to be in config.h?
		switch(e->data.click.button) {
		case 4:
			swk_scroll_up(e->win);
			break;
		case 5:
			swk_scroll_down(e->win);
			break;
		default:
			for(b=e->win->boxes; b->cb; b++) {
				if(SWK_HIT(b->r, e->data.click.point)) {
					e->box = e->win->box = b;
					e->box->cb(e);
				}
			}
		}
		swk_update();
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
swk_focus_first() {
	w->box = w->boxes;
	while(w->box->cb == swk_filler)
		w->box++;
	if(w->box->cb == NULL)
		w->box = w->boxes;
}

void
swk_focus_next() {
	w->box++;
	if(w->box->cb == NULL)
		w->box = w->boxes;
	while(w->box->cb == swk_filler)
		w->box++;
	if(w->box->cb == NULL)
		swk_focus_first(w);
}

void
swk_focus_prev() {
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
		r.w += 6;
		swk_gi_text(r, e->box->text);
		r.w -= 6;
		if(e->win->box == e->box)
			swk_gi_line(r.x, r.y+1, r.w, 0, ColorHI);
		break;
	default:
		break;
	}
}

void
swk_password(SwkEvent *e) {
	char *str, *ptr;
	int len;
	Rect r;
	switch(e->type) {
	case EExpose:
		r = e->box->r;
		if(e->win->box == e->box)
			swk_gi_line(r.x, r.y+1, r.w, 0, ColorHI);
		len = strlen(e->box->text);
		if(len>0) {
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
	case EExpose:
		// XXX: add support for cursor (handle arrow keys)
		len = e->box->r.x+(strlen(e->box->text)*0.7);
		swk_label(e);
		swk_gi_line(len, e->box->r.y, 0, 1, ColorFG);
		break;
	}
}

void
swk_button(SwkEvent *e) {
	Rect r;
	switch(e->type) {
	case EExpose:
		r = e->box->r;
		//r.h = 3; // TODO: add support for multiple-line widgets
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
		if(b==(void*)0) e->box->data = (void*)1;
		else if(b==(void*)1) e->box->data = (void*)0;
		else *b = (e->box==*b)?NULL:e->box;
		break;
	case EExpose:
		r = e->box->r;
		if(e->win->box == e->box)
			swk_gi_line(r.x, r.y+1, r.w, 0, ColorHI);
		r.w = r.h = 1;
		if(b==(void*)1) swk_gi_fill(r, ColorHI, 1);
		else if(b==(void*)0) swk_gi_fill(r, ColorFG, 1);
		else if(e->box==*b) swk_gi_fill(r, ColorHI, 1);
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

void
swk_progress(SwkEvent *e) {
	int pc, len;
	Rect r;
	switch(e->type) {
	case EExpose:
		r = e->box->r;
		r.x+=1;
		swk_gi_text(r, e->box->text);
		r.x-=1;
		swk_gi_rect(r, ColorFG);
		len = strlen(e->box->text)+2;
		r.x += len*0.8;
		r.w -= len*0.6;
		pc = atoi(e->box->text);
		if(pc<0) pc = 0; else if(pc>100) pc = 100;
		r.w = (int)((float)r.w*((float)pc/100));
		if(r.w>0)
			swk_gi_fill(r, ColorFG, 1);
		break;
	default:
		break;
	}
}

/* -- */
void
swk_image_free(SwkBox *b) {
	swk_gi_img_free(b->data);
	b->data = NULL;
}

void
swk_image(SwkEvent *e) {
	if(e->box->data == NULL)
		e->box->data = swk_gi_img_load(e->box->text);
	switch(e->type) {
	case EExpose:
		if (e->box->data)
			swk_gi_img(e->box->r, e->box->data);
		else swk_gi_rect(e->box->r, ColorFG);
		if(e->win->box == e->box) {
			Rect r = e->box->r;
			swk_gi_line(r.x, r.y+1, r.w, 0, ColorHI);
		}
		break;
	default:
		break;
	}
}

void
swk_sketch(SwkEvent *e) {
	int x, y;
	if(e->box->data == NULL)
		e->box->data = swk_gi_img_new(e->box->r.w, e->box->r.h, ColorHI);
	switch(e->type) {
	case EClick:
		// TODO: implement low-level primitives for pixel-level rendering
		for(x=y=5;x<10;x=++y)
			swk_gi_img_set(e->box->data, x, y, ColorFG);
		for(x=y=5;x<10;x++,y--)
			swk_gi_img_set(e->box->data, x, y, ColorFG);
		printf("CLICKED %p %d %d\n", e->box->data, e->data.click.point.x, e->data.click.point.y);
		break;
	case EExpose:
		swk_gi_rect(e->box->r, ColorHI); // border
		swk_gi_img(e->box->r, e->box->data);
		break;
	default:
		break;
	}
}
