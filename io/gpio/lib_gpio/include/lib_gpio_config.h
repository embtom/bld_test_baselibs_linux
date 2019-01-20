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

#ifndef LIB_GPIO_CONFIG
#define LIB_GPIO_CONFIG

/* *******************************************************************
 * includes
 * ******************************************************************/
/* project*/
#include <gpio_access_types.h>

/* *******************************************************************
 * \brief	Setup of the configuration table necessary translating
 * 			virtual pins to physical 

 * \param	*_config [in] : base address of the config table 
 * \param   _config_count : number of entries at pin config table
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int lib_gpio__set_config_table(struct gpio_access * const _config, unsigned int _config_count);

/* *******************************************************************
 * \brief	Request corresponding entry of config table by _pinId

 * \param	_pinId : to get config entry 
 * ---------
 * \return	'pointer to config entry', if successful, NULL if not 
 * ******************************************************************/
 struct gpio_access* lib_gpio__get_config(unsigned int _pinId);

#endif /*LIB_GPIO_CONFIG */