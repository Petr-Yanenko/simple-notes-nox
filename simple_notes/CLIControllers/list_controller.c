//
//  list_controller.c
//  simple_notes
//
//  Created by Petr Yanenko on 3/25/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#include "list_controller.h"
#include "commands.h"
#include <string.h>
#include <stdlib.h>

G_DEFINE_TYPE(SimpleNotesListController, simple_notes_list_controller, SIMPLE_NOTES_TYPE_BASE_CONTROLLER)

static gchar **simple_notes_list_controller_real_create_options_names (SimpleNotesBaseController *object);
static gchar ***simple_notes_list_controller_real_create_required_options_names (SimpleNotesBaseController *object);
static gboolean simple_notes_list_controller_real_check_value_for_option (SimpleNotesBaseController *object, gchar *option, gchar *value);

void simple_notes_list_controller_class_init (SimpleNotesListControllerClass *klass) {
    SimpleNotesBaseControllerClass *baseClass = SIMPLE_NOTES_BASE_CONTROLLER_CLASS(klass);
    baseClass->create_options_names = simple_notes_list_controller_real_create_options_names;
    baseClass->create_required_options_names = simple_notes_list_controller_real_create_required_options_names;
    baseClass->check_value_for_option = simple_notes_list_controller_real_check_value_for_option;
}

void simple_notes_list_controller_init (SimpleNotesListController *object) {}

static gchar **simple_notes_list_controller_real_create_options_names (SimpleNotesBaseController *object) {
    gulong optionsNumber = 6;
    gchar **buff = g_malloc(sizeof(gchar *) * optionsNumber);
    buff[0] = simple_notes_create_string(kAllOption);
    buff[1] = simple_notes_create_string(kInsertOption);
    buff[2] = simple_notes_create_string(kDeleteOption);
    buff[3] = simple_notes_create_string(kSelectOption);
    buff[4] = simple_notes_create_string(kEditOption);
    buff[optionsNumber - 1] = NULL;
    return buff;
}

static gchar ***simple_notes_list_controller_real_create_required_options_names (SimpleNotesBaseController *object) {
    gulong optionsNumber = 2;
    gulong itemsNumber = 6;
    gulong len = sizeof(gchar **) * itemsNumber;
    gchar ***buff = g_malloc(len);
    for (gulong i = 0; i < itemsNumber - 1; i++) {
        gulong optionLen = sizeof(gchar *) * optionsNumber;
        buff[i] = g_malloc(optionLen);
        memset(buff[i], 0, optionLen);
    }
    buff[0][0] = simple_notes_create_string(kAllOption);
    buff[1][0] = simple_notes_create_string(kInsertOption);
    buff[2][0] = simple_notes_create_string(kDeleteOption);
    buff[3][0] = simple_notes_create_string(kSelectOption);
    buff[4][0] = simple_notes_create_string(kEditOption);
    buff[5] = NULL;
    return buff;
}

static gboolean simple_notes_list_controller_real_check_value_for_option (SimpleNotesBaseController *object, gchar *option, gchar *value) {
    if ((g_strcmp0(option, kAllOption) == 0 || g_strcmp0(option, kEditOption) == 0) && g_strcmp0(value, NULL) == 0) {
        return TRUE;
    } else if ((g_strcmp0(option, kDeleteOption) == 0 ||
            g_strcmp0(option, kSelectOption) == 0) &&
               value) {
        while ((*value) != 0) {
            if (!g_ascii_isdigit((*value))) {
                return FALSE;
            }
            value++;
        }
        return TRUE;
    } else if (g_strcmp0(option, kInsertOption) == 0) {
        return TRUE;
    }
    return FALSE;
}
