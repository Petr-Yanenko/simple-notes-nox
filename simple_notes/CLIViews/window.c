//
//  window.c
//  simple_notes
//
//  Created by Petr Yanenko on 1/16/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "window.h"

typedef struct _SimpleNotesWindowPrivate {
    gboolean _isActive;
} SimpleNotesWindowPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesWindow, simple_notes_window, SIMPLE_NOTES_TYPE_VIEW)

static void simple_notes_window_real_display (SimpleNotesView *object);
static SimpleNotesSize simple_notes_window_real_size_that_fits (SimpleNotesView *object, SimpleNotesSize size);

void simple_notes_window_class_init (SimpleNotesWindowClass *klass) {
    g_type_add_instance_private(SIMPLE_NOTES_TYPE_WINDOW, sizeof(SimpleNotesWindowPrivate));

    SimpleNotesViewClass *viewClass = SIMPLE_NOTES_VIEW_CLASS(klass);
    viewClass->display = simple_notes_window_real_display;
    viewClass->size_that_fits = simple_notes_window_real_size_that_fits;
}

void simple_notes_window_init (SimpleNotesWindow *object) {
    SimpleNotesWindowPrivate *private = simple_notes_window_get_instance_private(object);
    private->_isActive = FALSE;
}

SimpleNotesWindow *simple_notes_window_new (SimpleNotesResponder *next) {
    SimpleNotesResponder *window = g_object_new(SIMPLE_NOTES_TYPE_WINDOW, NULL);
    simple_notes_responder_set_ref_next_responder(window, next);
    return SIMPLE_NOTES_WINDOW(window);
}

gboolean simple_notes_window_is_active (SimpleNotesWindow *object) {
    SimpleNotesWindowPrivate *private = simple_notes_window_get_instance_private(object);
    return private->_isActive;
}

void simple_notes_window_assign_active (SimpleNotesWindow *object, gboolean isActive) {
    SimpleNotesWindowPrivate *private = simple_notes_window_get_instance_private(object);
    private->_isActive = isActive;
}

static SimpleNotesSize simple_notes_window_real_size_that_fits (SimpleNotesView *object, SimpleNotesSize size) {
    gulong maxWidth = 0;
    gulong maxHeight = 0;
    gulong count = 0;
    SimpleNotesView **subviews = simple_notes_view_get_subviews(object, &count);
    for (gulong i = 0; i < count; i++) {
        SimpleNotesView *item = subviews[i];
        simple_notes_view_size_to_fit(item);
        SimpleNotesRect frame = simple_notes_view_get_frame(item);
        gulong maxX = frame.origin.x + frame.size.width;
        gulong maxY = frame.origin.y + frame.size.height;
        if (maxWidth < maxX) {
            maxWidth = maxX;
        }
        if (maxHeight < maxY) {
            maxHeight = maxY;
        }
    }
    return simple_notes_make_size(maxWidth, maxHeight);
}

static void simple_notes_window_real_display (SimpleNotesView *object) {
    SimpleNotesWindow *window = SIMPLE_NOTES_WINDOW(object);
    g_return_if_fail(window);
    
    SimpleNotesWindowPrivate *private = simple_notes_window_get_instance_private(window);
    if (private->_isActive) {
        SimpleNotesRect bounds = simple_notes_view_get_bounds(object);
        simple_notes_view_initialize_graphics_buffer(object, bounds.size.width, bounds.size.height);
        SIMPLE_NOTES_VIEW_CLASS(simple_notes_window_parent_class)->display(object);
        gchar *content = simple_notes_view_create_graphics_buffer_content(object);
        g_print("\n%s\n", content);
        g_free(content);
    }
}
