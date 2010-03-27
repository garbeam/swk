#define TRUE 1
#define FALSE 0
typedef int bool_t;

typedef struct swk_widget_t {
	int (*render) (struct swk_widget_t *widget, int w, int h);
	int (*activate) (struct swk_widget_t *widget, int type);
	void *user;
} SwkWidget;

typedef struct container_t {
	char *title; // nullable
	int show; // boolean
	struct SwkWidget *widget;
	struct SwkBox *next;
} SwkBox;

typedef struct swk_window_t {
	char *title;
	SwkWidget *selected;
	SwkBox *box;
	SwkBox *boxes;
	int (*box_align) (struct swk_window_t *window);
	struct swk_window_t *next;
} SwkWindow;

typedef struct scene_t {
	SwkWindow *windows;
	SwkWindow *window;
} SwkScene;

typedef enum {
	CLICK,
	RELEASE,
	MOUSE_OVER,
	MOUSE_OUT,
} SwkEvent;

int swk_widget_set_event_mask (SwkWidget *w, int evmask);

int swk_scene_next (SwkScene *s, int dir);
int swk_event ();

void flow_layout_align (SwkScene *scene);

extern SwkScene swkscene;
extern int swkret;

// uhm? not here maybe
extern int swk_window_layout_flow (SwkWindow *window);
extern int swk_box_layout_flow (SwkBox* box);
