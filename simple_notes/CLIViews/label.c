//
//  label.c
//  simple_notes
//
//  Created by Petr Yanenko on 1/22/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include <ctype.h>
#include <stdlib.h>
#include <Block.h>
#include "label.h"
#include <string.h>

typedef gulong (^LineBreakModeStrategy) (GString *text, gulong start, gulong end);

typedef struct _SimpleNotesLabelPrivate {
    GString *_text;
    GString **_lines;
    SimpleNotesLabelLineBreakMode _mode;
    SimpleNotesLabelTextAlignment _alignment;

} SimpleNotesLabelPrivate;

G_DEFINE_TYPE_WITH_PRIVATE(SimpleNotesLabel, simple_notes_label, SIMPLE_NOTES_TYPE_VIEW)

static void simple_notes_label_real_draw_rect (SimpleNotesView *object, SimpleNotesRect rect);
static void simple_notes_label_real_layout_subviews (SimpleNotesView *object);
static GString **simple_notes_label_create_split_text (SimpleNotesLabel *object, SimpleNotesRect bounds);
static SimpleNotesSize simple_notes_label_real_size_that_fits (SimpleNotesView *object, SimpleNotesSize size);
static void simple_notes_label_delete_lines (GString **lines);

void simple_notes_label_dispose (GObject *object) {
    SimpleNotesLabelPrivate *private = simple_notes_label_get_instance_private(SIMPLE_NOTES_LABEL(object));
    if (private->_text) {
        g_string_free(private->_text, TRUE);
    }
    simple_notes_label_delete_lines(private->_lines);
    private->_lines = NULL;
    G_OBJECT_CLASS(simple_notes_label_parent_class)->dispose(object);
}

void simple_notes_label_class_init (SimpleNotesLabelClass *klass) {
    g_type_add_instance_private(SIMPLE_NOTES_TYPE_LABEL, sizeof(SimpleNotesLabelPrivate));

    G_OBJECT_CLASS(klass)->dispose = simple_notes_label_dispose;

    SimpleNotesViewClass *viewClass = SIMPLE_NOTES_VIEW_CLASS(klass);
    viewClass->draw_rect = simple_notes_label_real_draw_rect;
    viewClass->layout_subviews = simple_notes_label_real_layout_subviews;
    viewClass->size_that_fits = simple_notes_label_real_size_that_fits;
}

void simple_notes_label_init (SimpleNotesLabel *object) {
    SimpleNotesLabelPrivate *private = simple_notes_label_get_instance_private(object);
    private->_text = g_string_new("\0");
    private->_lines = NULL;
    private->_alignment = SimpleNotesLabelTextAlignmentLeft;
    private->_mode = SimpleNotesLabelLineBreakModeCharacter;
}

SimpleNotesLabel *simple_notes_label_new (gchar *text) {
    SimpleNotesLabel *label = g_object_new(SIMPLE_NOTES_TYPE_LABEL, NULL);
    simple_notes_label_copy_text(label, text);
    return label;
}

gchar *simple_notes_label_get_copy_text (SimpleNotesLabel *object) {
    SimpleNotesLabelPrivate *private = simple_notes_label_get_instance_private(object);
    return g_strdup(private->_text->str);
}

void simple_notes_label_copy_text (SimpleNotesLabel *object, gchar *text) {
    SimpleNotesLabelPrivate *private = simple_notes_label_get_instance_private(object);
    g_string_erase(private->_text, 0, private->_text->len);
    if (text) {
        g_string_append(private->_text, text);
    }
    simple_notes_view_set_needs_layout(SIMPLE_NOTES_VIEW(object));
}

SimpleNotesLabelTextAlignment simple_notes_label_get_text_alignment (SimpleNotesLabel *object) {
    SimpleNotesLabelPrivate *private = simple_notes_label_get_instance_private(object);
    return private->_alignment;
}

void simple_notes_label_assign_text_alignment (SimpleNotesLabel *object, SimpleNotesLabelTextAlignment alignment) {
    SimpleNotesLabelPrivate *private = simple_notes_label_get_instance_private(object);
    if (private->_alignment != alignment) {
        private->_alignment = alignment;
        simple_notes_view_set_needs_layout(SIMPLE_NOTES_VIEW(object));
    }
}

SimpleNotesLabelLineBreakMode simple_notes_label_get_line_break_mode (SimpleNotesLabel *object) {
    SimpleNotesLabelPrivate *private = simple_notes_label_get_instance_private(object);
    return private->_mode;
}

void simple_notes_label_assign_line_break_mode (SimpleNotesLabel *object, SimpleNotesLabelLineBreakMode mode) {
    SimpleNotesLabelPrivate *private = simple_notes_label_get_instance_private(object);
    if (private->_mode != mode) {
        private->_mode = mode;
        simple_notes_view_set_needs_layout(SIMPLE_NOTES_VIEW(object));
    }
}

