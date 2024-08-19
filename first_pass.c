#ifndef FIRST_PASS_C
#define FIRST_PASS_C

#include "first_pass.h"

/*main logic of first pass:
* translates every line (that needs to be translated) into code/data image,
* stores lables to label table, 
* stores extern/entry defined labels to according arrays, 
* updates IC and DC to correspond with instruction and data count*/
boolean first_pass(FILE *infile, label labelTab[], codeImg codeImage[],dataImg dataImage[] ,extNode externList[], entNode entryList[], int *IC, int *DC){
    /*string to store and parse the string input*/
    char buf[MAX_LINE] = {' '};
    char word[MAX_LINE] = {' '};
    char currentLabName[MAX_LABEL_NAME+1] = {' '};
    char *token;

    /*flag for a line with a label definition*/
    boolean labelDef  = false;

    /*flag for wether we have error in the file or not*/
    boolean ERRFLAG = false;

    /*counters*/
    int labelCounter = 0;
    int externCounter = 0;
    int entryCounter = 0;
    int lineCounter = 0;

    /*flags to mark current line operation/ instruction*/
    opcode currentOpcode;
    instruction currentInst;

    while(fgets(buf,MAX_LINE,infile) != NULL){
        lineCounter++; /*got a line*/

        /*reset variables for next line (we reset at start of loop because we might use continue)*/
        labelDef = false;
        strcpy(currentLabName,"\0");
        currentInst = NONE_INST;
        currentOpcode = NONE_OP;

        if(!ignore(buf)){ /*we translate a line only if it should not be ignored (not blank, not comment)*/

            skipSpace(buf); /*skip spaecs at the start of every line*/

            strcpy(word,get1stW(buf)); /*get first word*/
            
            if(isLabelDef(word) == 1){ /*return values for isLabelDef listed below (in function definition), checks if a word is a legal label declaration*/
                if(!isDefinedLabel(word,labelTab)){ /*check if a label with this name has already been declared somewhere else*/
                    labelDef = true;
                    strcpy(currentLabName, strtok(word,":")); /*if a label is ok, then when we tokenize it with a colon we should get the label name*/

                    /*we do not initialize labe's name and don't increment label counter because the label might be ignored due to .extern or .entry instruction*/
                }else{
                    printf("error in line %d: label declared twice - %s\n",lineCounter,word);
                    ERRFLAG = true;
                    continue;
                }
            }else if(isLabelDef(word) == 2){
                printf("error in %d: illegal label name - %s\n",lineCounter,word);
                ERRFLAG = true;
                continue;
            }
            
            /*if a label is declared in this line, we have a line that looks like this:
            LABEL: <value>
            so we need to skip to the start of the value*/
            
            /*skipping label name in case we got a label*/
            if(labelDef){
                token = strtok(buf, " \t");
                token = strtok(NULL, "\0");
            
                if(token == NULL){
                    printf("error in line %d: emptry string after label\n",lineCounter);
                    ERRFLAG = true;
                    continue;
                }
            }else{ /*if there is no label, the line should stay the same*/
                token = buf;
            }
            
            strcpy(word,get1stW(token));
            
            /*if we got a newline character at the end of input, it may mess up our parsing inside the translation functions, thus we delete it*/
            if(token[strlen(token)-1] == '\n'){
                token[strlen(token)-1] = '\0';
            }
    
            /*getting opcode and insturction type of current word (at least one will be none)*/
            currentOpcode = getCommandNum(word);
            currentInst = getInstType(word);

            if(currentInst != NONE_INST){
                if(currentInst == EXTERN_INST || currentInst == ENTRY_INST){ /*we have a .extern or .entry instruction*/
                    if(labelDef){ /*according to the maman's instructions, if a label is defined and inside the label is a .extern or .entry instruction, we ignore the label*/
                        printf("warning - line %d: label definition includes .extern or .entry, label %s ignored\n",lineCounter,currentLabName);
                    }
                    /*skip to after the .extern/.entry instruction*/
                    token = strtok(token, " ");
                    token = strtok(NULL, "\0");
                    
                    /*empty line after .extern or .entry call*/
                    if(token == NULL || *token == '\0' || *token == '\n'){
                        printf("error in line %d: no arguments for .extern or .entry\n", lineCounter);
                        ERRFLAG = true;
                        continue;
                    }
                    
                    strcpy(word,token); /*copying buf to word to tokenize word without changing buf*/
                    strcpy(currentLabName, strtok(word," \t")); /*get 1st word after .extern or .entry call*/
                    token = strtok(NULL," \t");

                    if(token != NULL){ /*if we have another token after one argument, it's an error*/
                        printf("error in line %d: too many arguments for .extern/ .entry instruction\n", lineCounter);
                        ERRFLAG = true;
                        continue;
                    }
                    if(!isLegalLabel(currentLabName)){ /*if we get an illegal label name inside the instruction it's an error*/
                        printf("error in line %d: illegal label name in .extern/ .entry instruction\n", lineCounter);
                        ERRFLAG = true;
                        continue;
                    }
                    switch(currentInst){
                        case ENTRY_INST:
                            strcpy(entryList[entryCounter].name, currentLabName); /*store label name inside of entry list*/
                            entryCounter++;
                            break;
                        case EXTERN_INST:
                            strcpy(externList[externCounter].name, currentLabName); /*store label name inside of extern list*/
                            externCounter++;
                            break;
                        default:
                            break;
                    }
                }else{ /*the instruction is of type .data, .string or .struct*/
                    if(labelDef){
                        strcpy(labelTab[labelCounter].name,currentLabName); /*initializing name field of label*/
                        labelTab[labelCounter].address = (*DC);
                        labelTab[labelCounter].dataFlag = true; /*is a data label, data flag is true*/
                        labelCounter++;
                    }
                    if(!dataToWords(token, dataImage, DC, lineCounter)){ /*translate data into data image array*/
                        ERRFLAG = true;
                        continue;
                    }
                }
                
            }else if(currentOpcode != NONE_OP){
                if(labelDef){
                    strcpy(labelTab[labelCounter].name,currentLabName); /*initializing name field of label*/
                    labelTab[labelCounter].address = (*IC) + FIRST_ADDRESS; /*setting address of label to 100 + instructioons so far (according to maman's instructions)*/
                    labelTab[labelCounter].dataFlag = false; /*not a data label, data flag is false*/
                    labelCounter++;
                }
                if(!CodeToWords(token, codeImage, IC, lineCounter)){ /*translate buffer to code into code image*/
                    ERRFLAG = true;
                    continue;
                } 
            }else{
                printf("error in line %d: %s in not a valid expression\n",lineCounter,word); /*if the first word of a line is neither an instruction or an operation, it is an error*/
                ERRFLAG = true;
                continue;
            }
        }
    }
    return !ERRFLAG; /*if we had errors, the flag is tre and we want to return true*/
}

