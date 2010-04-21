#include <SDL/SDL.h>
#include <SDL/SDL_ttf.h>
#include "swk.h"

#define HICOLOR 0xa0,0x00,0x00
#define FGCOLOR 0xa0,0xa0,0xa0
#define BGCOLOR 0x00,0x00,0x00
#define TFCOLOR 0xff,0xff,0xff
#define FONTNAME "Inconsolata.otf"
//#define FONTSIZE 16
#define FONTSIZE 14
#define FS FONTSIZE
#define BPP 32
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
	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Cannot initialize SDL\n");
		return 0;
	}
	if (TTF_Init()==-1) {
		fprintf(stderr, "Cannot initialize TTF: %s\n", TTF_GetError());
		return 0;
	}
	SDL_SetVideoMode(w->r.w, w->r.h, BPP, SDL_DOUBLEBUF|SDL_RESIZABLE);
	SDL_WM_SetCaption(w->title, NULL);
	screen = SDL_GetVideoSurface();
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	pal[ColorFG] = SDL_MapRGB(screen->format, FGCOLOR);
	pal[ColorBG] = SDL_MapRGB(screen->format, BGCOLOR);
	pal[ColorHI] = SDL_MapRGB(screen->format, HICOLOR);
	font = TTF_OpenFont(FONTNAME, FONTSIZE); 
	if (font == NULL) {
		fprintf(stderr, "Cannot open font '%s'\n", FONTNAME);
		return 0;
	} //else TTF_SetFontStyle(font, TTF_STYLE_BOLD);
	return 1;
}

int
swk_gi_update(SwkWindow *w) {
	SDL_SetVideoMode(screen->w, screen->h, BPP, SDL_DOUBLEBUF|SDL_RESIZABLE);
	screen = SDL_GetVideoSurface();
	w->r.w = screen->w / FS;
	w->r.h = screen->h / FS;
	return 1;
}

void
swk_gi_exit() {
	SDL_Quit();
}


static int has_event = 0;
static SDL_Event lastev = {.type=-1};

int
swk_gi_has_event() {
	if (!has_event)
		has_event = SDL_PollEvent(&lastev);
	return has_event;
}

SwkEvent *
swk_gi_event(int dowait) {
	SDL_Event event;
	static SwkEvent ev;
	SwkEvent *ret = NULL;

	if(has_event) {
		event = lastev;
	} else has_event = SDL_WaitEvent(&event);

	if (has_event);
	switch(event.type) {
	case SDL_VIDEORESIZE:
		fprintf(stderr, "resize %d %d\n", event.resize.w, event.resize.h);
		SDL_SetVideoMode(event.resize.w, event.resize.h,
			32, SDL_DOUBLEBUF|SDL_RESIZABLE);
	case SDL_VIDEOEXPOSE:
		ret = &ev;
		ev.type = EExpose;
		ev.data.expose.x = ev.data.expose.y = \
		ev.data.expose.w = ev.data.expose.h = 0;
		break;
	case SDL_MOUSEMOTION:
		ret = &ev;
		ev.type = EMotion;
		ev.data.motion.x = event.motion.x / FS;
		ev.data.motion.y = event.motion.y / FS;
	//	fprintf(stderr, "event: motion %d %d\n",
	//		event.motion.x, event.motion.y);
		break;
	case SDL_MOUSEBUTTONDOWN:
		ret = &ev;
		ev.type = EClick;
		ev.data.click.button = event.button.button;
		ev.data.click.point.x = event.button.x / FS;
		ev.data.click.point.y = event.button.y / FS;
		fprintf(stderr, "event: click %d\n", event.button.button);
		break;
	case SDL_KEYDOWN:
		if (ev.data.key.keycode != 0 && event.key.keysym.unicode != 0) {
			ret = &ev;
			ev.type = EKey;
			ev.data.key.keycode = event.key.keysym.unicode;
			ev.data.key.modmask = 0;
			if(event.key.keysym.mod & KMOD_CTRL)
				ev.data.key.modmask |= Ctrl;
			if(event.key.keysym.mod & KMOD_SHIFT)
				ev.data.key.modmask |= Shift;
			if(event.key.keysym.mod & KMOD_ALT)
				ev.data.key.modmask |= Alt;
			if(event.key.keysym.mod & KMOD_META)
				ev.data.key.modmask |= Meta;
			fprintf(stderr, "event: key %d %d\n", 
				ev.data.key.modmask, ev.data.key.keycode);
		}
		break;
	case SDL_QUIT:
		fprintf(stderr, "event: quit\n");
		ev.type = ev.type = EQuit;
		ret = &ev;
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
swk_gi_line(int x, int y, int w, int h, int color) {
	int i;
	x *= FS; y *= FS;
	w *= FS; h *= FS;

	if (w==0) for(i=0;i<h;i++) putpixel(x, y+i, pal[color]);
	else
	if (h==0) for(i=0;i<w;i++) putpixel(x+i, y, pal[color]);
}

void
swk_gi_fill(int x, int y, int w, int h, int color) {
	SDL_Rect area = { x*FS, y*FS, w*FS, h*FS };
	SDL_FillRect(screen, &area, pal[color]);
}

void
swk_gi_rect(int x, int y, int w, int h, int color) {
	swk_gi_line(x, y, w, 0, color);
	swk_gi_line(x, y+h, w, 0, color);
	swk_gi_line(x, y, 0, h, color);
	swk_gi_line(x+w, y, 0, h, color);
}

void
swk_gi_text(int x, int y, const char *text) {
	SDL_Surface *ts = TTF_RenderText_Solid(font, text, fontcolor);
	if (ts) {
		SDL_Rect to = { x*FS, y*FS, ts->w, ts->h };
		SDL_BlitSurface(ts, NULL, screen, &to);
		SDL_FreeSurface(ts);
	} else fprintf(stderr, "Cannot render string (%s)\n", text);
}
