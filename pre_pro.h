#ifndef PRE_PRO_H
#define PRE_PRO_H

#include "lib_include.h"
#include "utils.h"
#include "structs.h"

/*main logic of the pre proccessor*/
boolean pre_pro(FILE *,FILE *);

/*get macro name from a line, replace macro's name with it's content*/
char *getMacName(char *);
boolean replaceMacro(FILE *, macro [], char *);

/*check if a line is a comment or a defined macro*/
boolean isComment(char *);
boolean isMacro(macro [],char *,int);

#endif