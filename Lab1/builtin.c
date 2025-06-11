#include <stdbool.h>
#include <stdio.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "tests/syscall_mock.h"
#include "command.h"
#include "builtin.h"

const char* internal_commands[] = {"cd", "exit", "help", "echo", "pwd"};


cmdType get_command_type(const char* command_name) {
    if (!strcmp(command_name, "cd")) return CD;
    if (!strcmp(command_name, "help")) return HELP;
    if (!strcmp(command_name, "exit")) return EXIT;
    if (!strcmp(command_name, "echo")) return ECHO;
    if (!strcmp(command_name, "pwd")) return PWD;
    return UNKNOWN;
}

bool builtin_is_internal(scommand cmd){
    assert(cmd != NULL);

    for(unsigned int i = 0; i<5; i++){
        if(!strcmp(scommand_front(cmd), internal_commands[i])){
            return true;
        }
    }
    return false;
}

bool builtin_alone(pipeline p){
    assert(p != NULL);

    return(pipeline_length(p) == 1 && builtin_is_internal(pipeline_front(p)));
}

void builtin_run(scommand cmd){
    assert(builtin_is_internal(cmd));

    const char* cmd_name = scommand_front(cmd);
    cmdType cmd_type = get_command_type(cmd_name);

    switch(cmd_type){
        case CD:
            if (scommand_length(cmd) <= 1) //si es cd solo
            {
                char *homedir = getenv("HOME"); //cambio directorio a home del usuario
                if (homedir != NULL){ //si se encontro home entonces cambio el directorio actual al directorio de inicio
                    chdir(homedir); 
                }
            } else {
                scommand_pop_front(cmd); //elimino cd para laburar con la ruta
                int res = chdir(scommand_front(cmd)); //scomand front devuelve la ruta y chdir intenta cambiar el directorio actual
                if (res != 0)
                {
                    char *str =scommand_to_string(cmd); //si hay error 
                    perror(str); //imprime msj de error incluye el comando que se intentó ejecutar, seguido de una descripción del err
                    free(str); 
                }
            }
            break;
        case HELP:
            printf("Integrantes:\n" 
            "Facundo Gabriel Garione Chavez\n"
            "Malena Bustamante\n"
            "Santino Bianco Monasterio\n"
            "Luis Bautista Cordoba Buffa\n");
            printf("Comandos internos implementados:\n");
            printf("\tcd: Cambia el directorio. Sintaxis: cd [directorio]\n");
            printf("\thelp: Muestra este mensaje actual, con informacion adicional de mybash.\n");
            printf("\texit: Termina la ejecución del shell. Sintaxis: exit\n");
            printf("\techo: Muestra un mensaje en la salida estándar. Sintaxis: echo [mensaje]\n");
            printf("\tpwd: Muestra el directorio de trabajo actual. Sintaxis: pwd\n");
            printf("\nUsage: mybash <command>\n");
            break;
        case EXIT:
            printf("Fin del programa; Hasta Pronto!!\n");
            exit(EXIT_SUCCESS);
            break;
        case ECHO:
        scommand_pop_front(cmd); 
        char **args = scommand_to_array(cmd); 
        for (int i = 0; args[i] != NULL; i++)
        {
            printf("%s", args[i]);
        }
        printf("\n");
        free(args);
        break;
    
        case PWD:
        {
            char cwd[1024];
            if (getcwd(cwd, sizeof(cwd)) != NULL)
            {
                printf("%s\n", cwd);
            }
            else
            {
                perror("pwd");
            }
            break;
        }
            default:
            
                break;
        }
}