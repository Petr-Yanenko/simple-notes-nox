//
// note_presenter.h
//


#ifndef sn_note_presenter____FILEEXTENSION___
#define sn_note_presenter____FILEEXTENSION___

#include "entity_presenter.h"


#define SN_TYPE_NOTE_PRESENTER sn_note_presenter_get_type()
G_DECLARE_FINAL_TYPE(SNNotePresenter,
		     sn_note_presenter,
		     SN,
		     NOTE_PRESENTER,
		     SNEntityPresenter)


SNNotePresenter *
sn_note_presenter_new(SNIEntityModel *model);


void
sn_note_presenter_fetch(SNNotePresenter *self);

void
sn_note_presenter_move_note(SNNotePresenter *self,
			    guint64 id,
			    guint64 newFolderID);

void
sn_note_presenter_add_note(SNNotePresenter *self);

void
sn_note_presenter_delete_note(SNNotePresenter *self, guint64 id);

void
sn_note_presenter_select_note(SNNotePresenter *self, guint64 id);


#endif
