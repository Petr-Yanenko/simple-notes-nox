//
//  label.h
//  simple_notes
//
//  Created by Petr Yanenko on 1/22/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_label____FILEEXTENSION___
#define simple_notes_label____FILEEXTENSION___

#include "view.h"

typedef enum _SimpleNotesLabelTextAlignment {
    SimpleNotesLabelTextAlignmentLeft,
    SimpleNotesLabelTextAlignmentRight
} SimpleNotesLabelTextAlignment;

typedef enum _SimpleNotesLabelLineBreakMode {
    SimpleNotesLabelLineBreakModeCharacter,
    SimpleNotesLabelLineBreakModeWord
} SimpleNotesLabelLineBreakMode;

#define SIMPLE_NOTES_TYPE_LABEL simple_notes_label_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesLabel, simple_notes_label, SIMPLE_NOTES, LABEL, SimpleNotesView)

struct _SimpleNotesLabelClass {
    SimpleNotesViewClass parent;
};

SimpleNotesLabel *simple_notes_label_new (gchar *text);

gchar *simple_notes_label_get_copy_text (SimpleNotesLabel *object);
void simple_notes_label_copy_text (SimpleNotesLabel *object, gchar *text);

SimpleNotesLabelTextAlignment simple_notes_label_get_text_alignment (SimpleNotesLabel *object);
void simple_notes_label_assign_text_alignment (SimpleNotesLabel *object, SimpleNotesLabelTextAlignment alignment);

SimpleNotesLabelLineBreakMode simple_notes_label_get_line_break_mode (SimpleNotesLabel *object);
void simple_notes_label_assign_line_break_mode (SimpleNotesLabel *object, SimpleNotesLabelLineBreakMode mode);

#endif
