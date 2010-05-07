/* See LICENSE file for copyright and license details. */
// ascii-art user interface //
#if 0
	{HelloWorld
	'Press a button'
	----------------
	(save password)
	(store options)
	[Ok] [Cancel]
	================
	'This is new'
	<image.png>
	}
#endif

#include <swk.h>
#include <stdio.h>

// TODO: Rename to swk_win_ swk_window_ ?
void
swk_ui_free(SwkWindow *w) {
	// leaks in box->text ?
	free(w->boxes);
	free(w->title);
	free(w);
}

SwkWindow *
swk_ui(const char *text) {
	SwkBox *b;
	SwkWindow *w = (SwkWindow*)malloc(sizeof(SwkWindow));
	int sz, stri = 0, mode = 0;
	char str[128];
	const char *ptr = text;

	if(!w) return NULL;
	memset(w, 0, sizeof(SwkWindow));

	// TODO: count widgets and allocate stuff
	for(sz=0; ptr && *ptr; ptr++) {
		// TODO
		switch(*ptr) {
		case '\n':
		case '[':
		case '{':
		case '\'':
			sz++;
			sz++;
		default:
			break;
		}
	}
	printf("WINDETS=%d\n", sz);

	w->box = w->boxes = malloc(128*sizeof(SwkBox)); // Use sz after counting
	memset(w->box, 0, 128*sizeof(SwkBox));

	while(text && *text) {
		switch(mode) {
		case '\'':
			if ((*text=='\''&&str[stri-1]!='\\') || *text=='\n') {
				printf("label(%s)\n", str);
				stri = 0;
				mode = 0;
				//w->boxes[count++] ...
				w->title = strdup(str);
			} else {
				str[stri++] = *text;
				str[stri] = 0;
			}
			break;
		case '{':
			if (*text=='}' || *text=='\n') {
				printf("WINDOW TITLE(%s)\n", str);
				stri = 0;
				mode = 0;
				w->title = strdup(str);
			} else {
				str[stri++] = *text;
				str[stri] = 0;
			}
			break;
		default:
			mode = *text;
			break;
		}
		text++;
	}
	return w;
}

#define UI \
	"{Hello World}\n" \
	"'This is a label'\n" \
	"-----------------\n" \
	"^\n" \
	"[yes][no]\n"

static SwkWindow *w = NULL;

int main() {
	w = swk_ui(UI);
	if(!w||!swk_init(w))
		return 1;
	swk_loop(w);
	return 0;
}
