/* See LICENSE file for copyright and license details. */
#define _BSD_SOURCE // strdup
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <locale.h>
#include <X11/keysym.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include "swk.h"
#define SWK
#include "config.h"

#define FONTNAME "-*-*-medium-*-*-*-14-*-*-*-*-*-*-*"

static Drawable drawable;
static int fs = FONTSIZE; // TODO: we need fsW and fsH
static Window window;
static int screen;
static Display *display = NULL;
static int has_event = 0;
static XEvent lastev;
static int first = 1;
#define EVENTMASK PointerMotionMask | ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask

int
swk_gi_fontsize(int sz) {
	fs += sz*2;
	/* TODO */
	return 1;
}

int
swk_gi_init(SwkWindow *w) {
	if(first) {
		first = 0;
		display = XOpenDisplay(NULL);
		if(display == NULL) {
			fprintf(stderr, "Cannot open display\n");
			return 0;
		}
		screen = DefaultScreen(display);
		window = XCreateSimpleWindow(display,
			RootWindow(display, screen),
			10, 10, w->r.w, w->r.h, 1,
			BlackPixel(display, screen),
			WhitePixel(display, screen));
		drawable = XCreatePixmap(display, window, w->r.w, w->r.h, DefaultDepth(display, screen));
		XSelectInput(display, window, EVENTMASK);
		XMapWindow(display, window);
	}
	return swk_gi_fontsize(0);
}

int
swk_gi_update(SwkWindow *w) {
	XWindowAttributes wa;
	XGetWindowAttributes(display, window, &wa);
	w->r.w = (wa.width / fs)-1;
	w->r.h = (wa.height / fs)-1;
	return 1;
}

void
swk_gi_exit() {
	XCloseDisplay(display);
}

int
swk_gi_has_event(SwkWindow *w) {
	if(!has_event)
		has_event = XNextEvent(display, &lastev); // XXX This must be nonblocking
	return has_event;
}

SwkEvent *
swk_gi_event(SwkWindow *w, int dowait) {
	static int mousedowny, mousedownx, mousedown = 0;
	static int mousemoved = 0;
	KeySym ksym;
	XEvent event;
	SwkEvent *ret = &w->_e;

	if(has_event) event = lastev;
	else has_event = !XNextEvent(display, &event);

	if(has_event);
	switch(event.type) {
	case Expose:
		ret->type = EExpose;
		ret->data.expose.x = ret->data.expose.y = \
		ret->data.expose.w = ret->data.expose.h = 0;
		break;
	case MotionNotify:
		// TODO: move this stuff into swk.c.. shoudlnt be backend dependent
//		fprintf(stderr, "event: motion (%d,%d)\n", event.motion.x,event.motion.y);
		if(mousedown) {
			// touchscreen spaguetti trick
			if(mousedowny==-1) mousedowny = event.xmotion.y; else mousemoved = 1;
			if(mousedownx==-1) mousedownx = event.xmotion.x; else mousemoved = 1;
			if(event.xmotion.y>mousedowny+fs) {
				mousedowny = event.xmotion.y;
				swk_scroll_up(w);
			} else
			if(event.xmotion.y<mousedowny-fs) {
				mousedowny = event.xmotion.y;
				swk_scroll_down(w);
			}
			if(event.xmotion.x>mousedownx+fs) {
				mousedownx = event.xmotion.x;
				swk_column_move_right();
				swk_column_move_right();
			} else
			if(event.xmotion.x<mousedownx-fs) {
				mousedownx = event.xmotion.x;
				swk_column_move_left();
				swk_column_move_left();
			}
			ret->type = EExpose;
			ret->data.expose.x = ret->data.expose.y = \
			ret->data.expose.w = ret->data.expose.h = 0;
		} else {
			ret->type = EMotion;
			ret->data.motion.x = event.xmotion.x / fs;
			ret->data.motion.y = event.xmotion.y / fs;
		}
		break;
	case ButtonRelease:
		//fprintf(stderr, "event: up %d (%d,%d)\n", event.button.button,event.button.x,event.button.y);
		mousedown = 0;
		if(!mousemoved) {
			ret->type = EClick;
			ret->data.click.button = event.xbutton.state;
			ret->data.click.point.x = event.xbutton.x / fs;
			ret->data.click.point.y = event.xbutton.y / fs;
		}
		break;
	case ButtonPress:
		//fprintf(stderr, "event: down %d (%d,%d)\n", event.button.button,event.button.x,event.button.y);
		mousemoved = 0;
		mousedown = 1;
		mousedowny = event.xbutton.y;
		break;
	case KeyPress:
		ret->type = EKey;
		XLookupString(&event.xkey, NULL, 0, &ksym, NULL);
		printf("ksym=%d\n", (int)ksym);
		switch(ksym) {
		case XK_BackSpace:
			ret->data.key.keycode = 8;
			break;
		case XK_Return:
			ret->data.key.keycode = '\n';
			break;
		default:
			ret->data.key.keycode = ksym;
		}
		ret->data.key.modmask = 0;
		if(event.xkey.state&ShiftMask)
			ret->data.key.modmask |= Shift;
		if(event.xkey.state&Mod1Mask)
			ret->data.key.modmask |= Alt;
		if(event.xkey.state&ControlMask)
			ret->data.key.modmask |= Ctrl;
		fprintf(stderr, "event: key %d %d (%c)\n", 
			ret->data.key.modmask, ret->data.key.keycode, ret->data.key.keycode);
		break;
	case 0://SDL_QUIT:
		ret->type = EQuit;
		break;
	default:
		ret = NULL;
		break;
	}
	has_event = 0;
	return ret;
}

