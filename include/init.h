#ifndef __INIT_H__
#define __INIT_H__

typedef struct {
	char name[25];
	char module[25];
	char *args;
	int arglen;
	int result;
} module_t;

extern unsigned char modules_tgz[];
extern unsigned int  size_modules_tgz;

#ifdef __cplusplus
extern "C" {
#endif

	// Resets IOP and loads needed bios modules
	void reset_iop(void);

	// Loads modules.tgz from directory or uses embedded buffer
	char *init_modules_tgz(const char *path, int *gz_size);

	// Loads modules from bios
	int init_load_bios(module_t *modules, int num);

	// Loads irx modules from tar file in memory
	int init_load_irx(char *gz, int gz_size, module_t *modules, int num);

	// Enables the patches needed for module loading
	void init_sbv_patches(void);

	// Loads modules needed for DVDV support
	void init_load_erom(void);

	// Init basic irx modules
	void init_basic_modules(const char *path);

	// Init dev9 irx modules
	void init_dev9_modules(const char *path);

	// Init usb irx modules
	void init_usb_modules(const char *path);

	// Init hdd irx modules
	void init_hdd_modules(const char *path);

	// Init sound irx modules
	void init_sound_modules(const char *path);

	// Init cdvd irx modules
	void init_cdvd_modules(const char *path);

#ifdef __cplusplus
};
#endif

#endif /*__INIT_H__*/
