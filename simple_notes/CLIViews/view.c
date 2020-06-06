//
//  view.c
//  simple_notes
//
//  Created by Petr Yanenko on 1/12/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "view.h"

typedef struct {
    SimpleNotesRect _frame;
    SimpleNotesPoint _center;

    SimpleNotesView *_superview;
    GPtrArray *_subviews;

    gboolean _needsLayout;

    gulong _tag;

} SimpleNotesViewPrivate;

SimpleNotesRect simple_notes_make_rect (gulong x, gulong y, gulong width, gulong height) {
    SimpleNotesRect rect;
    rect.origin = simple_notes_make_point(x, y);
    rect.size = simple_notes_make_size(width, height);
    return rect;
}

gboolean simple_notes_rect_equal (SimpleNotesRect left, SimpleNotesRect right) {
    gboolean equal = simple_notes_point_equal(left.origin, right.origin) && simple_notes_size_equal(left.size, right.size);
    return equal;
}

SimpleNotesPoint simple_notes_make_point (gulong x, gulong y) {
    SimpleNotesPoint point;
    point.x = x;
    point.y = y;
    return point;
}

gboolean simple_notes_point_equal (SimpleNotesPoint left, SimpleNotesPoint right) {
    gboolean equal = left.x == right.x && left.y == right.y;
    return equal;
}

SimpleNotesSize simple_notes_make_size (gulong width, gulong height) {
    SimpleNotesSize size;
    size.width = width;
    size.height = height;
    return size;
}
gboolean simple_notes_size_equal (SimpleNotesSize left, SimpleNotesSize right) {
    gboolean equal = left.width == right.width && left.height == right.height;
    return equal;
}

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesView, simple_notes_view, SIMPLE_NOTES_TYPE_RESPONDER)

static void simple_notes_view_layout_subviews (SimpleNotesView *object);
static void simple_notes_view_draw_rect (SimpleNotesView *object, SimpleNotesRect rect);

static void simple_notes_view_real_layout_subviews (SimpleNotesView *object);
static void simple_notes_view_real_draw_rect (SimpleNotesView *object, SimpleNotesRect rect);
static void simple_notes_view_private_assign_frame (SimpleNotesView *object, SimpleNotesViewPrivate *private, SimpleNotesRect frame);
static void simple_notes_view_real_display (SimpleNotesView *object);
static void simple_notes_view_clear_graphics_buffer (SimpleNotesView *object);
static SimpleNotesSize simple_notes_view_real_size_that_fits (SimpleNotesView *object, SimpleNotesSize size);

GPtrArray *simple_notes_view_get_graphics_buffer (void) {
    static GPtrArray *_graphicsBuffer = NULL;
    if (!_graphicsBuffer) {
        _graphicsBuffer = g_ptr_array_new();
    }
    return _graphicsBuffer;
}

void simple_notes_view_dispose (GObject *object) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(SIMPLE_NOTES_VIEW(object));

    if (private->_subviews) {
        g_ptr_array_unref(private->_subviews);
    }

    G_OBJECT_CLASS(simple_notes_view_parent_class)->dispose(object);
}

void simple_notes_view_class_init (SimpleNotesViewClass *klass) {
    GObjectClass *objectClass = G_OBJECT_CLASS(klass);
    objectClass->dispose = simple_notes_view_dispose;

    klass->layout_subviews = simple_notes_view_real_layout_subviews;
    klass->draw_rect = simple_notes_view_real_draw_rect;
    klass->size_that_fits = simple_notes_view_real_size_that_fits;
    klass->display = simple_notes_view_real_display;

    g_type_add_instance_private(SIMPLE_NOTES_TYPE_VIEW, sizeof(SimpleNotesViewPrivate));
}

void simple_notes_view_init (SimpleNotesView *object) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    private->_frame = simple_notes_make_rect(0, 0, 0, 0);
    private->_center = simple_notes_make_point(0, 0);
    private->_superview = NULL;
    private->_subviews = g_ptr_array_new_with_free_func(g_object_unref);
    private->_needsLayout = FALSE;
    private->_tag = kInvalidTag;
}

SimpleNotesView *simple_notes_view_new (SimpleNotesRect frame) {
    SimpleNotesView *view = g_object_new(SIMPLE_NOTES_TYPE_VIEW, NULL);
    simple_notes_view_assign_frame(view, frame);
    return view;
}

SimpleNotesRect simple_notes_view_get_frame (SimpleNotesView *object) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    return private->_frame;
}

void simple_notes_view_assign_frame(SimpleNotesView *object, SimpleNotesRect frame) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    simple_notes_view_private_assign_frame(object, private, frame);
}

SimpleNotesRect simple_notes_view_get_bounds (SimpleNotesView *object) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    return simple_notes_make_rect(0, 0, private->_frame.size.width, private->_frame.size.height);
}

void simple_notes_view_assign_bounds(SimpleNotesView *object, SimpleNotesRect bounds) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    SimpleNotesRect frame = bounds;
    frame.origin = private->_frame.origin;
    simple_notes_view_private_assign_frame(object, private, frame);
}

