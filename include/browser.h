#ifndef __BROWSER_H__
#define __BROWSER_H__

#include "lists.h"

#ifdef __cplusplus
extern "C" {
#endif

	// Returns path for browser
	char *browser_get_path();

	// Cleans up path related problems like mounted partitions
	void browser_reset_path();

	// Browser for list
	int browser_list(list_t *list, int buttons);

#ifdef __cplusplus
};
#endif

#endif /*__BROWSER_H__*/