/*returns if a given string represents a legal label declaration*/
/*0 = not a label declaration*/
/*1 = legal label name and label decalaration*/
/*2 = label declaration and illegal label name*/
int isLabelDef(char *p){
    char cpy[MAX_LINE] = {' '};
    /*if the last character of a string is a colon, it is eligible to be a label definition*/
    /*we then need to check if a label is legal or not (alpha-numeric, starts with a letter etc.)*/
    if(p[strlen(p)-1] == ':'){
        strcpy(cpy,p);        
        strcpy(cpy, strtok(cpy,":"));
        if(isLegalLabel(cpy)){
        return 1;
        }
        return 2;
    } 
    return 0;
}

/*checks if a string consists only out of alpha-numeric characters*/
boolean isalnumStr(char *p){
    int i;
    for(i=0;i<strlen(p);i++){
        /*if a character inside the string is not alpha-numeric, we return false*/
        if(!isalnum(p[i])){
            return false;
        }
    }
    return true;
}

/*checks for a given string, if it legal for a label name (alpha-numeric,starts with a letter, no longer than 30 characters, not reserved)*/
boolean isLegalLabel(char *labCheck){
    /*a label is ok only if it is alpha-numeric, not a reserved word, starts with a letter, ans is no longer thn 30 characters*/
    if(isalpha(labCheck[0]) && !isReserved(labCheck) && isalnumStr(labCheck) && strlen(labCheck)<=MAX_LABEL_NAME){
        return true;
    }
    return false;
}

