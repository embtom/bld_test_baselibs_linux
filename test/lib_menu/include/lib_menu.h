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

#ifndef _LIB_MENU_H_
#define _LIB_MENU_H_

#ifdef __cplusplus
extern "C" {
#endif


/* *******************************************************************
 * includes
 * ******************************************************************/


/* *******************************************************************
 * defines
 * ******************************************************************/

/* the list size for menu items */
#define LIB_MENU__CONF_LISTSIZE_MENUITEMS               30
#define LIB_MENU__CONF_CMDLENGTH                        50

#define M_LIB_MENU__LIST_INITIALZER(__cmd, __ident, __cb)	\
{															\
	.cmd = __cmd,											\
	.ident = __ident,										\
	.menu_item_desc	= NULL,									\
	.cb = __cb												\
}

/* *******************************************************************
 * custom data types (e.g. enumerations, structures, unions)
 * ******************************************************************/

struct lib_menu__item;
/* a function prototype for a menu-call */
typedef void (*menu_cb)(struct lib_menu__item *_item);

/* register function for menu items */
struct lib_menu__item
{
    const char *cmd;
    const char *ident;
    struct lib_menu__item *menu_item_desc;
    menu_cb    cb;
};

/* *******************************************************************
 * function declarations
 * ******************************************************************/

int lib_menu__init(void);

int lib_menu__cleanup(void);

int lib_menu__add_item(struct lib_menu__item *_item);

int   lib_menu__show_message(const char *_msg, ...);
float lib_menu__get_float(const char *_title);
int   lib_menu__get_int__decimal(const char *_title);
int   lib_menu__get_int__hex(const char *_title);

#ifdef __cplusplus
}
#endif

#endif /* _LIB_MENU_H_ */
