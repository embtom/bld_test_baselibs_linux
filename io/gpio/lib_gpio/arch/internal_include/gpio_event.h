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


#ifndef _GPIO_EVENT_LINUX_H_
#define _GPIO_EVENT_LINUX_H_

#ifdef __cplusplus
extern "C" {
#endif


/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdint.h>

/* project */
#include <lib_gpio_types.h>

/* *******************************************************************
 * GLOBAL FUNCTION DECLARATIONS
 * ******************************************************************/

/* *******************************************************************
 * \brief Init of the gpio event handling
 * ---------
 * \remark The GPIO init routine is called by each event driven pin
 * ---------
 * \return	if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_event__init();

/* *******************************************************************
 * \brief Cleanup of the gpio event handling
 * ---------
 * \remark The GPIO cleanup routine is called by each event driven pin
 * ---------
 * \return	if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_event__cleanup();

/* *******************************************************************
 * \brief current pin is registerd at the event handling
 * ---------
 * \return	if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_event__register(gpio_hdl_t _hdl);

/* *******************************************************************
 * \brief current pin is unregisterd from the event handling
 * ---------
 * \return	if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_event__unregister(gpio_hdl_t _hdl);


#ifdef __cplusplus
}
#endif

#endif /* _GPIO_EVENT_LINUX_H_ */
