#include <stdio.h>
#include "swk.h"

static SwkBox helloworld[] = {
	{ .cb=swk_label, .text="Press this button", },
	{ SWK_NEWLINE },
	{ .cb=swk_filler, },
	{ .cb=swk_button, .text="clickme" },
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
