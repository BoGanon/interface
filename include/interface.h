#ifndef __INTERFACE_H__
#define __INTERFACE_H__

#ifdef __cplusplus
extern "C" {
#endif

	// Does system init, only needs to be called once
	void init(const char *file);

	// Checks mc?:/SYS-CONF for file
	// Returns directory if it exists, otherwise NULL
	char *check_memcards(char *file);

	// Checks settings home directory for file
	// Returns directory if it exists, otherwise NULL
	char *check_home(char *file);

	// Initializes data structures and values for GUI
	void interface_open(void);

	// The main interface containing the browser
	// Returns when the browser has a path
	void interface(void);

	// Closes the interface and cleans up
	void interface_close(void);

#ifdef __cplusplus
};
#endif

#endif /*__INTERFACE_H__*/
