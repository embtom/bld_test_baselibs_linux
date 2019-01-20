/* ****************************************************************************************************
 * lib_gpio_RPI.c within the following project: lib_gpio_RPI
 *
 *  compiler:   GNU Tools ARM Embedded (4.7.201xqx)
 *  target:     Cortex Mx
 *  author:		Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/* *******************************************************************
 * includes
 * ******************************************************************/

/*c-runtime */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*system*/
#include <unistd.h>
#include <fcntl.h>
#include <sys/epoll.h>

/* frame */
#include <lib_thread.h>
#include <lib_convention__errno.h>

/*project*/
//#include <lib_gpio__types_if.h>
#include <gpio_access.h>

 /********************************************************************
 * Constants
  * ******************************************************************/

#define SYSFS_GPIO_DIR "/sys/class/gpio"
#define POLL_TIMEOUT (3 * 1000) /* 3 seconds */
#define MAX_BUF 64


/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/
enum gpio_dir {
	GPIO_DIR_input,
	GPIO_DIR_output
};

//"none", "rising", "falling", or
//		"both"
enum gpio_event {
	GPIO_EVENT_none,
	GPIO_EVENT_rising,
	GPIO_EVENT_falling,
	GPIO_EVENT_rising_falling
};

 /********************************************************************
  * static function declarations
  * ******************************************************************/
static int gpio_access__export(unsigned int _gpio);
static int gpio_access__unexport(unsigned int _gpio);
static int gpio_access__set_dir(unsigned int _gpio, enum gpio_dir _mode);
static int gpio_access__set_event(unsigned int _gpio, enum gpio_event _event);
static int gpio_access__open(unsigned int _gpio);


/* *******************************************************************
 * \brief	open gpio device
 * ---------
 * \remark Close of the gpio file descriptor
 * ---------
 * \param	_pin	Pin to open
 * \param   _mode 	Usage mode of the pin
 * ---------
 * \return	fd return if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_access__pin_open(struct gpio_access * const _hdl, enum gpio_mode _mode, enum gpio_pull _pull)
{
	int ret;

	ret = gpio_access__export(_hdl->pinPhys);
	if(ret < EOK)
		goto ERR1;

	switch (_mode)
	{
		case GPIO_MODE_input: /* input */
			ret = gpio_access__set_dir(_hdl->pinPhys, GPIO_DIR_input);
			if (ret<EOK) goto ERR2;

		//		switch (_arg->pull_mode)
		//		{
		//			case GPIO_PULL_no: break;
		//			case GPIO_PULL_up: break;
		//			case GPIO_PULL_down: break;
		//			default: ret = -EINVAL; goto err_open;
		//		}

			break;

		case GPIO_MODE_output_pp:
			ret = gpio_access__set_dir(_hdl->pinPhys, GPIO_DIR_output);
			if (ret < EOK) goto ERR2;

			break;

		case GPIO_MODE_output_od:
			ret = -ENOTSUP;
			if (ret < EOK) goto ERR2;

			break;

		case GPIO_MODE_event_rising:
			ret = gpio_access__set_dir(_hdl->pinPhys, GPIO_DIR_input);
			if (ret<EOK) goto ERR2;

			ret = gpio_access__set_event(_hdl->pinPhys, GPIO_EVENT_rising);
			if (ret<EOK) goto ERR2;

			break ;

		case GPIO_MODE_event_falling:
			ret = gpio_access__set_dir(_hdl->pinPhys, GPIO_DIR_input);
			if (ret<EOK) goto ERR2;

			ret = gpio_access__set_event(_hdl->pinPhys, GPIO_EVENT_falling);
			if (ret<EOK) goto ERR2;

			break;

		case GPIO_MODE_event_rising_falling:
			ret = gpio_access__set_dir(_hdl->pinPhys, GPIO_DIR_input);
			if (ret<EOK) goto ERR2;

			ret = gpio_access__set_event(_hdl->pinPhys, GPIO_EVENT_rising_falling);
			if (ret<EOK) goto ERR2;

			break;

		default:	ret = -EINVAL; goto ERR2;
	}

	ret = gpio_access__open(_hdl->pinPhys);
	if(ret < EOK) goto ERR2;

	_hdl->pinFD = ret;

	return EOK;

	ERR2:
	gpio_access__unexport(_hdl->pinPhys);

	ERR1:
	return ret;

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
int gpio_access__pin_close(struct gpio_access *_hdl)
{
	int ret;

	ret = close(_hdl->pinFD);
	if (ret < EOK)
		return -errno;


	ret = gpio_access__unexport(_hdl->pinPhys);
	if(ret < EOK)
		return -ret;

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
	int ret = EOK;

	if (_value) {
		ret = write(_hdl->pinFD, "1", 2);
	}
	else {
		ret = write(_hdl->pinFD, "0", 2);
	}

	lseek(_hdl->pinFD, 0, SEEK_SET);
	if (ret == -1) {
		return -ESTD_IO;
	}

	return ret;
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
	int ret = EOK;
	char ch;

	ret = read(_hdl->pinFD, &ch, 1);
	lseek(_hdl->pinFD, 0, SEEK_SET);

	if (ret == -1) {
		ret = -ESTD_IO;
	}
	else {
		if (ch != '0') {
			*_value = 1;
		} else {
			*_value = 0;
		}	
	}
	return ret;
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
	int ret = EOK;
	char value;

	ret = read(_hdl->pinFD, &value, 1);
	lseek(_hdl->pinFD, 0, SEEK_SET);

	if (ret == -1) {
		ret = -ESTD_IO;
	}
	else {
		if (value != '0') {
			ret = write(_hdl->pinFD, "0", 2);
		}
		else {
			ret = write(_hdl->pinFD, "1", 2);
		}
		lseek(_hdl->pinFD, 0, SEEK_SET);
		if(ret == -1) {
			ret = -ESTD_IO;
		}
	}
	return ret;
}

