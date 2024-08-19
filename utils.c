#ifndef UTILS_C
#define UTILS_C

#define INCLUDE_ARRAYS_FLAG
#include "utils.h"


/*boolean function to determine wether you should ignore a line or not*/
boolean ignore(char *line){
    char cpy[MAX_LINE] = {' '};
    strcpy(cpy, skipSpace(line));
    if(*cpy == '\0' || *cpy == '\n' || *cpy == COMMENT_CHAR){
        return true;
    }
    return false;
}

/*gets a string with spaces in it's beginning and returns a string without them*/
char *skipSpace(char *p){
    if(p==NULL){return p;}
    while(isspace(*p)){
        p++;
    }
    return p;
}

/*gets a string and skips a single word until a space*/
char *skipWord(char *p){
    if(p==NULL){return p;}
    while(!isspace(*p)){
        p++;
    }
    return p;
}


/*a function that returns the 1st word of a string (also a string)*/
char *get1stW(char *p){
    char q[MAX_LINE] = {' '};
    if(p == NULL){
        return NULL;
    }
    else{
        strcpy(q,p);
        return strtok(q," \t\n\0");
    }
}

/*checks for a string p if a label with this name exists already*/
boolean isDefinedLabel(char *p, label list[]){
    int i;
    for(i=0;i<MEM_CAP;i++){ /*checks for every label, if it matches the input*/
        if(strcmp(list[i].name, p) == 0){
            return true;
        }
    }
    return false;
}

/*checks for a given string, if it represents a reserved word (eg: command,register, etc.)*/
boolean isReserved(char *p){
    /*reserved words include: commands, registers, and instructions*/
    if(getCommandNum(p) == NONE_OP && getRegNum(p) == NONE_REG && getInstType(p) == NONE_INST){
        return false;
    }
    return true;
}

/*gets the command num (opcode) of a given string, if not found, returns NONE_OP*/
opcode getCommandNum(char *p){
    int i;
    for(i=0;i<TOTAL_COMMAND_NUM;i++){ /*gets opcode from the command array (defined in first_pass.h)*/
        if(strcmp(commandArray[i],p) == 0){
            return i;
        }
    }
    return NONE_OP;
}

/*gets the register number of a given string, if not found, returns NONE_REG*/
reg getRegNum(char *p){
    int i;
    for(i=0;i<TOTAL_REGISTER_NUM;i++){ /*gets register from the registers array (defined in first_pass.h)*/
        if(strcmp(registerArray[i],p) == 0){
            return i;
        }
    }
    return NONE_REG;
}

/*gets the instruction type of a given string (eg: .data, .string etc.), if not found, returns NONE_INST*/
instruction getInstType(char *p){
    int i;
    for(i=0;i<TOTAL_INSTRUCTION_NUM;i++){ /*gets instruction from the instructions array (defined in first_pass.h)*/
        if(strcmp(instArray[i],p) == 0){
            return i;
        }
    }
    return NONE_INST;
}

#endif