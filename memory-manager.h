#pragma once
#include "generator.h"

#define REGISTERS_DIM   14
#define OBJECTS_DIM     256
#define VARIABLES_DIM   256
#define HW_STACK_DIM    256
#define FUNCTIONS_DIM   256

void print_regs();

void print_var();

void print_mem_structs();

void init_memory();

Mem_obj* obj_alloc(Mem_obj obj);

// Pushes variable on the stack
Block var_push(Variable var);

// Pops the top variable from variables
void var_pop();

// Finds variable by name
Variable* var_find(char* name);

// Pushes variable stack frame
void var_push_stack_frame();

// Pops variable stack frame
Block var_pop_stack_frame();

// Locks register
void r_lock(Mem_obj* obj);

// Unlocks register
void r_unlock(Mem_obj* obj);

// Reserves a register or a stack position
Block r_alloc(int size);

// Frees a register or a stack position
Block r_free(Mem_obj* obj);

// Puts obj in a register if it was on the stack
Block r_load(Mem_obj* obj);

// Pust obj on the stack if it was in a register. If obj refers to a variable, it gets put in the corresponding memory address
Block r_store(Mem_obj* obj);

// Moves obj to the register indexed reg
Block r_move(Mem_obj* obj, int reg);

// Generates assembly to move src to dest
Block g_mov(Mem_obj* dest, Mem_obj src);

// Generates assembly name of obj
Block g_name(Mem_obj* obj);

Function* func_push(Function func);

Function* func_find(char* name);
