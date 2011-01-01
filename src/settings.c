#include <stdio.h>
#include <string.h>

#include <graph.h>
#include <input.h>

#include <libconfig.h>

#include "gui.h"
#include "cfg.h"
#include "settings.h"

settings_t *settings_init(void)
{

	settings_t *settings;

	settings = (settings_t*)calloc(1,sizeof(settings_t));

	if (settings == NULL)
	{
		return NULL;
	}

	settings->font.fsfont = fontstudio_init(16);

	if (settings->font.fsfont == NULL)
	{
		free(settings);
		return NULL;
	}

	strcpy(settings->home.directory,"mc0:/SYS-CONF");

	return settings;

}

void settings_free(settings_t* settings)
{
	if (settings->font.fsfont != NULL)
	{
		fontstudio_free(settings->font.fsfont);
		free(settings->font.fsfont);
	}
	free(settings);
}

void settings_parse(settings_t *settings, config_t *config)
{

	int i = 0;

	const char ps2[4] = "PS2";

	char section_path[256];
	char setting[256];

	if (settings == NULL || config == NULL)
	{
		return;
	}

	/// Home
	strcpy(section_path,ps2);
	strcat(section_path,".Home.");

	sprintf(setting,"%s%s",section_path,"Partition");
	strcpy(settings->home.partition,  cfg_get_string(setting,""));

	sprintf(setting,"%s%s",section_path,"Directory");
	strcpy(settings->home.directory,  cfg_get_string(setting,"mc0:/SYS-CONF"));


	/// Display
	strcpy(section_path,ps2);
	strcat(section_path,".Display.");

	sprintf(setting,"%s%s",section_path,"Mode");
	settings->display.mode = cfg_get_uint(setting,GRAPH_MODE_AUTO);

	sprintf(setting,"%s%s",section_path,"Interlace");
	settings->display.interlace = cfg_get_bool(setting,0);

	sprintf(setting,"%s%s",section_path,"Offset.X");
	settings->display.x = cfg_get_int(setting,0);

	sprintf(setting,"%s%s",section_path,"Offset.Y");
	settings->display.y = cfg_get_int(setting,0);

	/// Sound
	strcpy(section_path,ps2);
	strcat(section_path,".Sound.");

	sprintf(setting,"%s%s",section_path,"Stereo");
	settings->sound.stereo = cfg_get_bool(setting,1);

	sprintf(setting,"%s%s",section_path,"Volume");
	settings->sound.volume = cfg_get_int(setting,100);

	/// Font
	strcpy(section_path,ps2);
	strcat(section_path,".Font.");

	sprintf(setting,"%s%s",section_path,"Height");
	settings->font.fsfont->height = cfg_get_int(setting,16);

	sprintf(setting,"%s%s",section_path,"Color");
	for (i = 0; i < 4; i++)
	{
		settings->font.color[i] = cfg_get_int_elem(setting,i,0x80);
	}

	sprintf(setting,"%s%s",section_path,"Highlight");
	for (i = 0; i < 4; i++)
	{
		settings->font.highlight[i] = cfg_get_int_elem(setting,i,0xFF);
	}

	/// Input
	strcpy(section_path,ps2);
	strcat(section_path,".Input.");

	sprintf(setting,"%s%s",section_path,"Port");
	settings->input.port = cfg_get_uint(setting,0);

	sprintf(setting,"%s%s",section_path,"Slot");
	settings->input.slot = cfg_get_uint(setting,0);

	sprintf(setting,"%s%s",section_path,"Confirm");
	if (!strcmp("X",cfg_get_string(setting,"X")))
	{
		settings->input.confirm = PAD_CROSS;
	}
	else
	{
		settings->input.confirm = PAD_CIRCLE;
	}

	/// Devices
	strcpy(section_path,ps2);
	strcat(section_path,".Devices.");

	sprintf(setting,"%s%s",section_path,"CDVD");
	settings->devices.mass = cfg_get_bool(setting,0);

	sprintf(setting,"%s%s",section_path,"Mass");
	settings->devices.mass = cfg_get_bool(setting,0);

	sprintf(setting,"%s%s",section_path,"HDD");
	settings->devices.hdd = cfg_get_bool(setting,0);

}

