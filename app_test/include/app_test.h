/* ****************************************************************************************************
 * test application for unit tests
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

#ifndef _VA_APP_TEST_H_
#define _VA_APP_TEST_H_

#ifdef __cplusplus
extern "C" {
#endif


/* *******************************************************************
 * function declarations
 * ******************************************************************/

/* ************************************************************************//**
 * \brief	Initialization of the test runner application
 *
 * \param	void
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int app_test__init(void);

#ifdef __cplusplus
}
#endif

#endif /* _VA_APP_TEST_H_ */
