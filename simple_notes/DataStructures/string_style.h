//
//  symbol_style.h
//  simple_notes
//
//  Created by Petr Yanenko on 12/4/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_string_style____FILEEXTENSION___
#define simple_notes_string_style____FILEEXTENSION___

#include <glib-object.h>

#define SIMPLE_NOTES_TYPE_STRING_STYLE simple_notes_string_style_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesStringStyle, simple_notes_string_style, SIMPLE_NOTES, STRING_STYLE, GObject)

struct _SimpleNotesStringStyleClass {
    GObjectClass parent;
};

#endif
