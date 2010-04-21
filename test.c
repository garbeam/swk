#include <stdio.h>
#include "swk.h"

static int count = 3;
static char text[64];
static SwkBox helloworld[];

static void mybutton(SwkEvent *e) {
	if (e->type == EClick) {
		sprintf(text, "Do it again %d times\n", count);
		helloworld[0].text = text;
		if (count-- == 0)
			swk_exit();
	}
	swk_button(e);
}


static SwkBox helloworld[] = {
	{ SWK_NEWLINE(1) },
	{ .cb=swk_label, .text="Press a button", },
	{ SWK_NEWLINE(2) },
	{ .cb=mybutton, .text="yes" },
	{ .cb=swk_filler, },
	{ .cb=mybutton, .text="no" },
#if 0
	{ SWK_NEWLINE(1) },
	{ .cb=mybutton, .text="yes" },
	{ .cb=mybutton, .text="no" },
#endif
	{ SWK_NEWLINE(5) },
	{ .cb=swk_label, .text="--swktest", },
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
