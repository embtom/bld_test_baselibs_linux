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

/* c-runtime */
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* system */

/* framework */
#include <test_cases.h>
#include <lib_clock.h>
#include <embUnit.h>
#include <lib_log.h>
#include <lib_convention__errno.h>
#include <lib_timer.h>
#include <lib_thread.h>
#include <lib_clock.h>

#ifdef __gnu_linux__
	#include <sched.h>		// POSIX scheduling parameter functions and definitions
	#include <pthread.h>
	#include <unistd.h>
	#include <fcntl.h>			// open()
	#include <sys/syscall.h>	// SYS_gettid
#endif

/* local libs */
#include "test_lib_timer.h"

/* *******************************************************************
 * Defines
 * ******************************************************************/
#define M_APP_LIB_TIMER_TEST_ID 		"APP_TIM_TST"


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/



/* *******************************************************************
 * Static Variables
/* *******************************************************************/
static timer_hdl_t test_timer_1;
static timer_hdl_t test_timer_2;
static timer_hdl_t test_timer_3;

static thread_hdl_t wakeup_worker_th;
static unsigned int wakeup_worker_running = 0;

/* *******************************************************************
 * Static Function Prototypes
 * ******************************************************************/
static void test_timer__callback_1(timer_hdl_t _hdl, void* _arg);
static void test_timer__callback_2(timer_hdl_t _hdl, void* _arg);
static void* test_lib_timer__wakeup (void *_arg);

/* *******************************************************************
 * Static Functions
 * ******************************************************************/
static void setUp(void)
{
    int ret;

}

static void tearDown(void)
{
    int ret;

}

