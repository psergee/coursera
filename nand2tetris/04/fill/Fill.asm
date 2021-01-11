// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input.
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel;
// the screen should remain fully black as long as the key is pressed.
// When no key is pressed, the program clears the screen, i.e. writes
// "white" in every pixel;
// the screen should remain fully clear as long as no key is pressed.

(MAIN_LOOP)
@SCREEN
D=A
@video_mem_ptr
M=D

@KBD
D=M
@CLR_SCR
D;JEQ

@8192
D=A
(BLACK_SCREEN_LOOP)
@video_mem_ptr
A=M
M=-1
@video_mem_ptr
M=M+1
D=D-1
@BLACK_SCREEN_LOOP
D;JGT
@MAIN_LOOP
0;JMP

(CLR_SCR)
@8192
D=A
(CLR_SCR_LOOP)
@video_mem_ptr
A=M
M=0
@video_mem_ptr
M=M+1
D=D-1
@CLR_SCR_LOOP
D;JGT
@MAIN_LOOP
0;JMP
