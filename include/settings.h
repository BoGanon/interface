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
	char mode;
	char interlace;
	char x;
	char y;
} display_t;

typedef struct {
	unsigned char color[4];
	unsigned char highlight[4];
	unsigned char height;
} font_t;

typedef struct {
	char port;
	char slot;
	short confirm;
	char pad_num;
} input_t;

typedef struct {
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
"PS2.Devices.Mass"
"PS2.Devices.HDD"
*/

#ifdef __cplusplus
extern "C" {
#endif


	// Initializes settings values
	void settings_init(const char * file);

	// Returns a pointer to the settings
	settings_t settings_get(void);

	// Parses the configuration's values for the settings
	void settings_parse(config_t *config);

	// Adds the settings to config
	void settings_add_to_config(config_t *config);

#ifdef __cplusplus
};
#endif

#endif /*__SETTINGS_H__*/
