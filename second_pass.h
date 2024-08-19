#ifndef SECOND_PASS__H
#define SECOND_PASS__H

#include "lib_include.h"
#include "utils.h"
#include "structs.h"

/*main logic of second pass - replaces label names with addresses in code image, prints all data into corresponding files*/
boolean second_pass(label labelTab[], codeImg codeImage[],dataImg dataImage[] ,extNode externList[], entNode entryList[], int IC,int DC, FILE *OBfile, FILE *EXTfile, FILE *ENTfile);

/*check if a given label is extern or entry*/
boolean isExtern(char *, extNode []);
boolean isEntry(char *, entNode []);

/*get information out of a labels' name (index in extern list, address)*/
int getExternIndex(char *, extNode []);
unsigned getAddress(char *, label []);

/*translates a number into a base 32 string that will be stored inside given string (changes original string)*/
void translateToBase32(char *, unsigned );

#endif