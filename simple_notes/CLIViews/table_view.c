//
//  table_view.c
//  simple_notes
//
//  Created by Petr Yanenko on 1/17/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include <string.h>
#include "table_view.h"

typedef struct _SimpleNotesTableViewPrivate {
    gulong _columnsNumber;
    gulong _rowsNumber;

    gulong _rowHeight;
    gulong _columnWidth;

    gulong (*_height_for_row) (SimpleNotesTableView *object, gulong row);
    gulong (*_width_for_column) (SimpleNotesTableView *object, gulong column);

    SimpleNotesTableViewCell **_cells;

    SimpleNotesTableViewCellSeparatorStyle _separatorStyle;
} SimpleNotesTableViewPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesTableView, simple_notes_table_view, SIMPLE_NOTES_TYPE_VIEW)

static void simple_notes_table_view_delete_cells (SimpleNotesTableView *object);
static gulong simple_notes_table_view_row_height (SimpleNotesTableView *object, gulong row);
static gulong simple_notes_table_view_column_width (SimpleNotesTableView *object, gulong column);
static void simple_notes_table_view_real_layout_subviews (SimpleNotesView *object);
static void simple_notes_table_view_real_draw_rect (SimpleNotesView *object, SimpleNotesRect rect);
static SimpleNotesSize simple_notes_table_view_real_size_that_fits (SimpleNotesView *object, SimpleNotesSize size);
static void simple_notes_table_view_set_separator_style_in_cells (SimpleNotesTableView *object, SimpleNotesTableViewCellSeparatorStyle style);

void simple_notes_table_view_dispose (GObject *object) {
    simple_notes_table_view_delete_cells(SIMPLE_NOTES_TABLE_VIEW(object));

    G_OBJECT_CLASS(simple_notes_table_view_parent_class)->dispose(object);
}

void simple_notes_table_view_class_init (SimpleNotesTableViewClass *klass) {
    g_type_add_instance_private(SIMPLE_NOTES_TYPE_TABLE_VIEW, sizeof(SimpleNotesTableViewPrivate));

    G_OBJECT_CLASS(klass)->dispose = simple_notes_table_view_dispose;

    SimpleNotesViewClass *viewClass = SIMPLE_NOTES_VIEW_CLASS(klass);
    viewClass->layout_subviews = simple_notes_table_view_real_layout_subviews;
    viewClass->draw_rect = simple_notes_table_view_real_draw_rect;
    viewClass->size_that_fits = simple_notes_table_view_real_size_that_fits;
}

void simple_notes_table_view_init (SimpleNotesTableView *object) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);

    private->_columnsNumber = 0;
    private->_rowsNumber = 0;
    private->_columnWidth = 0;
    private->_rowHeight = 0;
    private->_height_for_row = NULL;
    private->_width_for_column = NULL;
    private->_cells = NULL;
    private->_separatorStyle = SimpleNotesTableViewCellSeparatorStyleNone;
}

SimpleNotesTableView *simple_notes_table_view_new (
        SimpleNotesTableViewCell **cells,
        gulong rowsNumber,
        gulong columnsNumber,
        gulong (*width_for_column) (SimpleNotesTableView *object, gulong column),
        gulong (*height_for_row) (SimpleNotesTableView *object, gulong row)
) {
    SimpleNotesTableView *tableView = g_object_new(SIMPLE_NOTES_TYPE_TABLE_VIEW, NULL);
    simple_notes_table_view_copy_cells(tableView, cells, rowsNumber, columnsNumber);
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(tableView);
    private->_width_for_column = width_for_column;
    private->_height_for_row = height_for_row;
    return tableView;
}

SimpleNotesTableViewCell **simple_notes_table_view_get_cells (SimpleNotesTableView *object, gulong *rowsNumber, gulong *columnsNumber) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    if (rowsNumber) {
        *rowsNumber = private->_rowsNumber;
    }
    if (columnsNumber) {
        *columnsNumber = private->_columnsNumber;
    }
    return private->_cells;
}

void simple_notes_table_view_copy_cells (SimpleNotesTableView *object, SimpleNotesTableViewCell **cells, gulong rowsNumber, gulong columnsNumber) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);

    simple_notes_table_view_delete_cells(object);
    
    size_t size = sizeof(SimpleNotesTableViewCell) * rowsNumber * columnsNumber;
    private->_cells = g_malloc(size);
    
    memcpy(private->_cells, cells, size);
    
    private->_rowsNumber = rowsNumber;
    private->_columnsNumber = columnsNumber;

    for (glong i = 0; i < columnsNumber * rowsNumber; i ++) {
        g_object_ref(private->_cells[i]);
        simple_notes_view_add_subview(SIMPLE_NOTES_VIEW(object), SIMPLE_NOTES_VIEW(private->_cells[i]));
    }
    
    simple_notes_table_view_set_separator_style_in_cells(object, private->_separatorStyle);

    simple_notes_view_set_needs_layout(SIMPLE_NOTES_VIEW(object));
}

gulong simple_notes_table_view_get_columns_number (SimpleNotesTableView *object) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    return private->_columnsNumber;
}

gulong simple_notes_table_view_get_rows_number (SimpleNotesTableView *object) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    return private->_rowsNumber;
}

SimpleNotesTableViewCellSeparatorStyle simple_notes_table_view_get_separator_style (SimpleNotesTableView *object) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    return private->_separatorStyle;
}

void simple_notes_table_view_assign_separator_style (SimpleNotesTableView *object, SimpleNotesTableViewCellSeparatorStyle separatorStyle) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    private->_separatorStyle = separatorStyle;
    simple_notes_table_view_set_separator_style_in_cells(object, separatorStyle);
    simple_notes_view_set_needs_layout(SIMPLE_NOTES_VIEW(object));
}

