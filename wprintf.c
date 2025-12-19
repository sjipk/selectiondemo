/*****************************************************
 * 
 *****************************************************/
//#include <varargs.h>
#include <stdarg.h>
#include <stdio.h>
#include <Xm/Xm.h>
#include <Xm/Label.h>

//void wprintf ( va_list )
//    va_dcl
void wprintf ( Widget w, char* format, ... )
{
    va_list args;
    char str[1000];
    Arg wargs[1];
    XmString xmstr;

    va_start ( args, format );

//    w = va_arg ( args, Widget );

    if ( !XtIsSubclass ( w, xmLabelWidgetClass ) ){
        XtError ( "wprintf () requires a Label Widget" );
    }

//    va_arg ( args, char * ) = "   ";

    vsprintf ( str, format, args );

    xmstr = XmStringCreateLtoR ( str, XmFONTLIST_DEFAULT_TAG );
    XtVaSetValues( w, XmNlabelString, xmstr, NULL );
    XmStringFree ( xmstr );

    va_end ( args );
}