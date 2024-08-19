#ifndef UTILS_H
#define UTILS_H

#include "definitions.h"
#include "structs.h"

/*coomand, register, and instruction numbers*/
#define TOTAL_COMMAND_NUM 16
#define TOTAL_REGISTER_NUM 8
#define TOTAL_INSTRUCTION_NUM 5

/*these functions are used in various files - pre proccessor, first and second pass,
* and perform various useful operations integral for the proper work of the project*/
boolean ignore(char *);
char *skipSpace(char *);
char *skipWord(char *);
char *get1stW(char *);
boolean isDefinedLabel(char *p, label list[]);
boolean isReserved(char *p);
opcode getCommandNum(char *p);
reg getRegNum(char *p);
instruction getInstType(char *p);

#ifdef INCLUDE_ARRAYS_FLAG
/*static data structures, used to find number of command/register/instruction*/
static char *commandArray[TOTAL_COMMAND_NUM] = {
	"mov",
	"cmp",
	"add",
	"sub",
	"not",
	"clr",
	"lea",
	"inc",
	"dec",
	"jmp",
	"bne",
	"get",
	"prn",
	"jsr",
	"rts",
	"hlt"
};

static char *registerArray[TOTAL_REGISTER_NUM] = {
    "r0",
    "r1",
    "r2",
    "r3",
    "r4",
    "r5",
    "r6",
    "r7"
};

static char *instArray[TOTAL_INSTRUCTION_NUM] = {
	".data",
	".string",
	".struct",
	".extern",
	".entry"
};
/*end of static data structures*/
#endif

#endif