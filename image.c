/* See LICENSE file for copyright and license details. */
#include "swk.h"
#include <stdlib.h>
#include <string.h>
#include <Imlib2.h>

#define MAXIMGS 32
//static SwkImage images[MAXIMGS];

SwkImage *
img_open(const char *str) {
	SwkImage *img = (SwkImage*)malloc(sizeof(SwkImage));
	if(str) {
		strncpy(img->name, str, sizeof(img->name)-1);
		img->priv = imlib_load_image (str);
		imlib_context_set_image ((Imlib_Image*)img->priv);
		img->w = imlib_image_get_width();
		img->h = imlib_image_get_height();
		img->data = imlib_image_get_data();
		img->bpp = 24;
	} else memset(img, 0, sizeof(SwkImage));
	return img;
}

void
img_free(SwkImage *img) {
	img->ref--; // XXX
	imlib_context_set_image ((Imlib_Image*)img->priv);
	imlib_free_image ();
}