void
swk_gi_clear() {
	XClearWindow(display, window);
}

void
swk_gi_flip() {
#if 0
	XWindowAttributes wa;
	XGetWindowAttributes(display, window, &wa);
	XCopyArea(display, drawable, window, gc, 0, 0, wa.width, wa.height, 0, 0);
	XFlush(display);
#endif
}

/* -- drawing primitives -- */
void
swk_gi_line(int x1, int y1, int x2, int y2, int color) {
	Rect r = { x1, y1, x2, y2 };
	if(!x2 || !y2)
		swk_gi_fill(r, color, 0);
	// TODO: add support for diagonal lines?
}

void
swk_gi_fill(Rect r, int color, int lil) {
	XRectangle area = { r.x*fs, r.y*fs, r.w*fs, r.h*fs };
	if(lil) {
		const int s = fs/4;
		area.x += s;
		area.y += s;
		area.width -= (s*2);
		area.height -= (s*2);
	}
	if(!area.width) area.width = 1;
	if(!area.height) area.height = 1;
	XFillRectangles(display, window, DefaultGC(display, screen), &area, 1);
}

void
swk_gi_rect(Rect r, int color) {
	swk_gi_line(r.x, r.y, r.w, 0, color);
	swk_gi_line(r.x, r.y+r.h, r.w, 0, color);
	swk_gi_line(r.x, r.y, 0, r.h, color);
	swk_gi_line(r.x+r.w, r.y, 0, r.h, color);
}

void
swk_gi_text(Rect r, const char *text) {
	if(!text||!*text)
		return;
	XDrawString(display, window, DefaultGC(display, screen), r.x*fs, ((1+r.y)*fs)-3, text, strlen (text));
}

void
swk_gi_img(Rect r, void *img) {
	/* TODO */
}

/* image api */
void*
swk_gi_img_new(int w, int h, int color) {
	/* TODO */
	return NULL;
}

void*
swk_gi_img_load(const char *str) {
	/* TODO */
	return NULL;
}

void
swk_gi_img_free(void *s) {
	/* TODO */
}

void
swk_gi_img_set(void *img, int x, int y, int color) {
	/* TODO */
}

int
swk_gi_img_get(void *img, int x, int y) {
	/* TODO */
	return 0;
}
