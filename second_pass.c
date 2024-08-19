#ifndef SECOND_PASS_C
#define SECOND_PASS_C

#include "second_pass.h"

/*main logic of second pass - replaces label names with addresses in code image, prints all data into corresponding files*/
boolean second_pass(label labelTab[], codeImg codeImage[],dataImg dataImage[] ,extNode externList[], entNode entryList[], int IC,int DC, FILE *OBfile, FILE *EXTfile, FILE *ENTfile){
    int i;
    int j;
    int currIndex;
    unsigned currAddress = FIRST_ADDRESS;

    char translatedNum[3] = {' '};
    char totalLen[3] = {' '};
    char dataLen[3] = {' '};

    boolean extFlag = false;
    boolean entFlag = false;

    boolean ERRFlag = false; /*flag is turned on if we got errors*/

    /*for code and data separation, we must put data labels after code labels*/
    for(i=0;i<MEM_CAP;i++){
        if(labelTab[i].name[0] == '\0'){ /*if we reached the end of the table we break out*/
            break;
        }
        if(labelTab[i].dataFlag){
            labelTab[i].address += (IC + FIRST_ADDRESS);
        }
    }

    /*check that all entries are defined, and print to entries file*/
    if(ENTfile != NULL){ /*we might not recieve an entries file, if we dont have any entry declarations in the source file*/
        for(i=0;i<MEM_CAP;i++){
            if(entryList[i].name[0] == '\0'){ /*if we have reached the end of the entry list we break out*/
                break;
            }
            if(!isDefinedLabel(entryList[i].name, labelTab)){
                printf("error: label %s declared as entry and not defined\n", entryList[i].name);
                ERRFlag = true;
                continue;
            }
            translateToBase32(translatedNum, getAddress(entryList[i].name, labelTab)); /*get label's address in base 32*/
            fprintf(ENTfile, "%s\t%s\n", entryList[i].name, translatedNum); /*print label's name and address*/
        }
    }

    /*if we have a label name in the code image, we need to translate it to it's address + ARE*/
    for(i=0;i<IC;i++){
        if(codeImage[i].labName[0] != '\0'){ /*if a field in data image is a label*/
            extFlag = false;
            entFlag = false;

            if(isExtern(codeImage[i].labName, externList)){
                /*if a label is extern we should save the address i which it was used, and it's translation will be the extern flag*/
                if(getAddress(codeImage[i].labName, labelTab) != UNASSIGNED){
                    printf("error: label %s defined as extern and initialized in source file\n",codeImage[i].labName);
                    ERRFlag = true;
                    continue;
                }
                extFlag = true;
                currIndex = getExternIndex(codeImage[i].labName, externList); 
                externList[currIndex].useArr[ externList[currIndex].useCount ] = (i + FIRST_ADDRESS); /*code image starts at 1st address, so we add it here*/
                externList[currIndex].useCount++;
                codeImage[i].content = E_FLAG;
            }

            if(isEntry(codeImage[i].labName, entryList)){ entFlag = true; }

            if(extFlag && entFlag){ /*the same label cannoot be extern and entry*/
                printf("error: label %s is defined as extern and as entry\n",codeImage[i].labName);
                ERRFlag = true;
                continue;
            }
            
            if(!extFlag && !isDefinedLabel(codeImage[i].labName, labelTab)){ /*if a label is used, but not defined, it's an error*/
                printf("error in line %d: label %s undefined\n", codeImage[i].lineNum, codeImage[i].labName);
                ERRFlag = true;
                continue;
            }

            if(!extFlag){
                codeImage[i].content += getAddress(codeImage[i].labName, labelTab); /*all ok, translate label name to number*/
                codeImage[i].content <<= SIZEOF_ARE;
                codeImage[i].content += R_FLAG;
            }
        }
    }

    if(ERRFlag){ /*if we got errors, we don't want to write to our files*/
        return false;
    }
    translateToBase32(totalLen, IC);
    translateToBase32(dataLen, DC);

    /*print total code length and data length*/
    fprintf(OBfile, "%s %s\n", totalLen, dataLen);

    /*print code image*/
    for(i=0;i<IC;i++){
        /*print address*/
        translateToBase32(translatedNum, currAddress);
        fprintf(OBfile, "%s\t", translatedNum);

        /*print value*/
        translateToBase32(translatedNum, codeImage[i].content);
        fprintf(OBfile, "%s\n", translatedNum);

        currAddress++;
    }
    
    /*print data image*/
    for(i=0;i<DC;i++){
        /*print address*/
        translateToBase32(translatedNum, currAddress);
        fprintf(OBfile, "%s\t", translatedNum);

        /*print value*/
        translateToBase32(translatedNum, dataImage[i].content);
        fprintf(OBfile, "%s\n", translatedNum);

        currAddress++;
    }

    /*print externals*/
    if(EXTfile != NULL){ /*we might not recieve an extern file, if we have no extern labels*/
        /*print to extern file*/
        for(i=0;i<MEM_CAP;i++){
            if(externList[i].name[0] == '\0'){ /*if we reached the end of the external lables, we break out*/
                break;
            }

            for(j=0;j<externList[i].useCount;j++){ /*for every external label, we print it's name and every address it has been used at*/
                translateToBase32(translatedNum, externList[i].useArr[j]);
                fprintf(EXTfile, "%s\t%s\n", externList[i].name, translatedNum);

            }
        }
    }

    return !ERRFlag;
}
/*checks if a label is defined as extern*/
boolean isExtern(char *label, extNode extList[]){
    int i;
    for(i=0;i<MEM_CAP;i++){
        if(strcmp(extList[i].name, label) == 0){
            return true;
        }
    }
    return false;
}

/*checks if a label is defined as entry*/
boolean isEntry(char *label, entNode entList[]){
    int i;
    for(i=0;i<MEM_CAP;i++){
        if(strcmp(entList[i].name, label) == 0){
            return true;
        }
    }
    return false;
}

/*gets the index of an extern label inside the eternals array*/
int getExternIndex(char *label, extNode list[]){
    int i=0;
    for(i=0;i<MEM_CAP;i++){
        if(strcmp(list[i].name, label) == 0){
            return i;
        }
    }
    return UNASSIGNED;
}

/*gets an address of a label defined in the label table*/
unsigned getAddress(char *labelCheck, label labelTab[]){
    int i;
    for(i=0;i<MEM_CAP;i++){
        if(strcmp(labelTab[i].name , labelCheck) == 0){
            return labelTab[i].address;
        }
    }
    return UNASSIGNED;
}

/*translated a number into a string in base 32 characters (according to maman) (changes input string)*/
void translateToBase32(char *change, unsigned num){
    char base32Values[32] = {'!','@','#','$','%','^','&','*','<','>','a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v'};
    change[1] = base32Values[num % 32];
    num /= 32;
    change[0] = base32Values[num % 32];
    change[2] = '\0';
}

#endif