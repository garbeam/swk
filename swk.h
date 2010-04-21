/* See LICENSE file for copyright and license details. */

#define SWK_NEWLINE(x) .data=(void*)(size_t)x, .r.w=-1, .r.h=-1, .cb = swk_filler

typedef enum { EVoid, EClick, EMotion, EKey, EExpose, EQuit, ELast } SwkEventType;
typedef enum { Shift=1, Ctrl=2, Alt=4, Meta=8 } SwkKeyMod;
typedef enum { ColorFG, ColorBG, ColorHI, ColorLast } Palete;

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
	int button;
	int modmask;
	Point point;
} Click;

typedef struct {
	int keycode;
	int modmask;
} Key;

typedef struct {
	SwkEventType type;
	SwkBox *box;
	union {
		Click click;
		Point motion;
		Key key;
		Rect expose;
		int rows;
	} data;
} SwkEvent; 

typedef void (*SwkEventCallback)(SwkEvent *e);

struct SwkBox {
	Rect r;
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
int swk_gi_update(SwkWindow *w);
void swk_update();
void swk_exit();
void swk_fit();
void swk_loop();
SwkEvent * swk_event();
void swk_event_handle(SwkEvent *e);
int swk_gi_has_event();

void swk_focus_next();
void swk_focus_prev();

void swk_button(SwkEvent *e);
void swk_label(SwkEvent *e);
void swk_entry(SwkEvent *e);
void swk_filler(SwkEvent *e);

/* graphic backend */

int swk_gi_init(SwkWindow *w);
void swk_gi_exit();
SwkEvent * swk_gi_event();

void swk_gi_flip();

void swk_gi_line(int x, int y, int w, int h, int color);
void swk_gi_fill(int x, int y, int w, int h, int color);
void swk_gi_rect(int x, int y, int w, int h, int color);
void swk_gi_text(int x, int y, const char *text);
