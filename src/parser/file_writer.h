#ifndef __file_writer__h
#define __file_writer__h

void fw_init();
void fw_write(const char* fmt, ...);
void fw_close();

#endif