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

SwkBox *b = swk_ui_get(w, "ok");
#endif

#include <swk.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

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
	SwkWindow *w = (SwkWindow*)malloc(sizeof(SwkWindow));
	int sz, stri = 0, mode = 0;
	int count = 0;
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

	w->box = w->boxes = (SwkBox*)malloc(128*sizeof(SwkBox)); // Use sz after counting
	memset(w->box, 0, 128*sizeof(SwkBox));

	while(text && *text) {
		switch(mode) {
		case '\'':
			if ((*text=='\''&&str[stri-1]!='\\') || *text=='\n') {
				printf("label(%s)\n", str);
				stri = mode = 0;
				w->boxes[count].cb = swk_label;
				w->boxes[count].text = strdup (str);
				count++;
			} else {
				str[stri++] = *text;
				str[stri] = 0;
			}
			break;
		case '<':
			if ((*text=='>'&&str[stri-1]!='\\') || *text=='\n') {
				printf("image(%s)\n", str);
				stri = mode = 0;
				w->boxes[count].cb = swk_image;
				w->boxes[count].text = strdup (str);
				count++;
			} else {
				str[stri++] = *text;
				str[stri] = 0;
			}
			break;
		case '*':
			if (*text=='\n') {
				w->boxes[count].cb = swk_filler;
				count++;
				mode = 0;
			}
			break;
		case '=':
			if (*text=='\n') {
				SwkBox b = SWK_BOX_NEWLINE(-1);
				w->boxes[count] = b;
				count++;
				mode = 0;
			}
			break;
		case '-':
			if (*text=='\n') {
				w->boxes[count].cb = swk_separator;
				count++;
				mode = 0;
			}
			break;
		case '(':
			if ((*text==')'&&str[stri-1]!='\\') || *text=='\n') {
				printf("option(%s)\n", str);
				stri = mode = 0;
				w->boxes[count].cb = swk_option;
				w->boxes[count].text = strdup (str);
				count++;
			} else {
				str[stri++] = *text;
				str[stri] = 0;
			}
			break;
		case '$':
			if ((*text=='$'&&str[stri-1]!='\\') || *text=='\n') {
				stri = mode = 0;
				if (*str=='*') {
					printf("pass(%s)\n", str);
					w->boxes[count].cb = swk_password;
					w->boxes[count].text = "";
				} else {
					printf("entry(%s)\n", str);
					w->boxes[count].cb = swk_entry;
					w->boxes[count].text = strdup (str);
				}
				count++;
			} else {
				str[stri++] = *text;
				str[stri] = 0;
			}
			break;
		case '[':
			if ((*text==']'&&str[stri-1]!='\\') || *text=='\n') {
				printf("button(%s)\n", str);
				stri = mode = 0;
				w->boxes[count].cb = swk_button;
				w->boxes[count].text = strdup (str);
				count++;
			} else {
				str[stri++] = *text;
				str[stri] = 0;
			}
			break;
		case '{':
			if (*text=='}' || *text=='\n') {
				printf("WINDOW TITLE(%s)\n", str);
				stri = mode = 0;
				w->title = strdup(str);
			} else {
				str[stri++] = *text;
				str[stri] = 0;
			}
			break;
		default:
			if (*text=='\n') {
				SwkBox b = SWK_BOX_NEWLINE(1);
				w->boxes[count] = b;
				count++;
			} else {
				mode = *text;
				stri = 0;
				str[0] = 0;
			}
			break;
		}
		text++;
	}
	w->running = 1;
	swk_init(w);
	return w;
}

#define UI \
	"{Hello World}\n" \
	"'This is a label'\n" \
	"-----------------\n" \
	"\n\n" \
	"$$\n" \
	"=================\n" \
	"[yes][no]\n"

static SwkWindow *w = NULL;

int main() {
	w = swk_ui(UI);
	if(!w||!swk_init(w))
		return 1;
	swk_loop(w);
	return 0;
}
