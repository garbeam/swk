#include <stdio.h>
#include <swk.h>

static int count = 3;
static int pccount = 0;
static char text[64];
static char pctext[64];
static SwkBox helloworld[];
static SwkBox about[];
static SwkBox *opt = NULL;
static void mybutton(SwkEvent *e);
static void mybutton_about(SwkEvent *e);
static void mybutton_about_ok(SwkEvent *e);

static void mybutton(SwkEvent *e) {
	if(e->type == EClick) {
		sprintf(text, "Do it again %d times", count);
		helloworld[0].text = text;
		if(opt == NULL)
			printf("Option: none\n");
		else printf("Option: %s\n", opt->text);
		if(count-- == 0)
			swk_exit(e->win);
	}
	swk_bigbutton(e);
}

static void myprogressbutton(SwkEvent *e) {
	if(e->type == EClick) {
		pccount+=6;
		if(pccount > 100) {
			pccount = 0;
			e->win->boxes[e->win->col] = helloworld;
			swk_update(e->win);
		}
		sprintf(pctext, "%d%%", pccount);
		about[11].text = pctext;
	}
	swk_button(e);
}

static SwkBox about[] = {
	SWK_BOX_NEWLINE(0),
	{ .cb=swk_label, .text="About this program...", },
	SWK_BOX_NEWLINE(1),
	{ .cb=swk_separator },
	 SWK_BOX_NEWLINE(-1),
	//SWK_BOX_NEWLINE(2),
	{ .cb=swk_label, .text="This is a test program for swk" },
	SWK_BOX_NEWLINE(1),
	{ .cb=swk_label, .text=" ... a simple widget kit " },
	SWK_BOX_NEWLINE(1),
	{ .cb=swk_label, .text="    ... from the suckless.org project" },
	SWK_BOX_NEWLINE(2),
	{ .cb=swk_progress, .text="0%", },
	SWK_BOX_NEWLINE(2),
	{ .cb=swk_filler },
	{ .cb=myprogressbutton, .text="next", },
	{ .cb=swk_filler },
	//SWK_BOX_NEWLINE(3),
	{ .cb=swk_filler },
	{ .cb=mybutton_about_ok, .text="Ok" },
	{ .cb=NULL }
};

static void mybutton_about_ok(SwkEvent *e) {
	if(e->type == EClick) {
		e->win->boxes[e->win->col] = helloworld;
		swk_update(e->win);
	}
	swk_button(e);
}

static void mybutton_about(SwkEvent *e) {
	if(e->type == EClick) {
		e->win->boxes[e->win->col] = about;
		swk_update(e->win);
	}
	swk_button(e);
}

static void mybutton_close(SwkEvent *e) {
	if(e->type == EClick) {
		e->win->boxes[e->win->col] = helloworld;
		swk_update(e->win);
	}
	swk_button(e);
}

static SwkBox scrollwin[] = {
	SWK_BOX_NEWLINE(0),
	{ .cb=swk_label, .text="Scroll to change value", },
	SWK_BOX_NEWLINE(1),
	{ .cb=mybutton_close, .text="Close" },
	{ .cb=swk_separator },
	SWK_BOX_NEWLINE(1),
	{ .cb=swk_label, .text="  /etc" },
	SWK_BOX_NEWLINE(1),
	{ .cb=swk_button, .text="/bin" },
	SWK_BOX_NEWLINE(1),
	{ .cb=swk_label, .text="  /sbin" },
	SWK_BOX_NEWLINE(-1),
	{ .cb=NULL }
};

static void mybutton_numscroll(SwkEvent *e) {
	if(e->type == EClick) {
		e->win->boxes[e->win->col] = scrollwin;
		swk_update(e->win);
	}
	swk_button(e);
}

static SwkBox helloworld[] = {
	{ .cb=swk_label, .text="Press a button", },
	SWK_BOX_NEWLINE(1),
	{ .cb=swk_separator },
	SWK_BOX_NEWLINE(2),
	{ .cb=swk_label, .text="Username:", },
	{ .cb=swk_entry, .text="____", },
	SWK_BOX_NEWLINE(1),
	{ .cb=swk_label, .text="Password:", },
	{ .cb=swk_password, .text="1234", },
	SWK_BOX_NEWLINE(-1),
	{ .cb=swk_label, .text="Click here ->" },
	{ .cb=swk_sketch },
	SWK_BOX_NEWLINE(2),
	{ .cb=swk_image, .text="image.png" },
	{ .cb=swk_image, .text="image.png" },
	{ .cb=swk_image, .text="image.png" },
	{ .cb=swk_image, .text="image.png" },
	{ .cb=swk_filler, },
	SWK_BOX_NEWLINE(2),
	{ .cb=mybutton, .text="yes" },
	{ .cb=mybutton, .text="no" },
	{ .cb=swk_filler, },
	SWK_BOX_NEWLINE(2),
	{ .cb=swk_option, .text="zoot" },
	SWK_BOX_NEWLINE(1),
	{ .cb=swk_option, .text="remember values", .data=&opt },
//	{ .cb=swk_option, .text="null" },
	SWK_BOX_NEWLINE(1),
	{ .cb=swk_option, .text="pasta barata", .data=&opt },
	SWK_BOX_NEWLINE(2),
	{ .cb=swk_label, .text="--swktest", },
	{ .cb=mybutton_about, .text="about" },
	{ .cb=mybutton_numscroll, .text="num" },
	{ .cb=NULL }
};

static SwkBox column[] = {
	{ .cb=swk_label, .text="this is a column", },
	SWK_BOX_NEWLINE(-1),
	{ .cb=swk_label, .text="text in column", },
	{ .cb=NULL }
};

int
main() {
	SwkWindow w = {
		.title="Hello World",
		.boxes={ helloworld, column },
		.box=helloworld+10,
		//.r = { 0, 0, 320, 240 },
/*
	// TODO: application workflow
	.ok=cb
	.cancel=cb
*/
	};
	if(!swk_use(&w))
		return 1;
	swk_loop();
	return 0;
}
