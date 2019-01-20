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
#include <string.h>
#include <stdio.h>
#include <stdarg.h>

/* system */
#include <lib_thread.h>
#include <lib_convention__errno.h>

/* own libs */
#include <mini-printf.h>
#include <lib_ttyportmux.h>
#include <lib_log.h>

/* project */
#include "lib_menu.h"


/*
 * TODO List:
 * - mutex lock the menu list
 * - add states (inited/uninited)
 * - add init parameters (streamin, streamout)
 * - parameter check at all function (especially add_item)
 */

/* *******************************************************************
 * defines
 * ******************************************************************/
#define M_LIB_MENU_NAME				"LIB_MNU"


/* *******************************************************************
 * static function declarations
 * ******************************************************************/
static void lib_menu__item_help(struct lib_menu__item *_item);
static void lib_menu__item_q(struct lib_menu__item *_item);
static void lib_menu__item_setll(struct lib_menu__item *_item);
static void lib_menu__item_getll(struct lib_menu__item *_item);
static void *thread_menu(void *_arg);


/* *******************************************************************
 * static data
 * ******************************************************************/

struct lib_menu__item   s_menu_list[LIB_MENU__CONF_LISTSIZE_MENUITEMS] =
{
		M_LIB_MENU__LIST_INITIALZER("help\0", "Display help information\0",&lib_menu__item_help),	\
		M_LIB_MENU__LIST_INITIALZER("q\0", "Toggle log quiet setting\0",  &lib_menu__item_q),		\
		M_LIB_MENU__LIST_INITIALZER("setll\0", "Set log level (verbosity)\0", &lib_menu__item_setll),\
		M_LIB_MENU__LIST_INITIALZER("getll\0", "Get log lovel (verbosity)\0", &lib_menu__item_getll)
};


static thread_hdl_t     s_hdl_thread_menu;


static int              s_menu_state = 0;
static uint8_t          s_quiet;
char 					s_cmd_in[LIB_MENU__CONF_CMDLENGTH];


/* *******************************************************************
 * function definition
 * ******************************************************************/

int lib_menu__init(void)
{
   int ret;
   int i;

   /* find first free item to clear list */
   for (i = 0; i < LIB_MENU__CONF_LISTSIZE_MENUITEMS; i++) {
       if (s_menu_list[i].cb == NULL) {
           break;
       }
   }

   /* clear remaining menu list */
   if (i != LIB_MENU__CONF_LISTSIZE_MENUITEMS) {
	   memset(&s_menu_list[i], 0, sizeof(struct lib_menu__item) * (LIB_MENU__CONF_LISTSIZE_MENUITEMS - i));
   }

   /* open up a thread for menu IO */
   ret = lib_thread__create(&s_hdl_thread_menu, thread_menu, NULL, 1, "lib_menu");
   if(ret < EOK) {
	   return ret;
   }
   s_menu_state = 1;
   return EOK;
}

int lib_menu__cleanup(void)
{
    if (s_menu_state != 1)
        return -ESTD_INVAL;

    /* wait until thread terminates */
    if (lib_thread__join(&s_hdl_thread_menu, NULL) != EOK)
        while(1) {
            ;
        }

    s_menu_state = 0;

    return EOK;
}


int lib_menu__add_item(struct lib_menu__item *_item)
{
    int i;

    if (s_menu_state != 1)
        return -ESTD_INVAL;

    /* search for free space */
    for (i = 0; i < LIB_MENU__CONF_LISTSIZE_MENUITEMS; i++) {
        if (s_menu_list[i].cb == NULL) {
        	s_menu_list[i] = *_item;
            s_menu_list[i].menu_item_desc = _item;
            return EOK;
        }
    }
    return -ESTD_NOSPC;
}


// int lib_menu__show_message(const char *_msg, ...)
// {
//     va_list args;

//     /* message */
//     if (_msg) {
//         va_start(args, _msg);
//         vfprintf(stdout, _msg, args);
//         va_end(args);
//     }

//     fprintf(stdout, "\n");
//     fflush(stdout);
//     return EOK;
// }

