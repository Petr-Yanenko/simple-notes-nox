//
//  mediator.c
//  simple_notes
//
//  Created by Petr Yanenko on 12/6/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include <string.h>
#include "mediator.h"
#include "folder.h"
#include "note.h"

struct _SimpleNotesMediator {
    GObject parent;

  // SimpleNotesContentModel *_contentModel;
    SimpleNotesNotesModel *_notesModel;
    SimpleNotesFoldersModel *_foldersModel;
};

G_DEFINE_TYPE(SimpleNotesMediator, simple_notes_mediator, G_TYPE_OBJECT)

static void simple_notes_mediator_handle_folders_model_changes (SimpleNotesMediator *object);
static void simple_notes_mediator_handle_notes_model_changes (SimpleNotesMediator *object);
/*static void simple_notes_mediator_handle_content_model_changes (SimpleNotesMediator *object);*/

/*static void simple_notes_mediator_check_content_model (SimpleNotesMediator *object);*/

static void simple_notes_mediator_dispose (GObject *object) {
    SimpleNotesMediator *self = SIMPLE_NOTES_MEDIATOR(object);
    //g_clear_object(&self->_contentModel);
    g_clear_object(&self->_notesModel);
    g_clear_object(&self->_foldersModel);

    G_OBJECT_CLASS(simple_notes_mediator_parent_class)->dispose(object);
}

static void simple_notes_mediator_class_init (SimpleNotesMediatorClass *klass) {
    G_OBJECT_CLASS(klass)->dispose = simple_notes_mediator_dispose;
}

static void simple_notes_mediator_init (SimpleNotesMediator *object) {
  //object->_contentModel = NULL;
    object->_notesModel = NULL;
    object->_foldersModel = NULL;
}

SimpleNotesMediator *simple_notes_mediator_new (void) {
    return g_object_new(SIMPLE_NOTES_TYPE_MEDIATOR, NULL);
}

void simple_notes_mediator_model_changed (SimpleNotesMediator *object, SimpleNotesBaseModel *model) {
    
    if (model == SIMPLE_NOTES_BASE_MODEL(simple_notes_mediator_get_folders_model(object))) {
        simple_notes_mediator_handle_folders_model_changes(object);
    } else if (model == SIMPLE_NOTES_BASE_MODEL(simple_notes_mediator_get_notes_model(object))) {
        simple_notes_mediator_handle_notes_model_changes(object);
    }/* else if (model == SIMPLE_NOTES_BASE_MODEL(simple_notes_mediator_get_content_model(object))) {
        simple_notes_mediator_handle_content_model_changes(object);
	}*/ else {
        g_warn_if_reached();
    }
}

SimpleNotesFoldersModel *simple_notes_mediator_get_folders_model (SimpleNotesMediator *object) {
    if (!(object->_foldersModel)) {
        object->_foldersModel = simple_notes_folders_model_new(object);
        simple_notes_base_model_load_data(SIMPLE_NOTES_BASE_MODEL(object->_foldersModel));
    }
    return object->_foldersModel;
}

SimpleNotesNotesModel *simple_notes_mediator_get_notes_model (SimpleNotesMediator *object) {
    if (!(object->_notesModel)) {
        SimpleNotesFoldersModel *folders = simple_notes_mediator_get_folders_model(object);
        guint64 selectedID = simple_notes_selected_list_model_get_selected_object_id(SIMPLE_NOTES_SELECTED_LIST_MODEL(folders));
        object->_notesModel = simple_notes_notes_model_new(selectedID, object);
        simple_notes_base_model_load_data(SIMPLE_NOTES_BASE_MODEL(object->_notesModel));
    }
    return object->_notesModel;
}

/*SimpleNotesContentModel *simple_notes_mediator_get_content_model (SimpleNotesMediator *object) {
    if (!(object->_contentModel)) {
        SimpleNotesNotesModel *notesModel = simple_notes_mediator_get_notes_model(object);
        guint64 selectedID = simple_notes_selected_list_model_get_selected_object_id(SIMPLE_NOTES_SELECTED_LIST_MODEL(notesModel));
        guint64 folderID = simple_notes_notes_model_get_folder_id(notesModel);
        object->_contentModel = simple_notes_content_model_new(selectedID, folderID, object);
    }
    return object->_contentModel;
    }*/

static void simple_notes_mediator_handle_folders_model_changes (SimpleNotesMediator *object) {
    SimpleNotesFoldersModel *foldersModel = simple_notes_mediator_get_folders_model(object);
    SimpleNotesNotesModel *notesModel = simple_notes_mediator_get_notes_model(object);
    if (!notesModel) return;
    SimpleNotesSelectedListModel *selectedListModel = SIMPLE_NOTES_SELECTED_LIST_MODEL(foldersModel);
    guint64 identifier = simple_notes_selected_list_model_get_selected_object_id(selectedListModel);
    guint64 noteFolderID = simple_notes_notes_model_get_folder_id(notesModel);
    if (identifier != noteFolderID) {
        SimpleNotesBaseModel *baseNotesModel = SIMPLE_NOTES_BASE_MODEL(notesModel);
        simple_notes_notes_model_assign_folder_id(notesModel, identifier);
        simple_notes_base_model_reset(baseNotesModel);
        simple_notes_base_model_load_data(baseNotesModel);

	// simple_notes_mediator_check_content_model(object);
    }
}