/*gets an operation and returns the amount of operands the operation takes, if not a legal operation, returns -1*/
int getOperandNum(opcode operation){
    switch(operation){
        /*two operand group*/
        case MOV_OP:
        case CMP_OP:
        case ADD_OP:
        case SUB_OP:
        case LEA_OP: 
            return 2;

        /*one operand group*/
        case NOT_OP:
        case CLR_OP:
        case INC_OP:
        case DEC_OP:
        case JMP_OP:
        case BNE_OP:
        case GET_OP:
        case PRN_OP:
        case JSR_OP:
            return 1;

        /*0 operand group*/
        case RTS_OP:
        case HLT_OP:
            return 0;

        /*default case for a non-operation input*/
        default: 
            return -1;
    }
}

/*checks if a given string is a legal number, legal numbers can start with either a plus or a minus, and can only contain digits (no decimal point etc.)*/
boolean isNumber(char *numCheck){
    int i = 0;
    if(numCheck[0] == '+' || numCheck[0] == '-'){ /*if a number starts with a plus or a minus its ok so we start from index 1*/
        i = 1;
    }
    for(;i<strlen(numCheck);i++){
        if(!isdigit(numCheck[i])){
            return false; /*found a non digit character inside the number*/
        }
    }
    return true; /*iterated through the whole string and evrything is OK*/
}

/*gets a string representing an operand, and returns it's addressing type according to the addressing type instructions*/
int getAdressingType(char *operand){
    
    char cpy[MAX_LINE] = {' '};
    char word[MAX_LINE] = {' '};

    strcpy(cpy,operand);

    if(cpy[0] == '#'){
        if(strchr(cpy,'#') != strrchr(cpy,'#')){ /*if we have two hastags, strtok won't recognize it, so we check if we have hashtags at two differrent locations*/
            return NONE_ADDR;
        }
       strcpy(word, strtok(cpy,"#"));
       /*word is now the token after '#', we need to check if it's a legal number, and there are no characters after it*/
       if(isNumber(word)){
            return IMMEDIATE_ADDR;
       }else{
            return NONE_ADDR;
       }
    }
    if(isLegalLabel(cpy)){ /*if the operand is a legal label, we assume it is a label (the character '.' makes a string not an ok label name, so we know it is not a struct)*/
        return DIRECT_ADDR;
    }

    if(getRegNum(cpy) != NONE_REG){ /*is a register*/
        return REGISTER_ADDR;
    }

    if(strchr(cpy,'.') != NULL && cpy[0] != '.'){ /*includes a point and does not start with a point (starting with a point makes strtok skip it)*/
        strcpy(word,strtok(cpy,"."));
        if(isLegalLabel(word)){ /*if the first part is an ok label name its ok*/
            strcpy(word,strtok(NULL," \t\n\0")); /*go to the next token by space (the next value after the point)*/
            if(strlen(word) == 1 && (word[0] == '1' || word[0] == '2')){ /*if the value after the label is 1 or 2 its ok and represents a struct*/
                return STRUCT_ADDR;
            }
        }
    }
    return NONE_ADDR;
}

/*checks if addressing types are compatible with a certain opcode (according to maman's instructions)*/
boolean checkAddrWithOpcode(opcode operation, addressing_type addr1, addressing_type addr2){
    switch(operation){
        /*two operand group*/
        case MOV_OP:
        case ADD_OP:
        case SUB_OP:
            if((addr1 != NONE_ADDR) && (addr2 != NONE_ADDR && addr2 != IMMEDIATE_ADDR)){
               return true; 
            }
            return false;

        case CMP_OP:
            if((addr1 != NONE_ADDR) && (addr2 != NONE_ADDR)){
               return true; 
            }
            return false;

        case LEA_OP:
            if((addr1 == DIRECT_ADDR || addr1 == STRUCT_ADDR) && (addr2 != NONE_ADDR && addr2 != IMMEDIATE_ADDR)){
               return true; 
            }
            return false;
            
        /*one operand group*/
        case NOT_OP:
        case CLR_OP:
        case INC_OP:
        case DEC_OP:
        case JMP_OP:
        case BNE_OP:
        case GET_OP:
        case JSR_OP:
            if((addr1 == NONE_ADDR) && (addr2 != NONE_ADDR && addr2 != IMMEDIATE_ADDR)){
               return true; 
            }
            return false;

        case PRN_OP:
            if((addr1 == NONE_ADDR) && (addr2 != NONE_ADDR)){
               return true; 
            }
            return false;
            
        /*0 operand group*/
        case RTS_OP:
        case HLT_OP:
            if((addr1 == NONE_ADDR) && (addr2 == NONE_ADDR)){
               return true; 
            }
            return false;

        /*default case for a non-operation input*/
        default: 
            return false;
    }
}

