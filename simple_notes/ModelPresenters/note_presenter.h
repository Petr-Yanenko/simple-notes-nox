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


#endif
