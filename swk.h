/* See LICENSE file for copyright and license details. */
#include "draw.h"

#define SWK_NEWLINE .h=-1, .w=-1

typedef enum { EVoid, EClick, EMotion, EKey, EExpose, EQuit, ELast } SwkEventType;
typedef enum { Ctrl=1, Meta=2, Shift=4 } SwkKeyMod;
typedef enum { Left=1, Middle=2, Right=3, WheelUp=4, WheelDown=5 } SwkClickButton;

typedef struct SwkBox SwkBox;

typedef struct {
	int x;
	int y;
} Point;

typedef struct {
	int x;
	int y;
	int w;
	int h;
} Rect;

typedef struct {
	SwkClickButton button;
	long modmask;
	Point point;
} Click;

typedef struct {
	int keycode;
	long modmask;
} Key;

typedef struct {
	SwkEventType type;
	SwkBox *box;
	union {
		Click click;
		Point motion;
		Key key;
		Rect expose;
	} data;
} SwkEvent; 

typedef void (*SwkEventCallback)(SwkEvent *e);

struct SwkBox {
	int w;
	int h;
	SwkEventCallback cb;
	char *text;
	void *data;
};

typedef struct {
	char *title;
	Rect r;
	SwkBox *boxes;
	SwkBox *box;
} SwkWindow;

int swk_init(SwkWindow *w);
void swk_exit();
void swk_fit();
SwkEvent * swk_event(int lock);
void swk_event_handle(SwkEvent *e);

void swk_focus_next();
void swk_focus_prev();

void swk_button(SwkEvent *e);
void swk_label(SwkEvent *e);
void swk_entry(SwkEvent *e);
void swk_filler(SwkEvent *e);
