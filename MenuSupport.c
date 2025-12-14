/***************************************************************
 *  MenuSupport.c: Convenience functions that make it
 *                  easier to create Motif menus
 ***************************************************************/
#include <Xm/Xm.h>
#include <Xm/Separator.h>
#include <Xm/PushB.h>
#include <Xm/CascadeB.h>
#include <Xm/RowColumn.h>
#include <Xm/Label.h>
#include <Xm/ToggleB.h>
#include "MenuSupport.h"

/*
 *  Declarations of internal functions used in this package
 */

static void PostMenuHandler ( Widget w,
                                XtPointer clientData,
                                XEvent *event,
                                Boolean *flag );

static void AttachPopup ( Widget parent, Widget menu );

static void CreateMenuChildren ( Widget parent,
                                    MenuDescription *desc,
                                    XtPointer defaultClientData );

/*
 *  External interface function, called by applications
 *  to create option menus, popup menus, or menubars.
 */

Widget CreateMenu ( MenuType menuType,
                    char *name,
                    Widget parent,
                    MenuDescription *desc,
                    XtPointer defaultClientData )
{
    Widget w;

    /*
     *
     */

    switch ( menuType )
    {
        case POPUP:

            /*
             *
             */

            w = XmCreatePopupMenu ( parent, name, NULL, 0 );

            AttachPopup ( parent, w );

            CreateMenuChildren ( w, desc, defaultClientData );

            return ( w );

        case MENUBAR:

            /*
             *
             */

            w = XmCreateMenuBar ( parent, name, NULL, 0 );

            CreateMenuChildren ( w, desc, defaultClientData );

            XtManageChild ( w );

            return ( w );
        
        case OPTION:
        {
            /*
             *
             */

            Widget option = XmCreateOptionMenu ( parent, name, NULL, 0 );
            
            w = XmCreatePulldownMenu ( parent, name, NULL, 0 );

            XtVaSetValues ( option, XmNsubMenuId, w, NULL );

            CreateMenuChildren ( w, desc, defaultClientData );

            XtManageChild ( option );

            return ( option );

        }

        default:

            XtError ( "Invalid Menu Type" );
            return ( NULL );
    }
}

static void CreateMenuChildren ( Widget parent,
                                    MenuDescription *desc,
                                    XtPointer defaultClientData )
{
    Widget w;
    int i;

    /*
     *  Create an entry for each item in the menu
     */

    for ( i = 0; desc[i].type != END; i++ )
    {
        switch ( desc[i].type )
        {
            case LABEL:

                w = XtCreateManagedWidget ( desc[i].name,
                                            xmLabelWidgetClass,
                                            parent, NULL, 0 );
                break;

            case BUTTON:

                /*
                 *
                 */

                w = XtCreateManagedWidget ( desc[i].name,
                                            xmPushButtonWidgetClass,
                                            parent, NULL, 0 );
                
                XtAddCallback ( w,
                                XmNactivateCallback,
                                desc[i].func,
                                desc[i].data ? desc[i].data : defaultClientData );
                
                break;

            case TOGGLE:

                /*
                 *
                 */

                w = XtCreateManagedWidget ( desc[i].name,
                                            xmToggleButtonWidgetClass,
                                            parent, NULL, 0 );
                
                XtAddCallback ( w,
                                XmNvalueChangedCallback,
                                desc[i].func,
                                desc[i].data ? desc[i].data : defaultClientData );
                
                break;
            
            case SEPARATOR:

                w = XtCreateManagedWidget ( "separator",
                                            xmSeparatorWidgetClass,
                                            parent, NULL, 0 );
                
                break;

            case PULLDOWN:
            {
                Widget pulldown;

                /*
                 *
                 */

                pulldown = XmCreatePulldownMenu ( parent,
                                                desc[i].name,
                                                NULL, 0 );
                
                w = XtVaCreateManagedWidget ( desc[i].name,
                                                xmCascadeButtonWidgetClass,
                                                parent,
                                                XmNsubMenuId, pulldown,
                                                NULL );

                CreateMenuChildren ( pulldown, desc[i].subMenu,
                                        defaultClientData );
                
                break;
            }

            case HELPPANE:
            {
                Widget pulldown;

                /*
                 *
                 */

                pulldown = XmCreatePulldownMenu ( parent, desc[i].name, NULL, 0 );

                w = XtVaCreateManagedWidget ( desc[i].name,
                                                xmCascadeButtonWidgetClass,
                                                parent,
                                                XmNsubMenuId, pulldown,
                                                NULL );

                XtVaSetValues ( parent, XmNmenuHelpWidget, w, NULL );

                CreateMenuChildren ( pulldown, desc[i].subMenu,
                                        defaultClientData );
                
                break;
            }

            case RADIOPULLDOWN:
            {
                Widget pulldown;
                /*
                 *
                 */
                
                pulldown = XmCreatePulldownMenu ( parent, desc[i].name, NULL, 0 );

                XtVaSetValues ( pulldown, XmNradioBehavior, TRUE, NULL );

                w = XtVaCreateManagedWidget ( desc[i].name,
                                                xmCascadeButtonWidgetClass,
                                                parent,
                                                XmNsubMenuId, pulldown,
                                                NULL );
                                                

                CreateMenuChildren ( pulldown, desc[i].subMenu,
                                        defaultClientData );
                
                break;

            }      
        }
    }
}

static void AttachPopup ( Widget parent, Widget menu )
{
    XtAddEventHandler ( parent, ButtonPressMask, FALSE,
                            PostMenuHandler, menu );
}

static void PostMenuHandler ( Widget w, XtPointer clientData,
                                XEvent *event, Boolean *flag )
{
    int button;
    Widget menu = ( Widget ) clientData;

    if ( event->type == ButtonPress && event->xbutton.button == Button3 )
    {
        /*
         *  Position the menu over the pointer and post the menu.
         */

        XmMenuPosition ( menu, ( XButtonPressedEvent * ) event );
        XtManageChild ( menu );
    }
}