
typedef struct {
	int *pixels;
	int w;
	int h;
} DrawBuffer;

int swk_gi_init();
void swk_gi_exit();
void swk_gi_flip();

int draw_init(int w, int h);
void draw_exit();

void draw_line(int x, int y, int w, int h);
void draw_box(int x, int y, int w, int h);
void draw_rect(int x, int y, int w, int h);
void draw_text(int x, int y, const char *text);
//void draw_pixmap();
