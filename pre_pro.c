#ifndef PRE_PRO_C
#define PRE_PRO_C

#include "pre_pro.h"

/*we get a line with a bigger size than max size, to check if we dont have too long lines*/
#define LINE_CHECK_LEN 100

/*main logic of the pre proccessor, makes macro table, saves macros, replaces macros*/
boolean pre_pro(FILE *in,FILE *out){
    /*initialize macro table*/
    macro macList[MEM_CAP] = { {"\0","\0"} };

    /*line, macro counters*/
    int lineNum = 0;
    int macroCounter = 0;

    /*flag for wether we encountered a macro definition*/
    boolean macFlag = false;

    /*buffer*/
    char buf[LINE_CHECK_LEN] = {' '};
    char word[MAX_LINE] = {' '};

    while(fgets(buf,LINE_CHECK_LEN,in) != NULL){
        lineNum++;
        if(strlen(buf) <= MAX_LINE){ /*if we get more than 80 characters in a line, it's illegal*/
            if(!ignore(buf)){ /*comments and blank lines should be ignored*/
                strcpy(word, get1stW(buf));
                
                /*if the first word is a name of a known macro, we print it's content to the output file*/
                if(isMacro(macList,word,macroCounter) ){
                    replaceMacro(out, macList, word);
                }else if(strcmp(word,"macro") == 0){ /*if it's a new macro definition*/
                    if(!isReserved(word)){ /*if it is legal for a macro's name (not reserved)*/
                        strcpy(word, getMacName(buf));
                        strcpy(macList[macroCounter].name , word); /*we save it's name*/
                        macroCounter++;

                        macFlag = true;
                        while(macFlag){ /*now we add lines to the macro's content, as long as we don't get the "endmacro" call*/
                            fgets(buf,LINE_CHECK_LEN,in);

                            if(strlen(buf) > MAX_LINE){ /*if a line is illegal we throw an error*/
                                printf("error in line %d: line is too long\n",lineNum);
                                return false;
                            }

                            if(ignore(buf)){ /*if a line is a comment line, or blank, it shouldn't be saved to the macro*/
                                fprintf(out,"%s",buf);
                                continue;
                            }
                            strcpy(word,get1stW(buf));

                            if(strcmp(word,"endmacro") == 0){
                                macFlag = false;
                            }else{ /*if the line does not end the macro, we add it to the macro's content*/
                                strcat(macList[macroCounter-1].data, skipSpace(buf));
                            }
                        }
                    }
                    else{ /*illeagal macro name*/
                        printf("error in line %d: macro name cannot be a reserved word\n", lineNum);
                        return false;
                    }

                }else{ /*it is not a macro definition, nor a macro's name*/
                    fprintf(out,"%s",buf);
                }
            }else{ /*ignored line*/
                fprintf(out,"%s",buf);
            }
        }else{ /*line longer than MAX_LINE*/
            printf("error in line %d: line is too long\n",lineNum);
            return false;
        }
    }
    return true; /*all ok, return true*/
}

/*gets a macro's name from the format "macro <macName>" */
char *getMacName(char *line){
    line = strtok(line, " ");
    return strtok(NULL, " \n\0");
}

/*Function to check wether a macro already exists with input name*/
boolean isMacro(macro list[],char *nameCheck,int len){
    int i;
    for(i=0;i<len;i++){
        if(strcmp((list)[i].name, nameCheck) == 0){
            return true;
        }
    }
    return false;
}

/*gets a macro's name, and prints out it's content into the out file*/
boolean replaceMacro(FILE *outf, macro list[], char *nameCheck){
    int i;
    for(i=0;i<100;i++){
        if(strcmp(list[i].name, nameCheck) == 0){
            break;
        }
    }
    /*here we either have the macro's index, or we have passed it and we have an empty field*/
    if(strcmp(list[i].name, nameCheck) == 0){ /*if we have the correct macro, we print it's content*/
        fprintf(outf,"%s",list[i].data);
        return true;
    }else{ /*if not, the macro is not found*/
        return false;
    }
}

#endif