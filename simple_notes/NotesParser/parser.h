//
//  parser.h
//  simple_notes
//
//  Created by Petr Yanenko on 12/4/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_parser____FILEEXTENSION___
#define simple_notes_parser____FILEEXTENSION___

#include "simple_notes.h"

#define SIMPLE_NOTES_TYPE_PARSER simple_notes_parser_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesParser, simple_notes_parser, SIMPLE_NOTES, PARSER, GObject)

SimpleNotesParser *simple_notes_parser_new (void);
GList *simple_notes_parser_parse_and_create_content (SimpleNotesParser *object, gchar *xml, gsize len, GError **error);

#endif
