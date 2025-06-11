#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <assert.h>
#include "strextra.h"
#include "command.h"
#include <string.h>
#include <glib.h>


struct scommand_s{
    GSList* list;
    char* input;
    char* output;
};

scommand scommand_new(void){
    scommand res = malloc(sizeof(struct scommand_s));
    
    res->list = NULL;
    res->input = NULL;
    res->output = NULL;

    assert(res != NULL && scommand_is_empty (res) && scommand_get_redir_in (res) == NULL && 
    scommand_get_redir_out (res) == NULL);
    return res;
}

scommand scommand_destroy(scommand self){
    assert(self != NULL);

    g_slist_free_full(self->list, free);
    free(self->input); self->input = NULL;
    free(self->output); self->output = NULL;
    free(self); self = NULL;

    assert(self == NULL);
    return self;
}

void scommand_push_back(scommand self, char * argument){
    assert(self != NULL && argument != NULL);

    self->list = g_slist_append(self->list, argument);

    assert(!scommand_is_empty(self));
}

void scommand_pop_front(scommand self) {
    assert(self != NULL && !scommand_is_empty(self));

    GSList *first_node = self->list;
    self->list = g_slist_remove_link(self->list, first_node);

    if(first_node!=NULL){
        free(first_node->data);
        g_slist_free_1(first_node);
    }
}

void scommand_set_redir_in(scommand self, char * filename){
    assert(self!=NULL);

    if(self->input != NULL){
        free(self->input);
    }
    self->input = filename;
}

void scommand_set_redir_out(scommand self, char * filename){
    assert(self!=NULL);

    if(self->output != NULL){
        free(self->output);
    }
    self->output = filename;
}

bool scommand_is_empty(const scommand self){
    assert(self != NULL);
    return(g_slist_length(self->list)==0);
}

unsigned int scommand_length(const scommand self){
    assert(self!=NULL);

    return(g_slist_length(self->list));
}

char * scommand_front(const scommand self){
    assert(self != NULL && !scommand_is_empty(self));

    char *result = (char *)g_slist_nth_data(self->list, 0);

    assert(result != NULL);
    return result;
}

char * scommand_get_redir_in(const scommand self){
    assert(self!=NULL);

    char *result = self->input;

    return result;
}

char * scommand_get_redir_out(const scommand self){
    assert(self!=NULL);

    char *result = self->output;

    return result;
}

char * scommand_to_string(const scommand self){
    assert(self!=NULL);
    char *result = strdup("");
    GSList *list_c = self->list;

    while(list_c != NULL){
        char *tmp_str = strmerge(result, (char *)g_slist_nth_data(list_c, 0));
        free(result);
        result = strmerge(tmp_str, " ");
        free(tmp_str);
        list_c = g_slist_next(list_c);
    }

    if(self->list != NULL && strlen(result) > 0){
        result[strlen(result) - 1] = '\0'; //delete last " "
    }

    if(self->input != NULL){
        char *tmp_str = strmerge(result, " < ");
        free(result);
        result = strmerge(tmp_str, scommand_get_redir_in(self));
        free(tmp_str);
    }

    if(self->output != NULL){
        char *tmp_str = strmerge(result, " > ");
        free(result);
        result = strmerge(tmp_str, scommand_get_redir_out(self));
        free(tmp_str);
    }

    assert(scommand_is_empty(self) || scommand_get_redir_in(self)==NULL || scommand_get_redir_out(self)==NULL ||
    strlen(result)>0);
    return result;
}

char **scommand_to_array(scommand cmd) {
    assert(cmd != NULL);
    unsigned int size = scommand_length(cmd);
    char **array = calloc(size + 1, sizeof(char*));
    if(array == NULL){
        exit(EXIT_FAILURE);
    }

    for(unsigned int i = 0; i < size; i++){
        assert(!scommand_is_empty(cmd));

        char *arg = g_slist_nth_data(cmd->list, 0);
        cmd->list = g_slist_remove(cmd->list, arg);

        array[i] = arg;
    }

    array[size] = NULL;

    return array;
}

struct pipeline_s{
    GSList* command;
    bool foreground; //true:foreground, false:background
};

pipeline pipeline_new(void){
    pipeline res = malloc(sizeof(struct pipeline_s));
    res->command = NULL;
    res->foreground = true;

    assert(res != NULL && pipeline_is_empty(res) && pipeline_get_wait(res));
    return res;
}

pipeline pipeline_destroy(pipeline self) {
    assert(self != NULL);

    unsigned int index = 0;
    unsigned int length = g_slist_length(self->command);

    while(index<length){
        scommand cmd = (scommand)g_slist_nth_data(self->command, index);
        scommand_destroy(cmd);
        index++;
    }

    g_slist_free(self->command);
    free(self);
    self = NULL;

    return self;
}

void pipeline_push_back(pipeline self, scommand sc){
    assert(self != NULL && sc != NULL);

    self->command = g_slist_append(self->command, sc);

    assert(!pipeline_is_empty(self));
}

void pipeline_pop_front(pipeline self) {
    assert(self != NULL && !pipeline_is_empty(self));

    GSList *first = g_slist_nth(self->command, 0);
    scommand cmd = (scommand)g_slist_nth_data(self->command, 0);
    self->command = g_slist_remove_link(self->command, first);
    scommand_destroy(cmd);
    g_slist_free_1(first);
}

void pipeline_set_wait(pipeline self, const bool w){
    assert(self!=NULL);

    self->foreground = w;
}

bool pipeline_is_empty(const pipeline self){
    assert(self!=NULL);

    return(g_slist_length(self->command)==0);
}

unsigned int pipeline_length(const pipeline self){
    assert(self!=NULL);

    return(g_slist_length(self->command));
}

scommand pipeline_front(const pipeline self){
    assert(self!=NULL && !pipeline_is_empty(self));

    scommand result = (scommand)g_slist_nth_data(self->command, 0);

    assert(result!=NULL);
    return result;
}

bool pipeline_get_wait(const pipeline self){
    assert(self!=NULL);

    return(self->foreground);
}

char * pipeline_to_string(const pipeline self) {
    assert(self!=NULL);
    char *result = strdup("");
    GSList *command_c = self->command;

    while(command_c!=NULL){
        scommand cmd = (scommand)g_slist_nth_data(command_c, 0);
        char *str_cmd = scommand_to_string(cmd);
        char *tmp = strmerge(result, str_cmd);
        free(result);
        result = strmerge(tmp, " | ");
        free(tmp);
        free(str_cmd);
        command_c = g_slist_next(command_c);
    }

    size_t len = strlen(result);
    if (len >= 3 && strcmp(result + len - 3, " | ") == 0) {
        result[len - 3] = '\0';
    }

    if(!self->foreground){
        result = strmerge(result, " &");
    }

    assert(pipeline_is_empty(self) || pipeline_get_wait(self) || strlen(result)>0);
    return result;
}