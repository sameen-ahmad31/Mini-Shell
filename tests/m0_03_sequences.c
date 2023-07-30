#include <sunit.h>
#include <msh_parse.h>

#include <string.h>

sunit_ret_t
seq(void)
{
	struct msh_sequence *s;
	struct msh_pipeline *p;
	struct msh_command *c;
	msh_err_t ret;

	s = msh_sequence_alloc();
	SUNIT_ASSERT("sequence allocation", s != NULL);
	ret = msh_sequence_parse("hello ; world here I ; am", s);
	SUNIT_ASSERT("3 commands in sequence parsed", ret == 0 && s != NULL);

	p = msh_sequence_pipeline(s);
	SUNIT_ASSERT("hello pipeline", p != NULL);
	c = msh_pipeline_command(p, 0);
	SUNIT_ASSERT("hello command", c != NULL);
	SUNIT_ASSERT("hello command program found", msh_command_program(c) != NULL);
	SUNIT_ASSERT("hello command program", strcmp(msh_command_program(c), "hello") == 0);
	msh_pipeline_free(p);

	p = msh_sequence_pipeline(s);
	SUNIT_ASSERT("world pipeline", p != NULL);
	c = msh_pipeline_command(p, 0);
	SUNIT_ASSERT("world command program found", msh_command_program(c) != NULL);
	SUNIT_ASSERT("world command program", strcmp(msh_command_program(c), "world") == 0);
	SUNIT_ASSERT("world command arg 1", strcmp(msh_command_args(c)[1], "here") == 0);
	SUNIT_ASSERT("world command arg 2", strcmp(msh_command_args(c)[2], "I") == 0);
	SUNIT_ASSERT("world command arg 3", msh_command_args(c)[3] == NULL);
	SUNIT_ASSERT("world command is final", msh_command_final(c));
	msh_pipeline_free(p);

	/* This should free the 3rd pipeline that hasn't been dequeued */
	msh_sequence_free(s);

	return SUNIT_SUCCESS;
}

sunit_ret_t
seq_pline(void)
{
	struct msh_sequence *s;
	struct msh_pipeline *p;
	struct msh_command *c, *c2;
	msh_err_t ret;

	s = msh_sequence_alloc();
	SUNIT_ASSERT("sequence allocation", s != NULL);
	ret = msh_sequence_parse("hello | world and universe ; hello | world and universe", s);
	SUNIT_ASSERT("2 cmd pipeline parsed", ret == 0 && s != NULL);

	p = msh_sequence_pipeline(s);
	SUNIT_ASSERT("hello pipeline", p != NULL);

	c = msh_pipeline_command(p, 0);
	SUNIT_ASSERT("hello command 0", c != NULL);
	SUNIT_ASSERT("hello command program found", msh_command_program(c) != NULL);
	SUNIT_ASSERT("hello command program", strcmp(msh_command_program(c), "hello") == 0);

	c2 = msh_pipeline_command(p, 1);
	SUNIT_ASSERT("hello command 1", c2 != NULL);
	SUNIT_ASSERT("world command program found", msh_command_program(c2) != NULL);
	SUNIT_ASSERT("world command program", strcmp(msh_command_program(c2), "world") == 0);
	SUNIT_ASSERT("world command arg 1", strcmp(msh_command_args(c2)[1], "and") == 0);
	SUNIT_ASSERT("world command arg 2", strcmp(msh_command_args(c2)[2], "universe") == 0);
	SUNIT_ASSERT("world command arg 3", msh_command_args(c2)[3] == NULL);

	SUNIT_ASSERT("hello command is not final", !msh_command_final(c));
	SUNIT_ASSERT("world command is final", msh_command_final(c2));
	msh_pipeline_free(p);

	p = msh_sequence_pipeline(s);
	SUNIT_ASSERT("hello pipeline", p != NULL);

	c = msh_pipeline_command(p, 0);
	SUNIT_ASSERT("hello command 0", c != NULL);
	SUNIT_ASSERT("hello command program found", msh_command_program(c) != NULL);
	SUNIT_ASSERT("hello command program", strcmp(msh_command_program(c), "hello") == 0);

	c2 = msh_pipeline_command(p, 1);
	SUNIT_ASSERT("hello command 1", c2 != NULL);
	SUNIT_ASSERT("world command program found", msh_command_program(c2) != NULL);
	SUNIT_ASSERT("world command program", strcmp(msh_command_program(c2), "world") == 0);
	SUNIT_ASSERT("world command arg 1", strcmp(msh_command_args(c2)[1], "and") == 0);
	SUNIT_ASSERT("world command arg 2", strcmp(msh_command_args(c2)[2], "universe") == 0);
	SUNIT_ASSERT("world command arg 3", msh_command_args(c2)[3] == NULL);

	SUNIT_ASSERT("hello command is not final", !msh_command_final(c));
	SUNIT_ASSERT("world command is final", msh_command_final(c2));
	msh_pipeline_free(p);

	msh_sequence_free(s);

	return SUNIT_SUCCESS;
}

int
main(void)
{
	struct sunit_test tests[] = {
		SUNIT_TEST("simple sequences", seq),
		SUNIT_TEST("sequence of pipelines", seq_pline),
		SUNIT_TEST_TERM
	};

	sunit_execute("sequences", tests);

	return 0;
}