/*writes to code image the translation of a single operation line*/
/*this function assumes that we got addressing types that match the operation (we run this function in translateCode after validation)*/
boolean writeToCodeImage(opcode thisOp,char *op1,char *op2,addressing_type addr1,addressing_type addr2, codeImg image[], int *IC, int lineNumber){
    char Cop1[MAX_LINE] = {' '};
    char Cop2[MAX_LINE] = {' '};

    boolean doubleRegFlag = false; /*used in case we have two register operands*/
    int i = 0; /*used for accessing the i element of our code words array*/
    unsigned int ans = 0;

    /*output is maximum of 5 binary words, so we declare and initialize an array of 5 code_word elements*/
    code_word words[5] = { {0,0,0,0,0,0,0,"\0"} };

    strcpy(Cop1,op1);
    strcpy(Cop2,op2);

    /*storing first word: operation translation*/
    words[i].opcode = (unsigned int)thisOp;

    if(addr1 == NONE_ADDR){
        words[i].src_addressing = 0; /*NONE_ADDR is -1 and will be interprated as 11, but we need 0*/
    }else{
        words[i].src_addressing = (unsigned int)addr1;
    }
    if(addr2 == NONE_ADDR){
        words[i].dest_addressing = 0; /*NONE_ADDR is -1 and will be interprated as 11, but we need 0*/
    }else{
        words[i].dest_addressing = (unsigned int)addr2;
    }

    words[i].ARE = A_FLAG; /*in the function word A,R,E field is always absolute*/

    ans += words[i].opcode;
    ans <<= SIZEOF_ADDRESSING; /*shift to the left by the number of bits that the source addressing field takes*/
    ans += words[i].src_addressing;
    ans <<= SIZEOF_ADDRESSING; /*shift to the left by the number of bits that the destination addressing field takes*/
    ans += words[i].dest_addressing;
    ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
    ans+= words[i].ARE;

    image[(*IC)].content = ans;
    image[(*IC)].lineNum = lineNumber;
    (*IC)++;
    ans = 0;

    i++; /*incrementing i to go to next word*/
    switch(addr1){
        /*meaning we got a number formatted like this: #<numValue>*/
        case IMMEDIATE_ADDR:
            Cop1[0] = ' '; /*replacing hashtag with space*/
            skipSpace(Cop1); /*skipping that space to leave us with only the number*/
            /*we can store a value between -128 and 127 with 8 bits (2's complement)*/
            if(atoi(Cop1) > 127 || atoi(Cop1) < -128){
                printf("error in line %d: number is too big!\n",lineNumber); /*if we got an illegal value, we exit*/
                return false; 
            }
            words[i].immediateNum = (unsigned)atoi(Cop1); /*getting number and storing it inside it's field*/
            words[i].ARE = A_FLAG; /*ARE is absolute in this case*/

            ans += words[i].immediateNum;
            ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
            ans += words[i].ARE;
            image[(*IC)].content = ans;
            image[(*IC)].lineNum = lineNumber;
            i++; /*incrementing i to go to next cell*/
            (*IC)++;
            break;

        /*meaning we got a label name in op1*/
        case DIRECT_ADDR:
            strcpy(image[(*IC)].labName, Cop1);
            image[(*IC)].lineNum = lineNumber;
            (*IC)++;
            i++; /*incrementing i to go to next cell*/
            break;

        /*meaning we have a label name followed by a point and a field number (1/2)*/
        case STRUCT_ADDR:
            strcpy(words[i].labelName,strtok(Cop1,".")); /*copying label name to the labl name field*/
            strcpy(image[(*IC)].labName, words[i].labelName);
            image[(*IC)].lineNum = lineNumber;
            i++; /*incrementing i to go to next cell*/
            (*IC)++;
            words[i].immediateNum = strcmp(strtok(NULL," \t"),"1") == 0? 1 : 2; /*getting field number*/
            words[i].ARE = A_FLAG; /*ARE is zero here*/

            ans += words[i].immediateNum;
            ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
            ans += words[i].ARE;
            image[(*IC)].content = ans;
            image[(*IC)].lineNum = lineNumber;
            i++;
            (*IC)++;
            break;

        /*meaning we have a register name*/
        case REGISTER_ADDR:
            words[i].src_register = getRegNum(Cop1);
            words[i].ARE = A_FLAG;
            if(addr2 == REGISTER_ADDR){
                doubleRegFlag = true;
                words[i].dest_register = getRegNum(Cop2);

                ans += words[i].src_register;
                ans <<= SIZEOF_REGISTER; /*shift to the left by the number of bits that the destination register field takes*/
                ans += words[i].dest_register;
                ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
                ans += words[i].ARE;
                image[(*IC)].content = ans;
                image[(*IC)].lineNum = lineNumber;
            }else{
                ans += words[i].src_register;
                ans <<= SIZEOF_REGISTER + SIZEOF_ARE; /*shift to the left by the number of bits that the destination register field takes plus the ARE field (since dest register is zero/none)*/
                ans += words[i].ARE;
                image[(*IC)].content = ans;
                image[(*IC)].lineNum = lineNumber;
            }
            (*IC)++;
            i++; /*incrementing i to go to next cell*/
            break;

        case NONE_ADDR:
            break;
    }
    ans = 0;
    switch(addr2){
        case IMMEDIATE_ADDR:
            Cop2[0] = ' '; /*replacing hashtag with space*/
            skipSpace(Cop2); /*skipping that space to leave us with only the number*/
            /*we can store a value between -128 and 127 with 8 bits (using 2's complement)*/
            if(atoi(Cop2) > 127 || atoi(Cop2) < -128){
                printf("error in line %d: number is too big!\n",lineNumber); /*if we got an illegal value, we exit*/
                return false; 
            }
            words[i].immediateNum = (unsigned)atoi(Cop2); /*getting number and storing it inside it's field*/
            words[i].ARE = A_FLAG; /*ARE is zero in this case*/

            ans += words[i].immediateNum;
            ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
            ans += words[i].ARE;
            image[(*IC)].content = ans;
            image[(*IC)].lineNum = lineNumber;
            (*IC)++;
            break;

        case DIRECT_ADDR:
            strcpy(image[(*IC)].labName, Cop2);
            image[(*IC)].lineNum = lineNumber;
            (*IC)++;
            break;

        case STRUCT_ADDR:
            strcpy(words[i].labelName,strtok(Cop2,".")); /*copying label name to the labl name field*/
            strcpy(image[(*IC)].labName,words[i].labelName);
            image[(*IC)].lineNum = lineNumber;
            i++; /*incrementing i to go to next cell*/
            (*IC)++;
            words[i].immediateNum = strcmp(strtok(NULL," \t"),"1") == 0? 1 : 2; /*getting field number*/
            words[i].ARE = A_FLAG; /*ARE is zero here*/

            ans += words[i].immediateNum;
            ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
            ans += words[i].ARE;
            image[(*IC)].content = ans;
            image[(*IC)].lineNum = lineNumber;
            (*IC)++;
            break;

        case REGISTER_ADDR:
            if(doubleRegFlag == true){ /*we already stored this register number before*/
                break;
            }else{ /*no 1st register, only 2nd register, so 1st register is 0 ans 2nd register is the register number*/
                words[i].dest_register = getRegNum(op2);
                words[i].ARE = A_FLAG; /*ARE is zero here*/

                ans += words[i].dest_register;
                ans <<= SIZEOF_ARE; /*shift to the left by the number of bits that the ARE field takes*/
                ans += words[i].ARE;
                image[(*IC)].content = ans;
                image[(*IC)].lineNum = lineNumber;
                (*IC)++;
                break;
            }
        case NONE_ADDR:
            break;
    }
    return true;
}