/* *******************************************************************
 * \brief	Export of the gpio pin to userspace
 * ---------
 * \remark Export of the GPIO pin to userspace by sysfs interface
 * ---------
 * \param	_gpio	Pin to export to userspace
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
static int gpio_access__export(unsigned int _gpio)
{
	int fd, len, ret = EOK;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/export", O_WRONLY);
	if (fd < 0) {
		return -errno;
	}

	len = snprintf(buf, sizeof(buf), "%d", _gpio);
	ret = write(fd, buf, len);
	if (ret == -1) {
		ret = -ESTD_IO;
	}
	close(fd);

	return 0;
}

/* *******************************************************************
 * \brief	Return of the gpio pin from userspace
 * ---------
 * \remark Remove an userspace exported "gpiox" pin
 * ---------
 * \param	_gpio	Pin to return from userspace
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
static int gpio_access__unexport(unsigned int _gpio)
{
	int fd, len, ret = EOK;
	char buf[MAX_BUF];

	fd = open(SYSFS_GPIO_DIR "/unexport", O_WRONLY);
	if (fd < 0) {
		return -errno;
	}

	len = snprintf(buf, sizeof(buf), "%d", _gpio);
	ret = write(fd, buf, len);
	if (ret == -1) {
		ret = -ESTD_IO;
	}
	close(fd);
	return 0;
}

/* *******************************************************************
 * \brief	Set of the gpio pin direction
 * ---------
 * \remark Configure of the GPIO's pin direction.
 * ---------
 * \param	_gpio	Pin to config direction
 * \param   _mode	Set of the direction mode
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
static int gpio_access__set_dir(unsigned int _gpio, enum gpio_dir _mode)
{
	int fd, len, ret = EOK;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR  "/gpio%d/direction", _gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		return -errno;
	}

	switch (_mode)
	{
		case GPIO_DIR_input : ret = write(fd, "in", 3); break;
		case GPIO_DIR_output : ret = write(fd, "out", 4); break;
		default: ret = -EINVAL; break;
	}
	if (ret == -1) {
		ret = -ESTD_IO;
	}

	close(fd);
	return ret;
}

/* *******************************************************************
 * \brief	Select of an trigger event
 * ---------
 * \remark Set of an edge trigger events, like "none", "rising", "falling", or
 *		   "both". It selects the signal edge(s) and will enable
 *		   poll on the "value" file return file descriptor.
 *
 *		   It is only working if the pin is configured as an
 *		   interrupt generating input pin.
 * ---------
 * \param	_gpio	Pin to config input event
 * \param   _event	Set of the trigger event
 * ---------
 * \return	'0', if successful, < '0' if not successful
 * ******************************************************************/
static int gpio_access__set_event(unsigned int _gpio, enum gpio_event _event)
{
	int fd, len;
	int ret = EOK;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/edge", _gpio);

	fd = open(buf, O_WRONLY);
	if (fd < 0) {
		return fd;
	}

	switch (_event)
	{
		case GPIO_EVENT_none: ret = write(fd, "none", 5); break;
		case GPIO_EVENT_rising: ret = write(fd, "rising", 7); break;
		case GPIO_EVENT_falling: ret = write(fd, "falling", 8); break;
		case GPIO_EVENT_rising_falling: ret = write(fd, "both", 5); break;
		default: ret = -EINVAL; break;
	}

	if (ret == -1) {
		ret = -ESTD_IO;
	}

	close(fd);
	return ret;
}

/* *******************************************************************
 * \brief	open gpio device
 * ---------
 * \remark Request of an gpio file descriptor
 * ---------
 * \param	_gpio	Pin to open
 * ---------
 * \return	fd return if successfully, < '0' if not successful
 * ******************************************************************/
static int gpio_access__open(unsigned int _gpio)
{
	int fd, len;
	char buf[MAX_BUF];

	len = snprintf(buf, sizeof(buf), SYSFS_GPIO_DIR "/gpio%d/value", _gpio);

	fd = open(buf, O_RDWR);  //| O_NONBLOCK );
	if (fd < 0) {
		return -errno;
	}
	return fd;
}



