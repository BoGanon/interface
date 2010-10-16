#ifndef __INIT_H__
#define __INIT_H__

typedef struct {
	char name[25];
	char module[25];
	char *args;
	int arglen;
	int result;
} module_t;

#ifdef __cplusplus
extern "C" {
#endif

	// Resets IOP and loads needed bios modules
	void init_iop(void);

	// Loads modules from bios
	int init_load_bios(module_t *modules, int num);

	// Loads irx modules from gzipped tarball called modules.tgz
	int init_load_irx(const char *dir, module_t *modules, int num);

	// Enables the patches needed for module loading
	void init_sbv_patches(void);

	// Loads modules needed for DVDV support
	void init_load_erom(void);

	// Init basic bios X modules
	void init_x_bios_modules(void);

	// Init basic bios non-X modules
	void init_bios_modules(void);

	// Init basic irx modules
	void init_x_irx_modules(const char *dir);

	// Init usb irx modules
	void init_usb_modules(const char *dir);

	// Init hdd irx modules
	void init_hdd_modules(const char *dir);

	// Init sound irx modules
	void init_sound_modules(const char *dir);

	// Init cdvd irx modules
	void init_cdvd_modules(const char *dir);

#ifdef __cplusplus
};
#endif

#endif /*__INIT_H__*/
