void (*swkhandler)(SwkWidget *, SwkEvent *);

void onclick(SwkWidget *w);

SwkTextField t = {
	SwkTypeTextField, 
	"enter some text",
	NULL
};
SwkButton b = {
	SwkTypeButton,
	"OK",
	NULL
};
SwkWin w = {
	SwkTypeWin,
	{ &t, &b, NULL }
};

void onclick(SwkWidget *w) {
	static int clicks = 0;
	SwkButton *b = (SwkButton *)w;
	fprintf(stdout, "onclick: %s\n", t.data);
	if(++clicks == 10)
		swk_exit();
}

static void myhandler(SwkWidget *w, SwkEvent *e) {
	if(e->type == SWK_CLICK && w->type == SwkTypeButton)
		onclick(w);
	else /* fallback */
		swkhandler(w, e);
}

int main() {
	swk_init(&w);
	swkhandler = swk_set_handler(myhandler);
	swk_loop(NULL); // loops until swk_exit() is called
	return 0;
}