/*takes string input, if it's ok, translates it according to the maman's instructions to the codeImage array*/
boolean CodeToWords(char *line, codeImg codeImage[], int *IC, int lineNumber){
    char cpy[MAX_LINE] = {' '};
    char word[MAX_LINE] = {' '}; /*used to store the first word of the input, but also used as a copy of the input (since we want to alter it and not lose cpy)*/
    char cmnd[4] = {' '};
    char *cpyStr;
    char *token; /*used to count how many times can we use strtok on the string (also used to save pointers)*/

    /*an operand of type struct is potentially the longest operand, which is a label+2 characters, so we make two operands (arrays) of that length*/
    char op1[MAX_LABEL_NAME+2] = {' '};
    char op2[MAX_LABEL_NAME+2] = {' '};

    int counter = 0; /*counter variable (how many times can we use strtok)*/
    int i; /*loop variable*/
    int operandNum; /*amount of operands the operation recieves*/
    opcode thisOpcode = NONE_OP; /*opcode*/
    addressing_type addr1 = NONE_ADDR; /*addressing type 1*/
    addressing_type addr2 = NONE_ADDR; /*addressing type*/

    strcpy(cpy,line);
    strcpy(word,get1stW(cpy));

    thisOpcode = getCommandNum(word);
    operandNum = getOperandNum(thisOpcode);

    if(thisOpcode == NONE_OP){
        printf("error in line %d: illegal function name\n",lineNumber);
        return false;
    }
    strcpy(cmnd, word);

    cpyStr = strtok(cpy," \t");
    cpyStr = strtok(NULL,"\0");
    
    if(cpyStr != NULL){ /*if this is null, we got an operation with zero operands and shouldnt copy*/
        strcpy(word,cpyStr); /*copying cpy to word (to use strtok without altering origin)*/
    }
    /*this next part extracts the operands (if there are any), checks the validity of the line construction (commas,num of operands etc.) and gets their addressing types*/
    if(operandNum == 2){

        if(cpyStr == NULL || cpyStr[0] == '\n' || cpyStr[0] == '\0'){
            printf("error in line %d: not enough operands, command %s recieves 2 operands\n",lineNumber, cmnd);
            return false;
        }

        if(strstr(cpyStr, ",,") != NULL){ /*if we have two commas in a row it's an error*/
            printf("error in line %d: too many commas between operands\n",lineNumber);
            return false;
        }
        if(strchr(cpyStr,',') == NULL){ /*if we don't have commas at all, that is also an error*/
            printf("error in line %d: no comma between operands\n",lineNumber);
            return false;
        }

        /*this while loop count how many times can we tokenize the string using space.
        * in other words, how many words separated by spaces do we have
        * if count will be one, we have something like this: <op1>,<op2> (no spaces)
        * if count will be two, we have somthing like this: <op1>, <op2> or like this <op1> ,<op2> (one space between)
        * if count will be three, we have something like this: <op1> , <op2> (space from each side)*/
        token = strtok(word," \t");
        while(token != NULL){
            counter++;
            token = strtok(NULL," \t");
        }

        /*we got too many words. (eg: <op1> , <op2>  <extraWord>)*/
        if(counter>3){
            printf("error in line %d: too many arguments for command %s\n",lineNumber, cmnd);
            return false;
        }

        /*in this scenario, we have moved 1 time with strtok, 
        * meaning that we have a string looking like this: <op1>,<op2> */
        if(counter == 1){
            if(cpyStr[0] == ','){
                printf("error in line %d: label name cannot contain a comma\n",lineNumber);
                return false;
            }
            strcpy(op1,strtok(cpyStr,","));
            token = strtok(NULL, " \t");
            if(token == NULL){
                printf("error in line %d: not enough operands for command %s\n",lineNumber, cmnd);
                return false;
            }
            strcpy(op2,token);
        }

        /*in this scenario, we have moved 2 times with strtok,
        * meaning that we have a string looking like this: <op1> ,<op2>
        * or like this: <op1>, <op2> */
        if(counter == 2){
            if(cpyStr[0] == ','){ /*strtok will ignore this comma*/
                printf("error in line %d: label name cannot contain a comma\n",lineNumber);
                return false;
            }
            if(cpyStr[strlen(cpyStr)-1] == ','){ /*strtok will ignore this comma*/
                printf("error in line %d: label name cannot contain a comma\n",lineNumber);
                return false;
            }
            strcpy(op1,strtok(cpyStr,",")); /*now in op1 we have either something like this: <op1> or like this: <op1><space>*/
            if(isspace(op1[strlen(op1)-1])){
                i=0;
                while(!isspace(op1[i])){
                    i++; /*getting to the first whitespace and deleting everything after it*/
                }
                op1[i] = '\0';
            }
            token = strtok(NULL," \t");
            if(*(skipSpace(token)) == '\n' || *(skipSpace(token)) == '\0'){
                printf("error in line %d: not enough operands for command %s\n",lineNumber, cmnd);
                return false;
            }
            strcpy(op2,token); /*no spaces after op1, so we only need to take the other operand from the string*/
        }


        /*in this scenario, we have mover 3 times with strtok,
        * meaning that we have a string looking like this: <op1> , <op2>*/
        if(counter == 3){
            strcpy(op1, strtok(cpyStr," \t")); /*beacause we have a spac after op1*/
            strtok(NULL," \t"); /*beacause we have another token (comma) if we tokenize using space*/
            strcpy(op2,strtok(NULL," \t")); /*last token is op2*/
        }

        /*we now have the two operands in the corressponding variables, now we take their addressing types/methods*/
        addr1 = getAdressingType(op1);
        addr2 = getAdressingType(op2);

    }else if(operandNum == 1){

        if(cpyStr == NULL || cpyStr[0] == '\n' || cpyStr[0] == '\0'){
            printf("error in line %d: not enough operands, command %s recieves 1 opernad\n",lineNumber, cmnd);
            return false;
        }

        /*this counts the amount of times we can tokenize the string with spaces.
        * in other words, how many words separated by spaces do we have.
        * if this is greater than one, we have too many words or too many arguments.*/
        token = strtok(word," \t");
        while(token != NULL){
            counter++;
            token = strtok(NULL," \t");
        }

        if(counter > 1){
            printf("error in line %d: too many operands, command %s recieves 1 opernad\n",lineNumber, cmnd);
            return false;
        }

        /*checking again with commas*/
        counter = 0;
        strcpy(word,cpyStr);
        token = strtok(word,",");
        while(token != NULL){
            counter++;
            token = strtok(NULL," \t");
        }

        if(counter > 1){
            printf("error in line %d: too many operands, command %s recieves 1 operand\n",lineNumber, cmnd);
            return false;
        }

        /*if the string includes commas, it is an illegal label name*/
        if(strchr(cpyStr,',') != NULL){
            printf("error in line %d: illegal label name\n",lineNumber);
            return false;
        }

        /*all ok, get operand and addressing type*/
        strcpy(op2,get1stW(cpyStr));
        addr2 = getAdressingType(op2);
    }else{
        /*we already skipped all spaces, now we either have an operand (illegal) or the end of the string (NULL)*/
        if(cpyStr != NULL){
            printf("error in line %d: too many operands, command %s doesn't recieve operands\n",lineNumber, cmnd);
            return false;
        }
    }

    /*we now have the operands, and their addressing types, now we need to check the validity of addressing types with the operation*/
    if(checkAddrWithOpcode(thisOpcode,addr1,addr2)){
        /*addressing types ok, now we need to translate the line and write it to the code Image using out function*/
        if(writeToCodeImage(thisOpcode,op1,op2,addr1,addr2,codeImage,IC,lineNumber)){
            return true;
        }
        return false;
    }else{
        printf("error in line %d: addressing types not compatible with function\n",lineNumber);
        return false;
    }
}

