#ifndef __SETTINGS_H__
#define __SETTINGS_H__

#include <gui.h>
#include <font.h>
#include <libconfig.h>

typedef struct {
	char partition[256];
	char directory[256];
} home_t;

typedef struct {
	unsigned char color[4];
	unsigned char highlight[4];
	fsfont_t *fsfont;
} font_t;

typedef struct {
	char mode;
	char interlace;
	char x;
	char y;
} display_t;

typedef struct {
	char port;
	char slot;
	short confirm;
	char pad_num;
} input_t;

typedef struct {
	char cdvd;
	char mass;
	char hdd;
} device_t;

typedef struct {
	char stereo;
	char volume;
} sound_t;

typedef struct {
	home_t    home;
	display_t display;
	sound_t   sound;
	font_t    font;
	input_t   input;
	device_t  devices;
} settings_t;

/*
"PS2.Home.Partition"
"PS2.Home.Directory"
"PS2.Display.Mode"
"PS2.Display.Interlace"
"PS2.Display.Offset.X"
"PS2.Display.Offset.Y"
"PS2.Font.Height"
"PS2.Font.Color"
"PS2.Font.Highlight"
"PS2.Sound.Stereo"
"PS2.Sound.Volume"
"PS2.Input.Port"
"PS2.Input.Slot"
"PS2.Input.Confim"
"PS2.Devices.CDVD"
"PS2.Devices.Mass"
"PS2.Devices.HDD"
*/

#ifdef __cplusplus
extern "C" {
#endif

	// Allocates and initializes settings
	settings_t *settings_init(void);

	// Deallocates settings 
	void settings_free(settings_t *settings);

	// Adds the settings to config
	void settings_add_to_config(settings_t *settings,config_t *config);

	// Tries to load a configuration file
	// Returns loaded settings on success
	// Returns default settings on failure to load file
	// Returns NULL on failure to allocate memory
	void settings_load_config(settings_t *settings, char *path);

	// Loads the files needed for the GUI
	void settings_load_files(settings_t *settings, gui_vram_t *vram);

#ifdef __cplusplus
};
#endif

#endif /*__SETTINGS_H__*/
