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

static void RequestSelection ( Widget w,
                                XtPointer clientData,
                                XtPointer callData );

static void OwnSelection ( Widget w,
                            XtPointer clientData,
                            XtPointer callData );

static MenuDescription selectionPaneDesc[] = {
    { BUTTON, "OwnSelection", OwnSelection },
    { BUTTON, "RequestSelection", RequestSelection },
    END
};

static MenuDescription typePaneDesc[] = {
    { TOGGLE, "String", SetStringType },
    { TOGGLE, "CompoundText", SetCTType },
    { TOGGLE, "Targets", SetTargetType },
    END
};

static MenuDescription menuBarDesc[] = {
    { PULLDOWN, "Selection", NULL, NULL, selectionPaneDesc },
    { RADIOPULLDOWN, "Type", NULL, NULL, typePaneDesc },
    END
};

int main ( int argc, char **argv )
{
    Widget shell, mainWindow;
    XtAppContext app;
    int n;
    Arg args[10];

    shell = XtAppInitialize ( &app, "Clipboard", NULL, 0,
                            &argc, argv, NULL, NULL, 0 );
    
    COMPOUND_TEXT = XInternAtom ( XtDisplay(shell), "COMPOUND_TEXT", FALSE );
    TARGETS = XInternAtom ( XtDisplay(shell), "TARGETS", FALSE );
    DELETE = XInternAtom ( XtDisplay(shell), "DELETE", FALSE );

    n=0;
    XtSetArg(args[n], XmNwidth, 300 ); n++;
    XtSetArg(args[n], XmNheight, 200 ); n++;

    mainWindow = XtCreateManagedWidget ( "mainWindow",
                                        xmMainWindowWidgetClass,
                                        shell, args, n );
    
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

    return 0;
}

static void RequestSelection ( Widget w,
                                XtPointer clientData,
                                XtPointer callData )
{
    XmAnyCallbackStruct *cb = ( XmAnyCallbackStruct* )callData;

    XtGetSelectionValue ( selectionLabel,
                            XA_PRIMARY,
                            current_type,
                            ShowSelection,
                            clientData,
                            cb->event->xbutton.time );
}

static void ShowSelection ( Widget w,
                            XtPointer clientData,
                            Atom *selection,
                            Atom *type,
                            XtPointer value,
                            unsigned long *length,
                            int *format )
{

    if ( *type == COMPOUND_TEXT )
    {
        XtVaSetValues ( selectionLabel, XmNlabelString, XmCvtCTToXmString ( value ), NULL );
        wprintf( msg, "Request Succeeded for selection as Compound Text" );
    }
    else if ( *type == XA_STRING )
    {
        wprintf ( selectionLabel, "%s", value );
        wprintf ( msg, "Request Succeeded for selection as String" );
    }
    else if ( *type == TARGETS )
    {
        int i;
        char buf [1000];

        sprintf( buf, "Length = %d\0", length );

        for ( i=0; i < *length; i++ )
        {
            char *s = XGetAtomName ( XtDisplay ( selectionLabel ), ( ( Atom* ) value ) [i] );
            sprintf ( buf + strlen ( buf ), "\n ( %d ) %s", i, s );
        }

        wprintf ( selectionLabel, "%s", buf );
        wprintf ( msg, "Current Targets listed" );
    }
    else if ( *type == XT_CONVERT_FAIL || *type == None )
    {
        wprintf ( selectionLabel, " " );
        wprintf ( msg, "Request Failed" );
    }
}

static void SetStringType ( Widget w,
                            XtPointer clientData,
                            XtPointer callData )
{
    XmToggleButtonCallbackStruct *cb = ( XmToggleButtonCallbackStruct* ) callData;

    if ( cb->set )
    {
        current_type = XA_STRING;
    }
}

static void SetCTType ( Widget w,
                        XtPointer clientData,
                        XtPointer callData )
{
    XmToggleButtonCallbackStruct *cb = ( XmToggleButtonCallbackStruct* ) callData;

    if ( cb->set )
    {
        current_type = COMPOUND_TEXT;
    }    
}

static void SetTargetType ( Widget w,
                            XtPointer clientData,
                            XtPointer callData )
{
    XmToggleButtonCallbackStruct *cb = ( XmToggleButtonCallbackStruct* ) callData;

    if ( cb->set )
    {
        current_type = TARGETS;
    }
}

static void OwnSelection ( Widget w,
                            XtPointer clientData,
                            XtPointer callData )
{
    Display *dpy = XtDisplay ( selectionLabel );

    if ( XtOwnSelection ( selectionLabel, XA_PRIMARY,
                            XtLastTimestampProcessed ( dpy ),
                            ConvertSelection,
                            LoseSelection, NULL ) )
    {
        wprintf( msg, "Selection Owned" );
    }
}

static Boolean ConvertSelection ( Widget w,
                                    Atom *selection,
                                    Atom *target,
                                    Atom *type,
                                    XtPointer *value,
                                    unsigned long *length,
                                    int *format )
{
    if( *target == TARGETS )
    {
        Atom *targets = ( Atom* ) XtMalloc ( sizeof( Atom ) * 4 );
        targets[0] = XA_STRING;
        targets[1] = COMPOUND_TEXT;
        targets[2] = TARGETS;
        targets[3] = DELETE;

        *value = ( XtPointer ) targets;
        *format = 32;
        *length = 4 * sizeof(Atom) >> 2;
        *type = TARGETS;
        return ( TRUE );
    }

    if( *target == DELETE )
    {

        wprintf ( msg, "Selection Deleted" );
        wprintf ( selectionLabel, "" );
        
        *type = DELETE;
        *value = NULL;
        *length = 0;
        *format = 8;
        return ( TRUE );
    }

    if( *target == COMPOUND_TEXT )
    {
        XmString xmstr;

        XtVaGetValues ( selectionLabel,
                            XmNlabelString, &xmstr,
                            NULL );
        
        *type = COMPOUND_TEXT;
        *value = XmCvtXmStringToCT ( xmstr );
        *length = strlen ( *value );
        *format = 8;
        return ( TRUE );
    }

    if( *target == XA_STRING )
    {
        char *str;
        XmString xmstr;

        XtVaGetValues ( selectionLabel,
                            XmNlabelString, &xmstr,
                            NULL );
        
        *type = XA_STRING;
        *value = GetTextFromXmstring ( xmstr );
        *length = strlen ( *value );
        *format = 8;
        return ( TRUE );
    }

    return ( FALSE );
}

static void LoseSelection ( Widget w, Atom *selection )
{
    wprintf ( msg, "Selection Lost" );
}

char *GetTextFromXmstring ( XmString xmstr )
{
    XmStringContext context;
    char *text;
    XmStringCharSet tag;
    XmStringDirection dir;
    Boolean separator;
    char *buf = NULL;
    int length;

    XmStringInitContext ( &context, xmstr );

    length = 0;
    while ( XmStringGetNextSegment ( context, &text, &tag, &dir, &separator ) )
    {
        if ( text )
        {
            buf = XtRealloc ( buf, length + strlen ( text ) + 1 );

            if( length == 0 )
            {
                strcpy ( buf, text );
            }
            else
            {
                strcat (buf, text );
            }

            length += strlen ( text );
            XtFree ( text );
        }

        if ( separator )
        {
            buf = XtRealloc ( buf, length + 2 );
            strcat ( buf, "\n" );
            length += 2;
        }
    }

    XmStringFreeContext ( context );

    return ( buf );
}


