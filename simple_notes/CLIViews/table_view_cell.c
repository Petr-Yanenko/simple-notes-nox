//
//  table_view_cell.c
//  simple_notes
//
//  Created by Petr Yanenko on 1/18/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "table_view_cell.h"

typedef struct _SimpleNotesTableViewCellPrivate {
    SimpleNotesView *_contentView;
    SimpleNotesTableViewCellSeparatorStyle _separatorStyle;
} SimpleNotesTableViewCellPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesTableViewCell, simple_notes_table_view_cell, SIMPLE_NOTES_TYPE_VIEW)

static SimpleNotesSize simple_notes_table_view_cell_real_size_that_fits (SimpleNotesView *object, SimpleNotesSize size);
static void simple_notes_table_view_cell_real_draw_rect (SimpleNotesView *object, SimpleNotesRect rect);
static void simple_notes_table_view_cell_real_layout_subviews (SimpleNotesView *object);

static void simple_notes_table_view_cell_dispose (GObject *object) {
    SimpleNotesTableViewCellPrivate *private = simple_notes_table_view_cell_get_instance_private(SIMPLE_NOTES_TABLE_VIEW_CELL(object));
    g_clear_object(&(private->_contentView));
    
    G_OBJECT_CLASS(simple_notes_table_view_cell_parent_class)->dispose(object);
}

void simple_notes_table_view_cell_class_init (SimpleNotesTableViewCellClass *klass) {
    g_type_add_instance_private(SIMPLE_NOTES_TYPE_TABLE_VIEW_CELL, sizeof(SimpleNotesTableViewCellPrivate));

    G_OBJECT_CLASS(klass)->dispose = simple_notes_table_view_cell_dispose;
    
    SimpleNotesViewClass *viewClass = SIMPLE_NOTES_VIEW_CLASS(klass);
    viewClass->size_that_fits = simple_notes_table_view_cell_real_size_that_fits;
    viewClass->draw_rect = simple_notes_table_view_cell_real_draw_rect;
    viewClass->layout_subviews = simple_notes_table_view_cell_real_layout_subviews;
}

void simple_notes_table_view_cell_init (SimpleNotesTableViewCell *object) {
    SimpleNotesTableViewCellPrivate *private = simple_notes_table_view_cell_get_instance_private(object);
    private->_contentView = simple_notes_view_new(simple_notes_make_rect(0, 0, 0, 0));
    simple_notes_view_add_subview(SIMPLE_NOTES_VIEW(object), private->_contentView);
}

SimpleNotesTableViewCell *simple_notes_table_view_cell_new (void) {
    return g_object_new(SIMPLE_NOTES_TYPE_TABLE_VIEW_CELL, NULL);
}

SimpleNotesView *simple_notes_table_view_cell_get_content_view (SimpleNotesTableViewCell *object) {
    SimpleNotesTableViewCellPrivate *private = simple_notes_table_view_cell_get_instance_private(object);
    return private->_contentView;
}

SimpleNotesTableViewCellSeparatorStyle simple_notes_table_view_cell_get_separator_style (SimpleNotesTableViewCell *object) {
    SimpleNotesTableViewCellPrivate *private = simple_notes_table_view_cell_get_instance_private(object);
    return private->_separatorStyle;
}
void simple_notes_table_view_cell_assign_separator_style (SimpleNotesTableViewCell *object, SimpleNotesTableViewCellSeparatorStyle style) {
    SimpleNotesTableViewCellPrivate *private = simple_notes_table_view_cell_get_instance_private(object);
    if (private->_separatorStyle != style) {
        private->_separatorStyle = style;
        simple_notes_view_set_needs_layout(SIMPLE_NOTES_VIEW(object));
    }
}

static SimpleNotesSize simple_notes_table_view_cell_real_size_that_fits (SimpleNotesView *object, SimpleNotesSize size) {
    SimpleNotesTableViewCellPrivate *private = simple_notes_table_view_cell_get_instance_private(SIMPLE_NOTES_TABLE_VIEW_CELL(object));
    simple_notes_view_size_to_fit(private->_contentView);
    SimpleNotesRect frame = simple_notes_view_get_frame(private->_contentView);
    return simple_notes_make_size(frame.origin.x + frame.size.width + 1, frame.origin.y + frame.size.height + 1);
}

static void simple_notes_table_view_cell_real_layout_subviews (SimpleNotesView *object) {
    SimpleNotesTableViewCellPrivate *private = simple_notes_table_view_cell_get_instance_private(SIMPLE_NOTES_TABLE_VIEW_CELL(object));
    SimpleNotesRect bounds = simple_notes_view_get_bounds(SIMPLE_NOTES_VIEW(object));
    g_return_if_fail(bounds.size.width > 0 && bounds.size.height > 0);
    simple_notes_view_assign_frame(private->_contentView, simple_notes_make_rect(0, 0, bounds.size.width - 1, bounds.size.height - 1));
}

static void simple_notes_table_view_cell_real_draw_rect (SimpleNotesView *object, SimpleNotesRect rect) {
    SimpleNotesTableViewCellPrivate *private = simple_notes_table_view_cell_get_instance_private(SIMPLE_NOTES_TABLE_VIEW_CELL(object));
    if (private->_separatorStyle & SimpleNotesTableViewCellSeparatorStyleHorizontal) {
        for (gulong i = rect.origin.x; i < rect.origin.x + rect.size.width; i++) {
            GString *line = simple_notes_view_get_graphics_buffer_line(object, rect.origin.y + rect.size.height - 1);
            g_string_overwrite(line, i, "-");
        }
    }
    if (private->_separatorStyle & SimpleNotesTableViewCellSeparatorStyleVertical) {
        for (gulong i = rect.origin.y; i < rect.origin.y + rect.size.height; i++) {
            GString *line = simple_notes_view_get_graphics_buffer_line(object, i);
            g_string_overwrite(line, rect.origin.x + rect.size.width - 1, "|");
        }
    }
}
