#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include <font.h>
#include <fileXio_rpc.h>
#include <libhdd.h>

#include <SMS_CDDA.h>
#include <SMS_CDVD.h>

#include "lists.h"

static char mass = 0;
static char cdfs = 0;
static char hdd  = 0;

list_t *list_init(int x, int y, int display, size_t size, size_t width)
{
	int i;

	list_t *list = calloc(1,sizeof(list_t));
	list->entries = calloc(size,sizeof(char*));

	for (i = 0; i < size; i++)
	{
		list->entries[i] = malloc(width);
	}

	list->x = x;
	list->y = y;
	list->display = display;
	list->size = size;
	list->width = width;
	list->num = 0;
	list->selection = 0;

	list_clear(list,size);

	return list;
}

void list_free(list_t *list)
{

	int i;

	for (i = 0; i < list->size; i++)
	{
		free(list->entries[i]);
	}

	free(list->entries);

	free(list);

}

void list_clear(list_t *list, size_t num)
{
	int i;

	for (i = 0; i < num; i++)
	{
		memset(list->entries[i],0,list->width);
	}
}

entry_t *add_reg_entry(entry_t **p, char *name, short number)
{

	strcpy(p[number],name);

	return p[number];
}

entry_t *add_dir_entry(entry_t **p, char *name, short number)
{

	strcpy(p[number],name);
	strcat(p[number],"/");

	return p[number];
}

int entry_cmp_normal(const void *a, const void *b) 
{
	const entry_t **ia = (const entry_t **)a;
	const entry_t **ib = (const entry_t **)b;
	return memcmp(*ia,*ib,256);
}

int entry_cmp_reverse(const void *a, const void *b) 
{
	const entry_t **ia = (const entry_t **)a;
	const entry_t **ib = (const entry_t **)b;
	return memcmp(*ib,*ia,256);
}

void list_sort(list_t *list,size_t num, int method)
{
	int start = 0;

	if (!strcmp(list->entries[0],".."))
	{
		start = 1;
	}

	if (method == LIST_REVERSE)
	{
		qsort(&list->entries[start],num,sizeof(entry_t*),entry_cmp_reverse);
	}
	else
	{
		qsort(&list->entries[start],num,sizeof(entry_t*),entry_cmp_normal);
	}
}

qword_t *list_display(qword_t *q, int context, list_t *list, fsfont_t *font)
{

	int i;
	int entry_height;

	int entry_start = 0;

	vertex_t position;
	color_t regular_color;
	color_t selection_color;

	entry_t *entry;

	regular_color.r = 0x80;
	regular_color.g = 0x80;
	regular_color.b = 0x80;
	regular_color.a = 0x80;
	regular_color.q = 1.0f;

	selection_color.r = 0x80;
	selection_color.g = 0x80;
	selection_color.b = 0x00;
	selection_color.a = 0x80;
	selection_color.q = 1.0f;

	position.x = (float)list->x;
	position.z = 6;

	entry = *(list->entries);

	// Make it so selection scrolls forward at 10 entries from bottom
	if (list->selection > list->display - 10)
	{
		entry_start = list->selection - 10;
	}

	// Make it so selection scrolls to bottom of list at end of list
	if (list->num > list->display)
	{
		if (list->selection >= list->num - 10)
		{
			entry_start = list->num - list->display;
		}
	}

	//printf("entry_start = %d\n", entry_start);

	for (i = 0; i < list->display; i++)
	{

		if (i >= list->num)
		{
			break;
		}

		entry = list->entries[entry_start + i];

		if (entry == NULL)
		{
			break;
		}
	
		entry_height = list->y + (i * font->height);
		position.y = (float)entry_height;

		if((entry_start+i) == list->selection)
		{
			q = fontstudio_print_string(q,0,entry,LEFT_ALIGN,&position,&selection_color,font);
		}
		else
		{
			q = fontstudio_print_string(q,0,entry,LEFT_ALIGN,&position,&regular_color,font);
		}

	}

	return q;

}

void list_enable_mass()
{
	mass = 1;
}

void list_enable_hdd()
{
	hdd = 1;
}

void list_enable_cdfs()
{
	cdfs = 1;
}

void list_disable_mass()
{
	mass = 0;
}

void list_disable_hdd()
{
	hdd = 0;
}

void list_disable_cdfs()
{
	cdfs = 0;
}

void list_device_types(list_t *list)
{

	//int i;
	int n = 0;

	//struct fileXioDevice devices[25];

	//fileXioGetDeviceList(devices,25);

	//for (i = 0; i < 25; i++)
	{
		//if (!strncmp(devices[i].name,"mc",2))
		{
			add_reg_entry(list->entries,"mc",n++);
		}

		//if (!strncmp(devices[i].name,"mass",4))
		if (mass)
		{
			add_reg_entry(list->entries,"mass",n++);
		}

		//if (!strncmp(devices[i].name,"cdfs",4))
		if (cdfs)
		{
			add_reg_entry(list->entries,"cdfs",n++);
		}

		//if (!strncmp(devices[i].name,"hdd",3))
		if (hdd)
		{
			add_reg_entry(list->entries,"hdd",n++);
		}

	}

	list->num = n;

}

