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


#ifndef SH_LIB_GPIO_LIB_GPIO_H_
#define SH_LIB_GPIO_LIB_GPIO_H_

/* *******************************************************************
 * includes
 * ******************************************************************/
/* system */
#include <stdint.h>

/* project */
#include <lib_gpio_types.h>

/* *******************************************************************
 * defines
 * ******************************************************************/

/* gpio handles */

/* *******************************************************************
 * function declarations
 * ******************************************************************/

/* *******************************************************************
 * \brief	opens a gpio port for further read/write/toggle accesses
 * ---------
 * \remark	special remarks on the function
 * ---------
 * \param	*_arg [in]
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int lib_gpio__open(gpio_hdl_t *_hdl, int _pin, struct gpio_open_args *_arg);

/* *******************************************************************
 * \brief	closes a previously opened port/pin
 * ---------
 * \remark	special remarks on the function
 * ---------
 * \param	*_arg [in]
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int lib_gpio__close(gpio_hdl_t *_hdl);

/* *******************************************************************
 * \brief	writes on a port/pin
 * ---------
 * \remark	special remarks on the function
 * ---------
 * \param	*_arg [in]
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int lib_gpio__write(gpio_hdl_t _hdl, unsigned int _val);

/* *******************************************************************
 * \brief	reads from a port/pin
 * ---------
 * \remark	special remarks on the function
 * ---------
 * \param	*_arg [in]
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int lib_gpio__read(gpio_hdl_t _hdl, unsigned int *_val);

/* *******************************************************************
 * \brief	toggles a pin port
 * ---------
 * \remark	special remarks on the function
 * ---------
 * \param	*_arg [in]
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int lib_gpio__toggle(gpio_hdl_t _hdl);

#endif /* SH_LIB_GPIO_LIB_GPIO_H_ */
