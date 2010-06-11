/* See LICENSE file for copyright and license details. */
#define _BSD_SOURCE // strdup
#include <SDL/SDL.h>
#include <SDL/SDL_image.h>
#include <SDL/SDL_ttf.h>
#include "swk.h"
#define SWK
#include "config.h"

#define FONTNAME "Inconsolata.otf"
#define FONTFACTOR 2.1 /* XXX */
#define BPP 32
#define SDLFLAGS SDL_DOUBLEBUF|SDL_RESIZABLE

static int first = 1;
static int fs = FONTSIZE; // TODO: we need fsW and fsH
static Uint32 pal[ColorLast];
static SDL_Color fontcolor = { TFCOLOR };
static SDL_Color bgcolor = { BGCOLOR };
static SDL_Surface *screen = NULL;
static TTF_Font *font = NULL;
/* FIXME: put ugly statics into void *aux of SwkWindow ? */
static int has_event = 0;
static SDL_Event lastev = { .type=-1 };

static inline Uint8 *
getscrpoint(SDL_Surface *scr, int x, int y) {
	Uint8 *p = (Uint8 *)scr->pixels + (y*scr->pitch+x*(BPP/8));
	Uint8 *pend = (Uint8 *)scr->pixels + (scr->h*scr->w*(BPP/8));
	if((p<((Uint8 *)scr->pixels)) || (p>=pend))
		return NULL;
	return p;
}

static void
putpixel(SDL_Surface *scr, int x, int y, Uint32 pixel) { 
	Uint8 *p = getscrpoint(scr, x, y);
	if(!p) return;
#if BPP == 8
	*p = pixel;
#elif BPP == 16
	*(Uint16 *)p = pixel; 
#elif BPP == 24
# if SDL_BYTEORDER == SDL_BIG_ENDIAN
	p[0] = (pixel >> 16) & 0xff;
	p[1] = (pixel >> 8) & 0xff;
	p[2] = pixel & 0xff;
# else
	p[0] = pixel & 0xff;
	p[1] = (pixel >> 8) & 0xff;
	p[2] = (pixel >> 16) & 0xff;
# endif
#elif BPP == 32
	*(Uint32 *)p = pixel;
#endif
}

int
swk_gi_fontsize(int sz) {
	fs += sz*2;
	font = TTF_OpenFont(FONTNAME, fs); 
	if(font == NULL) {
		fprintf(stderr, "Cannot open font '%s'\n", FONTNAME);
		return 0;
	} else
	if(FONTBOLD)
		TTF_SetFontStyle(font, TTF_STYLE_BOLD);
	return 1;
}

int
swk_gi_init(SwkWindow *w) {
	if(first) {
		if(SDL_Init(SDL_INIT_VIDEO)) {
			fprintf(stderr, "Cannot initialize SDL\n");
			return 0;
		}
		if(TTF_Init()==-1) {
			fprintf(stderr, "Cannot initialize TTF: %s\n", TTF_GetError());
			return 0;
		}
		first = 0;
	}
	SDL_SetVideoMode(w->r.w, w->r.h, BPP, SDLFLAGS);
	// double init is necesary to get window size
	SDL_SetVideoMode(w->r.w, w->r.h, BPP, SDLFLAGS);
	SDL_WM_SetCaption(w->title, NULL);
	screen = SDL_GetVideoSurface();
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	pal[ColorFG] = SDL_MapRGB(screen->format, FGCOLOR);
	pal[ColorBG] = SDL_MapRGB(screen->format, BGCOLOR);
	pal[ColorHI] = SDL_MapRGB(screen->format, HICOLOR);
	return swk_gi_fontsize(0);
}

int
swk_gi_update(SwkWindow *w) {
	screen = SDL_GetVideoSurface();
	if(screen == NULL)
		return 0;
	w->r.w = (screen->w / fs)-1;
	w->r.h = (screen->h / fs)-1;
	return 1;
}

void
swk_gi_exit() {
	SDL_Quit();
}

int
swk_gi_has_event(SwkWindow *w) {
	if(!has_event)
		has_event = SDL_PollEvent(&lastev);
	return has_event;
}

