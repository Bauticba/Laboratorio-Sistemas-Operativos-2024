#include <stdlib.h>
#include <stdbool.h>

#include "parsing.h"
#include "parser.h"
#include "command.h"

static scommand parse_scommand(Parser p) {
    char *arg;
    scommand cmd = scommand_new();
    arg_kind_t type;

    arg = parser_next_argument(p, &type);
    if(arg == NULL){
        return NULL;
    }
    while(!parser_at_eof(p)){
        if(arg == NULL){
            break;
        }
        else{
            if(type==ARG_NORMAL){
                if (arg == NULL) {
                free(arg);
                return cmd;
                }
                else{
                    scommand_push_back(cmd, arg);
                }
            }
            else if(type == ARG_INPUT){
                scommand_set_redir_in(cmd, arg);
            }
            else if(type == ARG_OUTPUT){
                scommand_set_redir_out(cmd, arg);
            }
            else{
                return NULL;
            }
        }
        if(!parser_at_eof(p)){
            arg = parser_next_argument(p, &type);
        }
    }
    return cmd;
}

pipeline parse_pipeline(Parser p) {
    pipeline result = pipeline_new();
    scommand cmd = NULL;
    bool error = false, another_pipe=true, is_op_background, garbage;

    while (another_pipe && !error) {
        cmd = parse_scommand(p);
        error = (cmd==NULL);
        if(!error){
            pipeline_push_back(result, cmd);
            parser_op_pipe(p, &another_pipe);
        }      
        else{
            printf("Error de parsing\n");
            pipeline_destroy(result);
            parser_skip_blanks(p);
            parser_garbage(p, &garbage);
            return NULL;
        }
    }
    if(!error){
        parser_op_background(p, &is_op_background);
        pipeline_set_wait(result, !is_op_background);
    }    
    /* Tolerancia a espacios posteriores */
    parser_skip_blanks(p);
    /* Consumir todo lo que hay inclusive el \n */
    parser_garbage(p, &garbage);
    /* Si hubo error, hacemos cleanup */
    return result;
}
