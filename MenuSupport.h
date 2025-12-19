/********************************************************************
 *  MenuSupport.h: Definitions for a simple menu package
 ********************************************************************/
#ifndef MENUSUPPORT_H
#define MENUSUPPORT_H
#include <Xm/Xm.h>

typedef enum _menu_type {
    END,
    POPUP,
    MENUBAR,
    OPTION,
    PULLDOWN,
    RADIOPULLDOWN,
    HELPPANE,
    BUTTON,
    LABEL,
    TOGGLE,
    SEPARATOR
} MenuType;

typedef struct _menu_desc {
    MenuType type;
    char *name;
    XtCallbackProc func;
    XtPointer data;
    struct _menu_desc *subMenu;
} MenuDescription;

/*
 *  A function that creates a menu from a MenuDescription structure
 */

Widget CreateMenu ( MenuType menuType,
                    char *name,
                    Widget parent,
                    MenuDescription *desc,
                    XtPointer defaultClientData );

void ContextHelpCallback ( Widget, XtPointer, XtPointer );

#endif