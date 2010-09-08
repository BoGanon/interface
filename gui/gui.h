#ifndef __GUI_H__
#define __GUI_H__

#include <draw.h>
#include <image.h>

#include <init.h>
#include <gui_skin.h>
#include <settings.h>

typedef struct {
	int bg;
	int fg;
	int fg_clut;
	int misc;
	int skin;
	int font;
	int font_clut;
} gui_vram_t;

#ifdef __cplusplus
extern "C" {
#endif

	// Allocates and initializes the vram map
	gui_vram_t *gui_vram_init(void);

	// Frees the vram map
	void gui_vram_free(gui_vram_t *vram);

	// Sends image to vram
	void gui_send_image(image_t *image,int texture, int clut);

	// Loads image from file and sends to vram
	void gui_load_image(char *dir, char *file, int texaddr, int clutaddr);

	// Loads the files needed for the GUI
	void gui_load_files(settings_t *interface, gui_vram_t *vram);

	// Switches the texturebuffer to a specific texture
	qword_t *gui_setup_texbuffer(qword_t *q,int context, int type, gui_vram_t *vram);

	// Renders GUI elements
	qword_t *gui_background(qword_t *q, int context);
	qword_t *gui_header(qword_t *q,int context, float width, unsigned char alpha);
	qword_t *gui_footer(qword_t *q,int context, float y, float width, unsigned char alpha);
	qword_t *gui_box(qword_t *q, int context, float x, float y, int width, int height, int active);
	qword_t *gui_button(qword_t *q, int context, float x, float y, float button, int active);
	qword_t *gui_icon(qword_t *q, int context, float x, float y, float type, color_t *color);
	qword_t *gui_foreground(qword_t *q, int context, unsigned char alpha);
	qword_t *gui_logo(qword_t *q, int context, float x, float y, unsigned char alpha);
	qword_t *gui_button_string(qword_t *q, int context, float x, float y, char *str, fsfont_t *font, unsigned char active);

	// Renders a basic GUI layout (header/footer/logo)
	qword_t *gui_basic_layout(qword_t *q, int context, unsigned char alpha);

#ifdef __cplusplus
};
#endif

#endif /*__GUI_H__*/
