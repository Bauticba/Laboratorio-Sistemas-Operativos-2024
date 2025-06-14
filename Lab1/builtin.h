#ifndef _BUILTIN_H_
#define _BUILTIN_H_

#include <stdbool.h>

#include "command.h"

//arreglo con los nombres nuestros comandos internos
typedef enum {
    CD,
    HELP,
    EXIT,
    ECHO,
    PWD,
    UNKNOWN
} cmdType;
//a cada comnando interno le agg un identificador 

cmdType get_command_type(const char* command_name);

bool builtin_is_internal(scommand cmd);
/*
 * Indica si el comando alojado en `cmd` es un comando interno
 *
 * REQUIRES: cmd != NULL
 *
 */


bool builtin_alone(pipeline p);
/*
 * Indica si el pipeline tiene solo un elemento y si este se corresponde a un
 * comando interno.
 *
 * REQUIRES: p != NULL
 *
 * ENSURES:
 *
 * builtin_alone(p) == pipeline_length(p) == 1 &&
 *                     builtin_is_internal(pipeline_front(p))
 *
 *
 */

void builtin_run(scommand cmd);
/*
 * Ejecuta un comando interno
 *
 * REQUIRES: {builtin_is_internal(cmd)}
 *
 */

#endif

