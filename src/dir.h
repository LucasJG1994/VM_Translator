#ifndef __dir__h
#define __dir__h

#define DIR_HANDLE unsigned int

#if _WIN32 || _WIN64

DIR_HANDLE   dir_open_win(const char* path);
const char** dir_gfl_win(DIR_HANDLE hdir);
void         dir_close_win(DIR_HANDLE hdir);

#define DIR_OPEN(PATH)            dir_open_win(PATH)
#define DIR_GET_FILE_LIST(HND) dir_gfl_win(HND);
#define DIR_CLOSE(HND)         dir_close_win(HND);

#endif

#endif