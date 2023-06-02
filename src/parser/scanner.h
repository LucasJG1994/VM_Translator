#ifndef __scanner__h
#define __scanner__h

#if __cplusplus
extern "C" {
#endif

void yy_init(const char* source);
int  yylex();

#if __cplusplus
}
#endif

#endif