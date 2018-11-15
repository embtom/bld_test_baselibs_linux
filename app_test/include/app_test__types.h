/* ****************************************************************************************************
 * lib_menu.c for console print out
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	07 Juli 2018	Tom				- creation of lib_console.c
 *
 */

/* *******************************************************************
 * includes
 * ******************************************************************/


#ifndef _VA_APP_TEST_APP_TEST__TYPES_H_
#define _VA_APP_TEST_APP_TEST__TYPES_H_

#ifdef __cplusplus
extern "C" {
#endif


/* *******************************************************************
 * includes
 * ******************************************************************/

/* project */
#include <test_cases.h>

/* *******************************************************************
 * defines
 * ******************************************************************/
#define M_APP_TEST__EMBUNIT_INITIALZER(__name, __cmd,__ident, __cb)	\
{															\
	.name = __name,											\
	.menu_item = M_LIB_MENU__LIST_INITIALZER(__cmd, __ident, &app_test__menu_cb_trigger), \
	.embu_test = __cb		\
}

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

// configuration attribute, which defines the availability of tests
struct embunitTestContainer {
	const char* name;
	struct lib_menu__item menu_item;
	embunit_t embunitTests;
};

#ifdef __cplusplus
}
#endif

#endif /* _VA_APP_TEST_APP_TEST__TYPES_H_ */
