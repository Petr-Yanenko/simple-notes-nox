//
//  view_utility.c
//  simple_notes
//
//  Created by Petr Yanenko on 4/1/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "view_utility.h"
#include <stdint.h>

gulong simple_notes_height_for_row (SimpleNotesTableView *object, gulong row, gulong column, gulong width) {
    gulong rowsNumber = 0, columnsNumber = 0;
    SimpleNotesTableViewCell **cells = simple_notes_table_view_get_cells(object, &rowsNumber, &columnsNumber);
    SimpleNotesTableViewCell *contentCell = cells[row * columnsNumber + column];
    SimpleNotesSize size = simple_notes_make_size(width, ULONG_MAX);
    SimpleNotesSize contentSize = simple_notes_view_size_that_fits(SIMPLE_NOTES_VIEW(contentCell), size);
    gulong height = contentSize.height > 1 ? contentSize.height : 2;
    return height;
}
