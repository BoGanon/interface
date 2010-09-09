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

#include <gs_psm.h>
#include <draw.h>
#include <packet.h>
#include <dma.h>
#include <graph.h>

// Standard headers
#include <stdio.h>
#include <string.h>

#include <zlib.h>


#include "init.h"

#include "tar.h"
#include "gzip.h"

void init_load_erom(void)
{

	int ret = 0;

	if (SifLoadStartModule("rom0:ADDDRV", 0, NULL, &ret) >= 0);
	else if (ret < 0)
	{
#ifdef DEBUG
		printf("Failed to load start module: ADDDRV\n");
#endif
	}
	if (SifLoadModuleEncrypted("rom1:EROMDRV", 0, NULL) < 0)
	{
#ifdef DEBUG
		printf("Failed to load encrypted module: EROMDRV\n");
#endif
	}

}

#if 0
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
#endif

int init_load_rom0(void)
{

	int i = 0,old = 0;
	int result = 0;

	smod_mod_info_t	mod_t;

	module_t basic_modules[6] = {
		{ "sio2man", "rom0:XSIO2MAN", 257, NULL, 0 },
		{ "mcman"  , "rom0:XMCMAN"  , 257, NULL, 0 },
		{ "mcserv" , "rom0:XMCSERV" , 257, NULL, 0 },
		{ "mtapman", "rom0:XMTAPMAN",   0, NULL, 0 },
		{ "padman" , "rom0:XPADMAN" , 276, NULL, 0 },
		{ "noname",  "rom0:XCDVDMAN",   0, NULL, 0 }
	};

	// Prevent XCDVDMAN from loading to prevent IOP reboot
#ifdef DEBUG
	for (i = 0; i < 5; i++)
#else
	for (i = 0; i < 6; i++)
#endif
	{
		if (!smod_get_mod_by_name(basic_modules[i].name, &mod_t))
		{
			if ((SifLoadStartModule(basic_modules[i].module, 0, NULL,&result) < 0))
			{
#ifdef DEBUG
				printf("Failed to load module: %s\n", basic_modules[i].module);
#endif
				old = 1;
			}
			else if (result)
			{
#ifdef DEBUG
				printf("Failed to load module: %s\n", basic_modules[i].module);
#endif
				old = 1;
			}
		}
		else if (mod_t.version == basic_modules[i].old_version )
		{
			old = 1;
		}
	}

	return old;

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

	strcpy(path,dir);
	strcat(path,"/modules.tgz");

	gz = gzip_load_file(path,&size);

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
				printf("Failed to load module: %s\n", modules[i].module);
#endif
				free(tar);
				return -1;
			}

			if (SifExecModuleBuffer(module, module_size,modules[i].length, modules[i].args, &ret) >= 0);
			else if (ret < 0)
			{
#ifdef DEBUG
				printf("Failed to load module: %s\n", modules[i].module);
#endif
				free(tar);
				return -1;
			}
		}
	}

	free(tar);

	return 0;
}

void init_sbv_patches()
{
	sbv_patch_enable_lmb();
	sbv_patch_disable_prefix_check();
}

void init_draw_env(int x, int y, int width, int height, int fbp, int mode)
{

	packet_t *packet = packet_init(20,PACKET_NORMAL);

	qword_t *q = packet->data;

	framebuffer_t frame;
	zbuffer_t z;

	frame.width = width;
	frame.height = height;
	frame.psm = GS_PSM_16S;
	frame.mask = 0;
	frame.address = fbp;

	z.enable = 0;
	z.method = ZTEST_METHOD_GREATER;
	z.address = 0;
	z.mask = 1;
	z.zsm = 0;

	switch (mode)
	{
		case GRAPH_MODE_AUTO:
		{
			mode = graph_get_region();
		}
		case GRAPH_MODE_NTSC:
		case GRAPH_MODE_PAL:
		{
			graph_set_mode(GRAPH_MODE_INTERLACED,mode,GRAPH_MODE_FIELD,GRAPH_ENABLE);
			break;
		}
		default:
		{
			graph_set_mode(GRAPH_MODE_NONINTERLACED,mode,GRAPH_MODE_FRAME,GRAPH_DISABLE);
			break;
		}
	}

	graph_set_screen(x,y,width,height);
	graph_set_bgcolor(0,0,0);

	switch (mode)
	{
		case GRAPH_MODE_AUTO:
		case GRAPH_MODE_NTSC:
		case GRAPH_MODE_PAL:
		{
			graph_set_framebuffer_filtered(frame.address,width,frame.psm,0,0);
			break;
		}
		default:
		{
			graph_set_framebuffer(0,frame.address,width,frame.psm,0,0);
			break;
		}
	}
	

	graph_enable_output();

	q = draw_setup_environment(q,0,&frame,&z);
	q = draw_dithering(q,DRAW_ENABLE);

	q = draw_finish(q);

	dma_channel_send_normal(DMA_CHANNEL_GIF,packet->data, q - packet->data, 0,0);
	dma_wait_fast();

	packet_free(packet);

}

void init_bios_modules(void)
{

	SifInitRpc(0);

	init_sbv_patches();

	SifIopReboot("rom0:EELOADCNF");

	// Load modules from bios
	init_load_erom();
	init_load_rom0();

	// Init various libraries
	mcInit(MC_TYPE_XMC);

	mtapInit();
	padInit(0);

	mtapPortOpen(0);
	mtapPortOpen(1);

}

void init_basic_modules(const char *dir)
{

	module_t basic_modules[4] =
	{
		{
			"IOX/File_Manager",
			"iomanX.irx",
			0,
			NULL,
			0
		},
		{
			"IOX/File_Manager_Rpc",
			"fileXio.irx",
			0,
			NULL,
			0
		},
		{
			"Poweroff_Handler",
			"poweroff.irx",
			0,
			NULL,
			0
		},
		{
			"dev9_driver",
			"ps2dev9.irx",
			0,
			NULL,
			0
		}
	};

	init_load_irx(dir,basic_modules,4);

	poweroffInit();
	fileXioInit();

}

void init_usb_modules(const char *dir)
{

	static int __initialized = 0;

	module_t usb_modules[2] =
	{
		{
			"usbd",
			"usbd.irx",
			0,
			NULL,
			0
		},
		{
			"usb_mass",
			"usbhdfsd.irx",
			0,
			NULL,
			0
		}
	};

	if (__initialized)
	{
		return;
	}

	init_load_irx(dir,usb_modules,2);

	__initialized = 1;

}

void init_hdd_modules(const char *dir)
{

	static int __initialized = 0;

	static char hddarg[] = "-o" "\0" "4" "\0" "-n" "\0" "20";
	static char pfsarg[] = "-m" "\0" "4" "\0" "-o" "\0" "10" "\0" "-n" "\0" "40";

	module_t hdd_modules[3] =
	{
		{
			"atad",
			"ps2atad.irx",
			0,
			NULL,
			0
		},
		{
			"hdd_driver",
			"ps2hdd.irx",
			0,
			hddarg,
			sizeof(hddarg)
		},
		{
			"pfs_driver",
			"ps2fs.irx",
			0,
			pfsarg,
			sizeof(pfsarg)
		}
	};

	if (__initialized)
	{
		return;
	}

	init_load_irx(dir,hdd_modules,3);

	__initialized = 1;

}
