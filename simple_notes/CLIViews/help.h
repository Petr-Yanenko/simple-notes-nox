//
//  help.h
//  simple_notes
//
//  Created by Petr Yanenko on 1/10/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_help____FILEEXTENSION___
#define simple_notes_help____FILEEXTENSION___

#include "window.h"

#define SIMPLE_NOTES_TYPE_HELP_WINDOW simple_notes_help_window_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesHelpWindow, simple_notes_help_window, SIMPLE_NOTES, HELP_WINDOW, SimpleNotesWindow)

SimpleNotesHelpWindow *simple_notes_help_window_new (SimpleNotesResponder *next);

#endif
