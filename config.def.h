/* See LICENSE file for copyright and license details. */

/* appearance */
#define SCROLLSPEED 2
#define COLSPLIT 20
#define FONTSIZE 18
#define FONTBOLD 0
#define WINWIDTH 640
#define WINHEIGHT 480
#define TOUCHSCREEN 0
#ifdef USE_SDL
#define SWK_COLOR(r,g,b) 0x##r,0x##g,0x##b
#else
#define SWK_COLOR(r,g,b) 0x##r##g##b
#endif

#define HICOLOR SWK_COLOR(00,66,ff)
#define BGCOLOR SWK_COLOR(10,10,10)
#define FGCOLOR SWK_COLOR(e0,e0,e0)
#define TFCOLOR SWK_COLOR(cc,cc,cc)
#define CCCOLOR SWK_COLOR(20,20,20)

/* key bindings */
static SwkKeyBind keys[] = {
	{ Ctrl, '\n',   swk_focus_activate},
	{ Ctrl, 'j',   swk_focus_next },
	{ Ctrl, 'k',   swk_focus_prev },
	//{ Ctrl,  8 ,   swk_focus_first },
	//{ Ctrl,  9 ,   swk_focus_prev },
	{ Ctrl,  8 ,   swk_column_move_left },
	{ Ctrl,  12 ,   swk_column_move_right },
	{ Ctrl,  'h' ,   swk_column_move_left },
	{ Ctrl,  'l',   swk_column_move_right },
	{   0 ,  9 ,   swk_focus_next },
	{ Ctrl, 10 ,   swk_focus_next },
	{ Ctrl, 11 ,   swk_focus_prev },
//	{ Ctrl, 12 ,   swk_focus_activate },
	{   0 , KUp,   swk_focus_prev },
	{   0 , KDown, swk_focus_next },
	{   0 , 13 ,   swk_focus_activate },
	{ Ctrl, 12 ,   swk_focus_activate },
	{ Ctrl|Shift, 10, swk_scroll_down },
	{ Ctrl|Shift, 11, swk_scroll_up },
	{ Ctrl|Shift, 'J', swk_scroll_up },
	{ Ctrl|Shift, 'K', swk_scroll_down },
	{ Ctrl, '+',   swk_fontsize_increase },
	{ Ctrl, '-',   swk_fontsize_decrease },
	{ 0 }
};
