#ifndef FIRST_PASS_H
#define FIRST_PASS_H

#include "lib_include.h"
#include "utils.h"
#include "structs.h"

/*all function prototypes - first pass*/

/*main logic of first pass:
* translates every line (that needs to be translated) into code/data image,
* stores lables to label table, 
* stores extern/entry defined labels to according arrays, 
* updates IC and DC to correspond with instruction and data count*/
boolean first_pass(FILE *infile, label labelTab[], codeImg codeImage[],dataImg dataImage[] ,extNode externList[], entNode entryList[], int *IC,int *DC);

/*check if a string represents a legal number / is it all constructed of alpha-numeral characters*/
boolean isNumber(char *);
boolean isalnumStr(char *);

/*check legality of a word (legal for label, not reserved etc.)*/
int isLabelDef(char *);
boolean isLegalLabel(char *);

/*getting various information from opcode and operands (operand number, addressing types etc.)*/
int getOperandNum(opcode);
int getAdressingType(char *);
boolean checkAddrWithOpcode(opcode , addressing_type, addressing_type);

/*parsing, translating, and writing to code image*/
boolean CodeToWords(char *, codeImg[], int *, int);
boolean writeToCodeImage(opcode,char *,char *,addressing_type ,addressing_type, codeImg [], int *,int);

/*parsing, translating, and writing to data image*/
boolean dataToWords(char *, dataImg [], int *, int);
boolean translateStruct(char *, dataImg [],int *, int);
boolean translateString(char *, dataImg [], int *, int);
boolean translateData(char *, dataImg [], int *, int);

/*end of function prototypes*/
#endif