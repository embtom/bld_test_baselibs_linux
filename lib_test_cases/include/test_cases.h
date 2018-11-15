/* ****************************************************************************************************
 * test_cases_init.c within the following project: lib_thread
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	11.09.2018			Tom			- creation of test_cases.h
 *
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
