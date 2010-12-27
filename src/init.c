#include <kernel.h>

// Module related
#include <smem.h>
#include <smod.h>
#include <loadfile.h>

// SIF RPC related
#include <sifrpc.h>

// sbv patches
#include <sbv_patches.h>

// IOP rebooting
#include <iopcontrol.h>

// Library initialization
#include <libmc.h>
#include <libmtap.h>
#include <libpad.h>
#include <libpwroff.h>
#include <fileXio_rpc.h>
#include <audsrv.h>

#include <gs_psm.h>
#include <draw.h>
#include <packet.h>
#include <dma.h>
#include <graph.h>

// Standard headers
#include <stdio.h>
#include <string.h>

#include <zlib.h>

#include "tar.h"
#include "gzip.h"

#include "init.h"

// Builtin modules
extern unsigned char modules_tgz[];
extern unsigned int  size_modules_tgz;

static int __dev9_initialized = 0;

void init_load_erom(void)
{
	int i;
	int ret = 0;
	int pos = 0;

	char mg_region[10];
	char eromdrv[15];

	if (SifLoadStartModule("rom0:ADDDRV", 0, NULL, &ret) < 0)
	{
#ifdef DEBUG
		printf("Failed to load module: ADDDRV\n");
#endif
	}

	if (ret)
	{
#ifdef DEBUG
		printf("Failed to start module: ADDDRV\n");
#endif
	}

	strcpy(mg_region, "ACEJMORU");
	strcpy(eromdrv,"rom1:EROMDRVA");

	pos = strlen(eromdrv)-1;

	for (i = 0; i < 9; i++)
	{
		eromdrv[pos] = mg_region[i];
	
		if (SifLoadModuleEncrypted(eromdrv, 0, NULL) < 0)
		{
#ifdef DEBUG
			printf("Failed to load encrypted module: EROMDRV\n");
#endif
		}
		else
		{
			break;
		}
	}

}

void list_loaded_modules(void)
{
	char search_name[60];
	smod_mod_info_t	mod_t;

	smod_get_next_mod(NULL,&mod_t);

	smem_read(mod_t.name, search_name, sizeof search_name);

	printf("Module %d is %s\n", mod_t.id, search_name);

	while(smod_get_next_mod(&mod_t,&mod_t))
	{
		smem_read(mod_t.name, search_name, sizeof search_name);
		printf("Module %d is %s\n", mod_t.id, search_name);
	}

}

int init_load_bios(module_t *modules, int num)
{

	int i = 0;
	int ret = 0;

	smod_mod_info_t	mod_t;

	for (i = 0; i < num; i++)
	{
		if (!smod_get_mod_by_name(modules[i].name, &mod_t))
		{
			if ((SifLoadStartModule(modules[i].module, 0, NULL,&modules[i].result) < 0))
			{
#ifdef DEBUG
				printf("Failed to load module: %s\n", modules[i].module);
#endif
				ret = -1;
			}

			if (modules[i].result)
			{
#ifdef DEBUG
				printf("Failed to start module: %s\n", modules[i].module);
#endif
				ret = -2;
			}
		}
		else
		{
#ifdef DEBUG
			printf("Possible module conflict\n");
#endif
			ret = -3;
		}
	}

	return ret;

}

int init_load_irx(const char *dir, module_t *modules, int num)
{

	char path[256];
	int i,size,ret = 0;

	char *gz = NULL;
	char *tar;

	char *module;
	int module_size;

	smod_mod_info_t mod_t;

	if (dir != NULL)
	{
		strcpy(path,dir);
		strcat(path,"/modules.tgz");

		gz = gzip_load_file(path,&size);
	}
	else
	{
		gz = modules_tgz;
		size = size_modules_tgz;
	}

	if (gz == NULL)
	{
		return -1;
	}

	size = gzip_get_size(gz,size);
	tar = malloc(size);

	if ((ret = gzip_uncompress(gz,tar)) != Z_OK)
	{
		free(gz);
		free(tar);
		return -1;
	}

	free(gz);

	for(i = 0; i < num; i++)
	{
		if(!smod_get_mod_by_name(modules[i].name, &mod_t))
		{
			if (get_file_from_tar(tar,size,modules[i].module, &module, &module_size) < 0)
			{
#ifdef DEBUG
				printf("Failed to find module: %s\n", modules[i].module);
#endif
				free(tar);
				modules[i].result = -1;
				return -1;
			}

			if (SifExecModuleBuffer(module, module_size,modules[i].arglen, modules[i].args, &modules[i].result) < 0)
			{
#ifdef DEBUG
				printf("Failed to load module: %s\n", modules[i].module);
#endif
				free(tar);
				modules[i].result = -2;
				return -2;
			}

			if (modules[i].result)
			{
				printf("Failed to start module: %s\n", modules[i].module);
				free(tar);
				return -3;
			}

		}
		else
		{
#ifdef DEBUG
			printf("Possible module conflict\n");
#endif
			free(tar);
			return -4;
		}
	}

	free(tar);

	return 0;
}

