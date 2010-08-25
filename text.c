/* See LICENSE file for copyright and license details. */
#define _BSD_SOURCE
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <swk.h>

int
text_rowcount(Text *t) {
	int rows = 1;
	char *p;
	for(p=t->text;*p;p++)
		if(*p=='\n')
			rows++;
	return rows;
}

int
text_rowoff(Text *t, int row) {
	char *p;
	int off = 0;
	if(row<1)
		return 0;
	for(p=t->text;row&&*p;p++) {
		if(*p=='\n')
			row--;
		off++;
	}
	return off;
}

int
text_rowcol(Text *t, int off, int *col) {
	char *p,*nl = NULL;
	char *e = t->text+off;
	int r = 0;
	int c = 0;
	for(p=t->text;*p&&p<=e;p++) {
		if(*p=='\n') {
			r++;
			c = 0;
			nl = p+1;
		} else c++;
	}
	if(col) *col = c;
	return r;
}
int
text_off(Text *t, int col, int row) {
	int off = text_rowoff(t, row);
	int len = strlen (t->text);
	if (col>len)
		col = len;
	return off+(t->text[off])?col:0;
}

char *
text_sub(Text *t, int col, int row, int rows) {
	// find row number N in text
	// +=col if < \n
	// count N rows and \0
	return NULL;
}

void
text_init(Text *t, const char *str) {
	if(!t->text) {
		t->len = strlen (t->otext);
		t->size = (t->len+1)*2;
		t->text = malloc (t->size);
		if (str) strcpy(t->text, t->otext);
		else strcpy(t->text, t->otext);
	}
}

void
text_set(Text *t, const char *text) {
	int len = strlen (text);
	if(t->text) {
		if(len>t->size) {
			t->size = (len+10)*2;
			t->text = realloc(t->text, t->size);
		}
		strcpy(t->text, text);
	} else text_init(t, text);
	text_sync(t);
}

char *
text_get(Text *t, int from, int to) {
	/* get buffer between from and to offsets */
	char *p;
	if(to!=-1&&(to<from || from>t->len))
		return strdup ("");
	if(to>t->len||to==-1)
		to = t->len;
	if (!t->text)
		if (t->otext)
			t->text = t->otext;
		else t->text = "";
	p = strdup (t->text+from);
	//if(to!=-1) p[to-from] = '\0';
	return p;
}

void
text_sync(Text *t) {
	// count cols, rows, fix xcur, ycur from cur, etc..
	t->len = strlen(t->text);
	t->ycur = text_rowcol(t, t->cur, &t->xcur);
}

void
text_cur(Text *t, int num, int dir) {
	if(dir) t->cur += (num*dir);
	else t->cur = num;
	if(t->cur<0)
		t->cur = 0;
	if(t->cur>t->len)
		t->cur = t->len;
	if(t->text[t->cur]=='\n')
		t->cur+=dir;
}

void
text_resize(Text *t, int newsize) {
	if (newsize>t->size) {
		t->text = realloc(t->text, newsize);
		t->size = newsize;
	}
}

void
text_insc(Text *t, char ch, int app) {
	char str[2] = {ch};
	text_ins(t, str, app);
}

void
text_ins(Text *t, const char *str, int app) {
	int len = strlen(str);
	text_resize(t, (2*len)+t->size);
	if(app) {
		char *tmp = strdup(t->text+t->cur);
		strcpy(t->text+t->cur, str);
		strcat(t->text+t->cur+len, tmp);
		free(tmp);
	} else memcpy(t->text+t->cur, str, len);
	t->cur += len;
}

void
text_del(Text *t, int num, int dir) {
	switch(dir) {
	case -1:
		if(t->cur-num<0)
			num = 0;
		strcpy(t->text+t->cur-num, t->text+t->cur);
		t->cur -= num;
		break;
	case 0:
		t->text[num] = 0;
		t->len = strlen (t->text);
		if (t->cur>t->len)
			t->cur = t->len;
		break;
	case 1:
		if (t->cur+num>=t->len)
			num = t->len-t->cur;
		strcpy(t->text+t->cur, t->text+t->cur+num);
		break;
	}
	text_sync(t);
}

void
text_sel(Text *t, int begin, int end) { // if off != off2 text is selected
	t->sel[0] = begin;
	t->sel[1] = end;
}

void
text_sel_mode(Text *t, int enable) {
	t->selmode = enable;
}

/* swk widget */
void
swk_text(SwkEvent *e) {
	Text *t = (Text*)e->box->data;
	int row, len, key;
	char *ptr;

	text_init(e->box->data, e->box->text);
	text_sync(e->box->data);
	switch(e->type) {
	case EClick:
		// TODO: text_sel//
		switch(e->data.click.button) {
		case 1:
			//text_sel_mode(1)
			break;
		case 2:
			// activate link (TODO: implement hyperlinks)
			break;
		case 3:
			//text_sel_mode(0)
			break;
		}
		break;
	case EKey:
		key = e->data.key.keycode;
		if(e->data.key.modmask&Ctrl)
			return;
		if(key == KDel)
			text_del(e->box->data, 1, -1);
		else if(key == KSupr)
			text_del(e->box->data, 1, 1);
		else if(key=='\n'||(key>=' '&&key<='~'))
			text_insc(e->box->data, key, 1);
		else if(key==KUp)
			text_cur(e->box->data, 10, -1); //XXX
		else if(key==KDown)
			text_cur(e->box->data, 10, 1); //XXX
		else if(key==KLeft)
			text_cur(e->box->data, 1, -1);
		else if(key==KRight)
			text_cur(e->box->data, 1, 1);
		break;
	case EExpose:
		swk_gi_fill(e->box->r, ColorBG, 0);
		row = t->ycur-2;
		/* text */{
		int len, rows = 0;
		int y = e->box->r.y--;
		char *p, *p0, *str;
		str = text_get (e->box->data, text_rowoff(e->box->data,row), -1);
		if(row<0) row = 0;
		//printf("str(%s)\n", str);
		for(p=p0=str;*p;p++) {
			if(*p=='\n') {
				if(++rows>e->box->r.h)
					break;
				*p = 0;
				e->box->text = p0;
				e->box->r.y++;
				len = strlen(p0);
				if(len>=e->box->r.w*3)
					p0[(e->box->r.w*3)-1]=0;
				swk_gi_text(e->box->r, p0);
				p0 = p+1;
			}
		}
		if((rows<=e->box->r.h-1)&&p0&&*p0) {
			e->box->r.y++;
			swk_gi_text(e->box->r, p0);
		}
		e->box->r.y = y;
		free(str);
		}
		/* cursor */ {
			int len, cut = e->box->r.w*2;
			int row = ((Text*)e->box->data)->ycur;
			int col = ((Text*)e->box->data)->xcur;
		if(row>=e->box->r.h-1) row = e->box->r.h-2;
#if 1
			#ifdef USE_SDL
			len = 4*e->box->r.x;
			#else
			len = 3*e->box->r.x;
			#endif
			if(col>cut)
				len += cut;
			#ifdef USE_SDL
			else len += 2*col+1;
			#else
			else len += col+1;
			#endif
#endif
			Text* t = e->box->data;
			text_sync(e->box->data);

			Rect r = { (e->box->r.x*3)+col, e->box->r.y+row, 1, 1};
			swk_gi_fill(r, ColorHI, 2);
		}
		swk_gi_rect(e->box->r, ColorFG);
		break;
	default:
		swk_label(e);
		break;
	}
}
