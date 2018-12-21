/* ****************************************************************************************************
 * lib_thread.c within the following project: lib_thread
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	06 April 2015			Tom			- creation of lib_thread.c
 *  21 April 2015			Tom			- add of comments
 *
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
#include <embUnit.h>
#include <lib_thread.h>
#include <lib_gpio.h>
#include <lib_gpio_config.h>
#include <lib_convention__errno.h>

/* local libs */
#include "test_lib_gpio.h"

/* *******************************************************************
 * Defines
 * ******************************************************************/
#define M_APP_LIB_GPIO_TEST_ID 		"APP_GPIO_TST"


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

/* *******************************************************************
 * Static Variables
/* *******************************************************************/
#ifdef LIB_GPIO_STM32
static  struct gpio_access s_pin_config_table[] = 
{
	{
		.pinId = 0,
		.pinPort = GPIOA,
        .pinNumber = 5
	},
    {
		.pinId = 1,
		.pinPort = GPIOC,
        .pinNumber = 0
	}
}; 
#elif LIB_GPIO_LINUX
static  struct gpio_access s_pin_config_table[] = 
{
	{
		.pinId = 0,
		.pinPhys = 17
	},
	{
		.pinId = 1,
		.pinPhys = 18
	}
}; 
#else
    #error "NO LIB_GPIO_CONFIG is set"
#endif



/* *******************************************************************
 * Static Function Prototypes
 * ******************************************************************/
static void test_gpio_event_handler(void *const _arg);
static void* test_gpio_toggle_worker(void* _arg);

/* *******************************************************************
 * Static Variables
 * ******************************************************************/
static volatile unsigned int event_count = 0;

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

static void test_lib_gpio__config(void)
{
    int ret;

    ret = lib_gpio__set_config_table(NULL,1);
    TEST_ASSERT_EQUAL_INT(-ESTD_INVAL, ret);

    ret = lib_gpio__set_config_table(&s_pin_config_table[0],0);
    TEST_ASSERT_EQUAL_INT(-ESTD_INVAL, ret);

    ret = lib_gpio__set_config_table(&s_pin_config_table[0], sizeof(s_pin_config_table)/sizeof(struct gpio_access));
    TEST_ASSERT_EQUAL_INT(EOK, ret);
}

static gpio_hdl_t pin2;

static void test_lib_gpio__toggle(void)
{
    gpio_hdl_t pin1;
    thread_hdl_t toggleThd;
    
    struct gpio_open_args openArgsOut = {
        .mode = GPIO_MODE_output_pp,
        .pull_mode = GPIO_PULL_no,
        .event_handler = NULL,
    };

    struct gpio_open_args openArgsIn = {
        .mode = GPIO_MODE_event_rising_falling,
        .pull_mode = GPIO_PULL_up,
        .event_handler = test_gpio_event_handler,
        .event_arg = (void*)&pin2,
    };

    int ret;
    event_count = 0;
    
    ret = lib_gpio__open(&pin1,0,&openArgsOut);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    ret = lib_gpio__open(&pin2,1,&openArgsIn);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    ret = lib_thread__create(&toggleThd,&test_gpio_toggle_worker,(void*)pin1,2,"gpio_toggle_worker");
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    ret = lib_thread__join(&toggleThd, NULL);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

    ret = lib_gpio__close(&pin1);
    TEST_ASSERT_EQUAL_INT(EOK, ret);
    
    ret = lib_gpio__close(&pin2);
    TEST_ASSERT_EQUAL_INT(EOK, ret);

}
/* *******************************************************************
 * Global Functions
/* *******************************************************************/
static TestRef lib_gpio_test(void) {
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture("test_lib_gpio__config", test_lib_gpio__config),
        new_TestFixture("test_lib_gpio__toggle", test_lib_gpio__toggle)
    };
    EMB_UNIT_TESTCALLER(lib_gpio_test, "test_lib_gpio_test", setUp, tearDown, fixtures);

    return (TestRef)&lib_gpio_test;
}

TEST_CASE_INIT(lib_gpio,test_lib_gpio__start, &lib_gpio_test)




static void test_gpio_event_handler(void *const _arg)
{
    gpio_hdl_t* pin = (gpio_hdl_t*)_arg;
    unsigned int value;

    lib_gpio__read(*pin, &value);
    TEST_INFO("Event val: %u\n", value);
    event_count++;
}


static void* test_gpio_toggle_worker(void* _arg)
{
	int ret;
	int count = 10;
	uint8_t value;
	gpio_hdl_t togglePin = (gpio_hdl_t)_arg;

	while (count>0)
	{
		count--;

		ret = lib_gpio__toggle(togglePin);
	
		//ret = lib_gpio__toggle(array_test_hdl);
		TEST_INFO("Test_count : %u  EventCount :%u\n",count, event_count);
		lib_thread__msleep(500);
	}

	return NULL;

}
