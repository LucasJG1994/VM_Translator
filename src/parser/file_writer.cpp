#include "file_writer.h"

#include <iostream>
#include <string>
#include <sstream>

extern "C" {
#include "vmth.tab.h"
#include <stdio.h>
#include <stdarg.h>
}

static FILE* fp;
static std::string src;
static int ret_count = 0;
static int cmp_count = 0;

static void fw(const char* fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	long len = vsnprintf(NULL, 0, fmt, ap);

	char* msg = new char[len + 1];
	if (msg == nullptr) {
		va_end(ap);
		return;
	}

	vsprintf(msg, fmt, ap);
	va_end(ap);

	std::string self = std::string(msg);
	src += (self + "\n");

	delete[] msg;
}

extern "C" void fw_init() {
	fw("sp = 0");
	fw("lcl = 1");
	fw("arg = 2");
	fw("this = 3");
	fw("that = 4");
	fw("temp = 5");
	fw("static = 16");

	fw("mov sp, 256");
	fw("mov lcl, 256");
	fw("mov arg, 256");
}

extern "C" void fw_push(int seg, int index) {
	switch (seg) {
		case TK_ARG:
			fw("; PUSH ARGUMENT %d", index);
			fw("add r14, arg, %d", index);
			fw("mov [sp], [r14]");
			fw("add sp, sp, 1");
			break;
		case TK_LCL:
			fw("; PUSH LOCAL %d", index);
			fw("add r14, lcl, %d", index);
			fw("mov [sp], [r14]");
			fw("add sp, sp, 1");
			break;
		case TK_STC:
			fw("; PUSH STATIC %d", index);
			fw("add, r14, &static, %d", index);
			fw("mov [sp], [r14]");
			fw("add sp, sp, 1");
			break;
		case TK_CST:
			fw("; PUSH CONSTANT %d", index);
			fw("mov [sp], %d", index);
			fw("add sp, sp, 1");
			break;
		case TK_THS:
			fw("; PUSH THIS %d", index);
			fw("add r14, this, %d", index);
			fw("mov [sp], [r14]");
			fw("add sp, sp, 1");
			break;
		case TK_THT:
			fw("; PUSH THAT %d", index);
			fw("add r14, that, %d", index);
			fw("mov [sp], [r14]");
			fw("add sp, sp, 1");
			break;
		case TK_PTR:
			fw("; PUSH POINTER %d", index);
			switch (index) {
				case 0:
					fw("mov [sp], this");
					fw("add sp, sp, 1");
					break;
				case 1:
					fw("mov [sp], that");
					fw("add sp, sp, 1");
					break;
			}
			break;
		case TK_TMP:
			fw("; PUSH TEMP %d", index);
			fw("add r14, &temp, %d", index);
			fw("mov [sp], [r14]");
			fw("add sp, sp, 1");
			break;
	}
}

extern "C" void fw_pop(int seg, int index) {
	switch(seg){
		case TK_ARG:
			fw("; POP ARGUMENT %d", index);
			fw("sub sp, sp, 1");
			fw("add r14, arg, %d", index);
			fw("mov [r14], [sp]");
			break;
		case TK_LCL:
			fw("; POP LOCAL %d", index);
			fw("sub sp, sp, 1");
			fw("add r14, lcl, %d", index);
			fw("mov [r14], [sp]");
			break;
		case TK_STC:
			fw("; POP STATIC %d", index);
			fw("sub sp, sp, 1");
			fw("add r14, &static, %d", index);
			fw("mov [r14], [sp]");
			break;
		case TK_THS:
			fw("; POP THIS %d", index);
			fw("sub sp, sp, 1");
			fw("add r14, this, %d", index);
			fw("mov [r14], [sp]");
			break;
		case TK_THT:
			fw("// POP THAT %d", index);
			fw("sub sp, sp, 1");
			fw("add r14, that, %d", index);
			fw("mov [r14], [sp]");
			break;
		case TK_PTR:
			fw("; POP POINTER %d", index);
			switch (index) {
				case 0:
					fw("sub sp, sp, 1");
					fw("mov this, [sp]");
					break;
				case 1:
					fw("sub sp, sp, 1");
					fw("mov that, [sp]");
					break;
			}
			break;
		case TK_TMP:
			fw("; POP TEMP %d", index);
			fw("sub sp, sp, 1");
			fw("add r14, &temp, %d", index);
			fw("mov [r14], [sp]");
			break;
	}
}

extern "C" void fw_call(const char* name, int args) {
	fw("; Get offset for ARG");
	fw("sub r13, sp, %d", args);

	fw("; Save Return Address");
	fw("mov [sp], &ret_%s_%d_%d", name, args, ret_count);
	fw("add sp, sp, 1");

	fw("; Save LCL");
	fw("mov [sp], lcl");
	fw("add sp, sp, 1");

	fw("; Save ARG");
	fw("mov [sp], arg");
	fw("add sp, sp, 1");

	fw("; Save THIS");
	fw("mov [sp], this");
	fw("add sp, sp, 1");

	fw("; Save THAT");
	fw("mov [sp], that");
	fw("add sp, sp, 1");

	fw("; Set LCL");
	fw("mov lcl, sp");

	fw("; Set ARG");
	fw("mov arg, r13");

	fw("; Call function and set up return label");
	fw("jmp %s_%d", name, args);
	fw("ret_%s_%d_%d", name, args, ret_count++);
}

