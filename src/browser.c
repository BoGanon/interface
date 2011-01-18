#include <stdio.h>
#include <string.h>
#include <libgen.h>

#include <input.h>

#include "browser.h"
#include "hdd.h"
#include "settings.h"

#include "strkat.h"

static char path[256];

char *browser_get_path()
{
	return path;
}

void browser_reset_path(void)
{
	char *temp;

	// Get rid of previous directory
	temp = path + strlen(path);

	while (*temp != '/') temp--;
	temp++;

	*temp = 0;

	printf("path is %s\n",path);
	// If path is pfs1, it's a non-home mounted partition
	//if (!strncmp(path,"pfs1",4))
	{
		//unmount_partition(1);
	}

	//path[0] = 0;

}

int browser_list(list_t *list, int buttons)
{

	char *temp;

	static short history[50];

	static int filled = 0;
	static int index = 0;

	settings_t settings = settings_get();

	if(buttons & PAD_DOWN)
	{
		list->selection++;

		if (!(list->selection < list->num))
		{
			list->selection = 0;
		}

	}

	if(buttons & PAD_UP)
	{
		list->selection--;

		if (list->selection < 0)
		{
			list->selection = list->num - 1;
		}
	}

	if(buttons & settings.input.confirm)
	{

		printf("entry = %s\n", list->entries[list->selection]);
		printf("path = %s\n", path);

		// Going backwards
		if (!strcmp(list->entries[list->selection],".."))
		{

			// Reset current history index
			history[index] = 0;
			index--;

			// Shouldn't happen, but just in case
			if (index <= 0)
			{
				index = 0;
			}

			if (index == 1)
			{
				// Fix the path based on mounted device
				if (!strncmp(path,"mc",2))
				{
					strcpy(path,"mc");
				}
				if (!strncmp(path,"mass",4))
				{
					strcpy(path,"mass");
				}
				if (!strncmp(path,"cdfs",4))
				{
					strcpy(path,"cdfs");
				}
				if (!strncmp(path,"pfs1",4))
				{
					unmount_partition(1);
					strcpy(path,"hdd");
				}
				if (!strncmp(path,"pfs0",4))
				{
					strcpy(path,"hdd");
				}
			}

			if (index > 1)
			{

				// Not sure why my dirname implementation isn't working...
				// Goes along with the strcat() bug
				temp = path + strlen(path);
				temp--;
				temp--;

				while (*temp != '/') temp--;
				temp++;

				*temp = 0;

			}

			filled = 0;

		}
		// Going forwards
		else
		{

			if (index == 0)
			{
				// index == 0 is device mounts list
				strcpy(path,list->entries[list->selection]);
				history[index] = 0;
			}

			if (index == 1)
			{
				// index == 1 is for mounting devices or any additional handling to get a root directory
				if (!strcmp(path,"hdd"))
				{
					mount_partition(path,list->entries[list->selection],1);
				}
				else
				{
					strcpy(path,list->entries[list->selection]);
				}
				history[index] = 0;
			}

			if (index > 1)
			{
				// If the selection is not a directory
				if (!strchr(list->entries[list->selection],'/'))
				{
					strkat(path,list->entries[list->selection]);
					filled = 0;
					history[index] = list->selection;
					return 1;
				}
				else
				{
					// If not then add entry to the path
					printf("selection = %s\n",list->entries[list->selection]);
					strkat(path,list->entries[list->selection]);
					history[index] = list->selection;
				}
			}

			// Reset selection to 0
			list->selection = 0;
			filled = 0;
			index++;

		}
	}

	if (!filled)
	{

		// List the directory, clearing only entries previously used
		list_clear(list,list->num);

		// List root if index is at start
		// else list directory
		if (index == 0)
		{
			list_device_types(list);
			list_sort(list,list->num,LIST_NORMAL);
		}
		else if (index == 1)
		{
			list_mountable_devices(path,list);
			list_sort(list,list->num-1,LIST_NORMAL);
		}
		else
		{

			list_path(path,list);
			printf("num = %d\n", list->num);

			list_sort(list,list->num-1,LIST_NORMAL);

		}

		list->selection = history[index];

		// List has been filled
		filled = 1;

	}

	return 0;

}