static void test_lib_timer__open(void)
{
    int i;
    int ret;

    //ret = lib_timer__init();
    //TEST_ASSERT_EQUAL_INT(EOK, ret);

    ret = lib_timer__open(NULL,NULL, &test_timer__callback_1);
    TEST_ASSERT_EQUAL_INT(-EPAR_NULL, ret);

    ret = lib_timer__open(&test_timer_1, NULL, &test_timer__callback_1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    ret = lib_timer__open(&test_timer_2, NULL, &test_timer__callback_1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    ret = lib_timer__close(&test_timer_1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    //ret = lib_timer__cleanup();
    //TEST_ASSERT_EQUAL_INT(-ESTD_BUSY, ret);

    ret = lib_timer__close(&test_timer_2);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    //ret = lib_timer__cleanup();
    //TEST_ASSERT_EQUAL_INT(EOK, ret);
}

static void test_lib_timer__start(void)
{
	uint32_t timestamp_timer_1;
	uint32_t timestamp_timer_2;

    int ret;
    TEST_INFO("test_lib_timer__start\n");

//    ret = lib_timer__init();
//    TEST_ASSERT_EQUAL_INT(EOK, ret);

    ret = lib_timer__open(&test_timer_1, &timestamp_timer_1, &test_timer__callback_1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    ret = lib_timer__open(&test_timer_3, &timestamp_timer_2, &test_timer__callback_2);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    timestamp_timer_1 = lib_clock__get_time_ms();
    ret = lib_timer__start(test_timer_1,1000);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    lib_thread__msleep(2000);

    timestamp_timer_1 = lib_clock__get_time_ms();
    ret = lib_timer__start(test_timer_1,1000);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    timestamp_timer_2 = lib_clock__get_time_ms();
    ret = lib_timer__start(test_timer_3,500);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    lib_thread__msleep(2000);

    timestamp_timer_1 = lib_clock__get_time_ms();
    ret = lib_timer__start(test_timer_1,1000);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    lib_thread__msleep(2000);
    ret = lib_timer__close(&test_timer_1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    lib_thread__msleep(2000);
    ret = lib_timer__open(&test_timer_1,&timestamp_timer_1, &test_timer__callback_1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    timestamp_timer_1 = lib_clock__get_time_ms();
    ret = lib_timer__start(test_timer_1,2000);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    lib_thread__msleep(2500);

    ret = lib_timer__close(&test_timer_1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    ret = lib_timer__close(&test_timer_3);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

   // ret = lib_timer__cleanup();
   // TEST_ASSERT_EQUAL_INT(EOK, ret);
}

static void test_lib_timer__resume(void)
{
	int ret;
	uint32_t timestamp_timer_1;

	TEST_INFO("%s\n",__func__);

//    ret = lib_timer__init();
//    TEST_ASSERT_EQUAL_INT(EOK, ret);

	ret = lib_timer__open(&test_timer_1, &timestamp_timer_1, &test_timer__callback_1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    timestamp_timer_1 = lib_clock__get_time_ms();
    ret = lib_timer__start(test_timer_1,1000);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    lib_thread__msleep(500);

    ret = lib_timer__stop(test_timer_1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    lib_thread__msleep(2000);

    ret = lib_timer__resume(test_timer_1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    lib_thread__msleep(3000);

    ret = lib_timer__close(&test_timer_1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

   // ret = lib_timer__cleanup();
   // TEST_ASSERT_EQUAL_INT(EOK, ret);
}

static void test_lib_timer__test_wakeup(void)
{
	int ret;
	wakeup_worker_running = 1;

//    ret = lib_timer__init();
//    TEST_ASSERT_EQUAL_INT(EOK, ret);

	ret = lib_timer__open(&test_timer_1, NULL, NULL);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    ret = lib_timer__start(test_timer_1, 500);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

	ret = lib_thread__create(&wakeup_worker_th,&test_lib_timer__wakeup,(void*)&test_timer_1, 0, "test_lib_timer_wakeup_worker");
	TEST_ASSERT_EQUAL_INT(EOK,ret);

}

static void test_lib_timer__test_duration(void)
{
    int ret;
	lib_thread__msleep(10000);

    ret = lib_timer__stop(test_timer_1);
    TEST_ASSERT_EQUAL_INT(EOK,ret);

    ret = lib_timer__start(test_timer_1, 1000);
    TEST_ASSERT_EQUAL_INT(EOK,ret);

    lib_thread__msleep(10000);
}

static void test_lib_timer__test_wakeup_cleanup(void)
{
	int ret;

    ret = lib_timer__stop(test_timer_1);
    TEST_ASSERT_EQUAL_INT(EOK,ret);

    ret = lib_timer__close(&test_timer_1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    lib_thread__msleep(3000);

  //  ret = lib_timer__cleanup();
  //  TEST_ASSERT_EQUAL_INT(EOK, ret);

    ret = lib_thread__join(&wakeup_worker_th, NULL);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

}

static void test_timer__callback_1(timer_hdl_t _hdl, void* _arg)
{
	uint32_t timediff;
	uint32_t *timestamp_timer = (uint32_t*)_arg;

	if (timestamp_timer != NULL) {
		timediff = lib_clock__get_time_since_ms(*timestamp_timer);
	}

    TEST_INFO("Timer 1 callback called %u\n",timediff);
    fflush(stdout);
}

static void test_timer__callback_2(timer_hdl_t _hdl, void* _arg)
{
	uint32_t timediff;
	uint32_t *timestamp_timer = (uint32_t*)_arg;

	if (timestamp_timer != NULL) {
		timediff = lib_clock__get_time_since_ms(*timestamp_timer);
	}
    TEST_INFO("Timer 2 callback called %u\n",timediff);
    fflush(stdout);
}

static void* test_lib_timer__wakeup (void *_arg)
{
	int ret;
	uint32_t timestamp, time_diff;
	timer_hdl_t *wakeup_timer = (timer_hdl_t *)_arg;

	timestamp = lib_clock__get_time_ms();

	while (wakeup_worker_running)
	{
        ret = lib_timer__wakeup_wait(*wakeup_timer);
        if (ret < EOK) {
            TEST_INFO("Wakeup_wait error occured with %i\n",ret);
            return NULL;
        }


		time_diff = lib_clock__get_time_since_ms(timestamp);

		timestamp = lib_clock__get_time_ms();
		if(time_diff > 500) {
			TEST_INFO("Wakeup_interval exceeded with %i\n",time_diff);
		}
		else {
			TEST_INFO("Wakeup %i ret %i \n",time_diff, ret);
		}
	}

	TEST_INFO("Wakeup interval error %i\n",ret);
	return NULL;
}


/* *******************************************************************
 * Global Functions
/* *******************************************************************/
static TestRef lib_timer_test(void) {
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture("test_lib_timer__open", test_lib_timer__open),
        new_TestFixture("test_lib_timer_start", test_lib_timer__start),
		new_TestFixture("test_lib_timer__resume", test_lib_timer__resume),
		new_TestFixture("test_lib_timer__test_wakeup", test_lib_timer__test_wakeup),
		new_TestFixture("test_lib_timer__test_duration", test_lib_timer__test_duration),
		new_TestFixture("test_lib_timer__test_wakeup_cleanup", test_lib_timer__test_wakeup_cleanup),
    };
    EMB_UNIT_TESTCALLER(lib_timer_test, "test_lib_timer_test", setUp, tearDown, fixtures);

    return (TestRef)&lib_timer_test;
}

TEST_CASE_INIT(lib_timer,test_lib_timer__start, &lib_timer_test)





