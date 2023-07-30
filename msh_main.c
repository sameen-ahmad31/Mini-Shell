#include <msh.h>
#include <msh_parse.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <malloc.h>

#include <linenoise.h>

char *
msh_input(void)
{
	char *line;

	/* You can change this displayed string to whatever you'd like ;-) */
	line = linenoise("msh > ");
	if (line && strlen(line) == 0) {
		free(line);

		return NULL;
	}
	if (line) linenoiseHistoryAdd(line);

	return line;
}

int
main(int argc, char *argv[])
{
	struct msh_sequence *s;

	if (argc > 1) {
		fprintf(stderr, "Usage: %s\n", argv[0]);

		return EXIT_FAILURE;
	}
	/*
	 * See `ln/README.markdown` for linenoise usage. If you don't
	 * see the `ln` directory, do a `make`.
	 */
	linenoiseHistorySetMaxLen(1<<16);

	msh_init();

	s = msh_sequence_alloc();
	if (s == NULL) {
		printf("MSH Error: Could not allocate msh sequence at initialization\n");
		return EXIT_FAILURE;
	}

	/* Lets keep getting inputs! */
	while (1) {
		char *str;
		struct msh_pipeline *p;
		msh_err_t err;

		str = msh_input();
		if (!str) break; /* you must maintain this behavior: an empty command exits */

		err = msh_sequence_parse(str, s);
		if (err != 0) {
			printf("MSH Error: %s\n", msh_pipeline_err2str(err));

			return err;
		}

		/* dequeue pipelines and sequentially execute them */
		while ((p = msh_sequence_pipeline(s)) != NULL) {
			msh_execute(p);
		}
		free(str);
	}

	msh_sequence_free(s);

	return 0;
}
