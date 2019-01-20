/* ****************************************************************************************************
 * lib_gpio_STM32F1.c within the following project: lib_gpio_STM32F1
 *
 *  compiler:   GNU Tools ARM Embedded (4.7.201xqx)
 *  target:     Cortex Mx
 *  author:		Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/* *******************************************************************
 * includes
 * ******************************************************************/

/* c-runtime */
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <stdio.h>

/* system */
#include <unistd.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <sys/un.h>

/* own libs */
#include <lib_thread.h>
#include <lib_log.h>
#include <lib_convention__errno.h>

/* project */
#include <gpio_types.h>
#include "gpio_event.h"

/* *******************************************************************
 * defines
 * ******************************************************************/
#define GPIO_EVENT__MODULE_NAME		"GPIO_EVT"

 /* *******************************************************************
  * (static) variables declarations
  * ******************************************************************/
static thread_hdl_t s_event_thd;
static int s_event_thread_running;
static int s_epoll_fd;
static int s_initialized = 0;
static int s_cancelSocket;
static struct sockaddr_un s_cancelSocketAddr;

/********************************************************************
  * static function declarations
  * ******************************************************************/
static void* gpio_event__processing(void* _arg);

/* *******************************************************************
 * GLOBAL FUNCTION DEFINITIONS
 * ******************************************************************/

/* *******************************************************************
 * \brief Init of the gpio event handling
 * ---------
 * \remark The GPIO init routine is called by each event driven pin
 * ---------
 * \return	if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_event__init()
{
	int ret;
	
	struct epoll_event ev;

	if (s_initialized > 0) {
		s_initialized++;
		return EOK;
	}

	ret = epoll_create(1);
	if(ret== -1) {
		goto ERR0;
	}

	s_initialized = 1;
	s_epoll_fd = ret;

	s_cancelSocket = socket(AF_UNIX, SOCK_DGRAM, PF_UNSPEC);
	s_cancelSocketAddr.sun_family = AF_UNIX;
	strcpy(s_cancelSocketAddr.sun_path, "/tmp/gpio_event");
	
	unlink((const char*)&(s_cancelSocketAddr.sun_path));
	bind(s_cancelSocket, (struct sockaddr *) &s_cancelSocketAddr, sizeof(struct sockaddr_un));
	ev.events = EPOLLIN;
	ev.data.fd = s_cancelSocket;

	epoll_ctl(s_epoll_fd, EPOLL_CTL_ADD, s_cancelSocket, &ev);

	ret = lib_thread__create(&s_event_thd, &gpio_event__processing, NULL, 1, "gpio_event_worker");
	if (ret < EOK) {
		goto ERR1;
	}

	return EOK;

	ERR1:
	s_initialized = 0;
	close(s_epoll_fd);
	
	ERR0:

	return ret;
}

/* *******************************************************************
 * \brief Cleanup of the gpio event handling
 * ---------
 * \remark The GPIO cleanup routine is called by each event driven pin
 * ---------
 * \return	if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_event__cleanup()
{
	int ret;
	int dummyVal = 0;
	
	if (s_initialized == 0) {
		return -EEXEC_NOINIT;
	}

	if (s_initialized > 1) {
		s_initialized--;
		return EOK;
	}

	s_event_thread_running = 0;
	sendto(s_cancelSocket, &dummyVal, 1, 0, (struct sockaddr *)&s_cancelSocketAddr, sizeof(s_cancelSocketAddr));

	ret = lib_thread__join(&s_event_thd, NULL);
	return ret;
}

/* *******************************************************************
 * \brief current pin is registerd at the event handling
 * ---------
 * \return	if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_event__register(gpio_hdl_t _hdl)
{
	int ret;
	struct epoll_event event;

	event.events = EPOLLIN | EPOLLET;
	event.data.ptr = (void*)_hdl;

	ret = epoll_ctl(s_epoll_fd, EPOLL_CTL_ADD, _hdl->access->pinFD, &event);
	if(ret == -1) {
		ret = convert_std_errno(errno);
		printf("Fehler %i\n",ret);
		return ret;
	}

	return EOK;
}

/* *******************************************************************
 * \brief current pin is unregisterd from the event handling
 * ---------
 * \return	if successfully, < '0' if not successful
 * ******************************************************************/
int gpio_event__unregister(gpio_hdl_t _hdl)
{
	struct epoll_event event;
	int ret;
	ret = epoll_ctl(s_epoll_fd, EPOLL_CTL_DEL, _hdl->access->pinFD, &event);
	return ret;
}

/* *******************************************************************
 * \brief processing of gpio events 
 * ---------
 * \return	if successfully, < '0' if not successful
 * ******************************************************************/
static void* gpio_event__processing(void* _arg)
{
	int ret, error;
	struct epoll_event event;
	volatile gpio_hdl_t gpio_object;

	s_event_thread_running = 1;

	while (s_event_thread_running)
	{
		do {
		    ret = epoll_wait(s_epoll_fd, &event, 1, -1);
		    if(s_event_thread_running == 0)
		    {
		    	msg(LOG_LEVEL_info, GPIO_EVENT__MODULE_NAME, "%s(): thread terminated with %u", __func__ ,errno);
		    	return NULL;
		    }
		} while (ret < 0 && errno == EINTR);

		if(ret == -1) {
			s_event_thread_running = 0;
			msg(LOG_LEVEL_error, GPIO_EVENT__MODULE_NAME, "%s(): epoll failed with error %i", __func__, errno);

			return NULL;
		}

		if (event.data.fd == s_cancelSocket) {
			continue;
		}

		gpio_object = (gpio_hdl_t)event.data.ptr;
		if(gpio_object != NULL) {
			(*(gpio_object->event_handler))(gpio_object->event_arg);
		}
	}


	return NULL;
}


