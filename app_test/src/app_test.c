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
 * 	27 Juli 2018	Tom				- creation of app_test.c
 *
 */

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdio.h>
//#include <sys/stdint.h>

/* system */
#include <lib_convention__errno.h>
#include <lib_convention__macro.h>
#include <lib_convention__mem.h>

/* frame */
#include <lib_thread.h>
#include <lib_menu.h>
#include <lib_tty_portmux.h>
#include <test_cases.h>
#include <mini-printf.h>

/* project */
#include "app_test__types.h"

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static int app_test__setup_tty_portmux_menu(void);
static void app_test__menu_item_settty(struct lib_menu__item *_item);
static void app_test__menu_item_gettty(struct lib_menu__item *_item);
static void app_test__print_tty_devices(void);
static void app_test__print_tty_streams(unsigned int *_stream_number);
static void app_test__get_tty_devices(unsigned int _device_idx, const struct device_info **_device_info);
static void app_test__menu_cb_trigger(struct lib_menu__item *_item);


void *app_test__load_worker(void *_p);

/* *******************************************************************
 * function definition
 * ******************************************************************/

/* ************************************************************************//**
 * \brief	Initialization of the test runner application
 *
 * \param	void
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
int app_test__init(void)
{
	int ret, port_number;
	unsigned cnt;
	struct queue_attr *testCase_list;
	struct list_node *testCase_node;
	struct test_case_instance *testInstance;
	struct embunitTestContainer *testContainer;

	ret = test_cases__init();
	if (ret < EOK) {
		return ret;
	}
	
	testCase_list = test_cases__get_list();
	if (testCase_list == NULL) {
		return -ESTD_NODEV;
	}

	ret = lib_menu__init();
	if(ret < EOK) {
		return ret;
	}

	ret = lib_list__get_begin(testCase_list ,&testCase_node, 0, NULL);
	if (ret < EOK) {
		return -ESTD_FAULT;
	}

	ret = app_test__setup_tty_portmux_menu();
	if (ret < EOK) {
		return ret;
	}

	ret = lib_tty_portmux__print(TTY_STREAM_CONTROL,"\n\n\nWelcome to the Test APP Manger\n\n");
	if (ret < EOK) {
		return ret;
	}

	cnt = 0;
	do {
		testInstance = (struct test_case_instance*)GET_CONTAINER_OF(testCase_node, struct test_case_instance, node);
		testContainer = (struct embunitTestContainer*)alloc_memory(1, sizeof(struct embunitTestContainer ));
		if(testContainer == NULL) {
			/* todo error handling */

		}

		mini_snprintf(&testInstance->command_name[0],M_COMMAND_NAME_LENGTH,"%u\0",cnt);
		testContainer->menu_item.cmd= &testInstance->command_name[0];
		testContainer->menu_item.ident = testInstance->name;
		testContainer->menu_item.cb = &app_test__menu_cb_trigger;
		testContainer->embunitTests = testInstance->embunitTest;
		lib_menu__add_item(&testContainer->menu_item);
		cnt++;
	}while(ret = lib_list__get_next(testCase_list,&testCase_node, 0, NULL), (ret == LIB_LIST__EOK));

	return EOK;
}

/* *******************************************************************
 * static function definitions
 * ******************************************************************/

/* ************************************************************************//**
 * \brief	Setup of the lib_menu entry to multiplex tty output channels
 *
 * \param	void
 * \return	EOK if successful, or negative errno value on error
 * ****************************************************************************/
static int app_test__setup_tty_portmux_menu(void)
{
	int i, ret;
	struct lib_menu__item s_port_selector_menu_entry[] = {  \
			M_LIB_MENU__LIST_INITIALZER("settty\0", "Set TTY device \0", &app_test__menu_item_settty),  \
			M_LIB_MENU__LIST_INITIALZER("gettty\0", "Get TTY device \0", &app_test__menu_item_gettty)  \
	};

	for (ret = EOK, i = 0; i < sizeof(s_port_selector_menu_entry)/sizeof(*s_port_selector_menu_entry);i++) {
		ret |= lib_menu__add_item(&s_port_selector_menu_entry[i]);
	}

	if (ret < EOK) {
		return ret;
	}

	return EOK;
}

/* ************************************************************************//**
 * \brief	Processing of lib_menu entry "Settty"
 *
 * \param	void
 * \return	void
 * ****************************************************************************/
static void app_test__menu_item_settty(struct lib_menu__item *_item)
{
	int ret;
	unsigned int stream_number, stream_selector, device_selector;
	const struct device_info *device_info;

	app_test__print_tty_devices();
	app_test__print_tty_streams(&stream_number);
	lib_tty_portmux__print(TTY_STREAM_CONTROL,"%u - Switch all streams\n",stream_number);
	
	stream_selector = lib_menu__get_int__decimal("Select tty stream to change");
	device_selector = lib_menu__get_int__decimal("Set tty device for selected stream");

	app_test__get_tty_devices(device_selector, &device_info);
	ret = lib_tty_portmux__set_stream_device((enum tty_stream)stream_selector, device_info);

}

