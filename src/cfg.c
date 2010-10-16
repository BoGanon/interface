#include <stdio.h>

#include "cfg.h"

config_t *cfg_open(char *path)
{

	FILE *file = NULL;
	config_t *config = (config_t*)malloc(sizeof(config_t));

	if (config == NULL)
	{
		return NULL;
	}

	if (path == NULL || path[0] == 0)
	{
		config_init(config);
		return config;
	}

	file = fopen(path,"r");

	if (file == NULL)
	{
		config_init(config);
		return config;
	}

	// Not initializing produces a crash
	config_init(config);

	if (config_read(config,file) != CONFIG_TRUE)
	{
#ifdef DEBUG
		if (config_error_type(config) == CONFIG_ERR_FILE_IO)
		{
			printf("Error opening configuration file.\n");
		}
		else
		{
			printf("Error parsing configuration file.\n");
			printf("Line: %d\n", config_error_line(config));
			printf("Text: %s\n", config_error_text(config));
		}
#endif
		// Destroy the config and initialize a new one
		config_destroy(config);
		config_init(config);
	}

	fclose(file);

	config_set_tab_width(config,0);

	return config;

}

void cfg_close(config_t *config)
{
	config_destroy(config);
	free(config);
}

unsigned char config_get_bool(config_t *config, char *setting, unsigned char initial)
{
	config_setting_t *config_setting = config_lookup(config,setting);

	if (config_setting == NULL)
	{
		return initial;
	}
	else
	{
		return (unsigned char)config_setting_get_bool(config_setting);
	}
}

unsigned int config_get_uint(config_t *config, char *setting, unsigned int initial)
{
	config_setting_t *config_setting = config_lookup(config,setting);

	if (config_setting == NULL)
	{
		return initial;
	}
	else
	{
		return (unsigned)config_setting_get_int(config_setting);
	}
}

int config_get_int(config_t *config, char *setting, int initial)
{
	config_setting_t *config_setting = config_lookup(config,setting);

	if (config_setting == NULL)
	{
		return initial;
	}
	else
	{
		return config_setting_get_int(config_setting);
	}
}

const char *config_get_string(config_t *config, char *setting, char *initial)
{
	const char *value;
	config_setting_t *config_setting = config_lookup(config,setting);

	if (config_setting == NULL)
	{
		return initial;
	}
	else
	{
		if ((value = config_setting_get_string(config_setting)) == NULL)
		{
			return initial;
		}
		else
		{
			return value;
		}
	}
}
