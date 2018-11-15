/* ****************************************************************************************************
 * lib_menu.c for console print out
 *
 *  compiler:   GNU Tools ARM LINUX
 *  target:     armv6
 *  author:	    Tom
 * ****************************************************************************************************/

/* ****************************************************************************************************/

/*
 *	******************************* change log *******************************
 *  date			user			comment
 * 	07 Juli 2018	Tom				- creation of lib_console.c
 *
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
