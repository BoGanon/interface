#include <kernel.h>

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <fileXio_rpc.h>

#include <input.h>
#include <font.h>
#include <gs_psm.h>
#include <graph.h>
#include <packet.h>
#include <draw.h>
#include <dma.h>

#include "browser.h"
#include "hdd.h"
#include "init.h"
#include "settings.h"
#include "strkat.h"
#include "gui.h"
#include "lists.h"
#include "video.h"

#define BROWSER_X      8
#define BROWSER_Y      64
#define BROWSER_WIDTH  496
#define BROWSER_HEIGHT 320

#define LIST_DISPLAY 19
#define LIST_SIZE    5000
#define LIST_WIDTH   256
#define LIST_X       BROWSER_X + 5
#define LIST_Y       BROWSER_Y + 5

#define OPTION_BROWSER  0
#define OPTION_START    1
#define OPTION_SETTINGS 2

static char boot_path[256];

static packet_t *packet;

void parse_args(int argc, char **argv)
{

	int i;
	char *temp;

	if ((argc > 0) && argv[0])
	{

		printf("argv[0] = %s\n",argv[0]);
		strcpy(boot_path,argv[0]);

		if (!strncmp(argv[0],"mass",4))
		{
			if (!strncmp(argv[0],"mass0:\\",7))
			{
				boot_path[4] = ':';
				strcpy(boot_path + 5,argv[0] + 6);

				for(i=0; boot_path[i] != 0; i++)
				{
					if(boot_path[i] == '\\')
					{
						boot_path[i] = '/';
					}
				}
			}
		}
		else if (!strncmp(argv[0],"cd",2))
		{
			strcpy(boot_path,"cdfs:/");
		}
		else if (!strncmp(argv[0],"hdd",3))
		{
			boot_path[0] = 0;
		}
#ifndef PS2LINK
		else if (!strncmp(argv[0],"host",4))
		{
			boot_path[0] = 0;
		}
#else
		else if (!strncmp(argv[0],"host",4))
		{
			;
		}
#endif
		else if (!strncmp(argv[0],"pfs",3))
		{
			boot_path[0] = 0;
		}
		else if (!strncmp(argv[0],"mc",2))
		{
			;
		}
		else
		{
			boot_path[0] = 0;
		}
	}
	else
	{
		boot_path[0] = 0;
	}

	if ((temp = strstr(boot_path,".elf")))
	{
		while (*temp != '/') temp--;
		*temp = 0;
		printf("boot_path is now %s\n",boot_path);
	}
	else if ((temp = strstr(boot_path,".ELF")))
	{
		while (*temp != '/') temp--;
		*temp = 0;
		printf("boot_path is now %s\n",boot_path);
	}

}

// Check mc0:/SYS-CONF or mc1:/SYS-CONF for file
// Returns pointer to directory on success
// Returns NULL on failure
char *check_boot(char *file)
{

	FILE *f;
	static char path[256];

	if (boot_path[0])
	{
		sprintf(path,"%s/%s",boot_path,file);

		f = fopen(path,"r");

		if (f != NULL)
		{
			fclose(f);
			printf("Using file %s\n", path);
			return path;
		}
	}

	sprintf(path,"%s/%s","mc0:/SYS-CONF",file);

	f = fopen(path,"r");

	if (f != NULL)
	{
		fclose(f);
		printf("Using file %s\n", path);
		return path;
	}
	else
	{
		path[2] = '1';
	}

	f = fopen(path,"r");

	if (f != NULL)
	{
		fclose(f);
		printf("Using file %s\n", path);
		return path;
	}
	else
	{
		printf("Using embedded %s\n", file);
		return NULL;
	}

}

char *check_home(char *file)
{

	FILE *f;

	static char path[256];
	settings_t settings = settings_get();

	sprintf(path,"%s/%s",settings.home.directory,file);

	printf("path = %s\n", path);

	f = fopen(path,"r");

	if (f != NULL)
	{
		printf("Using file %s\n", path);
		fclose (f);
		return path;
	}
	else
	{
		printf("Using embedded %s\n", file);
		return NULL;
	}

}



void init(char *file)
{

	int i;
	int hdd = 0;

	settings_t settings;

#ifndef PS2LINK
	reset_iop();
#endif

	init_basic_modules(NULL);
	init_cdvd_modules(NULL);

	list_enable_cdfs();

	init_usb_modules(check_boot("modules.tgz"));
	list_enable_mass();

	video_init_dmac();

	settings_init(check_boot(file));

	settings = settings_get();

	if (settings.devices.hdd)
	{
		hdd = 1;
		init_dev9_modules(NULL);
		init_hdd_modules(NULL);
		list_enable_hdd();
	}

	// Now try to setup the home directory path
	if (!strncmp(settings.home.directory,"pfs0",4))
	{

		if (!hdd)
		{
			init_dev9_modules(NULL);
			init_hdd_modules(NULL);
			list_enable_hdd();
		}

		for (i = 0; i < 3; i++)
		{
			unmount_partition(i);
		}

		if (mount_partition(NULL,settings.home.partition,0) < 0)
		{
			strcpy(settings.home.directory,"mc0:/SYS-CONF");
		}

	}

}

void interface_clear_screen()
{

	qword_t *q;

	q = packet->data;

	q = draw_clear(q,0,0,0,512,512,0x00,0x00,0x00);

	// Append finish token event
	q = draw_finish(q);

	// Send the packet and wait until the DMAC is done
	dma_channel_send_normal(DMA_CHANNEL_GIF,packet->data, q - packet->data, 0,0);
	dma_wait_fast();

	// Wait until drawing is finished
	draw_wait_finish();

	video_sync_wait();

}

