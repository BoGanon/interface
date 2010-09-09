#ifndef __GUI_SKIN_H__
#define __GUI_SKIN_H__

// header/footer banner
// 512x50
// height 0-49
// width  0-511

#define BANNER_U0        0.5f
#define BANNER_U1      510.5f

#define BANNER_V0        0.5f
#define BANNER_V1       49.5f

// Boxes
#define BOX_V0          50.5f
#define BOX_V1         113.5f

#define BOX_START_U0     0.5f
#define BOX_START_U1    31.5f

#define BOX_MID_U0      32.5f
#define BOX_MID_U1      63.5f

#define BOX_END_U0      64.5f
#define BOX_END_U1      95.5f

// Buttons
// 128x32
#define BUTTON_U0       96.5f
#define BUTTON_U1      223.5f

#define BUTTON_V0       50.5f

#define BUTTON_HEIGHT   31.5f

// Icons
// 32x32
#define ICON_V0         50.5f
#define ICON_V1         81.5f

#define ICON_U0        224.5f

#define ICON_WIDTH      32.0f

// Logo
// 288x64
#define LOGO_U0        224.5f
#define LOGO_U1        510.5f

#define LOGO_V0         82.5f
#define LOGO_V1        127.5f

enum
{
	BUTTON_START = 0,
	BUTTON_SETTINGS
};

enum
{
	ICON_HOME = 0,
	ICON_DISPLAY,
	ICON_FONT,
	ICON_INPUT,
	ICON_DEVICES,
	ICON_EXIT,
	ICON_VOLUME,
	ICON_MUTE,
	ICON_APP
};

enum
{
	FONT = 0,
	BACKGROUND,
	SKIN,
	FOREGROUND,
};

#endif /*__GUI_SKIN_H__*/
