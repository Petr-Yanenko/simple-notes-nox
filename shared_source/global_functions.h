//
//  global_functions.h
//  simple_notes
//
//  Created by Petr Yanenko on 12/16/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_global_functions_h
#define simple_notes_global_functions_h

#include <gio.h>

extern gchar *const kFile;
extern gchar *const kNoteFolder;

extern GQuark const kSimpleNotesDomain;

extern long const kLongLongSymbols;
extern long const kSelectedSymbols;

extern gchar *const kFolderPathFormat;
extern gchar *const kNotePathFormat;
extern glong const kFolderPathSymbols;
extern glong const kNotePathSymbols;

#define SIMPLE_NOTES_CREATE_ERROR(error, errorCode, format, argsNumber, args) {\
if (error) {\
GString *description = g_string_new("\0");\
g_string_append_vprintf(description, format, args);\
*error = g_error_new(kSimpleNotesDomain, errorCode, description->str, NULL);\
g_string_free(description, TRUE);\
}\
}

#define SIMPLE_NOTES_PRINT_ERROR(error) {\
if (error && (*error)) {\
g_printerr("\n%s %d %d\n", (*error)->message, (*error)->code, (*error)->domain);\
}\
}

#define SIMPLE_NOTES_CLEAR_ERROR(error) {\
if (error) {\
g_clear_error(error);\
}\
}

#define SIMPLE_NOTES_TRY(expression, error) {\
if (!expression) {\
SIMPLE_NOTES_PRINT_ERROR(error)\
return 0;\
}\
}

#define SIMPLE_NOTES_TRY_AND_CLEAR_ERROR(expression, error) {\
if (!expression) {\
SIMPLE_NOTES_PRINT_ERROR(error)\
SIMPLE_NOTES_CLEAR_ERROR(error)\
return 0;\
}\
}

#define SIMPLE_NOTES_TRY_WITHOUT_ERROR(expression) {\
if (!expression)\
return 0;\
}

#define SIMPLE_NOTES_CHECK_VIRTUAL_FUNC(object, outKlass, func_name, ModuleObjectName, MODULE, OBJECT_NAME, CLASS_OR_IFACE) {\
g_return_if_fail(MODULE##_IS_##OBJECT_NAME(object));\
g_return_if_fail(outKlass);\
*outKlass = MODULE##_##OBJECT_NAME##_GET_##CLASS_OR_IFACE(object);\
/* if the method is purely virtual, then it is a good idea to\
* check that it has been overridden before calling it, and,\
* depending on the intent of the class, either ignore it silently\
* or warn the user.\
*/\
g_return_if_fail(*outKlass != NULL);\
g_return_if_fail((*outKlass)->func_name != NULL);\
}

#define SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC(object, outKlass, func_name, ModuleObjectName, MODULE, OBJECT_NAME) {\
SIMPLE_NOTES_CHECK_VIRTUAL_FUNC(object, outKlass, func_name, ModuleObjectName, MODULE, OBJECT_NAME, CLASS)\
}

#define SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC(object, outKlass, func_name, ModuleObjectName, MODULE, OBJECT_NAME) {\
SIMPLE_NOTES_CHECK_VIRTUAL_FUNC(object, outKlass, func_name, ModuleObjectName, MODULE, OBJECT_NAME, IFACE)\
}

#define SIMPLE_NOTES_CHECK_VIRTUAL_FUNC_WITH_RETURN_VAL(\
object, outKlass, func_name, ModuleObjectName, MODULE, OBJECT_NAME, CLASS_OR_IFACE, returnVal\
) {\
g_return_val_if_fail(MODULE##_IS_##OBJECT_NAME(object), returnVal);\
g_return_val_if_fail(outKlass, returnVal);\
*outKlass = MODULE##_##OBJECT_NAME##_GET_##CLASS_OR_IFACE(object);\
g_return_val_if_fail(*outKlass != NULL, returnVal);\
g_return_val_if_fail((*outKlass)->func_name != NULL, returnVal);\
}

#define SIMPLE_NOTES_CHECK_VIRTUAL_CLASS_FUNC_WITH_RETURN_VAL(\
object, outKlass, func_name, ModuleObjectName, MODULE, OBJECT_NAME, returnVal\
) {\
SIMPLE_NOTES_CHECK_VIRTUAL_FUNC_WITH_RETURN_VAL(object, outKlass, func_name, ModuleObjectName, MODULE, OBJECT_NAME, CLASS, returnVal)\
}

#define SIMPLE_NOTES_CHECK_VIRTUAL_IFACE_FUNC_WITH_RETURN_VAL(\
object, outKlass, func_name, ModuleObjectName, MODULE, OBJECT_NAME, returnVal\
) {\
SIMPLE_NOTES_CHECK_VIRTUAL_FUNC_WITH_RETURN_VAL(object, outKlass, func_name, ModuleObjectName, MODULE, OBJECT_NAME, IFACE, returnVal)\
}

gboolean simple_notes_trash_file(gchar *fileName);
void simple_notes_print_byte_array (GByteArray *array, GString *string, gchar *title);
void simple_notes_copy (GType type, void (setter)(GValue *value), void (getter)(GValue *value));
void simple_notes_set_copy_byte_array (GByteArray *array, GByteArray **variable);
GByteArray *simple_notes_get_copy_byte_array (GByteArray *variable);
guint simple_notes_gulong_guint_cast (gulong value);
void simple_notes_create_error (GError **pError, gint code, gchar *const format, gint argsNumber, ...);
gchar *simple_notes_create_string (gchar *const string);
void simple_notes_print_guint64_value (gchar *buff, guint64 value);
void simple_notes_print_boolean_value (gchar *buff, gboolean value);
void simple_notes_print_long_value (gchar *buff, glong value);
void simple_notes_free_objects_array(gpointer *array, gulong count);

#endif
