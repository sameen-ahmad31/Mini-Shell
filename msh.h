#pragma once

/* Maximum number of background pipelines */
#define MSH_MAXBACKGROUND 16
/* each command can have MSH_MAXARGS or fewer arguments */
#define MSH_MAXARGS  16
/* each pipeline has MSH_MAXCMNDS or fewer commands */
#define MSH_MAXCMNDS 16

/**
 * A sequence of pipelines. Pipelines are separated by ";"s, enabling
 * a sequence to define a sequence of pipelines that execute one after
 * the other. A pipeline can run in the background, which enables us
 * to move on an execute the next pipeline.
 */
struct msh_sequence;

/**
 * A pipeline is a sequence of commands, separated by "|"s. The output
 * of a preceding command (before the "|") gets passed to the input of
 * the next (after the "|").
 */
struct msh_pipeline;

/**
 * Each command corresponds to either a program (in the `PATH`
 * environment variable, see `echo $PATH`), or a builtin command like
 * `cd`. Commands are passed arguments.
 */
struct msh_command;

/**
 * `msh_err_t` are the standard errors returned by many functions in
 * the API. Each error has a description below.
 */
typedef enum {
	/* pipeline has a redirection to multiple files, e.g. "cmd 1> a.txt b.txt" */
	MSH_ERR_REDIRECTED_TO_TOO_MANY_FILES = -1,
	/* pipeline has multiple redirections from same fd, e.g. "cmd 1> a.txt 1> b.txt" */
	MSH_ERR_MULT_REDIRECTIONS = -2,
	/* pipeline has multiple &s, or & not in last character, e.g. "cmd & &", or "cmd & " */
	MSH_ERR_MISUSED_BACKGROUND = -3,
	/* pipeline has a redirection *without* a file to redirect to. */
	MSH_ERR_NO_REDIR_FILE = -4,
	/* pipeline processes ran out of memory */
	MSH_ERR_NOMEM = -5,
	/* More than MSH_MAXARGS passed to a command */
	MSH_ERR_TOO_MANY_ARGS = -6,
	/* More than MSH_MAXCMNDS in a pipeline */
	MSH_ERR_TOO_MANY_CMDS = -7,
	/* Pipe either does not have a preceding command or a following command */
	MSH_ERR_PIPE_MISSING_CMD = -8,
	/* Could not execute program in command */
	MSH_ERR_NO_EXEC_PROG = -9,
	/* Provided both a pipe to the next command *and* a stdout redirection  */
	MSH_ERR_REDUNDANT_PIPE_REDIRECTION = -10,
	/*
	 * A pipeline in a sequence has a redirection and/or
	 * background, yet is missing a command
	 */
	MSH_ERR_SEQ_REDIR_OR_BACKGROUND_MISSING_CMD = -11,
	/* The sequence still has pipelines, cannot add more  */
	MSH_ERR_SEQ_BUSY = -12,
} msh_err_t;

/* Return a human-readable string corresponding to an msh error */
static char *
msh_pipeline_err2str(msh_err_t e)
{
	char *strs[] = {
		"Success",
		"Redirected to too many files",
		"Multiple redirections from the same descriptor",
		"Misused background (&) specification",
		"Redirection specified without a file to redirect to",
		"Could not allocate memory",
		"Too many arguments to command",
		"Too many pipeline commands",
		"Pipe with missing command",
		"Could not execute program",
		"Attempted to redirect output to pipe and to file redirection",
		"A pipeline has a redirection or &, but no command",
		"Attempted to parse into sequence, when it still has pipelines"
	};

	return strs[-e];
}

/**
 * `msh_init` is called on initialization. You can place anything
 * you'd like here, but for M2, you'll likely want to set up signal
 * handlers here.
 */
void msh_init(void);

/**
 * `msh_execute` is called with the parsed pipeline for the shell to
 * execute. If the pipeline doesn't run in the background, this will
 * only return after the pipeline completes.
 */
void msh_execute(struct msh_pipeline *p);
