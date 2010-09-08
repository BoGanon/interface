#include <stdio.h>
#include <string.h>

#include "tar.h"

int get_file_from_tar(void *tar, int tar_size, const char *name, char **file, int *file_size)
{

	char *filename;
	char *ptr = tar;
	char *offset = ptr;
	int size;
	tarheader_t *tarinfo;

	while(ptr[0] != 0)
	{
		tarinfo = (void *)ptr;
		ptr += 0x200;

		size = (int)strtoul(tarinfo->header.filesize, NULL, 8);

		if ((filename = strrchr(tarinfo->header.filename,'/')) != NULL)
		{
			filename++;
		}
		else
		{
			filename = (char*)tarinfo;
		}

		if (!strcmp(filename, name))
		{

			*file = ptr;
			*file_size = size;

			return 0;
		}

		ptr += size;

		if ((tar_size - (ptr - offset)) < 0x200) break;

		ptr += 0x100;

		if ((ptr - offset) & 0xFF)
		{
			ptr -= ((ptr - offset) & 0xFF);
		}

		if ((ptr - offset) % 0x200 != 0)
		{
			ptr += 0x100;
		}
	}

	// no file found
	return -1;

}

