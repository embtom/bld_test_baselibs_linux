/*
 * test_gpio.c
 *
 *  Created on: 09.12.2015
 *      Author: thomas
 */

#include <stdio.h>
#include <stdlib.h>
#include <lib_gpio.h>
#include <lib_thread.h>
#include <lib_gpio_config.h>


//#include <lib_gpio_LINUX__cfg.h>

//static struct lib_gpio__cfg_attr s_lib_gpio__cfg_tab[] = M_LIB_GPIO__STATIC_CFG;

static thread_hdl_t gpio_hdl;
static gpio_hdl_t test_hdl;
static gpio_hdl_t array_test_hdl;

static void* gpio_worker(void* _arg);
void test_event_handler (void *const _arg);


static  struct gpio_access s_pin_config_table[] = 
{
	{
		.pinId = 0,
		.pinPhys = 17,
		.pinFD = 0
	},
	{
		.pinId = 1,
		.pinPhys = 18,
		.pinFD = 0
	}
}; 


int main (void)
{
	int ret;
	printf("Hallo\n");
//	struct lib_gpio_open_attr arg;
	//arg.pull_mode = GPIO

	lib_gpio__set_config_table(&s_pin_config_table[0], sizeof(s_pin_config_table)/sizeof(struct gpio_access));

	lib_thread__init(PROCESS_SCHED_rr, 10);

	// arg.mode = GPIO_MODE_event_rising;
	// arg.event_handler = &event_handler;
	// ret = lib_gpio__open(&test_hdl,M_LIB_GPIO__TEST_READ, &arg);

	// arg.mode = GPIO_MODE_output_pp;
	// ret = lib_gpio__open(&array_test_hdl,M_LIB_GPIO__TEST_ARRAY, &arg);


	ret = lib_thread__create(&gpio_hdl,&gpio_worker,NULL,-1,"gpio_worker");

	lib_thread__join(&gpio_hdl, NULL);
//	lib_gpio__close(&test_hdl);
//	lib_gpio__close(&array_test_hdl);

}


static void* gpio_worker(void* _arg)
{
	int ret;
	int count = 200;
	uint8_t value;
	gpio_hdl_t test_pin, test_pin2;

	struct gpio_open_args args = {
		.mode =GPIO_MODE_output_pp,
		.pull_mode = GPIO_PULL_down,
		.event_handler = NULL };

	struct gpio_open_args inargs = {
		.mode =GPIO_MODE_event_rising,
		.pull_mode = GPIO_PULL_down,
		.event_handler = &test_event_handler };


	ret = lib_gpio__open(&test_pin,0,&args);
	ret = lib_gpio__open(&test_pin2,1,&inargs);




	while (count>0)
	{
		count--;

		ret = lib_gpio__toggle(test_pin);
		ret = lib_gpio__write(test_pin2, (count%2));

		//ret = lib_gpio__toggle(array_test_hdl);
		printf("Test_count : %u  Gpio :%u\n",count,value);
		lib_thread__msleep(100);
	}

	return NULL;

}


void test_event_handler (void *const _arg)
{
	printf("juhu\n");
}


static void* event_handler(gpio_hdl_t * const _hdl)
{
	unsigned ret, value;
	static unsigned count=0;
	ret = lib_gpio__read(*_hdl,&value);
	//ret = gpio_hdl__read((*hdl)->fd_table,&value);
	printf("value: %u, count %u\n",value,count);
	count++;



}




