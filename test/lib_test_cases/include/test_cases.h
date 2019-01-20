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

#ifndef _TEST_CASES_H_
#define _TEST_CASES_H_

#ifdef __cplusplus
extern "C" {
#endif

/* *******************************************************************
 * includes
 * ******************************************************************/

/* frame */
#include <lib_list.h>
#include <embUnit.h>

/* *******************************************************************
 * defines
 * ******************************************************************/
#define M_COMMAND_NAME_LENGTH                  5


#define TEST_CASE_INIT(__test_name, __init_function, __getter_embunitTests)     				\
        static struct test_case_instance __test_name##_instance = { \
        		.node = {0}, 										\
				.name = #__test_name,        						\
				.embunitTest = __getter_embunitTests,				\
		};															\
		void init_##__init_function() {								\
			test_cases__register(&__test_name##_instance);			\
		}															\

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

typedef TestRef(*embunit_t)(void);

typedef struct test_case_instance{
    struct list_node node;
    embunit_t embunitTest;
	char *name;
    char command_name[M_COMMAND_NAME_LENGTH];
} test_case_instance_t;

/* *******************************************************************
 * global functions declaration
 * ******************************************************************/

/* *******************************************************************
 * \brief	Initialization of the subordinated test cases 
 * ---------
 * \remark  Start of the test cases startup hooks, necessary to register 
 *          at the superior "test_cases" framework 
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int test_cases__init(void);

/* *******************************************************************
 * \brief   register unit test at the superior "test_cases" framework
 * ---------
 * \remark  Each of the test cases have to be registerd to be accessiable
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int test_cases__register(test_case_instance_t *_test_case);

/* *******************************************************************
 * \brief   get a list of test cases
 * ---------
 * \remark  List of test cases is provided as lib_list queue
 * ---------
 * \return	pointer to list if successful, NULL if not
 * ******************************************************************/
struct queue_attr* test_cases__get_list(void);

#ifdef __cplusplus
}
#endif

#endif /* _LIB_MENU_H_ */
