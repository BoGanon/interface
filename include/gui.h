#ifndef __GUI_H__
#define __GUI_H__

#include <draw.h>
#include <image.h>
#include <font.h>

#include "gui_skin.h"

typedef struct {
	int bg;
	int fg;
	int fg_clut;
	int misc;
	int skin;
	int font;
	int font_clut;
} gui_vram_t;

extern float gui_screen_height;
extern unsigned char skin_tgz[];
extern unsigned int  size_skin_tgz;

#ifdef __cplusplus
extern "C" {
#endif

	// Allocates and initializes the vram map
	gui_vram_t *gui_vram_init(void);

	// Frees the vram map
	void gui_vram_free(gui_vram_t *vram);

	// Allocates gui font and loads ini from skin.tgz
	fsfont_t *gui_font_init(char *dir, int height);

	// Frees gui font
	void gui_font_free(fsfont_t *gui_font);

	// Loads skin image files from skin.tgz
	void gui_load_skin(char *dir, gui_vram_t *vram, fsfont_t *gui_font);

	// Determines whether to draw background
	char gui_background_exists();

	// Determines whether to draw foreground
	char gui_foreground_exists();

	// Sends image to vram
	void gui_send_image(image_t *image,int texture, int clut);

	// Loads image from tar and sends to vram
	int gui_load_image(char *tar, int tar_size, char *file, int texaddr, int clutaddr);

	// Loads font ini
	int gui_load_font_ini(char *tar, int tar_size, fsfont_t *gui_font);

	// Switches the texturebuffer to a specific texture
	qword_t *gui_setup_texbuffer(qword_t *q, int type, gui_vram_t *vram);

	// Renders GUI elements
	qword_t *gui_background(qword_t *q);
	qword_t *gui_header(qword_t *q, float width, unsigned char alpha);
	qword_t *gui_footer(qword_t *q, float y, float width, unsigned char alpha);
	qword_t *gui_box(qword_t *q, float x, float y, int width, int height, int active);
	qword_t *gui_button(qword_t *q, float x, float y, float button, int active);
	qword_t *gui_icon(qword_t *q, float x, float y, float type, color_t *color);
	qword_t *gui_foreground(qword_t *q, unsigned char alpha);
	qword_t *gui_logo(qword_t *q, float x, float y, unsigned char alpha);
	qword_t *gui_button_string(qword_t *q, float x, float y, char *str, fsfont_t *font, unsigned char active);

	// Renders a basic GUI layout (header/footer/logo)
	qword_t *gui_basic_layout(qword_t *q, unsigned char alpha);

#ifdef __cplusplus
};
#endif

#endif /*__GUI_H__*/
