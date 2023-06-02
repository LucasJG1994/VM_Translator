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
	fw("@256");
	fw("D=A");
	fw("@SP");
	fw("M=D");
	fw("@ARG");
	fw("M=D");
	fw("@LCL");
	fw("M=D");
}

extern "C" void fw_push(int seg, int index) {
	switch (seg) {
		case TK_ARG:
			fw("// PUSH ARGUMENT %d", index);
			fw("@%d", index);
			fw("D=A");
			fw("@ARG");
			fw("A=M+D");
			fw("D=M");
			break;
		case TK_LCL:
			fw("// PUSH LOCAL %d", index);
			fw("@%d", index);
			fw("D=A");
			fw("@LCL");
			fw("A=M+D");
			fw("D=M");
			break;
		case TK_STC:
			fw("// PUSH STATIC %d", index);
			fw("@%d", index);
			fw("D=A");
			fw("@16");
			fw("A=A+D");
			fw("D=M");
			break;
		case TK_CST:
			fw("// PUSH CONSTANT %d", index);
			fw("@%d", index);
			fw("D=A");
			break;
		case TK_THS:
			fw("// PUSH THIS %d", index);
			fw("@%d", index);
			fw("D=A");
			fw("@THIS");
			fw("A=D+M");
			fw("D=M");
			break;
		case TK_THT:
			fw("// PUSH THAT %d", index);
			fw("@%d", index);
			fw("D=A");
			fw("@THAT");
			fw("A=D+M");
			fw("D=M");
			break;
		case TK_PTR:
			fw("// PUSH POINTER %d", index);
			switch (index) {
				case 0:
					fw("@THIS");
					fw("D=M");
					break;
				case 1:
					fw("@THAT");
					fw("D=M");
					break;
			}
			break;
		case TK_TMP:
			fw("// PUSH TEMP %d", index);
			fw("@%d", index);
			fw("D=A");
			fw("@5");
			fw("A=D+A");
			fw("D=M");
			break;
	}

	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("@SP");
	fw("M=M+1");
}

extern "C" void fw_pop(int seg, int index) {
	fw("// Get value from stack and dec stack ptr.");
	fw("@SP");
	fw("M=M-1");
	fw("D=M");
	fw("@R13");
	fw("M=D");

	switch(seg){
		case TK_ARG:
			fw("// POP ARGUMENT %d", index);
			fw("@%d", index);
			fw("D=A");
			fw("@ARG");
			fw("D=M+D");
			fw("@R14");
			fw("M=D");
			break;
		case TK_LCL:
			fw("// POP LOCAL %d", index);
			fw("@%d", index);
			fw("D=A");
			fw("@LCL");
			fw("D=M+D");
			fw("@R14");
			fw("M=D");
			break;
		case TK_STC:
			fw("// POP STATIC %d", index);
			fw("@%d", index);
			fw("D=A");
			fw("@16");
			fw("D=A+D");
			fw("@R14");
			fw("M=D");
			break;
		case TK_THS:
			fw("// POP THIS %d", index);
			fw("@%d", index);
			fw("D=A");
			fw("@THIS");
			fw("D=D+M");
			fw("@R14");
			fw("M=D");
			break;
		case TK_THT:
			fw("// POP THAT %d", index);
			fw("@%d", index);
			fw("D=A");
			fw("@THAT");
			fw("D=D+M");
			fw("@R14");
			fw("M=D");
			break;
		case TK_PTR:
			fw("// POP POINTER %d", index);
			switch (index) {
				case 0:
					fw("@THIS");
					fw("D=M");
					fw("@R14");
					fw("M=D");
					break;
				case 1:
					fw("@THAT");
					fw("D=M");
					fw("R14");
					fw("M=D");
					break;
			}
			break;
		case TK_TMP:
			fw("// POP TEMP %d", index);
			fw("@%d", index);
			fw("D=A");
			fw("@5");
			fw("D=D+A");
			fw("@R14");
			fw("M=D");
			break;
	}

	fw("@R13");
	fw("D=M");
	fw("@R14");
	fw("A=M");
	fw("M=D");
}

extern "C" void fw_call(const char* name, int args) {
	fw("// Get offset for ARG");
	fw("@%d", args);
	fw("D=A");
	fw("@SP");
	fw("D=M-D");
	fw("@R13");
	fw("M=D");

	fw("// Save Return Address");
	fw("@ret_%s_%d_%d", name, args, ret_count);
	fw("D=A");
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("D=A+1");
	fw("@SP");
	fw("M=D");

	fw("// Save LCL");
	fw("@LCL");
	fw("D=M");
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("D=A+1");
	fw("@SP");
	fw("M=D");

	fw("// Save ARG");
	fw("@ARG");
	fw("D=M");
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("D=A+1");
	fw("@SP");
	fw("M=D");

	fw("// Save THIS");
	fw("@THIS");
	fw("D=M");
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("D=A+1");
	fw("@SP");
	fw("M=D");

	fw("// Save THAT");
	fw("@THAT");
	fw("D=M");
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("D=A+1");
	fw("@SP");
	fw("M=D");

	fw("// Set LCL");
	fw("@LCL");
	fw("M=D");

	fw("// Set ARG");
	fw("@R13");
	fw("D=M");
	fw("@ARG");
	fw("M=D");

	fw("// Call function and set up return label");
	fw("@%s_%d", name, args);
	fw("0;JMP");
	fw("(ret_%s_%d_%d)", name, args, ret_count++);
}