float lib_menu__get_float(const char *_title)
{
    float ret;
    size_t cmd_in_size;

    if (s_menu_state != 1)
        return -ESTD_INVAL;

    cmd_in_size = sizeof(s_cmd_in);
    
    lib_ttyportmux__print(TTYSTREAM_control, "Enter '%s' (float): ", _title == NULL ? "integer value": _title);
    ret = lib_ttyportmux__getline(TTYSTREAM_control,&s_cmd_in[0], &cmd_in_size);
    if (ret < EOK) {
       	msg(LOG_LEVEL_error, M_LIB_MENU_NAME, "%s() getline failed with ret %i", __func__, ret);
       	return ret;
    }

    mini_sscanf(&s_cmd_in[0], "%f", &ret);
    lib_ttyportmux__print(TTYSTREAM_control,"\n");
    return ret;
}

int lib_menu__get_int__decimal(const char *_title)
{
    int ret;
    size_t cmd_in_size;

    if (s_menu_state != 1)
        return -ESTD_INVAL;

    cmd_in_size = sizeof(s_cmd_in);
    lib_ttyportmux__print(TTYSTREAM_control,"Enter '%s' (int, decimal): ", _title == NULL ? "integer value": _title);
    ret = lib_ttyportmux__getline(TTYSTREAM_control, &s_cmd_in[0], &cmd_in_size);
    if (ret < EOK) {
    	msg(LOG_LEVEL_error, M_LIB_MENU_NAME, "%s() getline failed with ret %i", __func__, ret);
    	return ret;
    }

    mini_sscanf(&s_cmd_in[0], "%i", &ret);
    lib_ttyportmux__print(TTYSTREAM_control,"\n");
    return ret;
}

int lib_menu__get_int__hex(const char *_title)
{
    int ret;
    size_t cmd_in_size;

    if (s_menu_state != 1)
        return -ESTD_INVAL;

    cmd_in_size = sizeof(s_cmd_in);
    lib_ttyportmux__print(TTYSTREAM_control, "Enter '%s' (int, hex - e.g.:0xA3): ", _title == NULL ? "integer value": _title);
    ret = lib_ttyportmux__getline(TTYSTREAM_control, &s_cmd_in[0], &cmd_in_size);
    if (ret < EOK) {
    	msg(LOG_LEVEL_error, M_LIB_MENU_NAME, "%s() getline failed with ret %i", __func__, ret);
    	return ret;
    }

    mini_sscanf(&s_cmd_in[0], "0x%x", &ret);
    lib_ttyportmux__print(TTYSTREAM_control,"\n");
    return ret;
}

/* *******************************************************************
 * static function definitions
 * ******************************************************************/

static void lib_menu__item_help(struct lib_menu__item *_item)
{
    int i;

    lib_ttyportmux__print(TTYSTREAM_control,"Help:\n");

    /* print all menu items */
    for (i = 0; i < LIB_MENU__CONF_LISTSIZE_MENUITEMS; i++)
    {
        if (s_menu_list[i].cb == NULL)
            continue;
        lib_ttyportmux__print(TTYSTREAM_control,"%s - %s\n", s_menu_list[i].cmd, s_menu_list[i].ident);
    }

    return;
}

