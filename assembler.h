#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include "lib_include.h"
#include "pre_pro.h"
#include "first_pass.h"
#include "second_pass.h"
#include "utils.h"
#include "structs.h"

/*main assembler run - runs pre proccessor and two passes on every file*/
int main(int argc, char **argv);

/*generates a file name using origin and a given extension*/
void generateFileName(char *origin, extension);

/*closes all given files*/
void closeFiles(FILE *, FILE *, FILE *, FILE *, FILE *);

/*resets all tables and variables*/
void resetVars(label[] , extNode[] , entNode[] , codeImg[] , dataImg[] , int *, int *);
#endif