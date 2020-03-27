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


#endif
