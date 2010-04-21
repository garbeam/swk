/* See LICENSE file for copyright and license details. */

#define SWK_BOX_NEWLINE(x) { .data=(void*)(size_t)x, .r.w=-1, .r.h=-1, .cb = swk_filler }
#define SWK_HIT(r,p) (p.x>=r.x && p.x<(r.x+r.w) && p.y>=r.y && p.y<(r.y+r.h))

typedef enum { EVoid, EClick, EMotion, EKey, EExpose, EQuit, ELast } SwkEventType;
typedef enum { Shift=1, Ctrl=2, Alt=4, Meta=8 } SwkKeyMod;
typedef enum { ColorFG, ColorBG, ColorHI, ColorLast } Palete;

typedef struct SwkBox SwkBox;
typedef struct SwkWindow SwkWindow;

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
	SwkWindow *win;
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

struct SwkWindow {
	char *title;
	Rect r;
	SwkBox *boxes;
	/* internal use */
	SwkBox *box;
	SwkEvent _e;
};

int swk_init(SwkWindow *w);
void swk_update(SwkWindow *w);
void swk_exit(void);
void swk_fit(SwkWindow *w);
void swk_loop(SwkWindow *w);
SwkEvent *swk_next_event(SwkWindow *w);
int swk_has_event(SwkWindow *w);
void swk_handle_event(SwkEvent *e);

void swk_focus_next(SwkWindow *w);
void swk_focus_prev(SwkWindow *w);

void swk_button(SwkEvent *e);
void swk_label(SwkEvent *e);
void swk_entry(SwkEvent *e);
void swk_filler(SwkEvent *e);

/* graphic backend */

int swk_gi_init(SwkWindow *w);
void swk_gi_exit();
SwkEvent *swk_gi_event(SwkWindow *w, int dowait);
int swk_gi_update(SwkWindow *w);
int swk_gi_has_event(SwkWindow *w);

/* FIXME: don't these need SwkWindow *w state, to avoid static'ness? */
void swk_gi_clear();
void swk_gi_flip();

void swk_gi_line(int x1, int y1, int x2, int y2, int color);
void swk_gi_fill(Rect r, int color);
void swk_gi_rect(Rect r, int color);
void swk_gi_text(Rect r, const char *text);
