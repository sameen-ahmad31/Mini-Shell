#include <msh_parse.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

struct msh_sequence
{
	struct msh_pipeline * head;
	struct msh_pipeline * tail;
};

struct msh_pipeline 
{
	struct msh_pipeline * next;
	struct msh_command * c[MSH_MAXCMNDS];
	char *in_str;
    int i;
    int background;
};

struct msh_command {
	char *args[MSH_MAXARGS + 2];
    int isLast;
    void *data;
    int count;
    int variant;
    struct msh_command * next;
    msh_free_data_fn_t freefn;
};

struct cmd_data {
    pid_t pid;
};

/**
 * `msh_pipeline_free` frees a pipeline.
 */

void msh_pipeline_free(struct msh_pipeline *p)
{
    for(int i = 0; i < MSH_MAXCMNDS; i++)
    {
        if(p->c[i] == NULL)
        {
            break;
        }
        for(int j = 0; j < MSH_MAXARGS; j++)
        {
            if(p->c[i]->args[j] == NULL)
            {
                break;
            }
            free(p->c[i]->args[j]);
        }
        free(p->c[i]->data);
        free(p->c[i]);
      
    }
    free(p->in_str);
    free(p);
}
 

/**
 * `msh_sequence_free` deallocates the entire sequence, including all
 * constituent pipelines and commands. However, pipelines that have
 * been removed from the sequence (using `msh_sequence_pipeline`) are
 * no longer part of the sequence.
 *
 * - `s` - The sequence to free. Ownership is passed for `s`.
 */


void msh_sequence_free(struct msh_sequence *s)
{
    while (s != NULL) 
    {
        struct msh_pipeline *p = s->head;
        while (p != NULL) 
        {
            struct msh_pipeline *pNext = p->next;
            msh_pipeline_free(p);
            p = pNext;
        }
        s->head = NULL;
        s->tail = NULL;
        free(s);
        s = NULL;
    }
}




/**
 * `msh_sequence_alloc` simply allocates a sequence structure which is
 * effectively a queue.
 */

struct msh_sequence *
msh_sequence_alloc(void)
{
	//struct msh_sequence *seq = (struct msh_sequence*) malloc(sizeof(struct msh_sequence));
    struct msh_sequence *seq = calloc(1, sizeof(struct msh_sequence));
    if (seq == NULL) 
    {
        return NULL;
    }
    seq->head = NULL;
    seq->tail = NULL;
    return seq;
}


/**
 * `msh_pipeline_input` returns the string used as input for the
 * pipeline. Most useful when printing out the "jobs" builtin command
 * output.
 *
 * - `@p` - The borrowed pipeline for which we retrieve the input
 * - `@return` - the borrowed input used to create the pipeline
 */
char *
msh_pipeline_input(struct msh_pipeline *p)
{
	return p->in_str;
}

/**
 * `msh_sequence_parse` takes the command string, parses it, and
 * inserts pipelines therein into the sequence queue.
 *
 * - `@str` - the string holding pipelines and commands. This function
 *     borrows this string, thus does not `free` it.
 * - `@s` - the sequence into which you can queue up pipelines.
 * - `@return` - return `0` on success (in which case `result` is
 *     set), or a `msg_err_t` otherwise.
 */
