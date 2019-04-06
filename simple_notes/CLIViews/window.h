//
//  window.h
//  simple_notes
//
//  Created by Petr Yanenko on 1/16/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_window____FILEEXTENSION___
#define simple_notes_window____FILEEXTENSION___

#include "view.h"

#define SIMPLE_NOTES_TYPE_WINDOW simple_notes_window_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesWindow, simple_notes_window, SIMPLE_NOTES, WINDOW, SimpleNotesView)

struct _SimpleNotesWindowClass {
    SimpleNotesViewClass parent;
};

SimpleNotesWindow *simple_notes_window_new (SimpleNotesResponder *next);

gboolean simple_notes_window_is_active (SimpleNotesWindow *object);
void simple_notes_window_assign_active (SimpleNotesWindow *object, gboolean isActive);

#endif
