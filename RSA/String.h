#ifndef H_CSTRING
#define H_CSTRING

#include <string.h>

	#ifdef __cplusplus
		extern "C" {
	#endif
		size_t strlcpy(char *d, const char *s, size_t bufsize);
		size_t strlcat(char *d, const char *s, size_t bufsize);
	#ifdef __cplusplus
		}
	#endif
#endif