/* ************************************************************************//**
 * \brief	Processing of lib_menu entry "Gettty"
 *
 * \param	void
 * \return	void
 * ****************************************************************************/
static void app_test__menu_item_gettty(struct lib_menu__item *_item)
{
	int ret;
	unsigned int i;
	
	char *device_name;

	app_test__print_tty_devices();
	app_test__print_tty_streams(NULL);
}

static void app_test__print_tty_devices(void)
{
	int i, ret;
	device_list_t device_node_hdl = NULL;
	const struct device_info *device_info;

	lib_tty_portmux__print(TTY_STREAM_CONTROL,"\nList of available tty devices:\n");
	ret = EOK;
	i=0;
	do{
		ret = lib_tty_portmux__get_device_info(&device_node_hdl, &device_info);
		if (ret == EOK) {
			lib_tty_portmux__print(TTY_STREAM_CONTROL,"%u - %s(%u)\n",i, device_info->device_type_name,device_info->device_index);
			i++;
		}
	}while (ret == EOK);
}

static void app_test__get_tty_devices(unsigned int _device_idx, const struct device_info **_device_info)
{
	int ret, i;
	int device_number;
	device_list_t device_node_hdl = NULL;
	const struct device_info *device_info;

	device_number = lib_tty_portmux__get_device_number();
	for(i=0; i < device_number; i++) 
	{
		ret = lib_tty_portmux__get_device_info(&device_node_hdl, &device_info);
		if(ret != EOK) {
			return;
		}
		if (_device_idx == i) {
			*_device_info = device_info;
			return;
		}
	}
}


static void app_test__print_tty_streams(unsigned int *_stream_number)
{
	int i, stream_count, ret;
	struct stream_info info;

	lib_tty_portmux__print(TTY_STREAM_CONTROL,"\nList of available tty devices:\n");

	ret = lib_tty_portmux__get_stream_number();
	if(ret < EOK) {
		lib_tty_portmux__print(TTY_STREAM_CONTROL,"No device found error with %i\n",ret);
		return ;
	}

	stream_count = ret;
	for(i = 0; i < stream_count; i++) {
		ret = lib_tty_portmux__get_stream_info((enum tty_stream)i, &info);
		if (ret < EOK) {
			lib_tty_portmux__print(TTY_STREAM_CONTROL,"Device info request failed %i\n",ret);
		}
		lib_tty_portmux__print(TTY_STREAM_CONTROL,"%u - %s - %s(%u)\n", i, info.stream_name, info.device_type_name, info.device_index);
	}

	if (_stream_number != NULL) {
		*_stream_number = i;
	}
}

/* ************************************************************************//**
 * \brief	Processing of the requested test case
 *
 * \param	void
 * \return	void
 * ****************************************************************************/
static void app_test__menu_cb_trigger(struct lib_menu__item *_item)
{
	struct embunitTestContainer *testContainer = (struct embunitTestContainer *)GET_CONTAINER_OF(_item, struct embunitTestContainer, menu_item);
	embunit_t embunitTests;
	lib_tty_portmux__print(TTY_STREAM_CONTROL,"Start of Test Case %s\n",testContainer->name);
	if(testContainer->embunitTests != NULL) {
		embunitTests = testContainer->embunitTests; 
		TestRunner_start();
    	TestRunner_runTest((*embunitTests)());
    	TestRunner_end();
	}
}

/* ************************************************************************//**
 * \brief	load thread. Please modify values during runtime in
 * 			case load shall be decreased or increasd
 *
 * \remark	important notes on the function, its usage or the pre - emptive
 * 			capability
 *
 * \param [in,out]	*_p
 * \return	EOK if successful, or negative errno value on error
 *			- custom errno values:
 *				+ '-'
 *			- system errno sources:
 *				+ 'list of system functions-	[if necessary]'
 *			- lib errno sources:
 *				+ 'list of library functions-	[if necessary]'
 * ****************************************************************************/
void *app_test__load_worker(void *_p)
{
	/* automatic variables */

	/* executable statements */
	while (1) {

		/* automatic variables */
		static volatile uint32_t s_app_test__cycle_time_ms = 10;
		static volatile uint32_t s_app_test__running_period_us = 3000;

		/* executable statements */

		/* first sleep */
		lib_thread__msleep(s_app_test__cycle_time_ms);

		/* the, do something (just burn CPU resources) */
	//	lib_module_time__delay_us(s_app_test__running_period_us);
	}
	return NULL;
}