SimpleNotesPoint simple_notes_view_get_center (SimpleNotesView *object) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    return private->_center;
}

void simple_notes_view_assign_center(SimpleNotesView *object, SimpleNotesPoint center) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    private->_center = center;
    simple_notes_view_set_needs_layout(object);
}

SimpleNotesView *simple_notes_view_get_superview (SimpleNotesView *object) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    return private->_superview;
}

SimpleNotesView **simple_notes_view_get_subviews (SimpleNotesView *object, gulong *count) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    if (count) {
        *count = private->_subviews->len;
    }
    return (SimpleNotesView **)private->_subviews->pdata;
}

void simple_notes_view_add_subview (SimpleNotesView *object, SimpleNotesView *subview) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    SimpleNotesViewPrivate *subviewPrivate = simple_notes_view_get_instance_private(subview);
    g_ptr_array_add(private->_subviews, g_object_ref(subview));
    subviewPrivate->_superview = object;
    simple_notes_view_set_needs_layout(object);
}

void simple_notes_view_remove_from_superview (SimpleNotesView *object) {
    g_return_if_fail(object);
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    SimpleNotesViewPrivate *superviewPrivate = simple_notes_view_get_instance_private(private->_superview);
    private->_superview = NULL;
    gboolean success = g_ptr_array_remove(superviewPrivate->_subviews, object);
    g_warn_if_fail(success);
}

gboolean  simple_notes_view_is_descendant_of_view (SimpleNotesView *object, SimpleNotesView *parent) {
    SimpleNotesViewPrivate *parentPrivate = simple_notes_view_get_instance_private(parent);

    for (glong i = 0; i < parentPrivate->_subviews->len; i++) {
      gpointer subview = g_ptr_array_index(parentPrivate->_subviews, i);
      if (subview == object) {
        return TRUE;
      }
    }

    return FALSE;
}

void simple_notes_view_size_to_fit (SimpleNotesView *object) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    SimpleNotesSize newSize = simple_notes_view_size_that_fits(object, private->_frame.size);
    if (simple_notes_size_equal(private->_frame.size, newSize) == FALSE) {
        SimpleNotesRect newFrame;
        newFrame.origin = private->_frame.origin;
        newFrame.size = newSize;
        simple_notes_view_private_assign_frame(object, private, newFrame);
    }
}

SimpleNotesSize simple_notes_view_size_that_fits (SimpleNotesView *object, SimpleNotesSize size) {
    SimpleNotesViewClass *klass = NULL;
    SN_GET_CLASS_OR_RETURN_VAL(
    object,
    &klass,
    size_that_fits,
    SimpleNotesView,
    SIMPLE_NOTES, VIEW,
    simple_notes_make_size(ULONG_MAX, ULONG_MAX)
    );
    return klass->size_that_fits(object, size);
}

void simple_notes_view_set_needs_layout (SimpleNotesView *object) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    private->_needsLayout = TRUE;
}

void simple_notes_view_layout_if_needed (SimpleNotesView *object) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    if (private->_needsLayout) {
        simple_notes_view_layout_subviews(object);
        private->_needsLayout = FALSE;
    }
    for (glong i = 0; i < private->_subviews->len; i++) {
        simple_notes_view_layout_if_needed(g_ptr_array_index(private->_subviews, i));
    }
}

void simple_notes_view_display (SimpleNotesView *object) {
    SimpleNotesViewClass *klass = NULL;
    SN_GET_CLASS(object, &klass, display, SimpleNotesView, SIMPLE_NOTES, VIEW);
    klass->display(object);
}

void simple_notes_view_initialize_graphics_buffer (SimpleNotesView *object, gulong columnsNumber, gulong linesNumber) {
    GPtrArray *buffer = simple_notes_view_get_graphics_buffer();
    simple_notes_view_clear_graphics_buffer(object);
    for (glong i = 0; i < linesNumber; i++) {
        GString *line = g_string_new_len("", columnsNumber + 1);
        for (gulong j = 0; j < columnsNumber + 1; j++) {
            g_string_overwrite(line, j, " ");
        }
        g_string_overwrite(line, columnsNumber, "\n");
        g_ptr_array_add(buffer, line);
    }
}

GString *simple_notes_view_get_graphics_buffer_line (SimpleNotesView *object, gulong line) {
    if (line < simple_notes_view_get_graphics_buffer()->len) {
        return g_ptr_array_index(simple_notes_view_get_graphics_buffer(), line);
    }
    g_return_val_if_reached(NULL);
}

gchar *simple_notes_view_create_graphics_buffer_content (SimpleNotesView *object) {
    GString *content = g_string_new("\0");
    gchar *returnContent = NULL;
    GPtrArray *buffer = simple_notes_view_get_graphics_buffer();
    for (gulong i = 0; i < buffer->len; i++) {
        g_string_append(content, simple_notes_view_get_graphics_buffer_line(object, i)->str);
    }
    returnContent = g_string_free(content, FALSE);
    return returnContent;
}

gulong simple_notes_view_get_tag (SimpleNotesView *object) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    return private->_tag;
}

