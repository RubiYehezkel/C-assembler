#ifndef ASSEMBLER_MAIN
#define ASSEMBLER_MAIN

#include "assembler.h"

int main(int argc, char **argv){
    /*all FILE pointers*/
    FILE *ASfile;
    FILE *AMfile;
    FILE *EXTfile;
    FILE *ENTfile;
    FILE *OBfile;

    char fileName[MAX_LINE] = {' '};
    char ASfileName[MAX_LINE] = {' '};
    char AMfileName[MAX_LINE] = {' '};
    char EXTfileName[MAX_LINE] = {' '};
    char ENTfileName[MAX_LINE] = {' '};
    char OBfileName[MAX_LINE] = {' '};

    /*label table initialization*/
    label labelTab[MEM_CAP] = { {"\0",0,false} };

    /*entry and extern lists initialization*/
    extNode extList[MEM_CAP] = { {"\0",{0},0} };
    entNode entList[MEM_CAP] = { {"\0"} };

    /*code and data image initialization*/
    codeImg codeImage[MEM_CAP] = { {0,0,"\0"} };
    dataImg dataImage[MEM_CAP] = { {0,0} };

    int IC;
    int DC;

    int i;
    
    for(i=1;i<argc;i++){ /*first value is executable call, we skip it*/
        strcpy(fileName, argv[i]); /*get argument from argv*/
    
        /*generate file names according to input*/
        strcpy(ASfileName, fileName);
        strcpy(AMfileName, fileName);
        
        generateFileName(ASfileName, AS_EXT);
        generateFileName(AMfileName, AM_EXT);
        
        ASfile = fopen(ASfileName ,"r"); /*open source file for reading*/
        /*if a file is not found, we continue compiling other files*/
        if(ASfile == NULL){
            printf("error: file %s could not be opened\n",ASfileName);
            resetVars(labelTab, extList, entList, codeImage, dataImage, &IC, &DC);
            continue;
        }

        AMfile = fopen(AMfileName, "w+"); /*open after macro file for writing*/
        if(!pre_pro(ASfile, AMfile)){ /*if we have an error in the pre proccessing stage we continue to next file*/
            printf("error in pre proccessing, couldn't assemble file %s\n", ASfileName);
            closeFiles(ASfile, AMfile, NULL, NULL, NULL);
            resetVars(labelTab, extList, entList, codeImage, dataImage, &IC, &DC);
            continue;
        }
        
        rewind(AMfile); /*go to beggining of after macro file*/

        if(!first_pass(AMfile, labelTab, codeImage, dataImage, extList, entList, &IC, &DC)){ /*if we have an error in the first pass, we continue to next file*/
            printf("error in first pass, couldn't assemble file %s\n", AMfileName);
            closeFiles(ASfile, AMfile, NULL, NULL, NULL);
            resetVars(labelTab, extList, entList, codeImage, dataImage, &IC, &DC);
            continue;
        }

        /*after the first pass we have all of our extern and entry labels, so we should check if we need to open ,ent and .ext files*/

        if(extList[0].name[0] != '\0'){ /*if the first extern field is empty, then the rest is also empty*/
            strcpy(EXTfileName, fileName);
            generateFileName(EXTfileName, EXT_EXT); /*generate externals file name*/
            EXTfile = fopen(EXTfileName, "w+");
        }else{ /*if we have no external labels, we won't generate an .ext file, and file pointer will be NULL*/
            EXTfile = NULL;
        }

        /*same for entry list*/
        if(entList[0].name[0] != '\0'){
            strcpy(ENTfileName, fileName);
            generateFileName(ENTfileName, ENT_EXT); /*generate entries file name*/
            ENTfile = fopen(ENTfileName, "w+");
        }else{
            ENTfile = NULL;
        }

        /*generate object file name*/
        strcpy(OBfileName, fileName);
        generateFileName(OBfileName, OB_EXT);
        OBfile = fopen(OBfileName, "w+");

        if(!second_pass(labelTab, codeImage, dataImage, extList, entList, IC, DC, OBfile, EXTfile, ENTfile)){
            printf("error in second pass, couldn't assemble file %s\n", AMfileName);
            closeFiles(ASfile, AMfile, OBfile, EXTfile, ENTfile); /*close all files*/
            resetVars(labelTab, extList, entList, codeImage, dataImage, &IC, &DC); /*reset all variables*/
            continue;
        }

        /*close all files*/
        closeFiles(ASfile, AMfile, OBfile, EXTfile, ENTfile);
        /*reset all variables*/
        resetVars(labelTab, extList, entList, codeImage, dataImage, &IC, &DC);
    }

    return 1;
}

/*generates a file with the correct extension (changes origin)*/
void generateFileName(char *origin, extension inputExt){
    switch(inputExt){
        case AS_EXT:
            strcat(origin, ".as");
            break;
        case AM_EXT:
            strcat(origin, ".am");
            break;
        case EXT_EXT:
            strcat(origin, ".ext");
            break;
        case ENT_EXT:
            strcat(origin, ".ent");
            break;
        case OB_EXT:
            strcat(origin, ".ob");
            break;
    }
}

/*closes all given files*/
void closeFiles(FILE *ASFILE, FILE *AMFILE, FILE *OBFILE, FILE *EXTFILE, FILE *ENTFILE){
    /*close all files*/
    fclose(ASFILE);
    fclose(AMFILE);
    if(OBFILE != NULL){
        fclose(OBFILE);
    }
    if(ENTFILE != NULL){
        fclose(ENTFILE);
    }
    if(EXTFILE != NULL){
        fclose(EXTFILE);
    }
}

/*resets all tables and variables*/
void resetVars(label LABELTAB[], extNode EXTLIST[], entNode ENTLIST[], codeImg CODEIMAGE[], dataImg DATAIMAGE[], int *IC, int *DC){
    int i;
    int j;

    for(i=0;i<MEM_CAP;i++){
        /*reset label table*/
        if(LABELTAB != NULL){
            strcpy(LABELTAB[i].name , "\0");
            LABELTAB[i].address = 0;
            LABELTAB[i].dataFlag = false;
        }
        /*reset externals table*/
        if(EXTLIST != NULL){
            strcpy(EXTLIST[i].name, "\0");
            for(j=0;j<EXTLIST[i].useCount;j++){
                EXTLIST[i].useArr[j] = 0;
            }
            EXTLIST[i].useCount = 0;
        }
        /*reset entries table*/
        if(ENTLIST != NULL){
            strcpy(ENTLIST[i].name, "\0");
        }
        /*reset code image*/
        if(CODEIMAGE != NULL){
            CODEIMAGE[i].lineNum = 0;
            CODEIMAGE[i].content = 0;
            strcpy(CODEIMAGE[i].labName, "\0");
        }
        /*reset data image*/
        if(DATAIMAGE != NULL){
            DATAIMAGE[i].lineNum = 0;
            DATAIMAGE[i].content = 0;
        }
    }

    /*reset IC and DC*/
    *IC = 0;
    *DC = 0;
}

#endif