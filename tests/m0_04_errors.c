#include <sunit.h>
#include <msh_parse.h>

#include <string.h>

sunit_ret_t
nocmd(void)
{
	/* add your own tests here. */

	return 0;
}

sunit_ret_t
too_many_cmd(void)
{
	/* add your own tests here. */

	return 0;
}

sunit_ret_t
too_many_args(void)
{
	/* add your own tests here. */

	return 0;
}

int
main(void)
{
	struct sunit_test tests[] = {
		SUNIT_TEST("pipeline with no command after |", nocmd),
		SUNIT_TEST("pipeline with no command before |", nocmd),
		SUNIT_TEST("too many commands", too_many_cmd),
		SUNIT_TEST("too many args", too_many_args),
		/* add your own tests here... */
		SUNIT_TEST_TERM
	};

	sunit_execute("Testing edge cases and errors", tests);

	return 0;
}
