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
