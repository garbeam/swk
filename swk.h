typedef enum { EVoid, EClick, EMotion, EKey, EExpose, ELast } SwkEventType;

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

void (*SwkEventCallback)(SwkEvent *e);

struct SwkBox {
	Rect r;
	SwkEventCallback *e;
	union {
		char *text;
		void *aux;
	} data;
};

typedef struct {
	Rect r;
	SwkBox *boxes;
} SwkWindow;


void swk_init();

void swk_button(SwkEvent *e);
void swk_label(SwkEvent *e);
void swk_text(SwkEvent *e);

void swk_deinit();