/*writes data instruction operands to data image*/
/*gets a string representing .data instruction's argumets, checks validity, and writes to data image if ok*/
boolean translateData(char *input, dataImg dataImage[], int *DC, int lineNumber){
    char cpy[MAX_LINE] = {' '};
    char *token;

    strcpy(cpy,input);

    /*no double commas*/
    if(strstr(cpy,",,") != NULL){
        printf("error in line %d: multiple commas between opernads\n",lineNumber);
        return false;
    }
    /*no comma at start*/
    if(cpy[0] == ','){
        printf("error in line %d: number cannot contain comma\n", lineNumber);
        return false;
    }
    /*no comma at end*/
    if(cpy[strlen(cpy)-1] == ','){
        printf("errorr in line %d: number cannot contain comma\n",lineNumber);
        return false;
    }

    token = strtok(cpy, " \t,");
    while(token != NULL){ /*foe evry token between commas, we check if it's a legal number (that may include spaces)*/
        if(isNumber(token)){
            if(atoi(token) > 511 || atoi(token) < -512){ /*we can store up to 511/ -512 in 10 bits using 2's complement*/
                printf("error in line %d: number is too big to be stored\n",lineNumber);
                return false;
            }else{
            dataImage[(*DC)].content = atoi(token); /*number ok, we store it*/
            dataImage[(*DC)].lineNum = lineNumber;
            }
        }else{
            printf("error in line %d: illegal number in data instruction\n",lineNumber);
            return false;
        }
        token = strtok(NULL, " \t,");
        (*DC)++;
    }
    return true;
}

