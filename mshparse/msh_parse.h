#pragma once

#include <stddef.h>

/***
 * Parse a string which is a pipeline of commands into a set of
 * commands, along with information about where their standard in,
 * error, and out should be connected.
 */

#include <msh.h>

/**
 * `msh_sequence_alloc` simply allocates a sequence structure which is
 * effectively a queue.
 */
struct msh_sequence *msh_sequence_alloc(void);

/**
 * `msh_pipeline_parse` takes the command string, parses it, and
 * inserts pipelines therein into the sequence queue.
 *
 * - `@str` - the string holding pipelines and commands. This function
 *     borrows this string, thus does not `free` it.
 * - `@s` - the sequence into which you can queue up pipelines.
 * - `@return` - return `0` on success (in which case `result` is
 *     set), or a `msg_err_t` otherwise.
 */
msh_err_t msh_sequence_parse(char *str, struct msh_sequence *s);

/**
 * `msh_sequence_free` deallocates the entire sequence, including all
 * constituent pipelines and commands. However, pipelines that have
 * been removed from the sequence (using `msh_sequence_pipeline`) are
 * no longer part of the sequence.
 *
 * - `s` - The sequence to free. Ownership is passed for `s`.
 */
void msh_sequence_free(struct msh_sequence *s);

/**
 * `msh_sequence_pipeline` dequeues the first pipeline in the sequence.
 *
 * - `@s` - the sequence we're querying
 * - `@return` - return a pointer to the zero-indexed `nth` command in
 *     the pipeline, or `NULL` if `nth` >= the number of commands in
 *     the pipeline. The caller of this function is passed the
 *     ownership for the pipeline, thus must free the pipeline.
 */
struct msh_pipeline *msh_sequence_pipeline(struct msh_sequence *s);

/**
 * `msh_pipeline_free` frees a pipeline.
 */
void msh_pipeline_free(struct msh_pipeline *p);

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
struct msh_command *msh_pipeline_command(struct msh_pipeline *p, size_t nth);

/**
 * `msh_pipeline_input` returns the string used as input for the
 * pipeline. Most useful when printing out the "jobs" builtin command
 * output.
 *
 * - `@p` - The borrowed pipeline for which we retrieve the input
 * - `@return` - the borrowed input used to create the pipeline
 */
char *msh_pipeline_input(struct msh_pipeline *p);

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
int msh_pipeline_background(struct msh_pipeline *p);

/**
 * `msh_command_final` tells us if the command `c` is the final
 * command in the pipeline, or not.
 *
 * - `@c` - the command for which we are querying its status.
 * - `@return` - `0` if it is *not* the final command, `1` otherwise.
 */
int msh_command_final(struct msh_command *c);

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
void msh_command_file_outputs(struct msh_command *c, char **stdout, char **stderr);

/**
 * `msh_command_program` retrieves the program to be executed for a
 * command.
 *
 * - `@c` - The command for which we retrieve the program.
 * - `@return` - The program, borrowed to the client
 */
char *msh_command_program(struct msh_command *c);

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
char **msh_command_args(struct msh_command *c);

/***
 * `msg_command_putdata` and `msh_command_getdata` are functions that
 * enable the shell to store some data for the command, and to
 * retrieve that data later.
 *
 * For example, if the shell wants to track the `pid` of
 * each command along with other data, it could:
 *
 * ```
 * struct proc_data {
 *     pid_t pid;
 *     // ...
 * };
 * // ...
 * struct proc_data *p = malloc(sizeof(struct proc_data));
 * *p = { .pid = child_pid, };
 * msh_command_putdata(c, p, free);
 * // later, when we want to find the process pid
 * if (msh_command_getdata(c)->pid == child_pid) {
 *     // ...
 * }
 * ```
 */

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
typedef void (*msh_free_data_fn_t)(void *);
void  msh_command_putdata(struct msh_command *c, void *data, msh_free_data_fn_t freefn);

/**
 * `msh_command_getdata` returns the previously `put` value, or `NULL`
 * if no value was previously `put`.
 *
 * - `@c` - the command with the associated data.
 * - `@return` - the `data` associated with the command. These data is
 *     borrowed by the client.
 */
void *msh_command_getdata(struct msh_command *c);
