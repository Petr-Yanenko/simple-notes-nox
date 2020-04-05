//
// folder_presenter.h
//


#ifndef sn_folder_presenter____FILEEXTENSION___
#define sn_folder_presenter____FILEEXTENSION___

#include "entity_presenter.h"


#define SN_TYPE_FOLDER_PRESENTER sn_folder_presenter_get_type()
G_DECLARE_FINAL_TYPE(SNFolderPresenter,
		     sn_folder_presenter,
		     SN,
		     FOLDER_PRESENTER,
		     SNEntityPresenter)


SNFolderPresenter *
sn_folder_presenter_new(SNIModel *model);


void
sn_folder_presenter_fetch(SNFolderPresenter *self);

void
sn_folder_presenter_change_title(SNFolderPresenter *self,
				 guint64 id,
				 gchar *title);

void
sn_folder_presenter_add_folder(SNFolderPresenter *self, gchar *title);

void
sn_folder_presenter_delete_folder(SNFolderPresenter *self, guint64 id);

void
sn_folder_presenter_select_folder(SNFolderPresenter *self, guint64 id);


#endif
