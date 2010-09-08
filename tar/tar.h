#ifndef __TAR_H__
#define __TAR_H__

typedef union {
	char ptr[512];
	struct {
		char filename[100];
		char filemode[8];
		char userid[8];
		char groupid[8];
		char filesize[12];
		char lastmod[12];
		char checksum[8];
		char link_ind;
		char linkname[100];
	} header;
} tarheader_t;

#ifdef __cplusplus
extern "C" {
#endif

	// Retrieves a pointer and size of a file by name in a tar file
	int get_file_from_tar(void *tar, int tar_size, const char *name, char **file, int *file_size);

#ifdef __cplusplus
};
#endif

#endif // __TAR_H__
