#include "file_writer.h"
#include <stdio.h>
#include <stdarg.h>

static FILE* fp;

void fw_init() {
	if(fopen_s(&fp, "out.asm", "w") != 0) fp = NULL;
}

void fw_close() {
	if(fp == NULL) return;
	fclose(fp);
}

void fw_write(const char* fmt, ...) {
	if(fp == NULL) return;
	va_list ap;
	va_start(ap, fmt);
	vfprintf(fp, fmt, ap);
	va_end(ap);
}