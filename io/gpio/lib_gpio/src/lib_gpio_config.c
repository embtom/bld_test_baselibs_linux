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

/*own libs */
#include <lib_convention__errno.h>

/* project */
#include <lib_gpio_config.h>

/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static  struct gpio_access *s_pin_config_table;
static unsigned int s_pin_config_number;

/* *******************************************************************
 * \brief	Setup of the configuration table necessary translating
 * 			virtual pins to physical 

 * \param	*_config [in] : base address of the config table 
 * \param   _config_count : number of entries at pin config table
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int lib_gpio__set_config_table(struct gpio_access * const _config, unsigned int _config_count)
{
    if ((_config == NULL) || (_config_count <= 0)) {
        return -ESTD_INVAL;
    }

    s_pin_config_table = _config;
    s_pin_config_number = _config_count;
	return EOK;
}

/* *******************************************************************
 * \brief	Request corresponding entry of config table by _pinId

 * \param	_pinId : to get config entry 
 * ---------
 * \return	'pointer to config entry', if successful, NULL if not 
 * ******************************************************************/
 struct gpio_access* lib_gpio__get_config(unsigned int _pinId)
{
	unsigned int i;

	if (s_pin_config_table == NULL) {
		return NULL;
	}

	if (_pinId < s_pin_config_number) {
		if (s_pin_config_table[_pinId].pinId == _pinId) {
			return &s_pin_config_table[_pinId];
		}
	}

	for(i = 0; i < s_pin_config_number; i++) {
		if (s_pin_config_table[_pinId].pinId == _pinId) {
			return &s_pin_config_table[_pinId];
		}
	}

	return NULL;
}