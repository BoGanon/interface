#include <stdio.h>
#include <string.h>
#include <libgen.h>

#include <input.h>

#include "browser.h"
#include "hdd.h"

static char path[256];

char *browser_path()
{
	return path;
}

int browser_list(list_t *list, int buttons, settings_t *settings)
{

	static int index = 0;

	static int prev_mnt = 0;

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

	if(buttons & settings->input.confirm)
	{

		//printf("entry = %s\n", list.entries[list.selection]);
		//printf("path = %s\n", path);

		// Going backwards
		if (!strcmp(list->entries[list->selection],".."))
		{
			index--;

			// Shouldn't happen, but just in case
			if (index <= 0)
			{
				// No need to worry about the path
				index = 0;
			}
			else if (index == 1)
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
				if (!strncmp(path,"pfs",3))
				{
					if (!prev_mnt)
					{
						unmount_partition(1);
					}
					else
					{
						prev_mnt = 0;
					}
					strcpy(path,"hdd");
				}
			}
			else if (index > 1)
			{
				strcpy(path,dirname(path));

				if (index > 2)
				{
					strcat(path,"/");
				}
			}
		}
		// Going forwards
		else
		{

			if (index == 0)
			{
				// index == 1 is device mounts list
				strcpy(path,list->entries[list->selection]);
			}
			else if (index == 1)
			{
				// index == 2 is for mounting devices or any additional handling to get a root directory
				if (!strcmp(path,"hdd"))
				{
					if (!strcmp(list->entries[list->selection],settings->home.partition))
					{
						prev_mnt = 1;
					}
					mount_partition(path,list->entries[list->selection],1);
				}
				else
				{
					strcpy(path,list->entries[list->selection]);
				}
			}
			else if (index > 1)
			{
				// If the selection is not a directory
				if (!strchr(list->entries[list->selection],'/'))
				{
					printf("file\n");
					index = 0;
					prev_mnt = 0;
					return 1;
				}
				else
				{
					// If not then add entry to the path
					strcat(path,list->entries[list->selection]);
				}
			}

			index++;
		}

		//printf("index = %d\n", index);
		//printf("path = %s\n", path);

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

			list_sort(list,list->num-1,LIST_NORMAL);

		}

		// Reset selection to 0
		list->selection = 0;

	}

	return 0;

}