void refresh_cdfs()
{

	DiskType type;

	type = CDDA_DiskType();

	if (type == DiskType_DVDV)
	{
		CDVD_SetDVDV(1);
	}
	else
	{
		CDVD_SetDVDV(0);
	}

}

void list_mountable_devices(char *device, list_t *list)
{

	int i,n = 0;

	iox_stat_t stat;

	char mc_path[6] = "mc0:";
	char mass_device[8] = "mass0:";

	add_reg_entry(list->entries,"..",n++);

	if (!strcmp(device,"mc"))
	{
		for (i = 0; i < 2; i++)
		{
			mc_path[2] = '0' + i;
			if(!fileXioGetStat(mc_path,&stat))
			{
				add_dir_entry(list->entries,mc_path,n++);
			}
		}

		list->num = n;

	}

	if (!strcmp(device,"mass"))
	{
		for(i=0; i < 10; i++)
		{

			mass_device[4] = '0'+i;

			if(!(fileXioGetStat(mass_device, &stat) < 0))
			{
				add_dir_entry(list->entries,mass_device,n++);
			}
		}

	// Older versions of the module only support "mass:"
		if (!n)
		{
			if (!(fileXioGetStat("mass:",&stat) < 0))
			{
				add_dir_entry(list->entries,"mass:",n++);
			}

		}

		list->num = n;

	}

	if (!strcmp(device,"hdd"))
	{

		// Checking for "hdd0:" doesn't work, maybe since it's a block device
		//if (!(fileXioGetStat("hdd0:",&stat) < 0))
		{
			list_partitions(list);
		}
	}

	if (!strcmp(device,"cdfs"))
	{
		//if(!(fileXioGetStat("cdfs:",&stat) < 0))
		{
			add_dir_entry(list->entries,"cdfs:", n++);
		}

		CDVD_FlushCache();
		refresh_cdfs();

		list->num = n;

	}
	else
	{
		CDVD_Stop();
	}

}

void list_partitions(list_t *list)
{

	iox_dirent_t hddEnt;
	int hddFd;

	int n = 1;

	printf("listing partitions\n");

	if((hddFd=fileXioDopen("hdd0:")) < 0)
	{
		list->num = n;
		return;
	}

	while(fileXioDread(hddFd, &hddEnt) > 0)
	{
		if((hddEnt.stat.attr != ATTR_MAIN_PARTITION) || (hddEnt.stat.mode != FS_TYPE_PFS))
			continue;

		//Patch this to see if new CB versions use valid PFS format
		//NB: All CodeBreaker versions up to v9.3 use invalid formats
		if(!strncmp(hddEnt.name, "PP.",3))
		{
			int len = strlen(hddEnt.name);

			if(!strcmp(hddEnt.name+len-4, ".PCB"))
				continue;
		}

		if(!strncmp(hddEnt.name, "__", 2))
		{
			if(strcmp(hddEnt.name, "__boot"))
				continue;
			if(strcmp(hddEnt.name, "__net"))
				continue;
			if(strcmp(hddEnt.name, "__system"))
				continue;
			if(strcmp(hddEnt.name, "__sysconf"))
				continue;
			if(strcmp(hddEnt.name, "__common"))
				continue;
		}

		add_reg_entry(list->entries,hddEnt.name, n++);
	}

	fileXioDclose(hddFd);

	list->num = n;

}

void list_path(char *path, list_t *list)
{
	int fd;
	int n = 0;

	iox_dirent_t buf;

	if(!strncmp(path,"cdfs:",5))
	{
		CDVD_FlushCache();
	}

	// Add fake .. directory entry
	add_reg_entry(list->entries, "..", n++);

	// Try to open the path
	if((fd=fileXioDopen(path)) < 0)
	{
		list->num = n;
		return;
	}
	else
	{

		// Add directories first
		while(fileXioDread(fd, &buf) > 0)
		{

			if(buf.stat.mode & FIO_S_IFDIR && (!strcmp(buf.name,".") || !strcmp(buf.name,"..")))
				continue;

			if(buf.stat.mode & FIO_S_IFDIR)
			{
				add_dir_entry(list->entries, buf.name, n++);
			}

			if (buf.stat.mode & FIO_S_IFREG)
			{
				add_reg_entry(list->entries, buf.name, n++);
			}

			// Prevent overflowing the list
			if (n >= list->size)
			{
				break;
			}

		}

		list->num = n;

		fileXioDclose(fd);

	}

}