void settings_add_to_config(settings_t *settings, config_t *config)
{

	int i;

	config_setting_t *root;
	config_setting_t *ps2;
	config_setting_t *group;
	config_setting_t *subgroup;
	config_setting_t *setting;

	if (config == NULL)
	{
		return;
	}

	root = config_root_setting(config);

	ps2 = config_setting_add(root,"PS2", CONFIG_TYPE_GROUP);
	{

		group = config_setting_add(ps2,"Home",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(group,"Partition",CONFIG_TYPE_STRING);
			config_setting_set_string(setting,settings->home.partition);

			setting = config_setting_add(group,"Directory",CONFIG_TYPE_STRING);
			config_setting_set_string(setting,settings->home.directory);

		}

		group = config_setting_add(ps2,"Display", CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(group,"Mode",CONFIG_TYPE_INT);
			config_setting_set_int(setting,settings->display.mode);

			setting = config_setting_add(group,"Interlace",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,settings->display.interlace);

			subgroup = config_setting_add(group,"Offset",CONFIG_TYPE_GROUP);
			{

				setting = config_setting_add(subgroup,"X",CONFIG_TYPE_INT);
				config_setting_set_int(setting,settings->display.x);

				setting = config_setting_add(subgroup,"Y",CONFIG_TYPE_INT);
				config_setting_set_int(setting,settings->display.y);

			}

		}

		group = config_setting_add(ps2,"Sound",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(group,"Stereo",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,settings->sound.stereo);

			setting = config_setting_add(group,"Volume",CONFIG_TYPE_INT);
			config_setting_set_int(setting,settings->sound.volume);

		}

		group = config_setting_add(ps2,"Font",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(group,"Height",CONFIG_TYPE_INT);
			config_setting_set_int(setting,settings->font.fsfont->height);

			setting = config_setting_add(group,"Color",CONFIG_TYPE_ARRAY);
			for (i = 0; i < 4; i++)
			{
				config_setting_set_int_elem(setting,-1,settings->font.color[i]);
			}

			setting = config_setting_add(group,"Highlight",CONFIG_TYPE_ARRAY);
			for (i = 0; i < 4; i++)
			{
				config_setting_set_int_elem(setting,-1,settings->font.highlight[i]);
			}
		}

		group = config_setting_add(ps2,"Input",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(group,"Port",CONFIG_TYPE_INT);
			config_setting_set_int(setting,settings->input.port);

			setting = config_setting_add(group,"Slot",CONFIG_TYPE_INT);
			config_setting_set_int(setting,settings->input.slot);

			setting = config_setting_add(group,"Confirm",CONFIG_TYPE_STRING);
			if (settings->input.confirm == PAD_CROSS)
			{
				config_setting_set_string(setting,"X");
			}
			else
			{
				config_setting_set_string(setting,"O");
			}

		}

		group = config_setting_add(ps2,"Devices",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(group,"CDVD",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,settings->devices.cdvd);

			setting = config_setting_add(group,"Mass",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,settings->devices.mass);

			setting = config_setting_add(group,"HDD",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,settings->devices.hdd);

		}

	}

}

void settings_load_config(settings_t *settings, char *path)
{

	config_t *cfg = cfg_open(path);

	settings_parse(settings,cfg);

	cfg_close(cfg);

}

void settings_load_files(settings_t *settings, gui_vram_t *vram)
{

	if (settings == NULL || vram == NULL)
	{
		return;
	}

	gui_load_image(settings->home.directory,"bg.png",vram->bg,0);
	gui_load_image(settings->home.directory,"skin.png",vram->skin,0);
	gui_load_image(settings->home.directory,"fg.png",vram->fg,vram->fg_clut);
	gui_load_image(settings->home.directory,"font.png",vram->font,vram->font_clut);
	gui_load_font_ini(settings->home.directory,settings->font.fsfont);

}
