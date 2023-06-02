#ifndef __file_writer__h
#define __file_writer__h

#if __cplusplus
extern "C" {
#endif

void fw_init();
void fw_push(int seg, int index);
void fw_pop(int seg, int index);
void fw_call(const char* name, int args);
void fw_return();
void fw_goto(const char* name);
void fw_ifgoto(const char* name);
void fw_label(const char* name);
void fw_fndef(const char* name, int args);
void fw_add();
void fw_sub();
void fw_neg();
void fw_eq();
void fw_gt();
void fw_lt();
void fw_and();
void fw_or();
void fw_not();
void fw_dump();

#if __cplusplus
}
#endif

#endif