msh_err_t
msh_sequence_parse(char *str, struct msh_sequence *seq)
{
	//(void)str;
	//(void)seq;

   // printf("--------------------------ENTERS INTO THE PARSE FUNCTION-------------------------------\n");
    msh_err_t err = 0;
 
    //struct msh_pipeline *pipeline = NULL;
    //struct msh_pipeline *prev = NULL;
    
    char *token0, *saveptr0;
    char *token, *saveptr;
    char *token2, *saveptr2;
    char * input = strdup(str);

    for(token0 = strtok_r(input, ";", &saveptr0); token0 != NULL; token0 = strtok_r(saveptr0, ";", &saveptr0))
    {
      //  pipeline = (struct msh_pipeline*) malloc(sizeof(struct msh_pipeline));

        struct msh_pipeline *pipeline = calloc(1, sizeof(struct msh_pipeline));
        if (pipeline == NULL) 
        {
            err = MSH_ERR_NOMEM;
            msh_pipeline_free(pipeline);
            //free(pipeline);
            return err;
        } 
        if(pipeline->i > MSH_MAXCMNDS)
        {
            err = MSH_ERR_TOO_MANY_CMDS;
            msh_pipeline_free(pipeline);
            return err;
        }

        pipeline->in_str = strdup(token0);
        if (seq->head == NULL) 
        {
            seq->head = pipeline;
            seq->tail = pipeline;
        } 
        else 
        {
            seq->tail->next = pipeline;
            seq->tail = pipeline;
        } 
        for (token = strtok_r(token0, "|", &saveptr); token != NULL; token = strtok_r(saveptr, "|", &saveptr))
        {     
            if(pipeline->i > MSH_MAXCMNDS)
            {
                err = MSH_ERR_TOO_MANY_CMDS;
                msh_pipeline_free(pipeline);
                return err;
            }
              
              
            //int i = 0;
            // struct msh_command *cmd = (struct msh_command*) malloc(sizeof(struct msh_command));
            struct msh_command *cmd =  calloc(1, sizeof(struct msh_command));
            if (cmd == NULL) 
            {
                err = MSH_ERR_NOMEM;
                msh_pipeline_free(pipeline);
                free(cmd);
                return err;
            } 

            if(cmd->count > MSH_MAXARGS)
            {
                err = MSH_ERR_TOO_MANY_ARGS;
                msh_pipeline_free(pipeline);
                free(cmd);
                return err;
            }
         //   cmd->program = strdup(token);

        
            for (token2 = strtok_r(token, " ",  &saveptr2); token2 != NULL; token2 = strtok_r(saveptr2, " ",  &saveptr2))
            {
                if(cmd->count > MSH_MAXARGS)
                {
                    err = MSH_ERR_TOO_MANY_ARGS;
                    msh_pipeline_free(pipeline);
                    free(cmd);
                    return err;
                }
                //printf("%s", token2);
                if(strcmp(token2, "&") == 0)
                {
                    pipeline->background = 1;
                }
                else 
                {
                    cmd->args[cmd->count] = strdup(token2);
                    cmd->count++;
                    cmd->data = NULL;
                    cmd->freefn = NULL;
                }
            }
            pipeline->c[pipeline->i] = cmd;
            pipeline->i++;
        }

        if (pipeline->i == 0) 
        {
            err = MSH_ERR_PIPE_MISSING_CMD;
            msh_pipeline_free(pipeline);
            return err;
        }
         
        seq->tail->c[pipeline->i-1] -> isLast = 1;
    }

    free(input);
        
	return 0;
}


/**
 * `msh_sequence_pipeline` dequeues the first pipeline in the sequence.
 *
 * - `@s` - the sequence we're querying
 * - `@return` - return a pointer to the zero-indexed `nth` command in
 *     the pipeline, or `NULL` if `nth` >= the number of commands in
 *     the pipeline. The caller of this function is passed the
 *     ownership for the pipeline, thus must free the pipeline.
 */
struct msh_pipeline *
msh_sequence_pipeline(struct msh_sequence *s)
{
	(void)s;

	if (s == NULL || s->head == NULL) 
    {
        return NULL;
    }
    struct msh_pipeline *pipeline = s->head;
    s->head = s->head->next;

    return pipeline;
}

/**
 * `msh_pipeline_command` queries a specific command in the pipeline.
 *
 * - `@p` - the pipeline we're querying
 * - `@nth` - for which command are we getting the command.
 * - `@return` - return a pointer to the zero-indexed `nth` command in
 *     the pipeline, or `NULL` if `nth` >= the number of commands in
 *     the pipeline. The caller of this function *borrows* the command
 *     (thus it will be freed when `p` is freed).
 */
struct msh_command *
msh_pipeline_command(struct msh_pipeline *p, size_t nth)
{
	(void)p;
	(void)nth;
   
	if (p == NULL || nth >= MSH_MAXCMNDS) 
    {
        return NULL;
    }
    return p->c[nth];

}

