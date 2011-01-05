#ifndef __BROWSER_H__
#define __BROWSER_H__

#include "lists.h"
#include "settings.h"

#ifdef __cplusplus
extern "C" {
#endif

	// Returns path for browser
	char *browser_path();

	// Browser for list
	int browser_list(list_t *list, int buttons, settings_t *settings);

#ifdef __cplusplus
};
#endif

#endif /*__BROWSER_H__*/
