#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include "lib_include.h"

/*max line length*/
/*MAX_LINE is set to 81 because it is mainly used in the fgets() function, (and initializing the buffer)
* a line is a maximum of 80 characaters long, but we need to account for adding a NULL terminator at the end of each line,
* thus the buffer in which we store a line from the input file should be at a length of 81 characters*/
#define MAX_LINE 81

/*max length of label/macro name*/
#define MAX_LABEL_NAME 30

/*address of the first command*/
#define START_ADDR 100

/*a line that starts with ";" is a comment line and should be ignored*/
#define COMMENT_CHAR ';'

/*according to maman's instructions, our first address is 100*/
#define FIRST_ADDRESS 100

/*a value for when a field is yet to be assigned*/
#define UNASSIGNED -1

/*memory is capped at 256 words (according to maman's instructions)*/
#define MEM_CAP 256


/*size of opcode field in code_word (in bits)*/
#define SIZEOF_OPCODE 4

/*size of addressing type fields in code_word (in bits)*/
#define SIZEOF_ADDRESSING 2

/*size of register fields in code_word (in bits)*/
#define SIZEOF_REGISTER 4

/*size of A,R,E field in code_word (in bits)*/
#define SIZEOF_ARE 2 

/*absolute translation means value 0 in A,R,E field of code_word*/
#define A_FLAG 0

/*relocatable translation means value 2 in A,R,E field of code_word*/
#define R_FLAG 2

/*external translation means value 1 in A,R,E field of code_word*/
#define E_FLAG 1


/*definition of boolean vriable*/
typedef enum BOOLEAN{
    false = 0,
    true = 1
}boolean;

/*all addressing types for later evaluation and translation*/
typedef enum addressing_type {
    /*addressing type 0 - used when the operand is the value itself*/
    IMMEDIATE_ADDR = 0,
    /*addressing type 1 - used when the operand is a label*/
    DIRECT_ADDR = 1,
    /*addressing type 2 - used when the operand is a struct*/
    STRUCT_ADDR = 2,
    /*addressing type 3 - used when the operand is a register*/
    REGISTER_ADDR = 3,
	/*default/none/error*/
	NONE_ADDR = -1
}addressing_type;

/*all instruction types for later comparison*/
typedef enum instructions{
	/* .data instruction */
	DATA_INST = 0,
	/* .string instruction */
	STRING_INST = 1,
	/* .struct instruction */
	STRUCT_INST = 2,
	/* .extern instruction */
	EXTERN_INST = 3,
	/* .entry instruction */
	ENTRY_INST = 4,
	/* Not found */
	NONE_INST = -1
}instruction;

/*all saved register names*/
typedef enum registers{
    r0 = 0,
    r1 = 1,
    r2 = 2,
    r3 = 3,
    r4 = 4,
    r5 = 5,
    r6 = 6,
    r7 = 7,

    NONE_REG = -1
}reg;

/*all function opcodes for translation*/
typedef enum opcodes {
    MOV_OP = 0,
    CMP_OP = 1,
    ADD_OP = 2,
    SUB_OP = 3,
    NOT_OP = 4,
    CLR_OP = 5,
    LEA_OP = 6,
    INC_OP = 7,
    DEC_OP = 8,
    JMP_OP = 9,
    BNE_OP = 10,
    GET_OP = 11,
    PRN_OP = 12,
    JSR_OP = 13,
    RTS_OP = 14,
    HLT_OP = 15,
	
    /*default/none/error*/
	NONE_OP = -1
}opcode;

typedef enum extensionTypes{
    AS_EXT,
    AM_EXT,
    EXT_EXT,
    ENT_EXT,
    OB_EXT
}extension;

#endif