/**
 * `msh_pipeline_background` returns if a pipeline should be run in
 * the background or not. That is, if the shell should await
 * termination of the pipeline's processes, or should immediately
 * accept the next command.
 *
 * - `@p` - the pipeline in question
 * - `@return` - `1` if it should be run in the background, `0`
 *     otherwise.
 */
int
msh_pipeline_background(struct msh_pipeline *p)
{
    if(p == NULL)
    {
        return 0;
    }
    return p->background;
}

/**
 * `msh_command_final` tells us if the command `c` is the final
 * command in the pipeline, or not.
 *
 * - `@c` - the command for which we are querying its status.
 * - `@return` - `0` if it is *not* the final command, `1` otherwise.
 */
int
msh_command_final(struct msh_command *c)
{
    if(c == NULL)
    {
        return 0;
    }
    return(c->isLast);
}

/**
 * `msh_command_file_outputs` returns the files to which the standard
 * output and the standard error should be written, or `NULL` if
 * neither is specified.
 *
 * - `@c` - Command being queried.
 * - `@stdout` - return value to hold the file name to which to send
 *     the standard output of the command, or `NULL` if it should be
 *     passed down the pipeline.
 * - `@stderr` - same as for `stdout`, but for stadard error.
 */
void
msh_command_file_outputs(struct msh_command *c, char **stdout, char **stderr)
{
	(void)c;
	(void)stdout;
	(void)stderr;
	*stdout = NULL;
    *stderr = NULL;
    for (int i = 0; c->args[i] != NULL; i++) 
    {
        if (strcmp(c->args[i], "1>>") == 0 && c->args[i+1] != NULL) 
        {
            *stdout = c->args[i+1];
            c->variant = 1;
        } 
        else if (strcmp(c->args[i], "1>") == 0 && c->args[i+1] != NULL) 
        {
            *stdout = c->args[i+1];
            c->variant = 0;
        } 
        else if (strcmp(c->args[i], "2>") == 0 && c->args[i+1] != NULL) 
        {
            *stderr = c->args[i+1];
            c->variant = 0; 
        }
        else if (strcmp(c->args[i], "2>>") == 0 && c->args[i+1] != NULL) 
        {
            *stderr = c->args[i+1];
            c->variant = 1;
        }
    }
}

/**
 * `msh_command_program` retrieves the program to be executed for a
 * command.
 *
 * - `@c` - The command for which we retrieve the program.
 * - `@return` - The program, borrowed to the client
 */
char *
msh_command_program(struct msh_command *c)
{
	return c->args[0];
}

/**
 * `msh_command_args` retrieves the arguments for the command as a
 * `NULL`-terminated array of strings (see the `execv` argument list
 * as an example).
 *
 * - `@c` - the command for which to retrieve the arguments
 * - `@return` - the `NULL`-terminated array of arguments to the
 *     program, borrowed by the client. Array item `[0]` is the
 *     program.
 */
char **
msh_command_args(struct msh_command *c)
{
	return c->args;

}


/**
 * `msh_command_putdata` stores `data` with the command. If a
 * previous, different `data` value was passed in, it is freed.
 *
 * - `@c` - The command with which to store the data.
 * - `@data` - client's data that can be stored with a command.
 *     Ownership is passed to the data-structure. Thus, if the
 *     sequence is freed, then the client's data is passed to the
 *     `freefn`.
 * - `@freefn` - the function to be used to free `data`. It is called
 *     to free the `data` if 1. a new `data` value is passed in for a
 *     command, or 2. if the sequence is freed.
 */
void
msh_command_putdata(struct msh_command *c, void *data, msh_free_data_fn_t fn)
{
    if (c->data != NULL) 
    {
        c->freefn(c->data);
    }
    c->data = data;
    c->freefn = fn;
}

/**
 * `msh_command_getdata` returns the previously `put` value, or `NULL`
 * if no value was previously `put`.
 *
 * - `@c` - the command with the associated data.
 * - `@return` - the `data` associated with the command. These data is
 *     borrowed by the client.
 */
void *
msh_command_getdata(struct msh_command *c)
{
	return c->data;
}




   