static void lib_menu__item_setll(struct lib_menu__item *_item)
{
    int verbosity_switch;
    enum log_level verbosity;

    lib_ttyportmux__print(TTYSTREAM_control,"\n");
    lib_ttyportmux__print(TTYSTREAM_control,"Verbosity Levels:\n");
    lib_ttyportmux__print(TTYSTREAM_control,"%s %i\n", "0 LOG_LEVEL_debug", LOG_LEVEL_debug);
    lib_ttyportmux__print(TTYSTREAM_control,"%s %i\n", "1 LOG_LEVEL_debug", LOG_LEVEL_debug);
    lib_ttyportmux__print(TTYSTREAM_control,"%s %i\n", "2 LOG_LEVEL_info", LOG_LEVEL_info);
    lib_ttyportmux__print(TTYSTREAM_control,"%s %i\n", "3 LOG_LEVEL_warning", LOG_LEVEL_warning);
    lib_ttyportmux__print(TTYSTREAM_control,"%s %i\n", "4 LOG_LEVEL_error", LOG_LEVEL_error);
    lib_ttyportmux__print(TTYSTREAM_control,"%s %i\n", "5 LOG_LEVEL_critical", LOG_LEVEL_critical);

    verbosity_switch = lib_menu__get_int__decimal("Verbosity selector");

    switch (verbosity_switch)
    {
    	case 0: verbosity = LOG_LEVEL_debug; break;
    	case 1: verbosity = LOG_LEVEL_debug; break;
    	case 2: verbosity = LOG_LEVEL_info; break;
    	case 3: verbosity = LOG_LEVEL_warning; break;
    	case 4: verbosity = LOG_LEVEL_error; break;
        case 5: verbosity = LOG_LEVEL_critical; break;
    	default: {
    		lib_ttyportmux__print(TTYSTREAM_control,"Invalid verbosity selected\n");
    		return;
    	}
    }
    lib_log__set_level(verbosity);
}

static void lib_menu__item_getll(struct lib_menu__item *_item)
{
	int verbosity;
	verbosity = lib_log__get_level();
	lib_ttyportmux__print(TTYSTREAM_control,"Current verbosity level %u\n", verbosity);
}


static void lib_menu__item_q(struct lib_menu__item *_item)
{
    s_quiet = s_quiet == 0 ? 1 : 0;
    if (s_quiet == 0) {
    	lib_ttyportmux__print(TTYSTREAM_control,"Quiet mode off!\n");
    }
    if (s_quiet == 1) {
    	lib_ttyportmux__print(TTYSTREAM_control,"Quiet mode on!\n");
    }
    return;
}

/* ************************************************************************//**
 * \brief	lib_menu worker thread
 * \param	*_arg	UNUSED (necessary for prototype)
 * \return	NULL in any case
 * ****************************************************************************/
static void *thread_menu(void *_arg)
{
	int ret;
    size_t cmd_in_size = 0;
    int  len_in;
    int  len_cmd;
    int  i;


    ret = lib_ttyportmux__getline(TTYSTREAM_control,&s_cmd_in[0], &cmd_in_size);
    if (ret < EOK) {
    	lib_ttyportmux__print(TTYSTREAM_control,"Menu thread terminated with %i (line %i)\n",ret,__LINE__);
    }

    do
    {
        s_cmd_in[cmd_in_size-1] = '\0';
        /* error */
        if (ret == LIB_MENU__CONF_CMDLENGTH) {
        	lib_ttyportmux__print(TTYSTREAM_control, "Error: Command Too Long\n");
            lib_menu__item_help(NULL);
            continue;
        }

        /* extract input length */
        len_in  = mini_strlen(s_cmd_in);

        /* search for menu item */
        for (i = 0; i < LIB_MENU__CONF_LISTSIZE_MENUITEMS; i++)
        {
            if (s_menu_list[i].cb == NULL)
                continue;


            /* strings are zero length */
            if (len_in == 0)
                continue;

            /* extract command length */
            len_cmd = mini_strlen(s_menu_list[i].cmd);

            /* different lengths of commands */
            if (len_cmd != len_in)
                continue;

            if(memcmp(&s_cmd_in[0], s_menu_list[i].cmd, len_cmd) == 0)
            {
                s_menu_list[i].cb(s_menu_list[i].menu_item_desc);
                lib_ttyportmux__print(TTYSTREAM_control,"<done>\n");
                break;
            }
        }

        /* check: command not found */
        if (i == LIB_MENU__CONF_LISTSIZE_MENUITEMS)
        {
        	lib_ttyportmux__print(TTYSTREAM_control, "Error: Command not found \n");
            lib_menu__item_help(NULL);
        }

        cmd_in_size = sizeof(s_cmd_in);
    } while (ret = lib_ttyportmux__getline(TTYSTREAM_control,&s_cmd_in[0], &cmd_in_size), ret == EOK);

    lib_ttyportmux__print(TTYSTREAM_control,"Menu thread terminated with %i (line %i)\n",ret,__LINE__);

    return NULL;
}


