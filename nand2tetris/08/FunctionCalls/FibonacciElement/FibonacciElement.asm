@256
D=A
@0
M=D
@57
D=A
@0
A=M
M=D
D=A+1
@0
M=D
@1
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@2
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@3
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@4
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@5
D=A
@0
A=M
D=A-D
@2
M=D
@0
D=M
@1
M=D
@Sys.init
0;JMP
(Sys.init)
@0
A=M
D=A
@0
M=D
@4
D=A
@0
A=M
M=D
D=A+1
@0
M=D
@123
D=A
@0
A=M
M=D
D=A+1
@0
M=D
@1
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@2
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@3
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@4
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@6
D=A
@0
A=M
D=A-D
@2
M=D
@0
D=M
@1
M=D
@Main.fibonacci
0;JMP
(WHILE)
@WHILE
0;JMP
(Main.fibonacci)
@0
A=M
D=A
@0
M=D
@0
D=A
@2
A=D+M
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@2
D=A
@0
A=M
M=D
D=A+1
@0
M=D
@0
M=M-1
@0
A=M
D=M
@0
M=M-1
A=M
D=M-D
@162
D=D;JLT
@163
D=0;JMP
D=-1
@0
A=M
M=D
@0
M=M+1
@0
M=M-1
@0
A=M
D=M
@IF_TRUE
D;JNE
@IF_FALSE
0;JMP
(IF_TRUE)
@0
D=A
@2
A=D+M
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@0
M=M-1
@0
A=M
D=M
@13
M=D
@2
D=M
@0
M=D
@1
D=M
@14
M=D
@14
D=M-1
@14
M=D
A=D
D=M
@4
M=D
@14
D=M-1
@14
M=D
A=D
D=M
@3
M=D
@14
D=M-1
@14
M=D
A=D
D=M
@2
M=D
@14
D=M-1
@14
M=D
A=D
D=M
@1
M=D
@14
A=M-1
D=M
@14
M=D
@13
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@14
A=M
0;JMP
(IF_FALSE)
@0
D=A
@2
A=D+M
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@2
D=A
@0
A=M
M=D
D=A+1
@0
M=D
@0
M=M-1
@0
A=M
D=M
@0
M=M-1
A=M
D=M-D
M=D
@0
M=M+1
@335
D=A
@0
A=M
M=D
D=A+1
@0
M=D
@1
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@2
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@3
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@4
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@6
D=A
@0
A=M
D=A-D
@2
M=D
@0
D=M
@1
M=D
@Main.fibonacci
0;JMP
@0
D=A
@2
A=D+M
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@1
D=A
@0
A=M
M=D
D=A+1
@0
M=D
@0
M=M-1
@0
A=M
D=M
@0
M=M-1
A=M
D=M-D
M=D
@0
M=M+1
@419
D=A
@0
A=M
M=D
D=A+1
@0
M=D
@1
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@2
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@3
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@4
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@6
D=A
@0
A=M
D=A-D
@2
M=D
@0
D=M
@1
M=D
@Main.fibonacci
0;JMP
@0
M=M-1
@0
A=M
D=M
@0
M=M-1
A=M
D=M+D
M=D
@0
M=M+1
@0
M=M-1
@0
A=M
D=M
@13
M=D
@2
D=M
@0
M=D
@1
D=M
@14
M=D
@14
D=M-1
@14
M=D
A=D
D=M
@4
M=D
@14
D=M-1
@14
M=D
A=D
D=M
@3
M=D
@14
D=M-1
@14
M=D
A=D
D=M
@2
M=D
@14
D=M-1
@14
M=D
A=D
D=M
@1
M=D
@14
A=M-1
D=M
@14
M=D
@13
D=M
@0
A=M
M=D
D=A+1
@0
M=D
@14
A=M
0;JMP
