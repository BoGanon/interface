#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/fcntl.h>
#include <fileXio_rpc.h>
#include <libhdd.h>

#include "hdd.h"

char mount_list[MOUNT_MAX][256] = { {0}, {0},{0} };

int check_mount_list(const char *partition)
{

	int i;

	for(i = 0; i < MOUNT_MAX; i++)
	{
		if(!strcmp(mount_list[i],partition))
		{
			return i;
		}
	}

	return -1;
}

int unmount_partition(int mount_point)
{

	int ret = 0;
	char mount_path[6] = "pfs0:";

	if (mount_point >= MOUNT_MAX)
	{
		return -1;
	}

	if (mount_list[mount_point][0] == 0)
	{
		return -1;
	}
	else
	{
		mount_path[3] = '0' + mount_point;
	}

	if ((ret = fileXioUmount(mount_path)) >= 0)
	{
		mount_list[mount_point][0] = 0;
#ifdef DEBUG
		printf("Unmounted pfs%d:\n", mount_point);
#endif
		return 0;
	}
	else
	{
#ifdef DEBUG
		printf("Unmounting failed\n");
#endif
	}

	return ret;

}

int mount_partition(char *path, const char *partition, int mount_point)
{

	int ret;
	char pfs_path[256] = "pfs0:";
	char hdd_path[256] = "hdd0:";

	if (mount_point >= MOUNT_MAX)
	{
		return -1;
	}

	// Check if partition is mounted already
	if ((ret = check_mount_list(partition)) >= 0)
	{
#ifdef DEBUG
		printf("Previously mounted at pfs%d:\n", ret);
#endif
		pfs_path[3] = '0' + ret;
		strcat(pfs_path,"/");
		if (path != NULL)
		{
			strcpy(path,pfs_path);
		}

		return ret;
	}

	// Create paths for mounting
	strcat(hdd_path,partition);
	pfs_path[3] = '0' + mount_point;

	// Try to mount
	if(fileXioMount(pfs_path, hdd_path, FIO_MT_RDWR) >= 0)
	{
		// Copy partition to mount_list
		strcpy(mount_list[mount_point], partition);

		// Add a slash to the path
		strcat(pfs_path,"/");

		if (path != NULL)
		{
			strcpy(path,pfs_path);
		}

		return mount_point;
	}

#ifdef DEBUG
	printf("Mount failed!\n");
#endif

	return -1;

}


