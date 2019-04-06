//
//  view.h
//  simple_notes
//
//  Created by Petr Yanenko on 1/12/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_view____FILEEXTENSION___
#define simple_notes_view____FILEEXTENSION___

#include "responder.h"

typedef struct _SimpleNotesPoint {
    gulong x;
    gulong y;
} SimpleNotesPoint;

typedef struct _SimpleNotesSize {
    gulong width;
    gulong height;
} SimpleNotesSize;

typedef struct _SimpleNotesRect {
    SimpleNotesPoint origin;
    SimpleNotesSize size;

} SimpleNotesRect;

typedef struct _SimpleNotesEdgeInsets {
    gulong top;
    gulong left;
    gulong bottom;
    gulong right;
} SimpleNotesEdgeInsets;

static gulong kInvalidTag = 0;

SimpleNotesRect simple_notes_make_rect (gulong x, gulong y, gulong width, gulong height);
gboolean simple_notes_rect_equal (SimpleNotesRect left, SimpleNotesRect right);

SimpleNotesPoint simple_notes_make_point (gulong x, gulong y);
gboolean simple_notes_point_equal (SimpleNotesPoint left, SimpleNotesPoint right);

SimpleNotesSize simple_notes_make_size (gulong width, gulong height);
gboolean simple_notes_size_equal (SimpleNotesSize left, SimpleNotesSize right);

#define SIMPLE_NOTES_TYPE_VIEW simple_notes_view_get_type ()
G_DECLARE_DERIVABLE_TYPE(SimpleNotesView, simple_notes_view, SIMPLE_NOTES, VIEW, SimpleNotesResponder)

struct _SimpleNotesViewClass {
    SimpleNotesResponderClass parent;

    SimpleNotesSize (*size_that_fits) (SimpleNotesView *object, SimpleNotesSize size);
    void (*display) (SimpleNotesView *object);

    //private
    void (*layout_subviews) (SimpleNotesView *object);
    void (*draw_rect) (SimpleNotesView *object, SimpleNotesRect rect);
};

SimpleNotesView *simple_notes_view_new (SimpleNotesRect frame);

SimpleNotesRect simple_notes_view_get_frame (SimpleNotesView *object);
void simple_notes_view_assign_frame (SimpleNotesView *object, SimpleNotesRect frame);
SimpleNotesRect simple_notes_view_get_bounds (SimpleNotesView *object);
void simple_notes_view_assign_bounds (SimpleNotesView *object, SimpleNotesRect bounds);
SimpleNotesPoint simple_notes_view_get_center (SimpleNotesView *object);
void simple_notes_view_assign_center (SimpleNotesView *object, SimpleNotesPoint center);

SimpleNotesView *simple_notes_view_get_superview (SimpleNotesView *object);
SimpleNotesView **simple_notes_view_get_subviews (SimpleNotesView *object, gulong *count);
void simple_notes_view_add_subview (SimpleNotesView *object, SimpleNotesView *subview);
void simple_notes_view_remove_from_superview (SimpleNotesView *object);
gboolean  simple_notes_view_is_descendant_of_view (SimpleNotesView *object, SimpleNotesView *parent);

void simple_notes_view_size_to_fit (SimpleNotesView *object);
SimpleNotesSize simple_notes_view_size_that_fits (SimpleNotesView *object, SimpleNotesSize size);

void simple_notes_view_set_needs_layout (SimpleNotesView *object);
void simple_notes_view_layout_if_needed (SimpleNotesView *object);

void simple_notes_view_display (SimpleNotesView *object);
void simple_notes_view_initialize_graphics_buffer (SimpleNotesView *object, gulong columnsNumber, gulong linesNumber);
GString *simple_notes_view_get_graphics_buffer_line (SimpleNotesView *object, gulong line);
gchar *simple_notes_view_create_graphics_buffer_content (SimpleNotesView *object);

gulong simple_notes_view_get_tag (SimpleNotesView *object);
void simple_notes_view_assign_tag (SimpleNotesView *object, gulong tag);
SimpleNotesView *simple_notes_view_with_tag (SimpleNotesView *object, gulong tag);

SimpleNotesPoint simple_notes_view_convert_point_to_view (SimpleNotesView *object, SimpleNotesPoint point, SimpleNotesView *toView);
SimpleNotesPoint simple_notes_view_convert_point_from_view (SimpleNotesView *object, SimpleNotesPoint point, SimpleNotesView *fromView);
SimpleNotesRect simple_notes_view_convert_rect_to_view (SimpleNotesView *object, SimpleNotesRect rect, SimpleNotesView *toView);
SimpleNotesRect simple_notes_view_convert_rect_from_view (SimpleNotesView *object, SimpleNotesRect rect, SimpleNotesView *fromView);

#endif
