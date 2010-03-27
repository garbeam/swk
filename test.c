#include <swk.h>

int main () {
	SwkWindow *w;
	SwkBox *c;

	swk_init ();

	w = swk_window (swk_window_layout_flow);
	c = swk_box (swk_box_layout_flow);
	swk_box_add (c, swk_label ("Hello World"));
	//swk_add_box (c);

	// TODO: not working with glib
	while (swk_event ());
	return swkret;
}
