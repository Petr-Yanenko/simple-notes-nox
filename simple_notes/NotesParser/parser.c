//
//  parser.c
//  simple_notes
//
//  Created by Petr Yanenko on 12/4/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include <string.h>
#include <stdlib.h>
#include "parser.h"
#include "notes_string.h"

struct _SimpleNotesParser {
    GObject parent;

    GMarkupParser parser;
    SimpleNotesString *_element;
    GList *_openTags;
    GList *_content;
};

G_DEFINE_TYPE(SimpleNotesParser, simple_notes_parser, G_TYPE_OBJECT)

static void simple_notes_parser_remove_data (SimpleNotesParser *object);
static void simple_notes_parser_start_element (
        GMarkupParseContext *context,
        const gchar         *element_name,
        const gchar        **attribute_names,
        const gchar        **attribute_values,
        gpointer             user_data,
        GError             **error
);
static void simple_notes_parser_end_element (
        GMarkupParseContext *context,
        const gchar         *element_name,
        gpointer             user_data,
        GError             **error
);
static void simple_notes_parser_text (
        GMarkupParseContext *context,
        const gchar         *text,
        gsize                text_len,
        gpointer             user_data,
        GError             **error
);
static void simple_notes_parser_passthrough (
        GMarkupParseContext *context,
        const gchar         *passthrough_text,
        gsize                text_len,
        gpointer             user_data,
        GError             **error
);
static void simple_notes_parser_error (
        GMarkupParseContext *context,
        GError              *error,
        gpointer             user_data
);

static void simple_notes_parser_dispose (GObject *object) {
    SimpleNotesParser *self = SIMPLE_NOTES_PARSER(object);
    simple_notes_parser_remove_data(self);

    G_OBJECT_CLASS(simple_notes_parser_parent_class)->dispose(object);
}

static void simple_notes_parser_class_init (SimpleNotesParserClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = simple_notes_parser_dispose;
}

static void simple_notes_parser_init (SimpleNotesParser *object) {
    object->parser.start_element = simple_notes_parser_start_element;
    object->parser.end_element = simple_notes_parser_end_element;
    object->parser.text = simple_notes_parser_text;
    object->parser.error = simple_notes_parser_error;
    object->parser.passthrough = simple_notes_parser_passthrough;

    object->_element = NULL;
    object->_openTags = NULL;
    object->_content = NULL;
}

SimpleNotesParser *simple_notes_parser_new (void) {
    return g_object_new(SIMPLE_NOTES_TYPE_PARSER, NULL);
}

GList *simple_notes_parser_parse_and_create_content (SimpleNotesParser *object, gchar *xml, gsize len, GError **error) {
    GList *data = NULL;
    GMarkupParseContext *parseContext = g_markup_parse_context_new(&object->parser, 0, object, NULL);
    g_return_val_if_fail(parseContext, NULL);

    gboolean parseResult = g_markup_parse_context_parse(parseContext, xml, len, error);
    if (parseResult) {
        parseResult = g_markup_parse_context_end_parse(parseContext, error);
    }
    g_markup_parse_context_free(parseContext);

    if (parseResult) {
        data = object->_content;
        object->_content = NULL;
    }
    simple_notes_parser_remove_data(object);

    return data;
}

static gboolean simple_notes_parser_check_element_name (SimpleNotesParser *object, const gchar *element_name) {
    return !strcmp(element_name, kStringTag);
}

static void simple_notes_parser_remove_data (SimpleNotesParser *object) {
    if (object->_element) {
        g_object_unref(object->_element);
        object->_element = NULL;
    }
    if (object->_openTags) {
        g_list_free_full(object->_openTags, g_free);
        object->_openTags = NULL;
    }
    if (object->_content) {
        g_list_free_full(object->_content, g_object_unref);
        object->_content = NULL;
    }
}

