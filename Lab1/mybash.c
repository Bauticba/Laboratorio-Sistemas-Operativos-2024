#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <pwd.h>

#include "command.h"
#include "execute.h"
#include "parser.h"
#include "parsing.h"
#include "builtin.h"
#include "obfuscated.h"

static void show_prompt(void) {
    char hostname[256];
    char cwd[1024];
    struct passwd *pw;
    char *username;

    pw = getpwuid(getuid());
    username = pw->pw_name;
    gethostname(hostname, sizeof(hostname));
    getcwd(cwd, sizeof(cwd));
    printf("\033[1;34m%s@%s\033[0m:\033[0;32m%s\033[0m> ", username, hostname, cwd);
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    pipeline pipe;
    Parser input;
    bool quit = false;

    input = parser_new(stdin);
    while (!quit) {
        //ping_pong_loop(NULL);
        show_prompt();
        pipe = parse_pipeline(input);

        if(pipe != NULL){
            execute_pipeline(pipe);
            pipeline_destroy(pipe);    
        }

        quit = parser_at_eof(input);
    }
    parser_destroy(input); 
    input = NULL;
    return EXIT_SUCCESS;
}