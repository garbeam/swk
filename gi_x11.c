/* See LICENSE file for copyright and license details. */
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

//#define FONTNAME "-*-*-medium-*-*-*-14-*-*-*-*-*-*-*"
#define FONTNAME "10x20"

static int fs = FONTSIZE; // TODO: we need fsW and fsH
static Window window;
static XWindowAttributes wa;
static DC *dc = NULL;
static int col[ColorLast];
static int colors[ColorLast] = { FGCOLOR, BGCOLOR, HICOLOR, TFCOLOR, CCCOLOR };
#define EVENTMASK PointerMotionMask | ExposureMask | KeyPressMask | ButtonPressMask | ButtonReleaseMask

static Window /* TODO: push into libdraw */
dc_window(DC *dc, int x, int y, int w, int h) {
	Window window;
	window = XCreateSimpleWindow(dc->dpy, RootWindow(dc->dpy, DefaultScreen(dc->dpy)),
		x, y, w, h, 1, col[ColorBG], col[ColorFG]);
	XSelectInput(dc->dpy, window, EVENTMASK);
	XMapWindow(dc->dpy, window);
	return window;
}
static void dc_window_title(Window w, const char *title) {
	XSetStandardProperties(dc->dpy, window, title, NULL, None, NULL, 0, NULL);
}

int
swk_gi_fontsize(int sz) {
	fs += sz*2;
	/* TODO: resize font */
	return 1;
}

int
swk_gi_init(SwkWindow *w) {
	char buf[128];
	int i;
	if (dc) return 0;
	dc = dc_init();
	for(i=0;i<ColorLast;i++) {
		sprintf(buf, "#%06x", colors[i]);
		col[i] = dc_color(dc, buf);
	}
	dc_font(dc, FONTNAME);
	// TODO: must be dc_window(dc, x, y, w, h, bg, fg)
	window = dc_window(dc, 10, 10, w->r.w, w->r.h);
	dc_window_title(window, w->title);
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
	dc = NULL;
}

SwkEvent *
swk_gi_event(SwkWindow *w, int dowait) {
	static int mousedowny, mousedownx, mousedown = 0;
	KeySym ksym;
	XEvent event;
	SwkEvent *ret = &w->_e;

	if(!dowait && !XPending(dc->dpy))
		return NULL;
	XNextEvent(dc->dpy, &event);
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
		ret->type = EClick;
		switch(event.xbutton.state) {
		case 4096: // 0x1000
			ret->data.click.button = 4;
			break;
		case 2048: // 0x800
			ret->data.click.button = 5;
			break;
		case 1024: // 0x400
			ret->data.click.button = 2;
			break;
		case 512: // 0x200
			ret->data.click.button = 3;
			break;
		case 256: // 0x100
			ret->data.click.button = 1;
			break;
		}
		ret->data.click.point.x = event.xbutton.x / fs;
		ret->data.click.point.y = event.xbutton.y / fs;
		break;
	case ButtonPress:
		//fprintf(stderr, "event: down %d (%d,%d)\n", event.button.button,event.button.x,event.button.y);
		mousedown = 1;
		mousedowny = event.xbutton.y;
		break;
	case KeyPress:
		ret->type = EKey;
		XLookupString(&event.xkey, NULL, 0, &ksym, NULL);
		printf("ksym=%d\n", (int)ksym);

		switch(ksym) {
		case 65535: // supr
			ret->data.key.keycode = 127;
			break;
		case 65511:
			ret->data.key.keycode = KUp;
			break;
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
	Rect r = {0};
	XGetWindowAttributes(dc->dpy, window, &wa);
	dc_resize(dc, wa.width, wa.height);
	r.w = wa.width; // TODO: propagate those values into SwkWindow?
	r.h = wa.height;
	swk_gi_fill(r, ColorBG, 0);
}

void
swk_gi_flip() {
	dc_map(dc, window, wa.width, wa.height);
}

/* -- drawing primitives -- */
void
swk_gi_line(int x1, int y1, int x2, int y2, int color) {
	XSetForeground(dc->dpy, dc->gc, col[color]);
	XDrawLine(dc->dpy, dc->canvas, dc->gc, x1*fs, y1*fs, (x1+x2)*fs, (y1+y2)*fs);
}

void
swk_gi_fill(Rect r, int color, int lil) {
	XRectangle area = { r.x*fs, r.y*fs, r.w*fs, r.h*fs };
	if(lil==1) {
		int s = fs/4;
		area.x += s;
		area.y += s;
		area.width -= (s*2);
		area.height -= (s*2);
	} else
	if(lil==2) {
		area.x/=3;
		area.x-=2;
		area.width=2;///=4;
		area.y+=4;
		area.height-=4;
	} else if (lil==3) {
		const int s = fs/4;
		area.width -= (s*2);
		area.height -= (s*4);
	}
	if(area.width<1) area.width = 1;
	if(area.height<1) area.height = 1;
	XSetForeground(dc->dpy, dc->gc, col[color]);
	XFillRectangles(dc->dpy, dc->canvas, dc->gc, &area, 1);
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
	// TODO: use libdraw to get length of string and support utf8..
	// TODO: retrieve character width before rendering
	XmbDrawString(dc->dpy, dc->canvas, dc->font.set, dc->gc,
		5+(r.x*fs), ((r.y+1)*fs-3), text, strlen(text));
}

void
swk_gi_img(Rect r, void *_img) {
	SwkImage *img = _img;
	if(img)
		XPutImage(dc->dpy, dc->canvas, DefaultGC(dc->dpy, 0), img->pub,
			0, 0, r.x*fs, r.y*fs, img->w, img->h);
}

void*
swk_gi_img_new(int w, int h, int color) {
	SwkImage *img = img_open(NULL);
	img->w = w*fs;
	img->h = h*fs;
	img->bpp = 24;
	img->priv = NULL;
	img->name = NULL;
	img->data = malloc(img->w*img->h*4);
	memset(img->data, colors[color]&0xff, img->w*img->h*4);
	img->pub = XCreateImage(dc->dpy, DefaultVisual(dc->dpy, 0), 24, ZPixmap,
		0, img->data, img->w, img->h, 32, 0);
	return img;
}

void*
swk_gi_img_load(const char *str) {
	SwkImage *img = img_open(str);
	if (img == NULL)
		return NULL;
	img->pub = XCreateImage(dc->dpy, DefaultVisual(dc->dpy, 0), 24, ZPixmap,
		0, img->data, img->w, img->h, 32, 0);
	return img;
}

void
swk_gi_img_free(void *s) {
	img_free(s);
}

void
swk_gi_img_set(void *_img, int x, int y, int color) {
	SwkImage *img = _img;
	int *ptr = img->data;
	if(ptr) ptr[(y*img->w)+x] = color;
}

int
swk_gi_img_get(void *_img, int x, int y) {
	SwkImage *img = _img;
	int *ptr = img->data;
	return ptr?ptr[(y*img->w)+x]:0;
}