gulong simple_notes_table_view_get_row_height (SimpleNotesTableView *object) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    return private->_rowHeight;
}
void simple_notes_table_view_assign_row_height (SimpleNotesTableView *object, gulong rowHeight) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    private->_rowHeight = rowHeight;
    simple_notes_view_set_needs_layout(SIMPLE_NOTES_VIEW(object));
}

gulong simple_notes_table_view_get_column_width (SimpleNotesTableView *object) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    return private->_columnWidth;
}

void simple_notes_table_view_assign_column_width (SimpleNotesTableView *object, gulong columnWidth) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    private->_columnWidth = columnWidth;
    simple_notes_view_set_needs_layout(SIMPLE_NOTES_VIEW(object));
}

static void simple_notes_table_view_delete_cells(SimpleNotesTableView *object) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    if (private->_cells) {
        for (glong i = 0; i < private->_columnsNumber * private->_rowsNumber; i++) {
            SimpleNotesTableViewCell *cell = private->_cells[i];
            simple_notes_view_remove_from_superview(SIMPLE_NOTES_VIEW(cell));
            g_object_unref(cell);
        }
        g_free(private->_cells);
    }
}

static gulong simple_notes_table_view_row_height (SimpleNotesTableView *object, gulong row) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    if (private->_height_for_row) {
        return private->_height_for_row(object, row);
    }
    return ULONG_MAX;
}

static gulong simple_notes_table_view_column_width (SimpleNotesTableView *object, gulong column) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    if (private->_width_for_column) {
        return private->_width_for_column(object, column);
    }
    return ULONG_MAX;
}

static void simple_notes_table_view_real_layout_subviews (SimpleNotesView *object) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(SIMPLE_NOTES_TABLE_VIEW(object));
    SimpleNotesTableViewCell *previousCell = NULL;
    for (gulong i = 0; i < private->_rowsNumber; i++)
        for (gulong  j = 0; j < private->_columnsNumber; j++) {
            SimpleNotesTableViewCell *cell = private->_cells[i * private->_columnsNumber + j];
            gulong height = simple_notes_table_view_row_height(SIMPLE_NOTES_TABLE_VIEW(object), i);
            gulong width = simple_notes_table_view_column_width(SIMPLE_NOTES_TABLE_VIEW(object), j);
            if (height == ULONG_MAX) {
                height = private->_rowHeight;
            }
            if (width == ULONG_MAX) {
                width = private->_columnWidth;
            }
            gulong x = 0, y = 0;
            if (previousCell) {
                SimpleNotesRect rect = simple_notes_view_get_frame(SIMPLE_NOTES_VIEW(previousCell));
                if (j != 0) {
                    x = rect.origin.x + rect.size.width;
                    y = rect.origin.y;
                } else {
                    y = rect.origin.y + rect.size.height;
                }
            }
            simple_notes_view_assign_frame(SIMPLE_NOTES_VIEW(cell), simple_notes_make_rect(x, y, width, height));

            previousCell = cell;
        }
}

static void simple_notes_table_view_real_draw_rect (SimpleNotesView *object, SimpleNotesRect rect) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(SIMPLE_NOTES_TABLE_VIEW(object));
    for (gulong i = 0; i < private->_columnsNumber * private->_rowsNumber; i++) {
        simple_notes_view_display(SIMPLE_NOTES_VIEW(private->_cells[i]));
    }
}

static gulong simple_notes_table_view_calculate_dimension (
        SimpleNotesTableView *object,
        gulong itemsNumber,
        gulong defaultSize,
        gulong (*get_size) (SimpleNotesTableView *object, gulong index)
) {
    gulong tableDimension = 0;
    for (gulong i = 0; i < itemsNumber; i++) {
        gulong size = get_size(object, i);
        if (size == ULONG_MAX) {
            size = defaultSize;
        }
        tableDimension += size;
    }
    return tableDimension;
}

static SimpleNotesSize simple_notes_table_view_real_size_that_fits (SimpleNotesView *object, SimpleNotesSize size) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(SIMPLE_NOTES_TABLE_VIEW(object));
    gulong tableWidth = simple_notes_table_view_calculate_dimension(
            SIMPLE_NOTES_TABLE_VIEW(object),
            private->_columnsNumber,
            private->_columnWidth,
            simple_notes_table_view_column_width);
    gulong tableHeight = simple_notes_table_view_calculate_dimension(
            SIMPLE_NOTES_TABLE_VIEW(object),
            private->_rowsNumber,
            private->_rowHeight,
            simple_notes_table_view_row_height);
    return simple_notes_make_size(tableWidth, tableHeight);
}

static void simple_notes_table_view_set_separator_style_in_cells (SimpleNotesTableView *object, SimpleNotesTableViewCellSeparatorStyle style) {
    SimpleNotesTableViewPrivate *private = simple_notes_table_view_get_instance_private(object);
    for (gulong i = 0; i < private->_columnsNumber * private->_rowsNumber; i++) {
        SimpleNotesTableViewCell *cell = private->_cells[i];
        if (private->_separatorStyle != SimpleNotesTableViewCellSeparatorStyleNone) {
            SimpleNotesTableViewCellSeparatorStyle style = private->_separatorStyle;
            if (i > private->_columnsNumber * (private->_rowsNumber - 1) - 1) {
                style &= SimpleNotesTableViewCellSeparatorStyleVertical;
            }
            if ((i + 1) % private->_columnsNumber == 0) {
                style &= SimpleNotesTableViewCellSeparatorStyleHorizontal;
            }
            simple_notes_table_view_cell_assign_separator_style(cell, style);
        }
    }
}
