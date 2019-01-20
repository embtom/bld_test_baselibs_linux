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

#ifndef LIB_GPIO_TYPES_H_
#define LIB_GPIO_TYPES_H_

/* *******************************************************************
 * typedefs
 * ******************************************************************/
typedef struct gpio_hdl* gpio_hdl_t;

typedef void (gpio_event_handler_t)(void *const _arg);


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
enum gpio_mode {
	GPIO_MODE_input,
	GPIO_MODE_output_pp,
	GPIO_MODE_output_od,
	GPIO_MODE_event_rising,
	GPIO_MODE_event_falling,
	GPIO_MODE_event_rising_falling
};

enum gpio_pull {
	GPIO_PULL_no,
	GPIO_PULL_up,
	GPIO_PULL_down
};



/* GPIO open attributes */
struct gpio_open_args {
	enum gpio_mode mode;
	enum gpio_pull pull_mode;
	gpio_event_handler_t* event_handler;
	void* event_arg;

};

#endif /* LIB_GPIO_TYPES_H_ */
