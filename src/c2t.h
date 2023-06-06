#ifndef __c2t__h
#define __c2t__h

#if __cplusplus
extern "C" {
#endif

#include <tchar.h>

TCHAR* char_to_t(const char* str);

#if __cplusplus
}
#endif

#endif