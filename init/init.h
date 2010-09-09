#ifndef __INIT_H__
#define __INIT_H__

typedef struct {
	char name[25];
	char module[25];
	short old_version;
	char *args;
	int length;
} module_t;

#ifdef __cplusplus
extern "C" {
#endif

	// Initializes drawing environment
	void init_draw_env(int x, int y, int width, int height, int fbp, int mode);

	// Loads modules needed for DVDV support
	void init_load_erom(void);

	// Loads modules needed for basic support
	// Returns 1 if old modules present
	int init_load_rom0(void);

	// Loads modules.tgz from directory
	int init_load_irx(const char *dir, module_t *modules, int num);

	// Enables the patches needed for module loading
	void init_sbv_patches(void);

	// Resets IOP and loads needed bios modules
	void init_bios_modules(void);

	// Init basic irx modules
	void init_basic_modules(const char *dir);

	// Init usb irx modules
	void init_usb_modules(const char *dir);

	// Init hdd irx modules
	void init_hdd_modules(const char *dir);

	// Init cdvd irx modules
	void init_cdvd_modules(const char *dir);

	// Init sound irx modules
	void init_sound_modules(const char *dir);

#ifdef __cplusplus
};
#endif

#endif /*__INIT_H__*/