extern "C" void fw_return() {
	fw("; Set up stack ptr for popping stack frame");
	fw("mov sp, lcl");

	fw("; Restore THAT");
	fw("sub sp, sp, 1");
	fw("mov that, [sp]");

	fw("; Restore THIS");
	fw("sub sp, sp, 1");
	fw("mov this, [sp]");

	fw("; Restore ARG");
	fw("sub sp, sp, 1");
	fw("mov arg, [sp]");

	fw("; Restore LCL");
	fw("sub sp, sp, 1");
	fw("mov lcl, [sp]");

	fw("; Get return address and jump");
	fw("sub sp, sp, 1");
	fw("jmp [sp]");
}

extern "C" void fw_fndef(const char* name, int args) {
	fw("%s_%d", name, args);
}

extern "C" void fw_goto(const char* name) {
	fw("; GOTO %s", name);
	fw("jmp %s", name);
}

extern "C" void fw_ifgoto(const char* name) {
	fw("; IF-GOTO %s", name);
	fw("sub sp, sp, 1");
	fw("mov r13, [sp]");
	fw("jne %s", name);
}

extern "C" void fw_label(const char* name) {
	fw("%s", name);
}

extern "C" void fw_add() {
	fw("; ADD");
	fw("sub sp, sp, 1");
	fw("mov r14, [sp]");
	fw("sub sp, sp, 1");
	fw("mov r13, [sp]");
	fw("add r13, r13, r14");
	fw("mov [sp], r13");
	fw("add sp, sp, 1");
}

extern "C" void fw_sub() {
	fw("; SUB");
	fw("sub sp, sp, 1");
	fw("mov r14, [sp]");
	fw("sub sp, sp, 1");
	fw("mov r13, [sp]");
	fw("sub r13, r13, r14");
	fw("mov [sp], r13");
	fw("add sp, sp, 1");
}

extern "C" void fw_neg() {
	fw("; NEG");
	fw("sub sp, sp, 1");
	fw("mov r13, [sp]");
	fw("not r13, r13");
	fw("mov [sp], r13");
	fw("add sp, sp, 1");
}

extern "C" void fw_lt() {
	fw("; LT");
	fw("sub sp, sp, 1");
	fw("mov r14, [sp]");
	fw("sub sp, sp, 1");
	fw("mov r13, [sp]");
	fw("sub r13, r13, r14");

	fw("jlt cmp_true_%d", cmp_count);
	fw("mov r13, 0");
	fw("jmp cmp_false_%d", cmp_count);
	fw("cmp_true_%d", cmp_count);
	fw("mov r13, 1");
	fw("cmp_false_%d", cmp_count++);
	fw("mov [sp], r13");
	fw("add sp, sp, 1");
}

extern "C" void fw_gt() {
	fw("; GT");
	fw("sub sp, sp, 1");
	fw("mov r14, [sp]");
	fw("sub sp, sp, 1");
	fw("mov r13, [sp]");
	fw("sub r13, r13, r14");

	fw("jgt cmp_true_%d", cmp_count);
	fw("mov r13, 0");
	fw("jmp cmp_false_%d", cmp_count);
	fw("cmp_true_%d", cmp_count);
	fw("mov r13, 1");
	fw("cmp_false_%d", cmp_count++);
	fw("mov [sp], r13");
	fw("add sp, sp, 1");

}

extern "C" void fw_eq() {
	fw("; EQ");
	fw("sub sp, sp, 1");
	fw("mov r14, [sp]");
	fw("sub sp, sp, 1");
	fw("mov r13, [sp]");
	fw("sub r13, r13, r14");

	fw("jeq cmp_true_%d", cmp_count);
	fw("mov r13, 0");
	fw("jmp cmp_false_%d", cmp_count);
	fw("cmp_true_%d", cmp_count);
	fw("mov r13, 1");
	fw("cmp_false_%d", cmp_count++);
	fw("mov [sp], r13");
	fw("add sp, sp, 1");
}

extern "C" void fw_and() {
	fw("; AND");
	fw("sub sp, sp, 1");
	fw("mov r14, [sp]");
	fw("sub sp, sp, 1");
	fw("mov r13, [sp]");
	fw("and r13, r13, r14");
	fw("mov [sp], r13");
	fw("add sp, sp, 1");
}

extern "C" void fw_or() {
	fw("; OR");
	fw("sub sp, sp, 1");
	fw("mov r14, [sp]");
	fw("sub sp, sp, 1");
	fw("mov r13, [sp]");
	fw("or r13, r13, r14");
	fw("mov [sp], r13");
	fw("add sp, sp, 1");
}

extern "C" void fw_not() {
	fw("; NOT");
	fw("sub sp, sp, 1");
	fw("mov r13, [sp]");
	fw("not r13, r13");
	fw("mov [sp], r13");
	fw("add sp, sp, 1");
}

extern "C" void fw_dump() {
	if (fopen_s(&fp, "out.casm", "wb") != 0) {
		std::cout << "Unable to create file...\n";
		return;
	}

	fwrite(src.c_str(), sizeof(char), src.length(), fp);
	fclose(fp);
}