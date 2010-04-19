#include <SDL/SDL.h>
#include "swk.h"

int
swk_gi_init(int w, int h) {
	if (SDL_Init(SDL_INIT_VIDEO)) {
		fprintf(stderr, "Cannot initialize SDL\n");
		return 0;
	}
	SDL_SetVideoMode(w, h, 32, SDL_HWSURFACE);
	SDL_EnableUNICODE(1);
	SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY, SDL_DEFAULT_REPEAT_INTERVAL);
	return 1;
}

void
swk_gi_exit() {
	SDL_Quit();
}

static char sdlkeys[256] = { 0 }; // TODO

SwkEvent *
swk_event(int lock) {
	int evret;
	SDL_Event event;
	static SwkEvent ev;
	SwkEvent *ret = NULL;

	if (lock) evret = SDL_WaitEvent(&event);
	else evret = SDL_PollEvent(&event);
	if (evret) {
		switch (event.type) {
		case SDL_KEYDOWN:
			ret = &ev;
			ev.type = EKey;
			ev.data.key.keycode = sdlkeys[event.key.keysym.sym];
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
			break;
		case SDL_MOUSEBUTTONDOWN:
			ret = &ev;
			ev.type = EClick;
			//ev.data.click.button = 
			break;
		case SDL_QUIT:
			ev.type = ev.type = EQuit;
			ret = &ev;
			break;
		}
	}
	return ret;
}

void
swk_gi_flip() {
	/* TODO */
}
