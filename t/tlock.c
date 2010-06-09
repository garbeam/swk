#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <swk.h>

// TODO: disable alarm if dpms off
// TODO: enable alarm when dpms on

static SwkBox contents[];
#define COUNT 0
static int count = COUNT;

static char timestring[80];
static void settimestring() {
	struct tm lt;
	time_t t = time(0);
	localtime_r(&t, &lt);
	snprintf(timestring, sizeof(timestring),
		"%04d/%02d/%02d    %02d:%02d:%02d",
		1900+lt.tm_year, lt.tm_mon+1, lt.tm_mday, 
		lt.tm_hour, lt.tm_min, lt.tm_sec);
}

static void timepoll() {
	settimestring();
	swk_update();
	if(count--<0) {
		contents[2].scroll = 0;
		count = COUNT;
	}
	alarm(1);
}

static void mylocklabel(SwkEvent *e) {
	if(e->type == EMotion) {
		count = 5;
	}
	if(e->type == EExpose) {
		int pos = e->box->r.y;
		if(pos<3 || pos>e->win->r.h) {
			printf("swkexit\n");
			swk_exit();
		}
	}
	swk_label(e);
}

static SwkBox contents[] = {
	{ .cb=swk_label, .text=timestring },
	{ .cb=swk_separator },
	SWK_BOX_NEWLINE(-1),
	{ .cb=mylocklabel, .text="      slide out to unlock", },
	{ .cb=NULL }
};

static void init_alarm() {
	signal(SIGALRM, timepoll);
	alarm(1);
	settimestring();
}

int main() {
	SwkWindow w = {
		.title="touch lock",
		.boxes=contents,
		.box=contents,
	};
	if(!swk_use(&w))
		return 1;
	init_alarm();
	swk_loop();
	return 0;
}
