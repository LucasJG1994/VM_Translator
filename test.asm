@256
D=A
@SP
M=D
@ARG
M=D
@LCL
M=D
// PUSH CONSTANT 0
@0
D=A
@SP
A=M
M=D
D=A+1
@SP
M=D
// Get value from stack and dec stack ptr.
@SP
M=M-1
D=M
@R13
M=D
// POP LOCAL 0
@0
D=A
@LCL
D=M+D
@R14
M=D
@R13
D=M
@R14
A=M
M=D
(LOOP_START)
// PUSH ARGUMENT 0
@0
D=A
@ARG
A=M+D
D=M
@SP
A=M
M=D
D=A+1
@SP
M=D
// PUSH LOCAL 0
@0
D=A
@LCL
A=M+D
D=M
@SP
A=M
M=D
D=A+1
@SP
M=D
// ADD
@SP
AM=M-1
D=M
@R13
M=D
@SP
AM=M-1
D=M
@R13
D=D+M
@SP
A=M
M=D
@SP
M=M+1
// Get value from stack and dec stack ptr.
@SP
M=M-1
D=M
@R13
M=D
// POP LOCAL 0
@0
D=A
@LCL
D=M+D
@R14
M=D
@R13
D=M
@R14
A=M
M=D
// PUSH ARGUMENT 0
@0
D=A
@ARG
A=M+D
D=M
@SP
A=M
M=D
D=A+1
@SP
M=D
// PUSH CONSTANT 1
@1
D=A
@SP
A=M
M=D
D=A+1
@SP
M=D
// SUB
@SP
AM=M-1
D=M
@R13
M=D
@SP
AM=M-1
D=M
@R13
D=D-M
@SP
A=M
M=D
@SP
M=M+1
// Get value from stack and dec stack ptr.
@SP
M=M-1
D=M
@R13
M=D
// POP ARGUMENT 0
@0
D=A
@ARG
D=M+D
@R14
M=D
@R13
D=M
@R14
A=M
M=D
// PUSH ARGUMENT 0
@0
D=A
@ARG
A=M+D
D=M
@SP
A=M
M=D
D=A+1
@SP
M=D
// IF-GOTO LOOP_START
@SP
AM=M-1
D=M
@LOOP_START
D;JNE
// PUSH LOCAL 0
@0
D=A
@LCL
A=M+D
D=M
@SP
A=M
M=D
D=A+1
@SP
M=D
