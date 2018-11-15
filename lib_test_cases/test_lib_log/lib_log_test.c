/* ****************************************************************************************************
 * test_lib_list.c within the following project: lib_thread
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	06 April 2015			Tom			- creation of lib_list.c
 *  21 April 2015			Tom			- add of comments
 *
 */

/* *******************************************************************
 * includes
 * ******************************************************************/

/*c-runime */

/*frame*/
#include <lib_convention__errno.h>
#include <lib_log.h>

/*project*/
#include <embUnit.h>
#include <test_cases.h>
#include <test_lib_log.h>
#include <lib_tty_portmux.h>

/* *******************************************************************
 * Defines
 * ******************************************************************/
#define M_TEST_MODULE_NAME 		"test_lib_log"

/* *******************************************************************
 * Global Functions
 * ******************************************************************/

static void setUp(void)
{
}

static void tearDown(void)
{
}

static void test_lib_log_init(void)
{
	int ret;
	msg(LOG_LEVEL_error, M_TEST_MODULE_NAME, "Test info");
	
	//ret = lib_tty_portmux__init(1, 1);
	//TEST_ASSERT_EQUAL_INT(EOK,ret);

	lib_tty_portmux__print(TTY_STREAM_CONTROL, "hello test 123\n");

}

static TestRef lib_log_test(void)
{
	EMB_UNIT_TESTFIXTURES(fixtures){
		// TEST: test cases
		new_TestFixture("test_lib_log_init", test_lib_log_init),		
	};
	EMB_UNIT_TESTCALLER(lib_log__test, "lib_log__test", setUp, tearDown, fixtures);

	return (TestRef)&lib_log__test;
}

TEST_CASE_INIT(lib_log, test_lib_log__start , &lib_log_test);


