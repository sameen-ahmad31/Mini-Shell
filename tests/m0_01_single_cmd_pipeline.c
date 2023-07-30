#include <sunit.h>
#include <msh_parse.h>

#include <string.h>

sunit_ret_t
mult_args(void)
{
	struct msh_sequence *s;
	struct msh_pipeline *p;
	struct msh_command *c;
	msh_err_t ret;
	char **args;

	s = msh_sequence_alloc();
	SUNIT_ASSERT("sequence allocation", s != NULL);
	ret = msh_sequence_parse("hey arg1 arg2 arg3", s);
	SUNIT_ASSERT("hey pipeline parsed", ret == 0 && s != NULL);
	p = msh_sequence_pipeline(s);
	SUNIT_ASSERT("hey found pipeline", p != NULL);
	c = msh_pipeline_command(p, 0);
	SUNIT_ASSERT("hey command 0", c != NULL);

	SUNIT_ASSERT("hey command program found", msh_command_program(c) != NULL);
	SUNIT_ASSERT("hey command program", strcmp(msh_command_program(c), "hey") == 0);
	args = msh_command_args(c);
	SUNIT_ASSERT("hey command args frommsh_command_args", args != NULL);
	SUNIT_ASSERT("hey command arg 0 in msh_command_args", args[0] != NULL && strcmp(args[0], "hey") == 0);
	SUNIT_ASSERT("hey command arg 1 in msh_command_args", args[1] != NULL && strcmp(args[1], "arg1") == 0);
	SUNIT_ASSERT("hey command arg 2 in msh_command_args", args[2] != NULL && strcmp(args[2], "arg2") == 0);
	SUNIT_ASSERT("hey command arg 3 in msh_command_args", args[3] != NULL && strcmp(args[3], "arg3") == 0);
	SUNIT_ASSERT("hey command arg 4 in msh_command_args", args[4] == NULL);
	SUNIT_ASSERT("hey command is final", msh_command_final(c));

	msh_pipeline_free(p);
	msh_sequence_free(s);

	return SUNIT_SUCCESS;
}

sunit_ret_t
one_arg(void)
{
	struct msh_sequence *s;
	struct msh_pipeline *p;
	struct msh_command *c;
	msh_err_t ret;
	char **args;

	s = msh_sequence_alloc();
	SUNIT_ASSERT("sequence allocation", s != NULL);
	ret = msh_sequence_parse("helloagain arg", s);
	SUNIT_ASSERT("helloagain pipeline parsed", ret == 0 && s != NULL);
	p = msh_sequence_pipeline(s);
	SUNIT_ASSERT("helloagain found pipeline", p != NULL);
	c = msh_pipeline_command(p, 0);
	SUNIT_ASSERT("helloagain command 0", c != NULL);

	SUNIT_ASSERT("helloagain command program found", msh_command_program(c) != NULL);
	SUNIT_ASSERT("helloagain command program", strcmp(msh_command_program(c), "helloagain") == 0);
	args = msh_command_args(c);
	SUNIT_ASSERT("helloagain command args frommsh_command_args", args != NULL);
	SUNIT_ASSERT("helloagain command arg 0 in msh_command_args", args[0] != NULL && strcmp(args[0], "helloagain") == 0);
	SUNIT_ASSERT("helloagain command arg 1 in msh_command_args", args[1] != NULL && strcmp(args[1], "arg") == 0);
	SUNIT_ASSERT("helloagain command arg 2 in msh_command_args", args[2] == NULL);
	SUNIT_ASSERT("helloagain command is final", msh_command_final(c));

	msh_pipeline_free(p);
	msh_sequence_free(s);

	return SUNIT_SUCCESS;
}

sunit_ret_t
no_args(void)
{
	struct msh_sequence *s;
	struct msh_pipeline *p;
	struct msh_command *c;
	msh_err_t ret;

	s = msh_sequence_alloc();
	SUNIT_ASSERT("sequence allocation", s != NULL);
	ret = msh_sequence_parse("hello", s);
	SUNIT_ASSERT("hello pipeline parsed", ret == 0 && s != NULL);
	p = msh_sequence_pipeline(s);
	SUNIT_ASSERT("hello found pipeline", p != NULL);
	c = msh_pipeline_command(p, 0);
	SUNIT_ASSERT("hello command 0", c != NULL);

	SUNIT_ASSERT("hello command program found", msh_command_program(c) != NULL);
	SUNIT_ASSERT("hello command program", strcmp(msh_command_program(c), "hello") == 0);
	SUNIT_ASSERT("hello command arg 0", strcmp(msh_command_args(c)[0], "hello") == 0);
	SUNIT_ASSERT("hello command arg 1", msh_command_args(c)[1] == NULL);
	SUNIT_ASSERT("hello command is final", msh_command_final(c));

	msh_pipeline_free(p);
	msh_sequence_free(s);

	return SUNIT_SUCCESS;
}

sunit_ret_t
parse(void)
{
	struct msh_sequence *s;
	struct msh_pipeline *p;
	msh_err_t ret;

	s = msh_sequence_alloc();
	SUNIT_ASSERT("sequence allocation", s != NULL);
	ret = msh_sequence_parse("hello", s);
	SUNIT_ASSERT("hello pipeline parsed", ret == 0 && s != NULL);
	p = msh_sequence_pipeline(s);
	SUNIT_ASSERT("hello pipeline found", p != NULL);

	msh_pipeline_free(p);
	msh_sequence_free(s);

	return SUNIT_SUCCESS;
}

int
main(void)
{
	struct sunit_test tests[] = {
		SUNIT_TEST("simply parse a command", parse),
		SUNIT_TEST("single command, no arguments", no_args),
		SUNIT_TEST("single command, one argument", one_arg),
		SUNIT_TEST("single command, multiple arguments", mult_args),
		SUNIT_TEST_TERM
	};

	sunit_execute("single command, various arguments", tests);

	return 0;
}
