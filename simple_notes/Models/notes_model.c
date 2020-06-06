//
//  notes_model.c
//  simple_notes
//
//  Created by Petr Yanenko on 11/30/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include "notes_model.h"
#include "note.h"
#include "note_presenter.h"
#include <stdio.h>


struct _SNNotesModel {
  SNSelectedListModel _parent;

  guint64 _folderID;
  SNNotePresenter *_pres;
};

G_DEFINE_TYPE(SNNotesModel, sn_notes_model, SN_TYPE_SELECTED_LIST_MODEL)


static void
sn_notes_model_real_fetch(SNListModel *object);


static void
sn_notes_model_class_init(SNNotesModelClass *klass)
{
  SNListModelClass *listModelClass = SN_LIST_MODEL_CLASS(klass);
  listModelClass->fetch = sn_notes_model_real_fetch;
}

static void
sn_notes_model_init(SNNotesModel *object)
{
  object->_folderID = 0;
  object->_pres = sn_note_presenter_new(SN_IENTITY_MODEL(object));
}

SNNotesModel *
sn_notes_model_new(void)
{
  SNNotesModel *instance = g_object_new(SN_TYPE_NOTES_MODEL, NULL);

  return instance;
}

void
sn_notes_model_insert_note(SNNotesModel *object, gchar *content)
{
  sn_note_presenter_add_note(object->_pres);
}

SNLightNote **
sn_notes_model_copy_notes(SNNotesModel *object, guint *out_count)
{
  SNSelectedListModelClass *parent;
  parent = SN_SELECTED_LIST_MODEL_CLASS(sn_notes_model_parent_class);
  SNSelectedListModel *list = SN_SELECTED_LIST_MODEL(object);
  return (SNLightNote **)parent->copy_items(list, out_count);
}

void
sn_notes_model_delete_note(SNNotesModel *object, guint64 identifier)
{
  sn_note_presenter_delete_note(object->_pres, identifier);
}

void
sn_notes_model_select_note(SNNotesModel *object, guint64 identifier)
{
  sn_note_presenter_select_note(object->_pres, identifier);
}

static void
sn_notes_model_real_fetch(SNListModel *object)
{
  SNNotesModel *self = SN_NOTES_MODEL(object);
  g_return_if_fail(self);
  sn_note_presenter_fetch(self->_pres);
}