static gulong simple_notes_label_check_end (GString *text, gulong end) {
    return end < text->len ? end : text->len;
}

static LineBreakModeStrategy simple_notes_label_create_strategy (SimpleNotesLabel *object) {
    SimpleNotesLabelPrivate *private = simple_notes_label_get_instance_private(object);
    if (private->_mode == SimpleNotesLabelLineBreakModeCharacter) {
        return _Block_copy(^(GString *text, gulong start, gulong end) {
            return simple_notes_label_check_end(text, end);
        });
    } else if (private->_mode == SimpleNotesLabelLineBreakModeWord) {
        return _Block_copy(^(GString *text, gulong start, gulong end) {
            gulong realEnd = simple_notes_label_check_end(text, end);
            gulong index = realEnd - 1;
            if (realEnd == text->len || isspace(text->str[realEnd])) {
                return realEnd;
            }
            while (index > start) {
                if (isspace(text->str[index])) {
                    return index + 1;
                }
                index--;
            }
            return realEnd;
        });
    } else {
        g_return_val_if_reached(NULL);
    }
}

static void simple_notes_label_real_draw_rect (SimpleNotesView *object, SimpleNotesRect rect) {
    SimpleNotesLabelPrivate *private = simple_notes_label_get_instance_private(SIMPLE_NOTES_LABEL(object));
    for (gulong i = rect.origin.y; i < rect.origin.y + rect.size.height; i++) {
        GString *line = simple_notes_view_get_graphics_buffer_line(SIMPLE_NOTES_VIEW(object), i);
        GString *labelLine = private->_lines[i - rect.origin.y];
        if (labelLine == NULL) {
            break;
        }
         gulong index;
         if (private-> _alignment == SimpleNotesLabelTextAlignmentLeft) {
             index = rect.origin.x;
         } else if (private->_alignment == SimpleNotesLabelTextAlignmentRight) {
             index = rect.origin.x + rect.size.width - labelLine->len;
         } else {
             g_return_if_reached();
         }
         g_string_overwrite_len(line, index, labelLine->str, labelLine->len);
    }
}

static void simple_notes_label_real_layout_subviews (SimpleNotesView *object) {
    SimpleNotesLabelPrivate *private = simple_notes_label_get_instance_private(SIMPLE_NOTES_LABEL(object));
    SimpleNotesRect bounds = simple_notes_view_get_bounds (SIMPLE_NOTES_VIEW(object));
    private->_lines = simple_notes_label_create_split_text(SIMPLE_NOTES_LABEL(object), bounds);
}

static GString **simple_notes_label_create_split_text (SimpleNotesLabel *object, SimpleNotesRect bounds) {
    SimpleNotesLabelPrivate *private = simple_notes_label_get_instance_private(object);
    GString *text = private->_text;
    gulong index = 0;
    for (gulong i = 0; i < text->len; i++) {
        gchar character = text->str[i];
        gint space = isspace(character);
        if (space) {
            text->str[i] = ' ';
        }
    }
    GString **lines = (GString **)calloc(1, sizeof(GString *));
    gulong (^lineBreakStrategy) (GString *splitText, gulong start, gulong end) = simple_notes_label_create_strategy(object);
    for (gulong i = 0; i < bounds.size.height && index < text->len; i++) {
        lines = realloc(lines, sizeof(GString *) * (i + 2));
        memset(lines + (i + 1), 0, sizeof(GString *));
        gulong newIndex = index + bounds.size.width;
        newIndex = lineBreakStrategy(text, index, newIndex);
        gsize lineLen = newIndex - index;
        lines[i] = g_string_new(NULL);
        g_string_overwrite_len(lines[i], 0, text->str + index, lineLen);
        index = newIndex;
    }
    _Block_release(lineBreakStrategy);
    return lines;
}

static SimpleNotesSize simple_notes_label_real_size_that_fits (SimpleNotesView *object, SimpleNotesSize size) {
    SimpleNotesRect bounds = simple_notes_make_rect(0, 0, size.width, size.height);
    GString **lines = simple_notes_label_create_split_text(SIMPLE_NOTES_LABEL(object), bounds);
    gulong i = 0;
    while (lines && lines[i]) {
        i++;
    }
    simple_notes_label_delete_lines(lines);
    return simple_notes_make_size(size.width, i);
}

static void simple_notes_label_delete_lines (GString **lines) {
    gulong index = 0;
    if (lines) {
        while (lines[index]) {
            g_string_free(lines[index], TRUE);
            index++;
        }
        g_free(lines);
    }}
