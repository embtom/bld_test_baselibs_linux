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
#include <stdio.h>
//#include <sys/stdint.h>

/* system */
#include <lib_convention__errno.h>
#include <lib_convention__macro.h>
#include <lib_convention__mem.h>

/* frame */
#include <lib_thread.h>
#include <lib_menu.h>
#include <lib_ttyportmux.h>
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
static void app_test__print_ttydevices(void);
static void app_test__print_ttystreams(unsigned int *_stream_number);
static void app_test__get_ttydevices(unsigned int _device_idx, const struct ttyDeviceInfo **_ttydevice_info);
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

	ret = lib_ttyportmux__print(TTYSTREAM_control,"\n\n\nWelcome to the Test APP Manger\n\n");
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
	const struct ttyDeviceInfo *deviceInfo;
	struct ttyStreamMap mapping[TTYSTREAM_CNT];

	app_test__print_ttydevices();
	app_test__print_ttystreams(&stream_number);
	lib_ttyportmux__print(TTYSTREAM_control,"%u - Switch all streams\n",stream_number);
	
	stream_selector = lib_menu__get_int__decimal("Select tty stream to change");
	device_selector = lib_menu__get_int__decimal("Set tty device for selected stream");

	app_test__get_ttydevices(device_selector, &deviceInfo);
	lib_ttyportmux__get_stream_mapping(&mapping[0], sizeof(mapping));
	mapping[stream_selector].deviceType = deviceInfo->deviceType;
	lib_ttyportmux__set_stream_mapping(&mapping[0], sizeof(mapping));
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

	app_test__print_ttydevices();
	app_test__print_ttystreams(NULL);
}

static void app_test__print_ttydevices(void)
{
	int i;
	struct ttyDeviceInfo* deviceInfo;
	struct list_node *itr_node;
	struct list_node *first_node = lib_ttyportmux__get_ttydevice_listentry();

	lib_ttyportmux__print(TTYSTREAM_control,"\nList of available tty devices:\n");
	
	i = 0;
	lib__ttyportmux_listentry_foreach(itr_node, first_node)
	{
		deviceInfo = lib_ttyportmux__get_ttydevice_info(itr_node);
		lib_ttyportmux__print(TTYSTREAM_control,"%u - %s(%u)\n",i, deviceInfo->deviceName,deviceInfo->deviceIndex);
		i++;
	}
}

static void app_test__get_ttydevices(unsigned int _device_idx, const struct ttyDeviceInfo **_ttydevice_info)
{
	int ret, i;
	struct ttyDeviceInfo* deviceInfo;
	struct list_node *itr_node;
	struct list_node *first_node = lib_ttyportmux__get_ttydevice_listentry();

	i = 0;
	lib__ttyportmux_listentry_foreach(itr_node, first_node)
	{
		deviceInfo = lib_ttyportmux__get_ttydevice_info(itr_node);
		if (_device_idx == i) {
			*_ttydevice_info = deviceInfo;
			return;
		}
		i++;
	}
}

static void app_test__print_ttystreams(unsigned int *_stream_number)
{
	int i, stream_count, ret;
	struct ttyStreamInfo info;
	struct ttyStreamMap mapping[TTYSTREAM_CNT];
	struct ttyStreamInfo streamInfo;

	lib_ttyportmux__print(TTYSTREAM_control,"\nList of available tty devices:\n");
	lib_ttyportmux__get_stream_mapping(&mapping[0], sizeof(mapping));
	for(i = 0; i< sizeof(mapping)/sizeof(struct ttyStreamMap); i++) {
		lib_ttyportmux__get_stream_info(&mapping[i], &streamInfo);
		lib_ttyportmux__print(TTYSTREAM_control,"%u - %s - %s(%u)\n", i, streamInfo.streamName, streamInfo.deviceName, streamInfo.deviceIndex);

	} 

	if (_stream_number != NULL) {
		*_stream_number = lib_ttyportmux__get_stream_count();
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
	lib_ttyportmux__print(TTYSTREAM_control,"Start of Test Case %s\n",testContainer->menu_item.ident);
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
