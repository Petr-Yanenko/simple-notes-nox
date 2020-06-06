//
//  main.c
//  simple_notes
//
//  Created by Petr Yanenko on 10/10/17.
//  Copyright (c) 2017 Petr Yanenko. All rights reserved.
//

#include <locale.h>
#include <string.h>
#include "simple_notes.h"
#include "application.h"
#include "responder_stub.h"
#include "help.h"
#include "folder_window.h"
#include "note_window.h"
#include "folders_model.h"
#include "notes_model.h"


gint32
sn_command_line(GApplication *application, GApplicationCommandLine *cmdline)
{
  SimpleNotesResponderStub *stub = simple_notes_responder_stub_new ();
  SNNotesModel *notes = sn_notes_model_new();
  SimpleNotesNoteWindow *notesWindow
    = simple_notes_note_window_new (SIMPLE_NOTES_RESPONDER (stub), notes);
  SNFoldersModel *folders = sn_folders_model_new();
  SimpleNotesFolderWindow *foldersWindow
    = simple_notes_folder_window_new (SIMPLE_NOTES_RESPONDER (notesWindow),
                                      folders);
  SimpleNotesHelpWindow *help
    = simple_notes_help_window_new (SIMPLE_NOTES_RESPONDER (foldersWindow));
  SimpleNotesApplication *notesApplication
    = simple_notes_application_new (SIMPLE_NOTES_RESPONDER (help));

  GInputStream *stdStream = g_application_command_line_get_stdin (cmdline);
  gssize const buffSize = 1000;
  gchar buff[buffSize];

  g_print ("\nType help to show commands list\n");
  while (simple_notes_application_get_proceed (notesApplication))
    {
      GError *error = NULL;
      memset (buff, '\0', buffSize);
      g_print ("\nsimple-notes> ");
      gsize symbolsMax = buffSize - 1;
      //        gchar buff[] = {'n','o','t','e',' ','-','-','a','l','l','\0'};
      gssize count = g_input_stream_read (stdStream,
					  buff,
					  symbolsMax,
					  NULL,
					  &error);
      /*buff[0] = 'f';buff[1] = 'o';buff[2] = 'l';buff[3] = 'd';buff[4] = 'e';buff[5] = 'r';buff[6] = ' ';buff[7] = '-';buff[8] = '-';buff[9] = 'i';buff[10] = 'n';buff[11] = 's';buff[12] = 'e';buff[13] = 'r';buff[14] = 't';buff[15] = ' ';
	for (; count < buffSize; count++) {
	buff[count] = 't';
	}*/
   gboolean success = count > -1;
      if (success)
	{
	  if (!count)
	    {
	      count = symbolsMax;
	    }
	  SimpleNotesEvent *event = simple_notes_event_new (buff,
							    count,
							    &error);
	  success = event != NULL;
	  if (success)
	    {
	      simple_notes_responder_handle_event
		(SIMPLE_NOTES_RESPONDER (notesApplication), event);
	      g_object_unref (event);
	    }
	}
      if (!success)
	{
	  simple_notes_responder_handle_error
	    (SIMPLE_NOTES_RESPONDER (notesApplication), error);
	}
      g_clear_error (&error);
    }

  g_object_unref(notesApplication);
  g_object_unref(stub);
  g_object_unref(help);
  g_object_unref(foldersWindow);
  g_object_unref(notesWindow);
  g_object_unref(notes);
  g_object_unref(folders);

  return 0;
}

gint32
main(int argc, char **argv)
{
  setlocale(LC_ALL, "");

  GApplication *app;
  int status;

  app = g_application_new("org.simple-notes.example",
			  G_APPLICATION_HANDLES_COMMAND_LINE);
  g_signal_connect(app,
		   "command-line",
		   G_CALLBACK(sn_command_line),
		   NULL);

  status = g_application_run(G_APPLICATION(app), argc, argv);
  g_object_unref(app);

  return status;
}
