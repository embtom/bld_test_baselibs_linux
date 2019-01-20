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

/*c-runtime */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*system*/
#ifdef STM32HAL_F1
	#include <stm32f1xx_hal_gpio.h>
	#include <stm32f1xx_hal_rcc.h>
#elif STM32HAL_F4
	#include <stm32f4xx_hal_gpio.h>
	#include <stm32f4xx_hal_rcc.h>
#else
	#error No STM32 HAL framework selected at lib_gpio
#endif

/*frame */
#include <lib_convention__errno.h>

/*project*/
#include "gpio_access.h"

 /****************************************************************
 * Constants
 ****************************************************************/

/* *******************************************************************
 * \brief	close gpio device
 * ---------
 * \remark Close of the gpio file descriptor
 * ---------
 * \param	_fd		fd to close
 * ---------
 * \return	fd return if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_access__pin_open(struct gpio_access *_hdl, enum gpio_mode _mode, enum gpio_pull _pull)
{
	GPIO_InitTypeDef init_arg;

	if (_hdl->pinPort == GPIOA) 	 { __HAL_RCC_GPIOA_CLK_ENABLE(); }
	else if (_hdl->pinPort == GPIOB) { __HAL_RCC_GPIOB_CLK_ENABLE(); }
	else if (_hdl->pinPort == GPIOC) { __HAL_RCC_GPIOC_CLK_ENABLE(); }
	else if (_hdl->pinPort == GPIOD) { __HAL_RCC_GPIOD_CLK_ENABLE(); }
	else if (_hdl->pinPort == GPIOE) { __HAL_RCC_GPIOE_CLK_ENABLE(); }
	#ifdef GPIOF
	else if (_hdl->pinPort == GPIOF) { __HAL_RCC_GPIOF_CLK_ENABLE(); }
	#endif
	#ifdef GPIOG
	else if (_hdl->pinPort == GPIOG) { __HAL_RCC_GPIOG_CLK_ENABLE(); }
	#endif
	else {
		return -ENODEV;
	}

	switch (_mode)
	{
		case GPIO_MODE_input: /* input */
		{
			init_arg.Mode			= GPIO_MODE_INPUT;
			switch (_pull)
			{
				case GPIO_PULL_no: 	 init_arg.Pull = GPIO_NOPULL; 	break;  /* floating*/
				case GPIO_PULL_up: 	 init_arg.Pull = GPIO_PULLUP;  	break;     /* pull up */
				case GPIO_PULL_down: init_arg.Pull = GPIO_PULLDOWN;	break;   /* pull down */
				default: return -EINVAL;
			}
		}
		break;

		case GPIO_MODE_output_pp: 	init_arg.Mode = GPIO_MODE_OUTPUT_PP; break;
		case GPIO_MODE_output_od:
		{
			init_arg.Mode = GPIO_MODE_OUTPUT_OD;

			// in case of open drain mode, set initial pin state to open instead of pull low
			HAL_GPIO_WritePin(_hdl->pinPort, 1 << _hdl->pinNumber, 1);
		}
		break;

		case GPIO_MODE_event_rising: init_arg.Mode = GPIO_MODE_IT_RISING; break ;
		case GPIO_MODE_event_falling: init_arg.Mode = GPIO_MODE_IT_FALLING; break;
		case GPIO_MODE_event_rising_falling: init_arg.Mode = GPIO_MODE_IT_RISING_FALLING; break;

		default: return -EINVAL;
	}


	init_arg.Pin = 1 << _hdl->pinNumber;
	/* standard mode is output */
	init_arg.Speed			= GPIO_SPEED_FREQ_MEDIUM;			/* medium speed pin */


	HAL_GPIO_Init(_hdl->pinPort, &init_arg);

	return EOK;
}

/* *******************************************************************
 * \brief	close gpio device
 * ---------
 * \remark Close of the gpio file descriptor
 * ---------
 * \param	_gpio	Pin to close
 * ---------
 * \return	fd return if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_access__pin_close( struct gpio_access *_hdl)
{
	HAL_GPIO_DeInit(_hdl->pinPort, (1 << _hdl->pinNumber));
	return EOK;
}

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
int gpio_access__write( struct gpio_access *_hdl, unsigned int _value)
{
	GPIO_PinState setValue;
	if(_hdl == NULL)
		return -EPAR_NULL;

	if(_value == 0) {
		setValue = GPIO_PIN_RESET;
	}
	else {
		setValue = GPIO_PIN_SET;
	}

	HAL_GPIO_WritePin(_hdl->pinPort, 1 << _hdl->pinNumber, setValue);
	return EOK;
}

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
int gpio_access__read( struct gpio_access *_hdl, unsigned int *_value)
{
	if((_hdl == NULL)||(_value == NULL))
			return -EPAR_NULL;

	*_value = (unsigned char)HAL_GPIO_ReadPin(_hdl->pinPort, 1 << _hdl->pinNumber);
	return EOK;
}

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
int gpio_access__toggle( struct gpio_access *_hdl)
{
	if(_hdl == NULL)
		return -EPAR_NULL;

	HAL_GPIO_TogglePin(_hdl->pinPort, 1 << _hdl->pinNumber);
	return EOK;
}




