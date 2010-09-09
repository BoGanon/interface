#ifndef __CONFIG_H__
#define __CONFIG_H__

#include <libconfig.h>

#ifdef __cplusplus
extern "C" {
#endif

	// Opens a configuration from path and returns it
	// Returns NULL on failure
	config_t *config_open(char *path);

	// Closes a configuration
	void config_close(config_t *config);

#ifdef __cplusplus
};
#endif

#endif /*__CONFIG_H__*/
