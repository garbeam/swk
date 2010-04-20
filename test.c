#include <stdio.h>
#include "swk.h"

static int count = 3;

static int mybutton(SwkEvent *e) {
	if (e->type == EClick) {
		fprintf(stderr, "Button clicked %d\n", count);
		if (count-- == 0)
			swk_exit();
	}
	return swk_button(e);
}

static SwkBox helloworld[] = {
	{ .cb=swk_label, .text="Press this button", },
	{ SWK_NEWLINE },
	{ .cb=swk_filler, },
	{ .cb=mybutton, .text="clickme" },
	{ .cb=NULL }
};

int
main() {
	SwkWindow w = {
		.title="Hello World",
		.boxes=helloworld
	};

	if (!swk_init(&w))
		return 1;
	swk_loop();
	return 0;
}
