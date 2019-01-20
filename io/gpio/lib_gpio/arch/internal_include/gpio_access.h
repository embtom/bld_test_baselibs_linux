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

#ifndef SH_LIB_GPIO_LIB_GPIO_ACCESS_H_
#define SH_LIB_GPIO_LIB_GPIO_ACCESS_H_
/* *******************************************************************
 * includes
 * ******************************************************************/
#include <gpio_access_types.h>
#include <lib_gpio_types.h>

/* *******************************************************************
 * \brief	close gpio device
 * ---------
 * \remark Close of the gpio file descriptor
 * ---------
 * \param	_fd		fd to close
 * ---------
 * \return	fd return if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_access__pin_open(struct gpio_access * _hdl, enum gpio_mode _mode, enum gpio_pull _pull);

/* *******************************************************************
 * \brief	close gpio device
 * ---------
 * \remark Close of the gpio file descriptor
 * ---------
 * \param	_fd		fd to close
 * ---------
 * \return	fd return if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_access__pin_close(struct gpio_access *_hdl);

/* *******************************************************************
 * \brief	write to gpio output
 * ---------
 * \remark Write gpio output value. For usage the direction have to be
 * 		   set as output
 * ---------
 * \param	_fd		fd to close
 * ---------
 * \return	fd return if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_access__write( struct gpio_access *_hdl, unsigned int _value);

/* *******************************************************************
 * \brief	read to gpio input
 * ---------
 * \remark Read gpio input value. For usage the direction have to be
 * 		   set as input
 * ---------
 * \param	_fd		fd to close
 * ---------
 * \return	fd return if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_access__read( struct gpio_access *_hdl, unsigned int *_value);

/* *******************************************************************
 * \brief	toggle pin
 * ---------
 * \remark Toggle value. For usage the direction have to be
 * 		   set as input
 * ---------
 * \param	_fd		fd to close
 * ---------
 * \return	fd return if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_access__toggle( struct gpio_access *_hdl);


#endif
