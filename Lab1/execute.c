#include <stdbool.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>                 // Contiene declaracion de funciones fork(), pipe(), dup2(), execvp()
#include <sys/wait.h>               // Contiene declaracion de wait()
#include <fcntl.h>                  // Contiene open(), y las constantes O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
#include <errno.h>                  // Contiene definicion de la constante errno
#include "tests/syscall_mock.h"     // Mock para syscalls en los tests

#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "command.h"
#include "builtin.h"

void apply_redirects(scommand scmd) {
    char *redir_in = scommand_get_redir_in(scmd);
    char *redir_out = scommand_get_redir_out(scmd);
    if (redir_in != NULL) {
        int input_fd = open(redir_in, O_RDONLY, 0);
        if (input_fd == -1) {
            perror("No se pudo abrir redireccion de entrada");
            exit(EXIT_FAILURE);
        }
        dup2(input_fd, STDIN_FILENO);
        close(input_fd);
    }
    if (redir_out != NULL) {
        int output_fd = open(redir_out, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        if (output_fd == -1) {
            perror("No se pudo abrir redireccion de salida");
            exit(EXIT_FAILURE);
        }
        dup2(output_fd, STDOUT_FILENO);
        close(output_fd);
    }
}
void execute_pipeline(pipeline apipe)
{
    scommand cmd;
    assert(apipe != NULL);

    if (!(pipeline_is_empty(apipe))) // Si hay comandos
    {
        cmd = pipeline_front(apipe);
        
        if (builtin_is_internal(cmd)) // si es interno
        {
            builtin_run(cmd);
        }
        else // si no es interno
        {
            unsigned int num_pipes = pipeline_length(apipe) - 1;
            int fd[num_pipes][2];

            // crear los pipes
            for (unsigned int i = 0; i < num_pipes; i++) {
                if (pipe(fd[i]) == -1) {
                    perror("Error al abrir los pipes");
                    exit(EXIT_FAILURE);
                }
            }

            // Ejecutar los comandos en el pipeline
            for (unsigned int i = 0; i <= num_pipes; i++) {
                
                pid_t pid = fork(); // crea un nuevo proceso con fork para ejecutar execv
                
                if (pid < 0) {
                    perror("fork"); // error al hacer el fork
                    exit(EXIT_FAILURE);
                }
                else if (pid == 0) { // proceso hijo
                    
                    // Redirigir la entrada estándar al pipe si no es el primer comando
                    if (i > 0) {
                        dup2(fd[i-1][0], STDIN_FILENO);
                    }
                    // Redirigir la salida estándar al pipe si no es el último comando
                    if (i < num_pipes) {
                        dup2(fd[i][1], STDOUT_FILENO);
                    }

                    // Cerrar todos los descriptores de pipe en el proceso hijo
                    for (unsigned int j = 0; j < num_pipes; j++) {
                        close(fd[j][0]);
                        close(fd[j][1]);
                    }
                    
                    apply_redirects(cmd);

                    char *cmd_name = scommand_front(cmd); // nombre del archivo
                    char **args = scommand_to_array(cmd); // argumentos
                    
                    execvp(cmd_name, args); // ejecutar el comando
                    printf("Comando no reconocido\n"); // error en el comando
                    free(args);
                    exit(EXIT_FAILURE);
                } else {
                    
                    // El proceso padre no debe hacer nada más que esperar a los hijos
                    if (i != 0) {
                        close(fd[i-1][0]);
                    }
                    if (i != num_pipes) {
                        close(fd[i][1]);
                    }
                }
                // Proceso padre no hace nada aún, solo sigue al siguiente comando
                pipeline_pop_front(apipe);
                if (i < num_pipes) {
                    cmd = pipeline_front(apipe); // Siguiente comando en el pipeline
                }
            }

            // Esperar a que todos los procesos hijos terminen
            if (pipeline_get_wait(apipe)) {
                for (unsigned int i = 0; i <= num_pipes; i++) {
                    wait(NULL);
                }
            }
        }
    }
}
