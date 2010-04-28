/* See LICENSE file for copyright and license details. */

//#define N900
#ifdef N900
#define FONTSIZE 32
#else
#define FONTSIZE 14
#endif

/* appearance */
#define FONTSIZE 40
#define FONTBOLD 1
#define WINWIDTH 640
#define WINHEIGHT 480
// SDL
#define SWK_COLOR(r,g,b) 0x##r,0x##g,0x##b
// X11
//#define SWK_COLOR(r,g,b) r##g##b

#define HICOLOR SWK_COLOR(0,66,ff)
#define FGCOLOR SWK_COLOR(e0,e0,e0)
#define BGCOLOR SWK_COLOR(00,00,00)
#define TFCOLOR SWK_COLOR(cc,cc,cc)

/* key bindings */
static SwkKeyBind keys[] = {
	{ Ctrl, 'j', swk_focus_next },
	{ Ctrl, 'k', swk_focus_prev },
	{ Ctrl,  8 , swk_focus_first },
	{ Ctrl,  9 , swk_focus_prev },
	{   0 ,  9 , swk_focus_next },
	{ Ctrl, 10 , swk_focus_next },
	{ Ctrl, 11 , swk_focus_prev },
	{ Ctrl, 12 , swk_focus_activate },
	{ 0, 225, swk_focus_activate }, // n900 enter
	{   0 , KUp, swk_focus_prev },
	{   0 , KDown, swk_focus_next },
	{   0 , 13 , swk_focus_activate },
	{ Ctrl, 12 , swk_focus_activate },
	{ Ctrl|Shift, 10, swk_scroll_down },
	{ Ctrl|Shift, 11, swk_scroll_up },
	{ Ctrl, '+', swk_fontsize_increase },
	{ Ctrl, '-', swk_fontsize_decrease },
	{ 0 }
};
