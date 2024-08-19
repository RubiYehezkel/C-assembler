#ifndef STRUCTS_H
#define STRUCTS_H

#include "definitions.h"

/*Definition of a label node struct*/
typedef struct LabelNode{
    char name[MAX_LABEL_NAME]; /*label name*/
    unsigned address; /*label definition address*/
	boolean dataFlag;
}label;

/*Definition of a macro node struct*/
typedef struct MacroNode{
    char name[MAX_LABEL_NAME]; /*macro name*/
    char data[MAX_LINE * 20]; /*macro content (capped to 20 lines)*/
}macro;

/** Represents a single data word. */
typedef struct data_word {
	/* The data content itself (a method for putting data into these field is defined) */
	unsigned int data;
} data_word;

typedef struct code_word {
	/*defined as a bitfield, each bit count represents the length of each field as requested in the Maman*/

	/*opcode for first word*/
	unsigned int opcode: SIZEOF_OPCODE;
	/*destination and source addressing methods for first word*/
	unsigned int src_addressing: SIZEOF_ADDRESSING;
	unsigned int dest_addressing: SIZEOF_ADDRESSING;
	/*source and destination register numbers in case we have registers as operands (second/third/fourth word)*/
	unsigned int src_register: SIZEOF_REGISTER;
	unsigned int dest_register: SIZEOF_REGISTER;
	/*the number in case we get an immediate addressing type operand, also used as field number with struct operand*/
	unsigned int immediateNum: 8;
	/*A,R,E (in every word)*/
	unsigned int ARE: SIZEOF_ARE;

	/*name of a used label to plug inside the translated code, and changed to address in second pass
	* used only if we have a label operand and is in second/third/fourth word*/
	char labelName[MAX_LABEL_NAME];
}code_word;

/*a struct for the code image array, includes line number, content, and an optional label name*/
typedef struct codeImage{
	int lineNum;
	int content;
	char labName[MAX_LABEL_NAME];
}codeImg;

/*a struct for the data image, includes line number, and content*/
typedef struct dataImage{
	int lineNum;
	int content;
}dataImg;

/*a struct for the extern labels array, includes the label's name, the amount of times it was used, and an array with all the use's addresses*/
typedef struct externNode{
	char name[MAX_LABEL_NAME];
	unsigned useArr[MEM_CAP];
	int useCount;
}extNode;

/*a struct for the entry labels array, includes only the label's name*/
typedef struct entryNode{
	char name[MAX_LABEL_NAME];
}entNode;

#endif