void simple_notes_view_assign_tag (SimpleNotesView *object, gulong tag) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    private->_tag = tag;
}

SimpleNotesView *simple_notes_view_with_tag (SimpleNotesView *object, gulong tag) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    if (private->_tag == tag) {
        return object;
    }
    for (glong i = 0; i < private->_subviews->len; i++) {
        SimpleNotesView *foundView = simple_notes_view_with_tag(g_ptr_array_index(private->_subviews, i), tag);
        if (foundView) {
            return foundView;
        }
    }
    return NULL;
}

SimpleNotesPoint simple_notes_view_convert_point_to_view (SimpleNotesView *object, SimpleNotesPoint point, SimpleNotesView *toView) {
    SimpleNotesPoint invalid = simple_notes_make_point(ULONG_MAX, ULONG_MAX);
    g_return_val_if_fail(object != NULL, invalid);
    if (toView == object) {
        return point;
    }
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    SimpleNotesView *superview = private->_superview;
    SimpleNotesView *view = object;
    SimpleNotesPoint newPoint = point;
    while (superview != NULL) {
        SimpleNotesViewPrivate *viewPrivate = simple_notes_view_get_instance_private(view);
        newPoint = simple_notes_make_point(newPoint.x + viewPrivate->_frame.origin.x, newPoint.y + viewPrivate->_frame.origin.y);
        if (superview == toView) {
            return newPoint;
        }
        SimpleNotesViewPrivate *superviewPrivate = simple_notes_view_get_instance_private(superview);
        view = superview;
        superview = superviewPrivate->_superview;
    }
    if (toView == NULL) {
        return newPoint;
    }
    g_return_val_if_reached(invalid);
}

SimpleNotesPoint simple_notes_view_convert_point_from_view (SimpleNotesView *object, SimpleNotesPoint point, SimpleNotesView *fromView) {
    return simple_notes_view_convert_point_to_view(fromView, point, object);
}

SimpleNotesRect simple_notes_view_convert_rect_to_view (SimpleNotesView *object, SimpleNotesRect rect, SimpleNotesView *toView) {
    SimpleNotesPoint origin = rect.origin;
    SimpleNotesPoint newOrigin = simple_notes_view_convert_point_to_view(object, origin, toView);
    SimpleNotesRect newRect = rect;
    newRect.origin = newOrigin;
    if (newOrigin.x == ULONG_MAX && newOrigin.y == ULONG_MAX) {
        newRect.size = simple_notes_make_size(ULONG_MAX, ULONG_MAX);
    }
    return newRect;
}

SimpleNotesRect simple_notes_view_convert_rect_from_view (SimpleNotesView *object, SimpleNotesRect rect, SimpleNotesView *fromView) {
    return simple_notes_view_convert_rect_to_view(fromView, rect, object);
}

static void simple_notes_view_layout_subviews (SimpleNotesView *object) {
    SimpleNotesViewClass *klass = NULL;
    SN_GET_CLASS(object, &klass, layout_subviews, SimpleNotesView, SIMPLE_NOTES, VIEW)
    klass->layout_subviews(object);
}

static void simple_notes_view_draw_rect (SimpleNotesView *object, SimpleNotesRect rect) {
    SimpleNotesViewClass *klass = NULL;
    SN_GET_CLASS(object, &klass, draw_rect, SimpleNotesView, SIMPLE_NOTES, VIEW)
    klass->draw_rect(object, rect);
}

static void simple_notes_view_real_layout_subviews (SimpleNotesView *object) {
}

static void simple_notes_view_real_draw_rect (SimpleNotesView *object, SimpleNotesRect rect){
}

static SimpleNotesSize simple_notes_view_real_size_that_fits (SimpleNotesView *object, SimpleNotesSize size) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    return private->_frame.size;
}

static void simple_notes_view_real_display (SimpleNotesView *object) {
    SimpleNotesViewPrivate *private = simple_notes_view_get_instance_private(object);
    SimpleNotesRect rect = simple_notes_view_convert_rect_to_view(object, simple_notes_view_get_bounds(object), NULL);
    simple_notes_view_draw_rect(object, rect);
    for (glong i = 0; i < private->_subviews->len; i++) {
        simple_notes_view_display(g_ptr_array_index(private->_subviews, i));
    }
}

static void simple_notes_view_private_assign_frame (SimpleNotesView *object, SimpleNotesViewPrivate *private, SimpleNotesRect frame) {
    if (simple_notes_rect_equal(frame, private->_frame) == FALSE) {
        private->_frame = frame;
        simple_notes_view_set_needs_layout(object);
    }
}

static void simple_notes_view_clear_graphics_buffer (SimpleNotesView *object) {
    GString **lines = (GString **) simple_notes_view_get_graphics_buffer()->pdata;
    gulong len = simple_notes_view_get_graphics_buffer()->len;
    g_ptr_array_remove_range(simple_notes_view_get_graphics_buffer(), 0, simple_notes_view_get_graphics_buffer()->len);
    for (glong i = 0; i < len; i++) {
        g_string_free(lines[i], TRUE);
    }
}
