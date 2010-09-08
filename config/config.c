#include <stdio.h>
#include "config.h"

config_t *config_open(char *path)
{

	FILE *file = NULL;
	config_t *config = (config_t*)malloc(sizeof(config_t));

	if (config == NULL)
	{
		return NULL;
	}

	if (path != NULL)
	{
		file = fopen(path,"r");
	}

	if (file == NULL)
	{
		config_init(config);
	}
	else
	{
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
	}

	fclose(file);

	config_set_tab_width(config,0);

	return config;

}

void config_close(config_t *config)
{
	config_destroy(config);
	free(config);
}
