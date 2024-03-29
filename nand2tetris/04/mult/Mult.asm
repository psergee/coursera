// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

@R2
M=0
@R0
D=M
@R1
D=D-M
@LOOP
D;JGE
@R0
D=M
@temp
M=D
@R1
D=M
@R0
M=D
@temp
D=M
@R1
M=D

// R1 contains min value of two from here, this is done to minimize iterations count
// repeat sum of R0 value R1 times
(LOOP)
@R1
D=M
@END
D;JLE
@R0
D=M
@R2
M=D+M
D=1
@R1
M=M-D
@LOOP
0;JMP

(END)
@END
0;JMP