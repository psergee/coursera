#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <glob.h>

#define asm_cmd_len 64
constexpr int SP = 0;
constexpr int LCL = 1;
constexpr int ARG = 2;

struct assembly
{
    char (*instructions)[asm_cmd_len];
    char base_name[256];
    unsigned int current_index, instruction_index;
};

struct assembly* create_assembly(unsigned instructions_count)
{
    struct assembly* a = (struct assembly*)malloc(sizeof(struct assembly));
    if (a == NULL)
        return NULL;

    a->current_index = a->instruction_index = 0;
    a->instructions = (char(*)[asm_cmd_len])calloc(instructions_count, sizeof(char[asm_cmd_len]));
    return a;
}

void free_assembly(struct assembly* a)
{
    free(a->instructions);
    free(a);
}

void at(int addr, struct assembly* a)
{
    snprintf(a->instructions[a->current_index], asm_cmd_len, "@%d", addr);
    ++a->current_index;
    ++a->instruction_index;
}

void at_label(const char* label, struct assembly* a)
{
    snprintf(a->instructions[a->current_index], asm_cmd_len, "@%s", label);
    ++a->current_index;
    ++a->instruction_index;
}

void label(const char* lbl, struct assembly* a)
{
    snprintf(a->instructions[a->current_index], asm_cmd_len, "(%s)", lbl);
    ++a->current_index;
}

void comp(char dest, const char* compute, struct assembly* a)
{
    snprintf(a->instructions[a->current_index], asm_cmd_len, "%c=%s", dest, compute);
    ++a->current_index;
    ++a->instruction_index;
}

void jmp(const char* compute, const char* jump, struct assembly* a)
{
    snprintf(a->instructions[a->current_index], asm_cmd_len, "%s;%s", compute, jump);
    ++a->current_index;
    ++a->instruction_index;
}

