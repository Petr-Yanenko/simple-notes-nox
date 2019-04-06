//
//  table_view.h
//  simple_notes
//
//  Created by Petr Yanenko on 1/17/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_table_view____FILEEXTENSION___
#define simple_notes_table_view____FILEEXTENSION___

#include "table_view_cell.h"

#define SIMPLE_NOTES_TYPE_TABLE_VIEW simple_notes_table_view_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesTableView, simple_notes_table_view, SIMPLE_NOTES, TABLE_VIEW, SimpleNotesView)

struct _SimpleNotesTableViewClass {
    SimpleNotesViewClass parent;
};

SimpleNotesTableView *simple_notes_table_view_new (
        SimpleNotesTableViewCell **cells,
        gulong rowsNumber,
        gulong columnsNumber,
        gulong (*width_for_column) (SimpleNotesTableView *object, gulong column),
        gulong (*height_for_row) (SimpleNotesTableView *object, gulong row)
);

SimpleNotesTableViewCell **simple_notes_table_view_get_cells (SimpleNotesTableView *object, gulong *rowsNumber, gulong *columnsNumber);
void simple_notes_table_view_copy_cells (SimpleNotesTableView *object, SimpleNotesTableViewCell **cells, gulong rowsNumber, gulong columnsNumber);

gulong simple_notes_table_view_get_columns_number (SimpleNotesTableView *object);
gulong simple_notes_table_view_get_rows_number (SimpleNotesTableView *object);

gulong simple_notes_table_view_get_row_height (SimpleNotesTableView *object);
void simple_notes_table_view_assign_row_height (SimpleNotesTableView *object, gulong rowHeight);

gulong simple_notes_table_view_get_column_width (SimpleNotesTableView *object);
void simple_notes_table_view_assign_column_width (SimpleNotesTableView *object, gulong columnWidth);

SimpleNotesTableViewCellSeparatorStyle simple_notes_table_view_get_separator_style (SimpleNotesTableView *object);
void simple_notes_table_view_assign_separator_style (SimpleNotesTableView *object, SimpleNotesTableViewCellSeparatorStyle separatorStyle);

#endif
