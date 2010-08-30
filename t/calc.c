#include <stdio.h>
#include <string.h>
#include <swk.h>

static int bufferi = 0;
static char buffer[256];

static void button(SwkEvent *e) {
	FILE *pd;
	static char buffer2[sizeof(buffer)+32];
	if(e->type==EClick) {
		int key = *e->box->text;
		switch(key) {
		case 'C':
			bufferi = buffer[0] = 0;
			break;
		case '<':
			if(bufferi>0)
				buffer[--bufferi] = 0;
			break;
		case '=':
			snprintf(buffer2, sizeof(buffer2), "echo '%s' | bc -q", buffer);
			pd = popen(buffer2, "r");
			if(pd) {
				fgets(buffer, sizeof(buffer), pd);
				bufferi = strlen(buffer)-1;
				buffer[bufferi] = 0;
				pclose(pd);
			}
			break;
		default:
			if(bufferi<sizeof(buffer)) {
				buffer[bufferi++] = key;
				buffer[bufferi] = 0;
			}
			break;
		}
	}
	return swk_bigbutton(e);
}

static SwkBox contents[] = {
	SWK_BOX_NEWLINE(1),
	{ .cb=swk_label, .text=buffer },
	{ .cb=button, .text="<" },
	SWK_BOX_NEWLINE(2),
	{ .cb=button, .text="1" },
	{ .cb=button, .text="2" },
	{ .cb=button, .text="3" },
	{ .cb=button, .text="C" },
	SWK_BOX_NEWLINE(1),
	{ .cb=button, .text="4" },
	{ .cb=button, .text="5" },
	{ .cb=button, .text="6" },
	{ .cb=button, .text="+" },
	SWK_BOX_NEWLINE(1),
	{ .cb=button, .text="7" },
	{ .cb=button, .text="8" },
	{ .cb=button, .text="9" },
	{ .cb=button, .text="-" },
	SWK_BOX_NEWLINE(1),
	{ .cb=button, .text="%" },
	{ .cb=button, .text="0" },
	{ .cb=button, .text="/" },
	{ .cb=button, .text="*" },
	SWK_BOX_NEWLINE(1),
	{ .cb=swk_separator },
	{ .cb=button, .text="." },
	{ .cb=button, .text="=" },
	{ .cb=NULL }
};

int main() {
	SwkWindow w = {
		.title="Calculator",
		.boxes={contents,NULL},
		.box=contents,
	};
	if(!swk_use(&w))
		return 1;
	*buffer = 0;
	swk_loop();
	return 0;
}
