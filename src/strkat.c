#include <string.h>

char *strkat(char *dest, const char *src)
{
	strcpy(dest+strlen(dest),src);

	return dest;
}