/* Called for open tags <foo bar="baz"> */
static void simple_notes_parser_start_element (
        GMarkupParseContext *context,
        const gchar         *element_name,
        const gchar        **attribute_names,
        const gchar        **attribute_values,
        gpointer             user_data,
        GError             **error
) {
    SimpleNotesParser *parser = SIMPLE_NOTES_PARSER(user_data);

    gulong len = strlen(element_name) + 1;
    gchar *currentTag = g_malloc(len);
    strcpy(currentTag, element_name);
    parser->_openTags = g_list_append(parser->_openTags, currentTag);

    if (simple_notes_parser_check_element_name(parser, element_name)) {
        glong typeIndex = 0;
        if (
                !attribute_names ||
                        !attribute_names[typeIndex] ||
                        strcmp(attribute_names[typeIndex], kTypeAttribute) ||
                        !attribute_values ||
                        !attribute_values[typeIndex]
                ) {
            *error = g_error_new(kSimpleNotesDomain, 1, "Invalid attribute at string element");
            return;
        }
        glong type = strtol(attribute_values[typeIndex], NULL, 0);
        if (!type) {
            *error = g_error_new(kSimpleNotesDomain, 2, "Invalid type at string element");
            return;
        }
        parser->_element = simple_notes_string_new((SimpleNotesStringTypeIdentifier)type);
    }
}

/* Called for close tags </foo> */
static void simple_notes_parser_end_element (
        GMarkupParseContext *context,
        const gchar         *element_name,
        gpointer             user_data,
        GError             **error
) {
    SimpleNotesParser *parser = SIMPLE_NOTES_PARSER(user_data);
    GList *last = g_list_last(parser->_openTags);
    if (last && !strcmp(last->data, element_name)) {
        gpointer data = last->data;
        parser->_openTags = g_list_delete_link(parser->_openTags, last);
        g_free(data);
    } else {
        *error = g_error_new(kSimpleNotesDomain, 30, "Possibly invalid xml");
        return;
    }

    if (simple_notes_parser_check_element_name(parser, element_name)) {
        parser->_content = g_list_append(parser->_content, g_object_ref(parser->_element));
        g_clear_object(&(parser->_element));
    }
}

/* Called for character data */
/* text is not nul-terminated */
static void simple_notes_parser_text (
        GMarkupParseContext *context,
        const gchar         *text,
        gsize                text_len,
        gpointer             user_data,
        GError             **error
) {
    SimpleNotesParser *parser = SIMPLE_NOTES_PARSER(user_data);
    if (!strcmp(g_list_last(parser->_openTags)->data, kTextTag)) {
        guchar *data = NULL;
        gsize dataLen = 0;
        if (text_len) {
            gsize len = text_len + 1;
            gchar copy[len];
            strncpy(copy, text, text_len);
            copy[text_len] = '\0';
            gchar *base64Data = copy;
            data = g_base64_decode(base64Data, &dataLen);
        }
        GByteArray *buffer = simple_notes_string_get_text(parser->_element);
        gboolean newByteArray = FALSE;
        if (!buffer) {
            buffer = g_byte_array_sized_new(simple_notes_gulong_guint_cast(dataLen));
            newByteArray = TRUE;
        }
        if (data) {
            g_byte_array_append(buffer, data, simple_notes_gulong_guint_cast(dataLen));
            g_free(data);
        }
        simple_notes_string_ref_text(parser->_element, buffer);
        if (newByteArray) {
            g_byte_array_unref(buffer);
        }
    }
}

/* Called for strings that should be re-saved verbatim in this same
 * position, but are not otherwise interpretable.  At the moment
 * this includes comments and processing instructions.
 */
/* text is not nul-terminated. */
static void simple_notes_parser_passthrough (
        GMarkupParseContext *context,
        const gchar         *passthrough_text,
        gsize                text_len,
        gpointer             user_data,
        GError             **error
) {
}

/* Called on error, including one set by other
 * methods in the vtable. The GError should not be freed.
 */
static void simple_notes_parser_error (
        GMarkupParseContext *context,
        GError              *error,
        gpointer             user_data
) {
}
