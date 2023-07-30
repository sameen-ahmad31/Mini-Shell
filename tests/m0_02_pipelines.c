#include <sunit.h>
#include <msh_parse.h>

#include <string.h>

sunit_ret_t
two_cmds_arg(void)
{
	struct msh_sequence *s;
	struct msh_pipeline *p;
	struct msh_command *c, *c2;
	msh_err_t ret;

	s = msh_sequence_alloc();
	SUNIT_ASSERT("sequence allocation", s != NULL);
	ret = msh_sequence_parse("hello | world and universe", s);
	SUNIT_ASSERT("2 cmd pipeline parsed", ret == 0 && s != NULL);
	p = msh_sequence_pipeline(s);
	SUNIT_ASSERT("hello pipeline", p != NULL);
	c = msh_pipeline_command(p, 0);
	SUNIT_ASSERT("hello command 0", c != NULL);
	c2 = msh_pipeline_command(p, 1);
	SUNIT_ASSERT("hello command 1", c2 != NULL);

	SUNIT_ASSERT("hello command program found", msh_command_program(c) != NULL);
	SUNIT_ASSERT("hello command program", strcmp(msh_command_program(c), "hello") == 0);

	SUNIT_ASSERT("world command program found", msh_command_program(c2) != NULL);
	SUNIT_ASSERT("world command program", strcmp(msh_command_program(c2), "world") == 0);

	SUNIT_ASSERT("hello command arg 1", msh_command_args(c)[1] == NULL);

	SUNIT_ASSERT("world command arg 1", strcmp(msh_command_args(c2)[1], "and") == 0);
	SUNIT_ASSERT("world command arg 2", strcmp(msh_command_args(c2)[2], "universe") == 0);
	SUNIT_ASSERT("world command arg 3", msh_command_args(c2)[3] == NULL);

	SUNIT_ASSERT("hello command is not final", !msh_command_final(c));
	SUNIT_ASSERT("world command is final", msh_command_final(c2));

	msh_pipeline_free(p);
	msh_sequence_free(s);

	return SUNIT_SUCCESS;
}

sunit_ret_t
two_cmds_noargs(void)
{
	struct msh_sequence *s;
	struct msh_pipeline *p;
	struct msh_command *c, *c2;
	msh_err_t ret;

	s = msh_sequence_alloc();
	SUNIT_ASSERT("sequence allocation", s != NULL);
	ret = msh_sequence_parse("hello | world", s);
	SUNIT_ASSERT("2 cmd pipeline parsed", ret == 0 && s != NULL);
	p = msh_sequence_pipeline(s);
	SUNIT_ASSERT("hello pipeline", p != NULL);
	c = msh_pipeline_command(p, 0);
	SUNIT_ASSERT("hello command 0", c != NULL);
	c2 = msh_pipeline_command(p, 1);
	SUNIT_ASSERT("hello command 1", c2 != NULL);

	SUNIT_ASSERT("hello command program found", msh_command_program(c) != NULL);
	SUNIT_ASSERT("hello command program", strcmp(msh_command_program(c), "hello") == 0);

	SUNIT_ASSERT("world command program found", msh_command_program(c2) != NULL);
	SUNIT_ASSERT("world command program", strcmp(msh_command_program(c2), "world") == 0);

	SUNIT_ASSERT("hello command arg 1", msh_command_args(c)[1] == NULL);
	SUNIT_ASSERT("world command arg 1", msh_command_args(c2)[1] == NULL);

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
		SUNIT_TEST("two commands, no arguments", two_cmds_noargs),
		SUNIT_TEST("two commands, one with arguments", two_cmds_arg),
		SUNIT_TEST_TERM
	};

	sunit_execute("pipeline of commands, various arguments", tests);

	return 0;
}
