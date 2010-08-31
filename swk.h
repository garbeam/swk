/* See LICENSE file for copyright and license details. */

#define IS_SCROLLBOX(b) (b->r.w==-1 && b->r.h==-1 && ((int)(size_t)b->data)<0)
#define SWK_BOX_NEWLINE(x) { .data=(void*)(size_t)x, .r.w=-1, .r.h=-1, .cb = swk_filler }
#define SWK_BOX_VFILL(x) { .data=(void*)(size_t)x, .r.w=-1, .r.h=-2, .cb = swk_filler }
#define SWK_HIT(r,p) (p.x>=r.x && p.x<(r.x+r.w) && p.y>=r.y && p.y<(r.y+r.h))

typedef enum { EVoid, EClick, EMotion, EKey, EExpose, EQuit, ELast } SwkEventType;
typedef enum { Shift=1, Ctrl=2, Alt=4, Meta=8 } SwkKeyMod;
typedef enum { ColorFG, ColorBG, ColorHI, ColorTF, ColorCC, ColorLast } Palete;
typedef enum { KDel=8, KSupr=127, KUp=0xe0, KDown=0xe1, KLeft=0xe2, KRight=0xe3 } SwkKeyCode;

typedef struct SwkBox SwkBox;
typedef struct SwkEvent SwkEvent;
typedef struct SwkWindow SwkWindow;
typedef void (*SwkEventCallback)(SwkEvent *e);
typedef void (*SwkKeyCallback)();

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
	int modmask;
	int keycode;
} Key;

typedef struct {
	int modmask;
	int keycode;
	SwkKeyCallback cb;
} SwkKeyBind;

struct SwkEvent {
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
}; 

struct SwkBox {
	Rect r;
	SwkEventCallback cb;
	char *text;
	void *data;
	int scroll;
};

struct SwkWindow {
	char *title;
	SwkEventCallback cb;
	Rect r;
	SwkBox *boxes[2];
	int col;
	int colpos;
	/* internal use */
	SwkBox *box;
	SwkEvent _e;
};

typedef struct {
	char* name;
	int ref;
	void *data;
	int bpp;
	int w;
	int h;
	void *priv;
	void *pub;
} SwkImage;

int swk_use(SwkWindow *w);
void swk_update();
void swk_exit();
void swk_fit();
void swk_loop();
SwkEvent *swk_next_event();
void swk_handle_event(SwkEvent *e);

void swk_focus_first();
void swk_focus_next();
void swk_focus_prev();
void swk_focus_activate();
void swk_scroll_up();
void swk_scroll_down();
void swk_fontsize_increase();
void swk_fontsize_decrease();
void swk_column_move_left();
void swk_column_move_right();

void swk_button(SwkEvent *e);
void swk_bigbutton(SwkEvent *e);
void swk_label(SwkEvent *e);
void swk_entry(SwkEvent *e);
void swk_password(SwkEvent *e);
void swk_filler(SwkEvent *e);
void swk_option(SwkEvent *e);
void swk_separator(SwkEvent *e);
void swk_progress(SwkEvent *e);
void swk_image(SwkEvent *e);
void swk_sketch(SwkEvent *e);

/* graphic backend */

int swk_gi_init(SwkWindow *w);
void swk_gi_exit();
SwkEvent *swk_gi_event(SwkWindow *w, int dowait);
int swk_gi_update(SwkWindow *w);
int swk_gi_fontsize(int sz);

/* FIXME: don't these need SwkWindow *w state, to avoid static'ness? */
void swk_gi_clear();
void swk_gi_flip();

void swk_gi_line(int x1, int y1, int x2, int y2, int color);
void swk_gi_fill(Rect r, int color, int lil);
void swk_gi_rect(Rect r, int color);
void swk_gi_text(Rect r, const char *t);

/* images */
void swk_gi_img(Rect r, void *img);
void* swk_gi_img_new(int w, int h, int color);
void* swk_gi_img_load(const char *str);
void swk_gi_img_free(void *s);
void swk_gi_img_set(void *img, int x, int y, int color);
int swk_gi_img_get(void *img, int x, int y);

/* text.c */
typedef struct {
	const char *otext;
	int cur;
	int xcur;
	int ycur;
	char *text;
	int len;
	int size;
	int yscroll;
	int sel[2];
	int selmode;
} Text;

int text_rowcount(Text *t);
int text_rowoff(Text *t, int row);
int text_rowcol(Text *t, int off, int *col);
int text_off(Text *t, int col, int row);
char * text_sub(Text *t, int col, int row, int rows);
void text_init(Text *t, const char *text);
void text_set(Text *t, const char *text);
char * text_get(Text *t, int from, int to);
void text_sync(Text *t);
void text_cur(Text *t, int num, int dir);
void text_ins(Text *t, const char *str, int app);
void text_insc(Text *t, char ch, int app);
void text_del(Text *t, int num, int dir);
void text_sel(Text *t, int begin, int end);
void text_sel_mode(Text *t, int enable);

/* text.c widgets */
void swk_text(SwkEvent *e);
SwkImage *img_open(const char *str);
void img_close(SwkImage *img);