/*writes string instruction operands to data image*/
/*gets a string representing .string instruction's argumets, checks validity, and writes to data image if ok*/
boolean translateString(char *line, dataImg dataImage[], int *DC, int lineNumber){
    char cpy[MAX_LINE] = {' '};
    int i = 1;
    boolean endOfString = false;

    strcpy(cpy,line);

    if(cpy[0] != '"'){ /*a string has to start with a quotation mark*/
        printf("error in line %d: a string must start with a quotation mark\n",lineNumber);
        return false;
    }

    /*for each charracter in the string, we check if it closes the string, or if it's another character to add*/
    /*if it closes the string, we should add a "NULL terminator" in our translation (the value zero), and stop the loop*/
    for(i=1;i<strlen(cpy);i++){
        if(cpy[i] == '"'){
            endOfString = true;
            dataImage[(*DC)].content = 0;
            dataImage[(*DC)].lineNum = lineNumber;
            i++;
            (*DC)++;
            break;
        }else{
            dataImage[(*DC)].content = (int)cpy[i];
            dataImage[(*DC)].lineNum = lineNumber;
            (*DC)++;
        }
    }

    /*if we went thourgh the entire string, and did not close out string, that's an error*/
    if(!endOfString){
        printf("error in line %d: a string must end with a quotation mark\n",lineNumber);
        return false;
    }

    for(;i<strlen(cpy);i++){
        if(! isspace(cpy[i]) ){
            printf("error in line %d: too many arguments for .string instruction\n",lineNumber);
            return false;
        }
    }
    return true;
}

