/*
 * This file is part of the EMBTOM project
 * Copyright (c) 2018-2019 Thomas Willetal 
 * (https://github.com/tom3333)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
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
#include <lib_ttyportmux.h>

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
	
	//ret = lib_ttyportmux__init(1, 1);
	//TEST_ASSERT_EQUAL_INT(EOK,ret);

	lib_ttyportmux__print(TTYSTREAM_control, "hello test 123\n");

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


