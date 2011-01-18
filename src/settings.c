#include <stdio.h>
#include <string.h>

#include <graph.h>
#include <input.h>

#include <libconfig.h>

#include "gui.h"
#include "cfg.h"
#include "settings.h"
#include "strkat.h"

static settings_t settings;

settings_t settings_get(void)
{
	return settings;
}

void settings_init(const char *path)
{

	config_t *cfg;

	cfg = cfg_open(path);

	settings_parse(cfg);

	cfg_close(cfg);

}

void settings_parse(config_t *config)
{

	int i = 0;

	const char ps2[4] = "PS2";

	char section_path[256];
	char setting[256];

	if (config == NULL)
	{
		return;
	}

	/// Home
	strcpy(section_path,ps2);
	strkat(section_path,".Home.");

	sprintf(setting,"%s%s",section_path,"Partition");
	strcpy(settings.home.partition,  cfg_get_string(setting,""));

	sprintf(setting,"%s%s",section_path,"Directory");
	strcpy(settings.home.directory,  cfg_get_string(setting,"mc0:/SYS-CONF"));


	/// Display
	strcpy(section_path,ps2);
	strkat(section_path,".Display.");

	sprintf(setting,"%s%s",section_path,"Mode");
	settings.display.mode = cfg_string_to_int(cfg_get_string(setting,"0"));
	printf("mode = %d\n",settings.display.mode);
	if (settings.display.mode == GRAPH_MODE_AUTO)
	{
		settings.display.mode = graph_get_region();
	}

	sprintf(setting,"%s%s",section_path,"Interlace");
	settings.display.interlace = cfg_get_bool(setting,0);

	sprintf(setting,"%s%s",section_path,"Offset.X");
	settings.display.x = cfg_string_to_int(cfg_get_string(setting,"0"));

	sprintf(setting,"%s%s",section_path,"Offset.Y");
	settings.display.y = cfg_string_to_int(cfg_get_string(setting,"0"));

	/// Sound
	strcpy(section_path,ps2);
	strkat(section_path,".Sound.");

	sprintf(setting,"%s%s",section_path,"Stereo");
	settings.sound.stereo = cfg_get_bool(setting,1);

	sprintf(setting,"%s%s",section_path,"Volume");
	settings.sound.volume = cfg_string_to_int(cfg_get_string(setting,"100"));

	/// Font
	strcpy(section_path,ps2);
	strkat(section_path,".Font.");

	sprintf(setting,"%s%s",section_path,"Height");
	settings.font.height = cfg_string_to_int(cfg_get_string(setting,"16"));

	sprintf(setting,"%s%s",section_path,"Color");
	for (i = 0; i < 4; i++)
	{
		settings.font.color[i] = cfg_string_to_int(cfg_get_string_elem(setting,i,"128"));
	}

	sprintf(setting,"%s%s",section_path,"Highlight");
	for (i = 0; i < 4; i++)
	{
		settings.font.highlight[i] = cfg_string_to_int(cfg_get_string_elem(setting,i,"255"));
	}

	/// Input
	strcpy(section_path,ps2);
	strkat(section_path,".Input.");

	sprintf(setting,"%s%s",section_path,"Port");
	settings.input.port = cfg_string_to_int(cfg_get_string(setting,"0"));

	sprintf(setting,"%s%s",section_path,"Slot");
	settings.input.slot = cfg_string_to_int(cfg_get_string(setting,"0"));

	sprintf(setting,"%s%s",section_path,"Confirm");
	if (!strcmp("X",cfg_get_string(setting,"X")))
	{
		settings.input.confirm = PAD_CROSS;
	}
	else
	{
		settings.input.confirm = PAD_CIRCLE;
	}

	/// Devices
	strcpy(section_path,ps2);
	strkat(section_path,".Devices.");

	sprintf(setting,"%s%s",section_path,"Mass");
	settings.devices.mass = cfg_get_bool(setting,0);

	sprintf(setting,"%s%s",section_path,"HDD");
	settings.devices.hdd = cfg_get_bool(setting,0);

}

void settings_add_to_config(config_t *config)
{

	int i;

	char value[256];

	config_setting_t *root;
	config_setting_t *ps2;
	config_setting_t *group;

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
			config_setting_set_string(setting,settings.home.partition);

			setting = config_setting_add(group,"Directory",CONFIG_TYPE_STRING);
			config_setting_set_string(setting,settings.home.directory);

		}

		group = config_setting_add(ps2,"Display", CONFIG_TYPE_GROUP);
		{


			setting = config_setting_add(group,"OffsetX",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,settings.display.x);
			config_setting_set_string(setting,value);

			setting = config_setting_add(group,"OffsetY",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,settings.display.y);
			config_setting_set_string(setting,value);

			setting = config_setting_add(group,"Mode",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,settings.display.mode);
			config_setting_set_string(setting,value);

			setting = config_setting_add(group,"Interlace",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,settings.display.interlace);

		}

		group = config_setting_add(ps2,"Sound",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(group,"Stereo",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,settings.sound.stereo);

			setting = config_setting_add(group,"Volume",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,settings.sound.volume);
			config_setting_set_string(setting,value);

		}

		group = config_setting_add(ps2,"Font",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(group,"Height",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,settings.font.height);
			config_setting_set_string(setting,value);

			setting = config_setting_add(group,"Color",CONFIG_TYPE_ARRAY);
			for (i = 0; i < 4; i++)
			{
				cfg_int_to_string(value,settings.font.color[i]);
				config_setting_set_string_elem(setting,-1,value);
			}

			setting = config_setting_add(group,"Highlight",CONFIG_TYPE_ARRAY);
			for (i = 0; i < 4; i++)
			{
				cfg_int_to_string(value,settings.font.highlight[i]);
				config_setting_set_string_elem(setting,-1,value);
			}
		}

		group = config_setting_add(ps2,"Input",CONFIG_TYPE_GROUP);
		{

			setting = config_setting_add(group,"Port",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,settings.input.port);
			config_setting_set_string(setting,value);

			setting = config_setting_add(group,"Slot",CONFIG_TYPE_STRING);
			cfg_int_to_string(value,settings.input.slot);
			config_setting_set_string(setting,value);

			setting = config_setting_add(group,"Confirm",CONFIG_TYPE_STRING);
			if (settings.input.confirm == PAD_CROSS)
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

			setting = config_setting_add(group,"Mass",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,settings.devices.mass);

			setting = config_setting_add(group,"HDD",CONFIG_TYPE_BOOL);
			config_setting_set_bool(setting,settings.devices.hdd);

		}

	}

}


