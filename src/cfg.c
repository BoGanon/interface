#include <stdio.h>

#include "cfg.h"

#define PRINT_SETTING printf("%s ",setting)

#define PRINT_NOT_FOUND printf("not_found")

#define PRINT_VALUE_NUM printf("\nvalue = %d\n",value)
#define PRINT_VALUE_STR printf("\nvalue = %s\n",value)

config_t *cfg_open(const char *path)
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

void cfg_save(char *path, config_t *cfg)
{

	FILE *file;

	file = fopen(path,"w+");

	if (file == NULL)
	{
		return;
	}

	config_write(cfg,file);

	fclose(file);

}

void cfg_close(config_t *config)
{
	config_destroy(config);
	free(config);
}

void cfg_int_to_string(char *out, int in)
{
	sprintf(out,"%d",in);
}

long cfg_string_to_int(const char *in)
{
	return strtol(in,NULL,0);
}

unsigned char config_get_bool(config_t *config, char *setting, unsigned char initial)
{
	unsigned char value = 0;
	config_setting_t *config_setting = config_lookup(config,setting);

	PRINT_SETTING;

	if (config_setting == NULL)
	{
		PRINT_NOT_FOUND;
		value = initial;
	}
	else
	{
		value = config_setting_get_bool(config_setting);
	}

	PRINT_VALUE_NUM;

	return value;
}

unsigned int config_get_uint(config_t *config, char *setting, unsigned int initial)
{
	unsigned int value = 0;
	config_setting_t *config_setting = config_lookup(config,setting);

	PRINT_SETTING;

	if (config_setting == NULL)
	{
		PRINT_NOT_FOUND;
		value = initial;
	}
	else
	{
		value = config_setting_get_int(config_setting);
	}

	PRINT_VALUE_NUM;

	return value;
}

int config_get_int(config_t *config, char *setting, int initial)
{
	int value = 0;
	config_setting_t *config_setting = config_lookup(config,setting);

	PRINT_SETTING;
	if (config_setting == NULL)
	{
		PRINT_NOT_FOUND;
		value = initial;
	}
	else
	{
		value = config_setting_get_int(config_setting);
	}

	PRINT_VALUE_NUM;

	return value;
}

int config_get_int_elem(config_t *config, char *setting, int element, int initial)
{
	int value = 0;
	config_setting_t *config_setting = config_lookup(config,setting);

	PRINT_SETTING;
	if (config_setting == NULL)
	{
		PRINT_NOT_FOUND;
		value = initial;
	}
	else
	{
		value = config_setting_get_int_elem(config_setting,element);
	}

	PRINT_VALUE_NUM;

	return value;
}

const char *config_get_string(config_t *config, char *setting, const char *initial)
{
	const char *value = NULL;
	config_setting_t *config_setting = config_lookup(config,setting);

	PRINT_SETTING;

	if (config_setting == NULL)
	{
		PRINT_NOT_FOUND;
		value = initial;
	}
	else
	{
		if ((value = config_setting_get_string(config_setting)) == NULL)
		{
			PRINT_NOT_FOUND;
			value = initial;
		}
	}

	PRINT_VALUE_STR;

	return value;
}

const char *config_get_string_elem(config_t *config, char *setting, int element, const char *initial)
{
	const char *value;
	config_setting_t *config_setting = config_lookup(config,setting);

	PRINT_SETTING;
	if (config_setting == NULL)
	{
		PRINT_NOT_FOUND;
		value = initial;
	}
	else
	{
		if ((value = config_setting_get_string_elem(config_setting,element)) == NULL)
		{
			PRINT_NOT_FOUND;
			value = initial;
		}
	}

	PRINT_VALUE_STR;

	return value;

}