static void simple_notes_mediator_update_notes_count (SimpleNotesMediator *object) {
    SimpleNotesFoldersModel *foldersModel = simple_notes_mediator_get_folders_model(object);
    SimpleNotesNotesModel *notesModel = simple_notes_mediator_get_notes_model(object);
    g_return_if_fail(notesModel);
    guint64 folderID = simple_notes_notes_model_get_folder_id(notesModel);
    glong count = simple_notes_list_model_get_count(SIMPLE_NOTES_LIST_MODEL(notesModel));
    SimpleNotesSelectedListModel *selectedListFoldersModel = SIMPLE_NOTES_SELECTED_LIST_MODEL(foldersModel);
    SimpleNotesObject *selectedObject = simple_notes_selected_list_model_find_selected_object(selectedListFoldersModel);
    g_return_if_fail(selectedObject);
    guint64 selectedFolderID = simple_notes_object_get_id(selectedObject);
    SimpleNotesFolder *selectedFolder = SIMPLE_NOTES_FOLDER(selectedObject);
    glong selectedFolderNotesCount = simple_notes_folder_get_count(selectedFolder);
    g_return_if_fail(selectedFolderID == folderID);
    if (count != selectedFolderNotesCount) {
        simple_notes_folder_assign_count(selectedFolder, count);
        glong const savedCount = 1;
        SimpleNotesObject *items[1] = { selectedObject };
        simple_notes_selected_list_model_save_objects(selectedListFoldersModel, items, savedCount);
        simple_notes_base_model_assign_new_data(SIMPLE_NOTES_BASE_MODEL(foldersModel), TRUE);
    }

}

static guint64 simple_notes_mediator_get_selected_note_identifier (SimpleNotesMediator *object) {
    SimpleNotesNotesModel *notesModel = simple_notes_mediator_get_notes_model(object);
    SimpleNotesSelectedListModel *selectedListNotesModel = SIMPLE_NOTES_SELECTED_LIST_MODEL(notesModel);
    guint64 identifier = simple_notes_selected_list_model_get_selected_object_id(selectedListNotesModel);
    return identifier;
}

/*static gboolean simple_notes_mediator_check_content_model_note_id (SimpleNotesMediator *object) {
    SimpleNotesContentModel *contentModel = simple_notes_mediator_get_content_model(object);
    guint64 identifier = simple_notes_mediator_get_selected_note_identifier(object);
    guint64 contentNoteID = simple_notes_content_model_get_note_id(contentModel);
    return identifier == contentNoteID;
    }*/

/*static void simple_notes_mediator_check_content_model (SimpleNotesMediator *object) {
    if (!simple_notes_mediator_check_content_model_note_id(object)) {
        guint64 identifier = simple_notes_mediator_get_selected_note_identifier(object);
        SimpleNotesNotesModel *notesModel = simple_notes_mediator_get_notes_model(object);
        guint64 folderID = simple_notes_notes_model_get_folder_id(notesModel);
        simple_notes_content_model_change_note_id(object->_contentModel, identifier, folderID);
    }
    }*/

static void simple_notes_mediator_handle_notes_model_changes (SimpleNotesMediator *object) {
    simple_notes_mediator_update_notes_count(object);
    //simple_notes_mediator_check_content_model(object);
}

/*static void simple_notes_mediator_handle_content_model_changes (SimpleNotesMediator *object) {
    g_return_if_fail(simple_notes_mediator_check_content_model_note_id(object));
    SimpleNotesNotesModel *notesModel = simple_notes_mediator_get_notes_model(object);
    SimpleNotesContentModel *contentModel = simple_notes_mediator_get_content_model(object);
    guint64 contentNoteID = simple_notes_content_model_get_note_id(contentModel);
    guint count = 0;
    SimpleNotesLightString **content = simple_notes_content_model_copy_items(contentModel, &count);
    gulong maxLen = 1000;
    guint8 buff[maxLen * 4 + 1];
    guint currentLen = 0;
    for (glong i = 0; i < count && currentLen < maxLen; i++) {
        GByteArray *fragment = simple_notes_light_string_get_text(content[i]);
        gulong fragmentLen = g_utf8_strlen((gchar *)fragment->data, fragment->len);
        gulong remainderLen = maxLen - currentLen;
        gulong copyLen = fragmentLen <= remainderLen ? fragmentLen : remainderLen;
        gchar *position = g_utf8_offset_to_pointer((gchar *)buff, currentLen);
        gchar *copyPosition = g_utf8_offset_to_pointer((gchar *)fragment->data, copyLen);
        memcpy(position, fragment->data, copyPosition - (gchar *)fragment->data);
        currentLen += copyLen;
    }
    simple_notes_free_objects_array((gpointer *)content, count);

    g_return_if_fail(currentLen <= maxLen);
    gchar *position = g_utf8_offset_to_pointer((gchar *)buff, currentLen);
    *position = '\0';
    guint len = (guint)strlen((gchar *)buff);
    simple_notes_notes_model_change_note_content(
                                                 notesModel,
                                                 contentNoteID,
                                                 buff,
                                                 len + 1
                                                 );
						 }*/
