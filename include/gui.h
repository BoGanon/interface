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

#ifdef __cplusplus
extern "C" {
#endif

	// Initializes GUI data structures
	void gui_init(int font_height);

	// Frees memory used by the GUI
	void gui_free();

	// Sets the screen height for rendering
	void gui_set_screen_height(float height);

	// Gets the screen height value
	float gui_get_screen_height(void);

	// Returns copy of vram map
	gui_vram_t gui_vram_get(void);

	// Returns the gui font
	fsfont_t *gui_font_get(void);

	// Frees gui font (takes up a bit of memory for large fonts)
	void gui_font_free(void);

	// Loads skin image files from skin.tgz
	void gui_load_skin(char *path);

	// Determines whether to draw background
	char gui_background_exists();

	// Determines whether to draw foreground
	char gui_foreground_exists();

	// Switches the texturebuffer to a specific texture
	qword_t *gui_setup_texbuffer(qword_t *q, int type);

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
