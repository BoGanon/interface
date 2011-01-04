#ifndef __LISTS_H__
#define __LISTS_H__

typedef char entry_t;

typedef struct {
	short x;			// X coordinate for display
	short y;			// Y coordinate for display
	short size;			// Total number of entries in list
	short width;		// Total width of each entry
	short display;		// The number of entries to display
	short num;			// The number of entries currently used in the list
	short selection;	// The current selected entry in the list
	entry_t **entries;	// The actual list of entries
} list_t;

#define LIST_NORMAL  0
#define LIST_REVERSE 1

#ifdef __cplusplus
extern "C" {
#endif

	// Initialize list
	list_t *list_init(int x, int y, int display, size_t size,size_t width);

	// Free list
	void list_free(list_t *list);

	// Allow for clearing a limited number of entries
	void list_clear(list_t *list, size_t num);

	// Allow for sorting a limited number of entries
	void list_sort(list_t *list,size_t num, int method);

	// Add regular entry
	entry_t *add_reg_entry(entry_t **p, char *filename, short number);

	// Add directory entry
	entry_t *add_dir_entry(entry_t **p, char *filename, short number);

	// Draw list
	qword_t *list_display(qword_t *q, int context, list_t *list, fsfont_t *font);

	// Enable mass device
	void list_enable_mass();

	// Enable hdd device
	void list_enable_hdd();

	// Enable cdfs device
	void list_enable_cdfs();

	// Disable mass device
	void list_disable_mass();

	// Disable hdd device
	void list_disable_hdd();

	// Disable cdfs device
	void list_disable_cdfs();

	// List root device types
	void list_device_types(list_t *list);

	// List devices that can be mounted
	void list_mountable_devices(char *path,list_t *list);

	// List partitions
	void list_partitions(list_t *list);

	// List a path
	void list_path(char *path, list_t *list);

#ifdef __cplusplus
};
#endif

#endif /*__LISTS_H__*/
