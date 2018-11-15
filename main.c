/* ****************************************************************************************************
 * lib_thread_test.c within the following project: lib_thread
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	16.12.2017			Tom			- creation of lib_thread.c
 *
 */


/* *******************************************************************
 * includes
 * ******************************************************************

/* c-runtime */
#include <stdio.h>
#include <stdlib.h>

/* system */

/* frame */
#include <lib_thread.h>
#include <lib_thread_wakeup.h>
#include <lib_tty_portmux.h>
#include <app_test.h>

/* project */

static struct tty_stream_mapping s_port_stream_mapping[	TTY_STREAM_CNT] = {
    M_STREAM_MAPPING_ENTRY(TTY_DEVICE_unix),  /* TTY_STREAM_critical */\
    M_STREAM_MAPPING_ENTRY(TTY_DEVICE_unix),  /* TTY_STREAM_error */\
    M_STREAM_MAPPING_ENTRY(TTY_DEVICE_unix),  /* TTY_STREAM_warning */\
    M_STREAM_MAPPING_ENTRY(TTY_DEVICE_unix),  /* TTY_STREAM_info */\
    M_STREAM_MAPPING_ENTRY(TTY_DEVICE_unix),  /* TTY_STREAM_info */\
    M_STREAM_MAPPING_ENTRY(TTY_DEVICE_unix),  /*TTY_STREAM_CONTROL*/
};


//		new_TestFixture("test_lib_can__mscp_client_cleanup", test_lib_can__mscp_client_cleanup),
/* *************************************************************************************************
 * \brief			Main
 * \description		Creates and executes the test runner instance
 * \return			int
 * *************************************************************************************************/
int main() 
{
    unsigned int running;

    lib_thread__init(PROCESS_SCHED_rr,10);
    lib_thread__wakeup_init();

    lib_tty_portmux__init(&s_port_stream_mapping[0], sizeof(s_port_stream_mapping));

    app_test__init();
    
    running = 1;
    while(running) {
        lib_thread__msleep(5000);
    }


}