SwkEvent *
swk_gi_event(SwkWindow *w, int dowait) {
	static int mousedowny, mousedownx, mousedown = 0;
	static int mousemoved = 0;
	SDL_Event event;
	SwkEvent *ret = &w->_e;

	if(has_event) event = lastev;
	else has_event = SDL_WaitEvent(&event);

	if(has_event);
	switch(event.type) {
	default: ret = NULL; break;
	case SDL_VIDEORESIZE:
		//fprintf(stderr, "resize %d %d\n", event.resize.w, event.resize.h);
		SDL_SetVideoMode(event.resize.w, event.resize.h, BPP, SDLFLAGS);
	case SDL_ACTIVEEVENT:
	case SDL_VIDEOEXPOSE:
		ret->type = EExpose;
		ret->data.expose.x = ret->data.expose.y = \
		ret->data.expose.w = ret->data.expose.h = 0;
		break;
	case SDL_MOUSEMOTION:
		// TODO: move this stuff into swk.c.. shoudlnt be backend dependent
		//fprintf(stderr, "event: motion (%d,%d)\n", event.motion.x,event.motion.y);
		if(mousedown) {
			// touchscreen spaguetti trick
			if(mousedowny==-1) mousedowny = event.motion.y; else mousemoved = 1;
			if(mousedownx==-1) mousedownx = event.motion.x; else mousemoved = 1;
			if(event.motion.y>mousedowny+fs) {
				mousedowny = event.motion.y;
				swk_scroll_up(w);
			} else
			if(event.motion.y<mousedowny-fs) {
				mousedowny = event.motion.y;
				swk_scroll_down(w);
			}
			if(event.motion.x>mousedownx+fs) {
				mousedownx = event.motion.x;
				swk_column_move_right();
				swk_column_move_right();
			} else
			if(event.motion.x<mousedownx-fs) {
				mousedownx = event.motion.x;
				swk_column_move_left();
				swk_column_move_left();
			}
			ret->type = EExpose;
			ret->data.expose.x = ret->data.expose.y = \
			ret->data.expose.w = ret->data.expose.h = 0;
		} else {
			ret->type = EMotion;
			ret->data.motion.x = event.motion.x / fs;
			ret->data.motion.y = event.motion.y / fs;
		}
		break;
	case SDL_MOUSEBUTTONUP:
		//fprintf(stderr, "event: up %d (%d,%d)\n", event.button.button,event.button.x,event.button.y);

		mousedown = 0;
		if(!mousemoved) {
			ret->type = EClick;
			ret->data.click.button = event.button.button;
			ret->data.click.point.x = event.button.x / fs;
			ret->data.click.point.y = event.button.y / fs;
		}
		break;
	case SDL_MOUSEBUTTONDOWN:
		//fprintf(stderr, "event: down %d (%d,%d)\n", event.button.button,event.button.x,event.button.y);
		mousemoved = 0;
		mousedown = 1;
		mousedowny = TOUCHSCREEN?-1:event.button.y;
		break;
	case SDL_KEYDOWN:
		ret->data.key.modmask = 0;
		ret->type = EKey;
		if(event.key.keysym.mod & KMOD_CTRL)
			ret->data.key.modmask |= Ctrl;
		if(event.key.keysym.mod & KMOD_SHIFT)
			ret->data.key.modmask |= Shift;
		if(event.key.keysym.mod & KMOD_ALT)
			ret->data.key.modmask |= Alt;
		if(event.key.keysym.mod & KMOD_META)
			ret->data.key.modmask |= Meta;
		if(ret->data.key.keycode != 0 && event.key.keysym.unicode != 0) {
			ret->data.key.keycode = event.key.keysym.unicode;
		} else // TODO key aliases defined in config.h
		switch((int)event.key.keysym.sym) {
		case 1073741906: // n900 up key
		case 273:
			ret->data.key.keycode = KUp;
			break;
		case 1073741912: // n900 down key
		case 274:
			ret->data.key.keycode = KDown;
			break;
		default:
			ret->data.key.keycode = event.key.keysym.sym;
			break;
		}
		fprintf(stderr, "event: key %d %d\n", 
			ret->data.key.modmask, ret->data.key.keycode);
		break;
	case SDL_QUIT:
		ret->type = EQuit;
		break;
	}
	has_event = 0;
	return ret;
}

void
swk_gi_clear() {
	SDL_Rect rect = { 0, 0, screen->w, screen->h };
	SDL_FillRect(screen, &rect, pal[ColorBG]);
}

void
swk_gi_flip() {
	SDL_LockSurface(screen);
	SDL_UpdateRect(screen, 0, 0, screen->w, screen->h); 
	SDL_UnlockSurface(screen);
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
	SDL_Rect area = { r.x*fs, r.y*fs, r.w*fs, r.h*fs };
	if(lil) {
		const int s = fs/4;
		area.x += s;
		area.y += s;
		area.w -= (s*2);
		area.h -= (s*2);
	}
	if(!area.w) area.w = 1;
	if(!area.h) area.h = 1;
	SDL_FillRect(screen, &area, pal[color]);
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
	const char *tptr = text;
	char *ptr = NULL;
	int w = (int)((double)r.w * FONTFACTOR);
	if(text && *text) {
		int len = text?strlen(text):0;
		if(len>w) {
			tptr = ptr = strdup(text);
			ptr[w]='\0';
		}
		SDL_Surface *ts = TTF_RenderText_Shaded(font, tptr, fontcolor, bgcolor);
		if(ts) {
			SDL_Rect to = { (r.x)*fs, r.y*fs, ts->w, ts->h };
			SDL_BlitSurface(ts, NULL, screen, &to);
			SDL_FreeSurface(ts);
		} else fprintf(stderr, "Cannot render string (%s)\n", text);
	}
	free(ptr);
}

void
swk_gi_img(Rect r, void *img) {
	if(img) {
		SDL_Rect area = { r.x*fs, r.y*fs, r.w*fs, r.h*fs };
		area.x++; area.y++;
		SDL_BlitSurface((SDL_Surface*)img, NULL, screen, &area);
	}
}

/* image api */
void*
swk_gi_img_new(int w, int h, int color) {
	return (void *)SDL_CreateRGBSurface(0, (w*fs)-2, (h*fs)-2, BPP, 0, 0, 0, 0);
}

void*
swk_gi_img_load(const char *str) {
	return IMG_Load(str);
}

void
swk_gi_img_free(void *s) {
	SDL_FreeSurface(s);
}

void
swk_gi_img_set(void *img, int x, int y, int color) {
	if(img) putpixel((SDL_Surface*)img, x, y, pal[color]);
}

int
swk_gi_img_get(void *img, int x, int y) {
	Uint8 *p = getscrpoint(img, x, y);
	return p?*p:0;
}
