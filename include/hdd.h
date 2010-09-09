#ifndef __HDD_H__
#define __HDD_H__

#define MOUNT_MAX 3

#ifdef __cplusplus
extern "C" {
#endif

	// Checks the mount_list for an already mounted partition
	// Returns the mount point on success or -1 on failure
	int check_mount_list(const char* partition);

	// Mounts a partition and sets the path
	// Returns the mount point on success or -1 on failure
	int mount_partition(char *path, const char *partition, int mount_point);

	// Unmounts a partition
	// Returns 0 on success or -1 on failure
	int unmount_partition(int mount_point);

#ifdef __cplusplus
};
#endif

#endif /*__HDD_H__*/