void init_sbv_patches()
{
	sbv_patch_enable_lmb();

	// Needed for loading ELFS from memory card
	sbv_patch_disable_prefix_check();
}

unsigned short int detect_bios_version(void)
{

	int fd;
	char romver_buffer[5];

	fd=fioOpen("rom0:ROMVER", O_RDONLY);

	// Read the PS2's BIOS version from rom0:ROMVER.
	fioRead(fd, romver_buffer, 4);

	fioClose(fd);

	// Null terminate the string.
	romver_buffer[4]='\0';

	// Return the PS2's BIOS version.
	return(strtoul(romver_buffer, NULL, 16));
}

void reset_iop(void)
{
	static unsigned short int __bios_version = 0;

	// Needed to initialize the SIF dma channel for the reset packet
	SifInitRpc(0);

	// Reset the IOP to prevent a possible failure in file I/O when an incompatible FILEIO module is being used.
	SifIopReset(NULL, 0);
	while(!SifIopSync());

	// Initialize all SIF-services on the EE side (These functions will link up with their corresponding RPCs on the IOP).

	// Initialize the SIF RPC.
	SifInitRpc(0);

	FlushCache(0);
	FlushCache(2);

	// Attempt to automatically determine the PS2's BIOS version.
	__bios_version=detect_bios_version();

	if(__bios_version > 0x0100)
	{
		// Note: This will load XCDVDMAN from rom0: !!
		SifIopReboot("rom0:EELOADCNF");
	}
	else
	{
		// This will load rom0:CDVDMAN. Hence, multitap support will not be available.
		SifIopReboot(NULL);
	}

	init_sbv_patches();

	init_basic_modules(NULL);

}

void init_basic_modules(const char *dir)
{

	module_t basic_modules[7] =
	{
		{              "sio2man", "freesio2.irx", NULL, 0, 0 },
		{                "mcman",    "mcman.irx", NULL, 0, 0 },
		{               "mcserv",   "mcserv.irx", NULL, 0, 0 },
		{              "mtapman", "freemtap.irx", NULL, 0, 0 },
		{               "padman",  "freepad.irx", NULL, 0, 0 },
		{     "IOX/File_Manager",   "iomanX.irx", NULL, 0, 0 },
		{ "IOX/File_Manager_Rpc",  "fileXio.irx", NULL, 0, 0 },
	};

	init_load_irx(dir, basic_modules, 7);

	// Init various libraries
	mcInit(MC_TYPE_MC);

	mtapInit();

	padInit(0);

	mtapPortOpen(0);
	mtapPortOpen(1);

	if (!basic_modules[6].result)
	{
		fileXioInit();
	}
	else
	{
		// Problem initializing fileXio modules
		fioInit();
	}

}

void init_dev9_modules(const char *dir)
{

	module_t dev9_modules[2] =
	{
		{ "Poweroff_Handler", "poweroff.irx", NULL, 0, 0},
		{      "dev9_driver",  "ps2dev9.irx", NULL, 0, 0}
	};

	init_load_irx(dir,dev9_modules,2);

	// Return if the poweroff module failed
	if (dev9_modules[0].result)
	{
		return;
	}

	if (!dev9_modules[1].result)
	{
		__dev9_initialized = 1;
	}

	poweroffInit();

}

void init_usb_modules(const char *dir)
{

	module_t usb_modules[2] =
	{
		{     "usbd",     "usbd.irx", NULL, 0, 0 },
		{ "usb_mass", "usbhdfsd.irx", NULL, 0, 0 }
	};

	init_load_irx(dir,usb_modules,2);

}

void init_hdd_modules(const char *dir)
{

	static char hddarg[] = "-o" "\0" "4" "\0" "-n" "\0" "20";
	static char pfsarg[] = "-m" "\0" "4" "\0" "-o" "\0" "10" "\0" "-n" "\0" "40";

	module_t hdd_modules[3] =
	{
		{       "atad", "ps2atad.irx",   NULL,              0, 0 },
		{ "hdd_driver",  "ps2hdd.irx", hddarg, sizeof(hddarg), 0 },
		{ "pfs_driver",   "ps2fs.irx", pfsarg, sizeof(pfsarg), 0 }
	};

	if (!__dev9_initialized)
	{
		return;
	}

	init_load_irx(dir,hdd_modules,3);

}

void init_sound_modules(const char *dir)
{

	module_t sound_modules[2] =
	{
		{ "freesd", "freesd.irx", NULL, 0, 0 },
		{ "audsrv", "audsrv.irx", NULL, 0, 0 }
	};

	init_load_irx(dir,sound_modules,2);

	audsrv_init();
}

void init_cdvd_modules(const char *dir)
{

	module_t cdvd_modules[2] =
	{
		{ "SMSUTILS", "SMSUTILS.irx", NULL, 0, 0 },
		{  "SMSCDVD",  "SMSCDVD.irx", NULL, 0, 0 }
	};

	init_load_irx(dir,cdvd_modules,2);

}
