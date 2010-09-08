#ifndef __GZIP_H__
#define __GZIP_H__

#define FTEXT    0x01
#define FHCRC    0x02
#define FEXTRA   0x04
#define FNAME    0x08
#define FCOMMENT 0x16

typedef union {
	char ptr[10];
	struct {
		unsigned char id1;
		unsigned char id2;
		unsigned char cm;
		unsigned char flg;
		unsigned char mtime[4];
		unsigned char xfl;
		unsigned char os;
	} header;
} gzheader_t;

#ifdef __cplusplus
extern "C" {
#endif

	//void parse_gzheader(void *buffer);

	// Loads a file into memory and returns pointer to it
	// Returns NULL on failure
	char *gzip_load_file(char *path, int *size);

	// Uncompresses a gzipped file buffer into output buffer
	int gzip_uncompress(void *gz, char *outbuffer);

	// Returns the uncompressed size of gzipped file buffer.
	unsigned int gzip_get_size(void *gz, int size);

#ifdef __cplusplus
};
#endif

#endif // __GZIP_H__