void jmp(const char* where, const char* compute, const char* jump, struct assembly* a)
{
    at_label(where, a);
    snprintf(a->instructions[a->current_index], asm_cmd_len, "%s;%s", compute, jump);
    ++a->current_index;
    ++a->instruction_index;
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

void pop_static(int offset, struct assembly* a)
{
       char label_name[256] = { 0 };
       snprintf(label_name, 256, "%s.%d", a->base_name, offset);
       stack_pop('D', a);
       at_label(label_name, a);
       comp('M', "D", a);
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
    return 0;
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
        pop_static(value, a);
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
    {
       snprintf(label_name, 256, "%s.%d", a->base_name, value);
       at_label(label_name, a);
       comp(dest, "M", a);
    }
    else if (segment[0] == 'p') //  pointer
        read_from_mem('D', 3 + value, a);
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
    at(a->instruction_index + 4, a);
    comp(dest, condition, a);
    at(a->instruction_index + 3, a);
    comp('D', "0;JMP", a);
    comp('D', "-1", a);
    write_by_pointer(SP, "D", a);
    inc_sp(a);
}

void conditional(const char* lbl, struct assembly* a)
{
    dec_sp(a);
    read_by_pointer('D', SP, a);
    jmp(lbl, "D", "JNE", a);
}

void function(const char* function_name, int nVars, struct assembly* a)
{
    label(function_name, a);

    read_from_mem('A', SP, a);
    for(int i = 0; i < nVars; ++i)
    {
        comp('M', "0", a);
        comp('A', "A+1", a);
    }
    comp('D', "A", a);
    write_to_mem(SP, "D", a);
}

void close_frame(struct assembly* a)
{
    pop_mem_value(13, a); // return val in Reg13

    read_from_mem('D', ARG, a); // read arg
    write_to_mem(SP, "D", a); // set arg to sp

    read_from_mem('D', LCL, a); // read lcl
    write_to_mem(14, "D", a); // save lcl in Reg14
    for (int i = 4; i > 0; --i)
    {
        at(14, a);
        comp('D', "M-1", a);
        write_to_mem(14, "D", a);
        comp('A', "D", a);
        comp('D', "M", a);
        write_to_mem(i, "D", a);
    }

    at(14, a);
    comp('A', "M-1", a);
    comp('D', "M", a);
    write_to_mem(14, "D", a); // save return address

    read_from_mem('D', 13, a); // return return value
    push("D", a); // push return value

    // make jump
    read_from_mem('A', 14, a); // read return addr
    jmp("0", "JMP", a);
}

void call(const char* function_name, int nArgs, struct assembly* a)
{
    unsigned inst_to_be_updated = a->current_index;
    at(a->instruction_index, a);
    comp('D', "A", a);
    push("D", a);

    for(int i = 1; i < 5; ++i)
    {
        read_from_mem('D', i, a);
        push("D", a);
    }

    // set arg pointer for the function
    at(5 + nArgs, a);
    comp('D', "A", a);
    read_from_mem('A', SP, a);
    comp('D', "A-D", a);
    write_to_mem(ARG, "D", a);

    // store local pointer
    read_from_mem('D', SP, a);
    write_to_mem(LCL, "D", a);

    // update jmp
    snprintf(a->instructions[inst_to_be_updated], asm_cmd_len, "@%d", a->instruction_index + 2);
    jmp(function_name, "0", "JMP", a);
}

void bootstrap(assembly* a)
{
    at(256, a);
    comp('D', "A", a);
    write_to_mem(SP, "D", a);
    call("Sys.init", 0, a);
}

void translate(const char* vm_instruction, struct assembly* a)
{
    char opcode[16] = { 0 }, segment[16] = { 0 }, lbl[256] = { 0 };
    int value = 0;

    if ((0 == sscanf(vm_instruction, "%s", opcode)) || (opcode[0] == '/'))
        return;

    if (0 == strcmp("push", opcode))
    {
        if (3 == sscanf(vm_instruction, "%s%s%d", opcode, segment, &value))
        {
            load_value('D', segment, value, a);
            push("D", a);
        }
    }
    else if (0 == strcmp("pop", opcode))
    {
        if (3 == sscanf(vm_instruction, "%s%s%d", opcode, segment, &value))
            pop_value(segment, value, a);
    }
    else if (0 == strcmp("label", opcode))
    {
        if(2 == sscanf(vm_instruction, "%s%s", opcode, lbl))
            label(lbl, a);
    }
    else if (0 == strcmp("if-goto", opcode))
    {
        if (2 == sscanf(vm_instruction, "%s%s", opcode, lbl))
            conditional(lbl, a);
    }
    else if (0 == strcmp("goto", opcode))
    {
        if (2 == sscanf(vm_instruction, "%s%s", opcode, lbl))
            jmp(lbl, "0", "JMP", a);
    }
    else if (0 == strcmp("function", opcode))
    {
        if (3 == sscanf(vm_instruction, "%s%s%d", opcode, lbl, &value))
            function(lbl, value, a);
    }
    else if (0 == strcmp("call", opcode))
    {
        if (3 == sscanf(vm_instruction, "%s%s%d", opcode, lbl, &value))
            call(lbl, value, a);
    }
    else if (0 == strcmp("return", opcode)) close_frame(a);
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

void translate_file(const char* file_path, assembly* a)
{
    const char* dot = strchr(file_path, '.');
    if (!dot)
        return;

    const char* slash = strrchr(file_path, '/');
    if (!slash)
        strncpy(a->base_name, file_path, dot - file_path);
    else
        strncpy(a->base_name, slash+1, dot - slash - 1);

    char buffer[1024] = { 0 };
    FILE* vmInputFile = fopen(file_path, "r");

    while (fgets(buffer, 1024, vmInputFile))
        translate(buffer, a);
    fclose(vmInputFile);
}

int main(int argc, char** argv)
{
    struct assembly* a = create_assembly(2048);

    char asmFileName[256] = { 0 };
    strncpy(asmFileName, argv[1], 255);

    struct stat status;
    stat(argv[1], &status);

    if (S_ISDIR(status.st_mode))
    {
        glob_t g;
        char pattern[2048] = {0};
        snprintf(pattern, 2047, "%s/*.vm", argv[1]);
        bootstrap(a);
        if (0 == glob(pattern, GLOB_NOSORT, nullptr, &g))
            for(int i = 0; i < g.gl_pathc; ++i)
                translate_file(g.gl_pathv[i], a);
        globfree(&g);

        snprintf(asmFileName, 255, "%s/%s.asm", argv[1], argv[1]);
    }
    else
    {
        translate_file(argv[1], a);
        label("PROGRAM_END", a);
        jmp("PROGRAM_END", "0", "JMP", a);

        // generate asm file name
        strcpy(strchr(asmFileName, '.'), ".asm");
    }

    // write asm instructions
    FILE* asmOutputFile = fopen(asmFileName, "w");
    for (unsigned int i = 0; i < a->current_index; ++i)
        fputs(a->instructions[i], asmOutputFile), fputs("\n", asmOutputFile);
    fclose(asmOutputFile);

    free_assembly(a);

    return 0;
}
