/******************************************************************
 *  selectiondemo.c: A simple demo of X selection functions
 ******************************************************************/
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/MainW.h>
#include "MenuSupport.h"
#include <X11/Xatom.h>
#include <stdio.h>

Widget  msg, selectionLabel;
Atom    TARGETS, DELETE;
Atom    COMPOUND_TEXT;
Atom    current_type = XA_STRING;
char *GetTextFromXmstring ( XmString string );
extern void wprintf( Widget w, const char *format, ... );
static void SetStringType ( Widget w,
                            XtPointer clientData,
                            XtPointer callData );

static void SetCTType ( Widget w,
                        XtPointer clientData,
                        XtPointer callData );

static void SetTargetType ( Widget w,
                            XtPointer clientData,
                            XtPointer callData );

static void LoseSelection ( Widget w, Atom *selection );

static Boolean ConvertSelection ( Widget w,
                                    Atom *selection,
                                    Atom *target,
                                    Atom *type,
                                    XtPointer *value,
                                    unsigned long *length,
                                    int *format );

static void ToggleType (    Widget w,
                            XtPointer clientData,
                            XtPointer callData );

static void ShowSelection ( Widget w,
                            XtPointer clientData,
                            Atom *selection,
                            Atom *type,
                            XtPointer value,
                            unsigned long *length,
                            int *format );

static void RequestSlection ( Widget w,
                                XtPointer clientData,
                                XtPointer callData );

static void OwnSelection ( Widget w,
                            XtPointer clientData,
                            XtPointer callData );

static MenuDescription selectionPaneDesc[] = {
    { BUTTON, "OwnSelection", OwnSelection },
    { BUTTON, "RequestSelection", RequestSelection },
    { NULL }
}

static MenuDescription typePaneDesc[] = {
    { TOGGLE, "String", SetStringType },
    { TOGGLE, "CompoundText", SetCTType },
    { TOGGLE, "Targets", SetTargetType },
    { NULL }
}

static MenuDescription menuBarDesc[] = {
    { PULLDOWN, "Selection", NULL, NULL, selectionPaneDesc },
    { RADIOPULLDOWN, "Type", NULL, NULL, typePaneDesc },
    { NULL }
}

void main ( int argc, char **argv )
{
    Widget shell, mainWindow;
    XtAppContext app;

    shell = XtAppIntialize ( &app, "Clipboard", NULL, 0,
                            &argc, argv, NULL, NULL, 0 );
    
    COMPOUND_TEXT = XInternAtom ( XtDisplay(shell), "COMPOUND_TEXT", FALSE );
    TARGETS = XInternAtom ( XtDisplay(shell), "TARGETS", FALSE );
    DELETE = XInternAtom ( XtDisplay(shell), "DELETE", FALSE );

    mainWindow = XtCreateManagedWidget ( "mainWindow",
                                        xmMainWindowWidgetClass,
                                        shell, NULL, 0 );
    
    CreateMenu ( MENUBAR, "menuBar", mainWindow, menuBarDesc, NULL );

    selectionLabel = XtCreateManagedWidget ( "label",
                                                xmLabelWidgetClass,
                                                mainWindow, NULL, 0 );
    
    msg = XtCreateManagedWidget ( "msgArea",
                                    xmLabelWidgetClass,
                                    mainWindow, NULL, 0 );
    
    XtVaSetValues ( mainWindow, XmNmessageWindow, msg, NULL );

    XtRealizeWidget ( shell );

    XtAppMainLoop ( app );
}

