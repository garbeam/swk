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
#include <draw.h>
#include "swk.h"
#define SWK
#include "config.h"

#define FONTNAME "-*-*-medium-*-*-*-14-*-*-*-*-*-*-*"
//#define FONTNAME "10x20"

static int fs = FONTSIZE; // TODO: we need fsW and fsH
static Window window;
static int first = 1;
static DC *dc = NULL;
static int col[ColorLast];
static int colors[ColorLast] = { FGCOLOR, BGCOLOR, HICOLOR, TFCOLOR };
#define EVENTMASK PointerMotionMask | ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask

int
swk_gi_fontsize(int sz) {
	fs += sz*2;
	/* TODO: resize font */
	return 1;
}

static Window dc_window(DC *dc, int x, int y, int w, int h) {
	Drawable drawable;
	Window window;
	int screen = DefaultScreen(dc->dpy);
	window = XCreateSimpleWindow(dc->dpy, RootWindow(dc->dpy, screen),
		x, y, w, h, 1, col[ColorBG], col[ColorFG]);
	drawable = XCreatePixmap(dc->dpy, window, w, h,
		DefaultDepth(dc->dpy, screen));
	XSelectInput(dc->dpy, window, EVENTMASK);
	XMapWindow(dc->dpy, window);
	return window;
}

int
swk_gi_init(SwkWindow *w) {
	int i;
	char buf[128];
	if(first) {
		first = 0;
		if (!(dc = dc_init()))
			return 0;
		for(i=0;i<ColorLast;i++) {
			sprintf(buf, "#%06x", colors[i]);
			col[i] = dc_color(dc, buf);
		}
		dc_font(dc, FONTNAME);
		// TODO: must be dc_window(dc, x, y, w, h, bg, fg)
		window = dc_window(dc, 10, 10, w->r.w, w->r.h);
	}
	return swk_gi_fontsize(0);
}

int
swk_gi_update(SwkWindow *w) {
	XWindowAttributes wa;
	XGetWindowAttributes(dc->dpy, window, &wa);
	w->r.w = (wa.width / fs)-1;
	w->r.h = (wa.height / fs)-1;
	return 1;
}

void
swk_gi_exit() {
	dc_free(dc);
}

SwkEvent *
swk_gi_event(SwkWindow *w, int dowait) {
	static int mousedowny, mousedownx, mousedown = 0;
	static int mousemoved = 0;
	KeySym ksym;
	XEvent event;
	SwkEvent *ret = &w->_e;

	if(!XCheckMaskEvent(dc->dpy, -1, &event))
		return NULL;
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
		case 65362:
			ret->data.key.keycode = KUp;
			break;
		case 65364:
			ret->data.key.keycode = KDown;
			break;
		case 65361:
			ret->data.key.keycode = KLeft;
			break;
		case 65363:
			ret->data.key.keycode = KRight;
			break;
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
	case 0:
		ret->type = EQuit;
		break;
	default:
		ret = NULL;
		break;
	}
	return ret;
}

void
swk_gi_clear() {
	Rect r ={0};
	XWindowAttributes wa;
	XGetWindowAttributes(dc->dpy, window, &wa);
	dc_resize(dc, wa.width, wa.height);
	r.w=wa.width;
	r.h=wa.height;
	swk_gi_fill(r, ColorBG, 0);
}

void
swk_gi_flip() {
#if 0
	XWindowAttributes wa;
	XGetWindowAttributes(dc->dpy, window, &wa);
	XCopyArea(dc->dpy, drawable, window, gc, 0, 0, wa.width, wa.height, 0, 0);
	XFlush(dc->dpy);
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
	XSetForeground(dc->dpy, dc->gc, col[color]);
	XFillRectangles(dc->dpy, window, dc->gc, &area, 1);
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
	XSetForeground(dc->dpy, dc->gc, col[ColorFG]);
	XDrawString(dc->dpy, window, dc->gc, r.x*fs, ((1+r.y)*fs)-3, text, strlen (text));
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
