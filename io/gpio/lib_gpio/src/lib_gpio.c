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
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <errno.h>
/* system */

/* own libs */
#include <lib_list.h>
#include <lib_log.h>
#include <lib_convention__errno.h>
#include <lib_convention__macro.h>
#include <lib_convention__mem.h>

/* project */
#include <gpio_types.h>
#include <gpio_access_types.h>
#include <gpio_access.h>
#include <gpio_event.h>
#include <gpio_types.h>
#include <lib_gpio_config.h>
#include <lib_gpio.h>



/* *******************************************************************
 * (static) variables declarations
 * ******************************************************************/
static unsigned int init_count = 0;

/* *******************************************************************
 * static function declarations
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
int lib_gpio__open(gpio_hdl_t *_hdl, int _pinID, struct gpio_open_args *_arg)
{
// 	/* local variables */
	struct gpio_access *pin_access;
	gpio_hdl_t hdl;
 	int ret;

	if ((_hdl == NULL) || ( _arg == NULL)) {
		return -EPAR_NULL;
	}

	hdl =  (gpio_hdl_t)alloc_memory(1, sizeof(struct gpio_hdl));
	if (hdl == NULL) {
		return -ENOMEM;
	}

	pin_access = lib_gpio__get_config(_pinID);
	if (pin_access == NULL) {
		return -ESTD_NOENT;
	}

	ret = gpio_access__pin_open(pin_access,_arg->mode,_arg->pull_mode);
	if (ret < EOK) {
		return ret ;
	}

	hdl->access = pin_access;
	hdl->event_handler = _arg->event_handler;
	hdl->event_arg = _arg->event_arg;

	if ((_arg->mode == GPIO_MODE_event_rising) || (_arg->mode == GPIO_MODE_event_falling) || (_arg->mode == GPIO_MODE_event_rising_falling)) {
		ret = gpio_event__init();
		if (ret < EOK) {
			return ret;
		}

		ret = gpio_event__register(hdl);
		if (ret < EOK) {
			return ret;
		}
		hdl->isEventPin = 1;

	}
	else {
		hdl->isEventPin = 0;
	}

	*_hdl = hdl;
	return ret;

	err_open:
	return ret;
}

/* *******************************************************************
 * \brief	closes a previously opened port/pin
 * ---------
 * \remark	special remarks on the function
 * ---------d
 * \param	*_arg [in]
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int lib_gpio__close(gpio_hdl_t *_hdl)
{
 	int ret;

 	if (_hdl == NULL) {
		return -EPAR_NULL;
 	}

	gpio_access__pin_close((*_hdl)->access);
	
	if((*_hdl)->isEventPin) {
	 	gpio_event__unregister((*_hdl));
		gpio_event__cleanup();
	}
    free_memory(*_hdl);
	*_hdl = NULL;
	return EOK;
}

/* *******************************************************************
 * \brief	writes on a port/pin
 * ---------
 * \remark	special remarks on the function
 * ---------
 * \param	*_arg [in]
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int lib_gpio__write(gpio_hdl_t _hdl, unsigned int _val)
{
	int ret;
	/* start of code section */
	if (_hdl == NULL) {
		return -EPAR_NULL;
	}

	ret = gpio_access__write(_hdl->access, _val);
	if (ret < EOK) {
		return ret;
	}
	return EOK;
}

/* *******************************************************************
 * \brief	reads from a port/pin
 * ---------
 * \remark	special remarks on the function
 * ---------
 * \param	*_arg [in]
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int lib_gpio__read(gpio_hdl_t _hdl, unsigned int *_val)
{
	/* local variables */
	int ret = 0;
	unsigned int count, val;

	/* start of code section */
	if ((_hdl == NULL)||(_val == NULL)) {
		return -EPAR_NULL;
	}

	ret = gpio_access__read(_hdl->access, _val);
	if (ret < EOK) {
		return ret;
	}

	// 	case M_LIB_GPIO__INIT_CMD_multiple:
	// 		for (count=0; count < M_CONV__CMD__SIZE(_hdl->channel); count++)
	// 		{
	// 			for(count = 0; count < M_CONV__CMD__SIZE(_hdl->channel); count++)
	// 			{
	// 				gpio_hdl__read(&_hdl->gpio_pin_hdl[count],&val);

	// 				if(val == 0) {CLEAR_BITVAL(*_val,count); }
	// 				else 		 {SET_BITVAL(*_val,count);}
	// 			}
	// 		}
	// 		break;
	// 		default: return -ESTD_FAULT;
	// }
	return ret;
}

/* *******************************************************************
 * \brief	toggles a pin port
 * ---------
 * \remark	special remarks on the function
 * ---------
 * \param	*_arg [in]
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
int lib_gpio__toggle(gpio_hdl_t _hdl)
{
	/* local variables */
	int ret = 0;
	unsigned int count;

	/* start of code section */
	if (_hdl == NULL) {
		return -EPAR_NULL;
	}

	ret = gpio_access__toggle(_hdl->access);
	if (ret < EOK) {
		return ret;
	}

	// 	case M_LIB_GPIO__INIT_CMD_multiple:
	// 		for (count=0; count < M_CONV__CMD__SIZE(_hdl->channel); count++)
	// 		{
	// 			for(count = 0; count < M_CONV__CMD__SIZE(_hdl->channel); count++)
	// 			{
	// 				gpio_hdl__toggle(&_hdl->gpio_pin_hdl[count]);
	// 			}
	// 		}

	// 		break;
	// 		default: return -ESTD_FAULT;
	// }

	return ret;
}






