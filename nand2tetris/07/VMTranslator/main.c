#include <stdio.h>
#include <string.h>

#define asm_cmd_len 32
const int SP = 0;

struct assembly
{
	char instructions[1024][asm_cmd_len];
	unsigned int current_index;
};

void at(int addr, struct assembly* a)
{
	snprintf(a->instructions[a->current_index], asm_cmd_len, "@%d", addr);
	++a->current_index;
}

void at_label(const char* label, struct assembly* a)
{
	snprintf(a->instructions[a->current_index], asm_cmd_len, "@%s", label);
	++a->current_index;
}

void comp(char dest, const char* compute, struct assembly* a)
{
	snprintf(a->instructions[a->current_index], asm_cmd_len, "%c=%s", dest, compute);
	++a->current_index;
}

void inc_sp(struct assembly* a)
{
	at(SP, a);
	comp('M', "M+1", a);
}

void dec_sp(struct assembly* a)
{
	at(SP, a);
	comp('M', "M-1", a);
}

void read_from_mem(char dest, int pointer_addr, struct assembly* a)
{
	at(pointer_addr, a);
	comp(dest, "M", a);
}

void write_to_mem(int addr, const char* compute, struct assembly* a)
{
	at(addr, a);
	comp('M', compute, a);
}

void read_by_pointer(char dest, int pointer_addr, struct assembly* a)
{
	read_from_mem('A', pointer_addr, a);
	comp(dest, "M", a);
}

void write_by_pointer(int pointer_addr, const char* compute, struct assembly* a)
{
	read_from_mem('A', pointer_addr, a);
	comp('M', compute, a);
}

void stack_pop(char dest, struct assembly* a)
{
	dec_sp(a);
	read_by_pointer(dest, SP, a);
}

void push(const char* what, struct assembly* a)
{
	write_by_pointer(SP, what, a);
	comp('D', "A+1", a);
	write_to_mem(SP, "D", a);
}

int segment_index_by_name(const char* segment)
{
	int pointer_location = 0;
	if (segment[0] == 'l')         // local
		return 1;
	else if (segment[0] == 'a')     // argument
		return 2;
	else if (0 == strcmp("this", segment))
		return 3;
	else if (0 == strcmp("that", segment))
		return 4;
}

void pop(int pointer_addr, int offset, struct assembly* a)
{
	at(offset, a);
	comp('D', "A", a);
	at(pointer_addr, a);
	comp('D', "D+M", a);
	read_from_mem('A', 0, a);
	comp('M', "D", a);
	stack_pop('D', a);
	read_from_mem('A', SP, a);
	comp('A', "A+1", a);
	comp('A', "M", a);
	comp('M', "D", a);
}

void pop_mem_value(int addr, struct assembly* a)
{
	stack_pop('D', a);
	at(addr, a);
	comp('M', "D", a);
}

void read_by_pointer_and_offset(char dest, int pointer_addr, int offset, struct assembly* a)
{
	at(offset, a);
	comp('D', "A", a);
	at(pointer_addr, a);
	comp('A', "D+M", a);
	comp(dest, "M", a);
}

void pop_value(const char* segment, int value, struct assembly* a)
{
	if (segment[0] == 's')
		pop_mem_value(16+value, a);
	else if (segment[0] == 'p')
		pop_mem_value(3 + value, a);
	else if (0 == strcmp("temp", segment))
		pop_mem_value(5 + value, a);
	else
		pop(segment_index_by_name(segment), value, a);
}

void load_value(char dest, const char* segment, int value, struct assembly* a)
{
	char label_name[256] = { 0 };

	if (segment[0] == 'c') // constant
	{
		at(value, a);
		comp(dest, "A", a);
	}
	else if (segment[0] == 's')
		read_from_mem('D', 16+value, a);
	else if (segment[0] == 'p') //  pointer
		read_from_mem('D', 3+value, a);
	else if (0 == strcmp("temp", segment))
		read_from_mem('D', 5 + value, a);
	else 
		read_by_pointer_and_offset(dest, segment_index_by_name(segment), value, a);
}

void stack_compute(const char* compute, struct assembly* a)
{
	stack_pop('D', a);
	dec_sp(a);
	comp('A', "M", a);
	comp('D', compute, a);
	comp('M', "D", a);
	inc_sp(a);
}

void unary_op(const char* op, struct assembly* a)
{
	stack_pop('D', a);
	comp('M', op, a);
	inc_sp(a);
}

void comparison(char dest, const char* compute, const char* condition, struct assembly* a)
{
	stack_pop('D', a);
	dec_sp(a);
	comp('A', "M", a);
	comp(dest, compute, a);
	at(a->current_index + 4, a);
	comp(dest, condition, a);
	at(a->current_index + 3, a);
	comp('D', "0;JMP", a);
	comp('D', "-1", a);
	write_by_pointer(SP, "D", a);
	inc_sp(a);
}


void translate(const char* vm_instruction, struct assembly* a)
{
	char opcode[16] = { 0 }, segment[16] = { 0 };
	int value = 0;

	if ((0 == sscanf(vm_instruction, "%s", opcode)) || (opcode[0] == '/'))
		return;

	if (0 == strcmp("push", opcode))
	{
		sscanf(vm_instruction, "%s%s%d", opcode, segment, &value);
		load_value('D', segment, value, a);
		push("D", a);
	}
	if (0 == strcmp("pop", opcode))
	{
		sscanf(vm_instruction, "%s%s%d", opcode, segment, &value);
		pop_value(segment, value, a);
	}
	else if (0 == strcmp("add", opcode)) stack_compute("M+D", a);
	else if (0 == strcmp("sub", opcode)) stack_compute("M-D", a);
	else if (0 == strcmp("and", opcode)) stack_compute("M&D", a);
	else if (0 == strcmp("or", opcode)) stack_compute("M|D", a);
	else if (0 == strcmp("not", opcode)) unary_op("!D", a);
	else if (0 == strcmp("neg", opcode)) unary_op("-D", a);
	else if (0 == strcmp("eq", opcode)) comparison('D', "M-D", "D;JEQ", a);
	else if (0 == strcmp("lt", opcode)) comparison('D', "M-D", "D;JLT", a);
	else if (0 == strcmp("gt", opcode)) comparison('D', "M-D", "D;JGT", a);
}

int main(int argc, char** argv)
{
	struct assembly a;
	memset(&a, 0, sizeof(struct assembly));

	FILE* vmInputFile = fopen(argv[1], "r");
	char buffer[1024];
	while (fgets(buffer, 1024, vmInputFile))
		translate(buffer, &a);
	fclose(vmInputFile);

	at(a.current_index + 1, &a);
	comp(' ', "0;JMP", &a);

	// generate asm file name
	char asmFileName[256] = { 0 };
	strncpy(asmFileName, argv[1], 255);
	strcpy(strchr(asmFileName, '.'), ".asm");

	// write asm instructions
	FILE* asmOutputFile = fopen(asmFileName, "w");
	for (unsigned int i = 0; i < a.current_index; ++i)
		fputs(a.instructions[i], asmOutputFile), fputs("\n", asmOutputFile);
	fclose(asmOutputFile);

	return 0;
}