void interface_draw(int option, list_t *list, fsfont_t *font, int alpha)
{

	qword_t *q;


	vertex_t position;
	color_t color;

	settings_t settings = settings_get();

	position.x = 15;
	position.y = 15;
	position.z = 20;

	color.r = 0xFF;
	color.g = 0xFF;
	color.b = 0x00;
	color.a = 0xFF;
	color.q = 1.0f;

	q = packet->data;

	q = draw_clear(q,0,0,0,512,512,0x80,0x80,0x80);


	// Display background

	if (alpha == 0x80)
	{
		draw_disable_blending();
	}

	if (gui_background_exists())
	{
		q = gui_setup_texbuffer(q,BACKGROUND);
		q = gui_background(q);
	}

	// Display the layout and options
	q = gui_setup_texbuffer(q,SKIN);
	q = gui_basic_layout(q,alpha);

	//draw_enable_blending();
	//q = gui_logo(q,256.0f,16.0f,alpha);
	//draw_disable_blending();

	if (option == OPTION_BROWSER)
	{
		// Display framed box
		q = gui_box(q,BROWSER_X,BROWSER_Y,BROWSER_WIDTH,BROWSER_HEIGHT,1);
	}
	else
	{
		// Display framed box
		q = gui_box(q,BROWSER_X,BROWSER_Y,BROWSER_WIDTH,BROWSER_HEIGHT,0);
	}

	// Disable other options for now
	/*
	if (option == OPTION_START)
	{
		q = gui_button(q,320.0f,290.0f,BUTTON_START,1);
	}
	else
	{
		q = gui_button(q,320.0f,290.0f,BUTTON_START,0);
	}

	if (option == OPTION_SETTINGS)
	{
		q = gui_button(q,320.0f,330.0f,BUTTON_SETTINGS,1);
	}
	else
	{
		q = gui_button(q,320.0f,330.0f,BUTTON_SETTINGS,0);
	}
	*/

	// Display list
	q = gui_setup_texbuffer(q,FONT);
	q = list_display(q,0,list,font);

	if (gui_foreground_exists())
	{
		// Display foreground
		q = gui_setup_texbuffer(q,FOREGROUND);
		q = gui_foreground(q,alpha);
	}

	q = fontstudio_print_string(q,0,settings.home.directory,LEFT_ALIGN,&position,&color,font);

	// Append finish token event
	q = draw_finish(q);

	// Send the packet and wait until the DMAC is done
	dma_channel_send_normal(DMA_CHANNEL_GIF,packet->data, q - packet->data, 0,0);
	dma_wait_fast();

	// Wait until drawing is finished
	draw_wait_finish();

}

void interface_run(void)
{

	unsigned char alpha = 0x00;

	int option = 0;

	unsigned short old_data = 0;
	unsigned short new_pad;

	pad_t *pad;
	fsfont_t *font;
	list_t *list;
	settings_t settings;

	settings = settings_get();

	list = list_init(LIST_X,LIST_Y,LIST_DISPLAY,LIST_SIZE,LIST_WIDTH);

	pad = pad_open(settings.input.port,settings.input.slot,MODE_DIGITAL,MODE_UNLOCKED);

	font = gui_font_get();

	draw_enable_blending();

	while(1)
	{

		alpha++;

		if (alpha >= 0x80)
		{
			alpha = 0x80;
		}

		// Do input
		pad_get_buttons(pad);
		pad->buttons->btns ^= 0xFFFF;

		new_pad  = pad->buttons->btns & old_data;
		old_data = ~pad->buttons->btns;

		// Move between options disabled for now
	/*
		if(new_pad & PAD_LEFT)
		{
			option--;

			if (option < OPTION_BROWSER)
			{
				option = 2;
			}
		}

		if(new_pad & PAD_RIGHT)
		{
			option++;

			if (option > OPTION_SETTINGS)
			{
				option = 0;
			}
		}
	*/

		// Do options
		if (option == OPTION_BROWSER)
		{
			if (browser_list(list,new_pad))
			{
				pad_close(pad);
				return;
			}
		}

		interface_draw(option,list,font,alpha);

		video_sync_wait();

	}

}



void interface_open()
{

	int mode;
	int interlace;

	settings_t settings = settings_get();

	packet = packet_init(10000,PACKET_NORMAL);

	printf("packet = %p\n",packet);

	gui_init(settings.font.height);
	gui_load_skin(check_home("skin.tgz"));

	video_packets_init();
	video_init_framebuffer(512,512);

	mode = settings.display.mode;

	if (mode == GRAPH_MODE_NTSC || mode == GRAPH_MODE_PAL)
	{
		interlace = GRAPH_ENABLE;
		gui_set_screen_height((float)(graph_mode[mode].height * 2.0f));
	}
	else
	{
		if (mode == GRAPH_MODE_HDTV_1080I)
		{
			interlace = GRAPH_ENABLE;
		}
		else
		{
			interlace = GRAPH_DISABLE;
		}
	}

	video_init_screen(settings.display.x,settings.display.y,
					  512,gui_get_screen_height(),interlace,
					  mode);

	video_init_draw_env(512,gui_get_screen_height());

	interface_clear_screen();

}

void interface_close()
{

	gui_free();

	packet_free(packet);

	video_packets_free();

}
