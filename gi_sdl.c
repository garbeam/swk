/* See LICENSE file for copyright and license details. */
#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "swk.h"

#define HICOLOR 0x00,0x66,0xff
#define FGCOLOR 0xff,0xff,0xff
#define BGCOLOR 0x00,0x00,0x00
#define TFCOLOR 0xcc,0xcc,0xcc
#define FONTNAME "Inconsolata.otf"
#define FONTSIZE 14
#define FS FONTSIZE
#define BPP 32
#define SDLFLAGS SDL_DOUBLEBUF|SDL_RESIZABLE
/* --- */

static Uint32 pal[ColorLast];
static SDL_Color fontcolor = { TFCOLOR };
static SDL_Surface *screen = NULL;
static TTF_Font *font = NULL;

static void putpixel(int x, int y, Uint32 pixel) { 
	int bpp = screen->format->BytesPerPixel;
	Uint8 *p;
	p = (Uint8 *)screen->pixels + y * screen->pitch + x * bpp; 
#if BPP == 8
	*p = pixel; 
#elif BPP == 16
	*(Uint16 *)p = pixel; 
#elif BPP == 24
	#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		p[0] = (pixel >> 16) & 0xff; 
		p[1] = (pixel >> 8) & 0xff; 
		p[2] = pixel & 0xff; 
	#else
		p[0] = pixel & 0xff; 
		p[1] = (pixel >> 8) & 0xff; 
		p[2] = (pixel >> 16) & 0xff; 
	#endif
#elif BPP == 32
	*(Uint32 *)p = pixel; 
#endif
}

int
swk_gi_init(SwkWindow *w) {
	if(SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Cannot initialize SDL\n");
		return 0;
	}
	if(TTF_Init()==-1) {
		fprintf(stderr, "Cannot initialize TTF: %s\n", TTF_GetError());
		return 0;
	}
	SDL_SetVideoMode(w->r.w, w->r.h, BPP, SDLFLAGS);
	SDL_WM_SetCaption(w->title, NULL);
	screen = SDL_GetVideoSurface();
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	pal[ColorFG] = SDL_MapRGB(screen->format, FGCOLOR);
	pal[ColorBG] = SDL_MapRGB(screen->format, BGCOLOR);
	pal[ColorHI] = SDL_MapRGB(screen->format, HICOLOR);
	font = TTF_OpenFont(FONTNAME, FONTSIZE); 
	if(font == NULL) {
		fprintf(stderr, "Cannot open font '%s'\n", FONTNAME);
		return 0;
	} else TTF_SetFontStyle(font, TTF_STYLE_BOLD);
	return 1;
}

int
swk_gi_update(SwkWindow *w) {
	screen = SDL_GetVideoSurface();
	w->r.w = screen->w / FS;
	w->r.h = screen->h / FS;
	return 1;
}

void
swk_gi_exit() {
	SDL_Quit();
}

/* FIXME: put ugly statics into void *aux of SwkWindow */
static int has_event = 0;
static SDL_Event lastev = {.type=-1};

int
swk_gi_has_event(SwkWindow *w) {
	if(!has_event)
		has_event = SDL_PollEvent(&lastev);
	return has_event;
}

SwkEvent *
swk_gi_event(SwkWindow *w, int dowait) {
	SDL_Event event;
	SwkEvent *ret = &w->_e;

	if(has_event) event = lastev;
	else has_event = SDL_WaitEvent(&event);

	if(has_event);
	switch(event.type) {
	default: ret = NULL; break;
	case SDL_VIDEORESIZE:
		fprintf(stderr, "resize %d %d\n", event.resize.w, event.resize.h);
		SDL_SetVideoMode(event.resize.w, event.resize.h, BPP, SDLFLAGS);
	case SDL_VIDEOEXPOSE:
		ret->type = EExpose;
		ret->data.expose.x = ret->data.expose.y = \
		ret->data.expose.w = ret->data.expose.h = 0;
		break;
	case SDL_MOUSEMOTION:
		ret->type = EMotion;
		ret->data.motion.x = event.motion.x / FS;
		ret->data.motion.y = event.motion.y / FS;
	//	fprintf(stderr, "event: motion %d %d\n",
	//		event.motion.x, event.motion.y);
		break;
	case SDL_MOUSEBUTTONDOWN:
		ret->type = EClick;
		ret->data.click.button = event.button.button;
		ret->data.click.point.x = event.button.x / FS;
		ret->data.click.point.y = event.button.y / FS;
		fprintf(stderr, "event: click %d\n", event.button.button);
		break;
	case SDL_KEYDOWN:
		if(ret->data.key.keycode != 0 && event.key.keysym.unicode != 0) {
			ret->type = EKey;
			ret->data.key.keycode = event.key.keysym.unicode;
			ret->data.key.modmask = 0;
			if(event.key.keysym.mod & KMOD_CTRL)
				ret->data.key.modmask |= Ctrl;
			if(event.key.keysym.mod & KMOD_SHIFT)
				ret->data.key.modmask |= Shift;
			if(event.key.keysym.mod & KMOD_ALT)
				ret->data.key.modmask |= Alt;
			if(event.key.keysym.mod & KMOD_META)
				ret->data.key.modmask |= Meta;
			fprintf(stderr, "event: key %d %d\n", 
				ret->data.key.modmask, ret->data.key.keycode);
		}
		break;
	case SDL_QUIT:
		fprintf(stderr, "event: quit\n");
		ret->type = ret->type = EQuit;
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
	SDL_UpdateRect(screen, 0, 0, screen->w, screen->h); 
}

/* -- drawing primitives -- */

void
swk_gi_line(int x1, int y1, int x2, int y2, int color) {
	int i;
	x1 *= FS; y1 *= FS;
	x2 *= FS; y2 *= FS;

	if(x2==0) for(i=0;i<y2;i++) putpixel(x1, y1+i, pal[color]);
	else
	if(y2==0) for(i=0;i<x2;i++) putpixel(x1+i, y1, pal[color]);
}

void
swk_gi_fill(Rect r, int color) {
	SDL_Rect area = { r.x*FS, r.y*FS, r.w*FS, r.h*FS };
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
	if(*text) {
		SDL_Surface *ts = TTF_RenderText_Solid(font, text, fontcolor);
		if(ts) {
			SDL_Rect to = { (r.x)*FS, r.y*FS, ts->w, ts->h };
			SDL_BlitSurface(ts, NULL, screen, &to);
			SDL_FreeSurface(ts);
		} else fprintf(stderr, "Cannot render string (%s)\n", text);
	}
}
