/* Copyleft 2010 -- pancake <nopcode.org> */

#include <stdio.h>
#include <stdlib.h>
#include "swk.h"

int swkret = 0;
SwkScene swkscene;

int swk_init () {
	swkscene.window = NULL;
	swkscene.windows = NULL;
}

SwkWindow *swk_widget () {
}

SwkWindow *swk_box () {
}

SwkWindow *swk_box_add (SwkBox *box) {
}

// TODO: rename to box_layout ??
int swk_window_layout_flow (SwkWindow *window) {
	return 0;
}

int swk_box_layout_flow (SwkBox *box) {
	return 0;
}

SwkWindow *swk_window () {
	SwkWindow *w = (SwkWindow *) malloc (sizeof (SwkWindow));
	w->next = NULL;
	if (swkscene.window) swkscene.window->next = w;
	else swkscene.windows = w;
	swkscene.window = w;
	return w;
}

int swk_window_add (SwkWindow *w) {
}

SwkWidget* swk_label () {
	/* .. */
}

int swk_window_delete (SwkWindow *w) {
	SwkWindow *iter = swkscene.window;
	// XXX
	while (iter) {
		if (iter == w)
			return 1;
		iter = iter->next;
	}
	return 0;
}

int swk_event () {
}

int swk_redraw () {
}
