#ifndef THROW_H
#define THROW_H

#include <stdio.h>
#include <stdarg.h>

#include "../Library/Console_Tools.h"
#include "../Library/CStr.h"
#include "../Library/String.h"
 
void Throw_Info(CStr format,...) {
    va_list args;
    va_start(args,format);
    printf(ANSI_FG_GREEN "Info: " ANSI_FG_WHITE);
    printf(format,args);
    printf("\n");
    va_end(args);
}
void Throw_Warning(CStr format,...) {
    va_list args;
    va_start(args,format);
    printf(ANSI_FG_YELLOW "Warning: " ANSI_FG_WHITE);
    printf(format,args);
    printf("\n");
    va_end(args);
}
void Throw_Error(CStr format,...) {
    va_list args;
    va_start(args,format);
    printf(ANSI_FG_RED "Error: " ANSI_FG_WHITE);
    printf(format,args);
    printf("\n");
    va_end(args);
}

#endif