extern "C" void fw_return() {
	fw("// Set up stack ptr for popping stack frame");
	fw("@LCL");
	fw("D=M");
	fw("@SP");
	fw("M=D");

	fw("// Restore THAT");
	fw("@SP");
	fw("M=M-1");
	fw("A=M");
	fw("D=M");
	fw("@THAT");
	fw("M=D");

	fw("// Restore THIS");
	fw("@SP");
	fw("M=M-1");
	fw("A=M");
	fw("D=M");
	fw("@THIS");
	fw("M=D");

	fw("// Restore ARG");
	fw("@SP");
	fw("M=M-1");
	fw("A=M");
	fw("D=M");
	fw("@ARG");
	fw("M=D");

	fw("// Restore LCL");
	fw("@SP");
	fw("M=M-1");
	fw("A=M");
	fw("D=M");
	fw("@LCL");
	fw("M=D");

	fw("// Get return address and jump");
	fw("@SP");
	fw("M=M-1");
	fw("A=M");
	fw("A=M");
	fw("0;JMP");
}

extern "C" void fw_fndef(const char* name, int args) {
	fw("(%s_%d)", name, args);
}

extern "C" void fw_goto(const char* name) {
	fw("// GOTO %s", name);
	fw("@%s", name);
	fw("0;JMP");
}

extern "C" void fw_ifgoto(const char* name) {
	fw("// IF-GOTO %s", name);
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@%s", name);
	fw("D;JNE");
}

extern "C" void fw_label(const char* name) {
	fw("(%s)", name);
}

extern "C" void fw_add() {
	fw("// ADD");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("M=D");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("D=D+M");
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("@SP");
	fw("M=M+1");
}

extern "C" void fw_sub() {
	fw("// SUB");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("M=D");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("D=D-M");
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("@SP");
	fw("M=M+1");
}

extern "C" void fw_neg() {
	fw("// NEG");
	fw("D=0");
	fw("@SP");
	fw("AM=M-1");
	fw("D=D-M");
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("@SP");
	fw("M=M+1");
}

extern "C" void fw_lt() {
	fw("// LT");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("M=D");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("D=D-M");
	fw("@cmp_true_%d", cmp_count);
	fw("D;JLT");
	fw("D=0");
	fw("@cmp_false_%d", cmp_count);
	fw("0;JMP");
	fw("(cmp_true_%d)", cmp_count);
	fw("D=1");
	fw("(cmp_false_%d)", cmp_count++);
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("@SP");
	fw("M=M+1");

}

extern "C" void fw_gt() {
	fw("// GT");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("M=D");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("D=D-M");
	fw("@cmp_true_%d", cmp_count);
	fw("D;JGT");
	fw("D=0");
	fw("@cmp_false_%d", cmp_count);
	fw("0;JMP");
	fw("(cmp_true_%d)", cmp_count);
	fw("D=1");
	fw("(cmp_false_%d)", cmp_count++);
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("@SP");
	fw("M=M+1");

}

extern "C" void fw_eq() {
	fw("// EQ");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("M=D");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("D=D-M");
	fw("@cmp_true_%d", cmp_count);
	fw("D;JEQ");
	fw("D=0");
	fw("@cmp_false_%d", cmp_count);
	fw("0;JMP");
	fw("(cmp_true_%d)", cmp_count);
	fw("D=1");
	fw("(cmp_false_%d)", cmp_count++);
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("@SP");
	fw("M=M+1");

}

extern "C" void fw_and() {
	fw("// AND");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("M=D");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("D=D&M");
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("@SP");
	fw("M=M+1");
}

extern "C" void fw_or() {
	fw("// OR");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("M=D");
	fw("@SP");
	fw("AM=M-1");
	fw("D=M");
	fw("@R13");
	fw("D=D|M");
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("@SP");
	fw("M=M+1");
}

extern "C" void fw_not() {
	fw("// NOT");
	fw("@SP");
	fw("AM=M-1");
	fw("D=!M");
	fw("@SP");
	fw("A=M");
	fw("M=D");
	fw("@SP");
	fw("M=M+1");
}

extern "C" void fw_dump() {
	if (fopen_s(&fp, "test.asm", "wb") != 0) {
		std::cout << "Unable to create file...\n";
		return;
	}

	fwrite(src.c_str(), sizeof(char), src.length(), fp);
	fclose(fp);
}