/*writes struct operands to data image*/
/*gets a string representing .struct instruction's argumets, checks validity, and writes to data image if ok*/
boolean translateStruct(char *line, dataImg dataImage[],int *DC, int lineNumber){
    char cpy[MAX_LINE] = {' '};
    char *token;

    strcpy(cpy,line);
    
    /*no double commas*/
    if(strstr(cpy,",,") != NULL){
        printf("error in line %d: multiple commas between opernads\n",lineNumber);
        return false;
    }
    /*there are commas in the input*/
    if(strchr(cpy,',') == NULL){
        printf("error in line %d: no comma between operands\n",lineNumber);
        return false;
    }
    /*no comma at start*/
    if(cpy[0] == ','){
        printf("error in line %d: number cannot contain comma\n",lineNumber);
        return false;
    }
    /*no comma at end*/
    if(cpy[strlen(cpy)-1] == ','){
        printf("error in line %d: extra comma after string in .struct instruction\n",lineNumber);
        return false;
    }

    token = strtok(cpy," ,\t");
    if(token == NULL){
        printf("error in line %d: no arguments for struct instruction\n",lineNumber);
        return false;
    }
    if(isNumber(token)){
        dataImage[(*DC)].content = atoi(token);
        dataImage[(*DC)].lineNum = lineNumber;
        (*DC)++;
    }else{
        printf("error in line %d: number in struct instruction is illegal\n",lineNumber);
        return false;
    }
    
    token = strtok(NULL,"\0");
    strcpy(token, skipSpace(token));
    if(*token == ','){
        token++;
        strcpy(token, skipSpace(token));
    }
    translateString(token, dataImage, DC, lineNumber);

    return true;
}

/*gets a string representing a data instruction (.data/.string/.struct) ,and translates it to binary code inside of the data Image as requested in maman's instructions */
boolean dataToWords(char *line, dataImg dataImage[], int *DC, int lineNumber){
    char cpy[MAX_LINE] = {' '};
    char word[MAX_LINE] = {' '};
    char *token;
    instruction currentInst;

    strcpy(cpy,line);
    strcpy(word,get1stW(cpy));

    currentInst = getInstType(word); /*getting instruction type*/

    /*skiping after the instruction, to the start of the value*/
    token = strtok(cpy, " \t");
    token = strtok(NULL, "\0");
    
    if(token == NULL || *token == '\0' || *token == '\n'){
        printf("error in line %d: no arguments after instruction\n", lineNumber);
        return false;
    }

    /*according to the type of data, we invoke the according function*/
    switch(currentInst){
        case DATA_INST:
            if(translateData(token, dataImage, DC, lineNumber)){
                return true;
            }
            break;
        case STRING_INST:
            if(translateString(token, dataImage, DC, lineNumber)){
                return true;
            }
            break;
        case STRUCT_INST:
            if(translateStruct(token, dataImage, DC, lineNumber)){
                return true;
            }
            break;
        default:
            return false;
    }
    return false;
}

#endif