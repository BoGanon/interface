#include <stdio.h>
#include <string.h>
#include <zlib.h>

#include "gzip.h"

#define GZ_CHUNK 16384

char *gzip_load_file(const char *path, int *size)
{

	FILE *file;
	char *gz;

	file = fopen(path,"r");

	if (file == NULL)
	{
		return NULL;
	}

	fseek(file,0,SEEK_END);
	*size = ftell(file);
	fseek(file,0,SEEK_SET);

	gz = (char*)malloc(*size);

	if (gz == NULL)
	{
		fclose(file);
		return NULL;
	}

	if (fread(gz,1,*size,file) < *size)
	{
		free(gz);
		fclose(file);
		return NULL;
	}

	fclose(file);

	return gz;

}

int gzip_uncompress(void *gz, char *outbuffer)
{
	int have = 0, rv = 0;
	unsigned char *out = (unsigned char*)malloc(GZ_CHUNK);

	z_stream strm;

	strm.zalloc = (alloc_func)0; // default allocation function
	strm.zfree = (free_func)0;   // default free function
	strm.opaque = (voidpf)0;     // can be defined to a custom pointer to manage memory

	strm.next_in = Z_NULL;
	strm.avail_in = 0;

	// 15+16 is gzip inflation only
	// 15+32 is zlib+gzip inflation support
	rv = inflateInit2(&strm, 15+16);

	do
	{
		strm.avail_in = GZ_CHUNK;
		strm.next_in = gz;

		do
		{
			strm.avail_out = GZ_CHUNK;
			strm.next_out = out;

			rv = inflate(&strm, Z_NO_FLUSH);
			if (rv == Z_STREAM_ERROR)
			{
				free(out);
				return rv;
			}

			switch (rv)
			{
				case Z_NEED_DICT:
				{
					rv = Z_DATA_ERROR;
				}
				case Z_DATA_ERROR:
				case Z_MEM_ERROR:
				{
					inflateEnd(&strm);
					free(out);
					return rv;
				}
			}

			// fill output buffer
			have = GZ_CHUNK - strm.avail_out;
			memcpy(outbuffer, out, have);
			outbuffer += have;

		}
		while (strm.avail_out == 0);

		// increment input buffer pointer
		gz += GZ_CHUNK;
	}
	while (rv != Z_STREAM_END);

	inflateEnd(&strm);

	free(out);

	return rv == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

#if 0
void parse_gzheader(void *buffer)
{
	char *filename = NULL;
	char *comment = NULL;
	char *ptr = buffer;
	unsigned short extra_len = 0;
	char *xlenptr = (char*)&extra_len;

	gzheader_t *gzinfo = (gzheader_t*)ptr;

	printf("\n");
	printf("gzip header:\n");
	printf("------------\n");
	printf("id1 = %d\n", gzinfo->header.id1);
	printf("id2 = %d\n", gzinfo->header.id2);
	printf("cm = %d\n", gzinfo->header.cm);
	printf("flags:\n");
	printf("FTEXT    = %s\n", (gzinfo->header.flg & FTEXT ? "on" : "off"));
	printf("FHCRC    = %s\n", (gzinfo->header.flg & FHCRC ? "on" : "off"));
	printf("FEXTRA   = %s\n", (gzinfo->header.flg & FEXTRA ? "on" : "off"));
	printf("FNAME    = %s\n", (gzinfo->header.flg & FNAME ? "on" : "off"));
	printf("FCOMMENT = %s\n", (gzinfo->header.flg & FCOMMENT ? "on" : "off"));
	printf("mtime = %d\n", (int)gzinfo->header.mtime);
	printf("xfl = %d\n", gzinfo->header.xfl);
	printf("os = %d\n",  gzinfo->header.os);
	printf("\n");

	ptr += 10; // pass the basic gzip header

	if (gzinfo->header.flg & FEXTRA)
	{
		xlenptr[0] = ptr[0]; //next two bytes are total size of the extra field
		xlenptr[1] = ptr[1];
		ptr += 2; // skip XLEN bytes
		ptr += extra_len;
	}

	if (gzinfo->header.flg & FNAME)
	{
		filename = ptr;
		printf("Compressed file is %s\n", filename);
		ptr += strlen(filename);
		ptr++;
	}

	if (gzinfo->header.flg & FCOMMENT)
	{
		comment = ptr;
		printf("Comment: %s\n", comment);
		ptr += strlen(comment);
		ptr++;
	}
	printf("\n");

	if (gzinfo->header.flg & FHCRC)
	{
		ptr += 2; // 2-byte crc16 number
	}

	// ptr is now at starting offset of compressed data

	return;

}
#endif

unsigned int gzip_get_size(void *gz, int size)
{
	char *ptr = gz;
	unsigned int realsize = 0;
	char *tmp = (char*)&realsize;

	ptr += size-4; // uncompressed size is the last 4 bytes of the file

	tmp[0] = ptr[0];
	tmp[1] = ptr[1];
	tmp[2] = ptr[2];
	tmp[3] = ptr[3];

	return realsize;
}

