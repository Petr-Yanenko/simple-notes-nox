//
//  responder_stub.h
//  simple_notes
//
//  Created by Petr Yanenko on 1/9/18.
//  Copyright (c) 2018 Petr Yanenko. All rights reserved.
//

#ifndef simple_notes_responder_stub____FILEEXTENSION___
#define simple_notes_responder_stub____FILEEXTENSION___

#include "responder.h"

#define SIMPLE_NOTES_TYPE_RESPONDER_STUB simple_notes_responder_stub_get_type ()
G_DECLARE_FINAL_TYPE(SimpleNotesResponderStub, simple_notes_responder_stub, SIMPLE_NOTES, RESPONDER_STUB, SimpleNotesResponder)

SimpleNotesResponderStub *simple_notes_responder_stub_new (void);

#endif
