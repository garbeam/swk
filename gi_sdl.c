#include <SDL/SDL.h>
#include "swk.h"

static SDL_Surface *screen = NULL;

int
swk_gi_init(SwkWindow *w) {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Cannot initialize SDL\n");
		return 0;
	}
	SDL_SetVideoMode(w->r.w, w->r.h, 32, SDL_DOUBLEBUF|SDL_RESIZABLE);
	screen = SDL_GetVideoSurface();
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	return 1;
}

int
swk_gi_update(SwkWindow *w) {
	SDL_SetVideoMode(screen->w, screen->h, 32, SDL_DOUBLEBUF|SDL_RESIZABLE);
	screen = SDL_GetVideoSurface();
	return 1;
}

void
swk_gi_exit() {
	SDL_Quit();
}

SwkEvent *
swk_gi_event(int dowait) {
	int evret;
	SDL_Event event;
	static SwkEvent ev;
	SwkEvent *ret = NULL;

	if (dowait) evret = SDL_WaitEvent(&event);
	else evret = SDL_PollEvent(&event);

	if (evret)
	switch (event.type) {
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
		ev.data.motion.x = event.motion.x;
		ev.data.motion.y = event.motion.y;
		fprintf(stderr, "event: motion %d %d\n",
			event.motion.x, event.motion.y);
		break;
	case SDL_MOUSEBUTTONDOWN:
		ret = &ev;
		ev.type = EClick;
		ev.data.click.button = event.button.button;
		ev.data.click.point.x = event.button.x;
		ev.data.click.point.y = event.button.y;
		fprintf(stderr, "event: click %d\n", event.button.button);
		break;
	case SDL_KEYDOWN:
		if (ev.data.key.keycode != 0 && event.key.keysym.unicode != 0) {
			ret = &ev;
			ev.type = EKey;
			ev.data.key.keycode = event.key.keysym.unicode;
			ev.data.key.modmask = 0;
			if(event.key.keysym.mod & KMOD_LCTRL ||
			   event.key.keysym.mod & KMOD_RCTRL)
				ev.data.key.modmask |= Ctrl;
			if(event.key.keysym.mod & KMOD_LSHIFT||
			   event.key.keysym.mod & KMOD_RSHIFT)
				ev.data.key.modmask |= Shift;
			if(event.key.keysym.mod & KMOD_LMETA ||
			   event.key.keysym.mod & KMOD_RMETA)
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
	return ret;
}

void
swk_gi_flip() {
	fprintf(stderr, "flip\n");
	SDL_Flip(screen);
}

/* -- drawing primitives -- */
void
swk_gi_line(int x, int y, int w, int h) {
}

void
swk_gi_box(int x, int y, int w, int h) {
}

void
swk_gi_rect(int x, int y, int w, int h) {
}

void
swk_gi_text(int x, int y, const char *text) {
}
