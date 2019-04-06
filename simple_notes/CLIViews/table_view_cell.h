//
//  table_view_cell.h
//  simple_notes
//
//  Created by Petr Yanenko on 1/18/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_table_view_cell____FILEEXTENSION___
#define simple_notes_table_view_cell____FILEEXTENSION___

#include "view.h"

typedef enum _SimpleNotesTableViewCellSeparatorStyle {
    SimpleNotesTableViewCellSeparatorStyleNone = 0,
    SimpleNotesTableViewCellSeparatorStyleHorizontal = 1 << 0,
    SimpleNotesTableViewCellSeparatorStyleVertical = 1 << 1
} SimpleNotesTableViewCellSeparatorStyle;

#define SIMPLE_NOTES_TYPE_TABLE_VIEW_CELL simple_notes_table_view_cell_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesTableViewCell, simple_notes_table_view_cell, SIMPLE_NOTES, TABLE_VIEW_CELL, SimpleNotesView)

struct _SimpleNotesTableViewCellClass {
    SimpleNotesViewClass parent;
};

SimpleNotesTableViewCell *simple_notes_table_view_cell_new (void);

SimpleNotesView *simple_notes_table_view_cell_get_content_view (SimpleNotesTableViewCell *object);

SimpleNotesTableViewCellSeparatorStyle simple_notes_table_view_cell_get_separator_style (SimpleNotesTableViewCell *object);
void simple_notes_table_view_cell_assign_separator_style (SimpleNotesTableViewCell *object, SimpleNotesTableViewCellSeparatorStyle style);

#endif
