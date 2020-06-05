(require 'cc-mode)
(require 'semantic)
(require 'ede)
(require 'whitespace)

(global-semanticdb-minor-mode 1)
(global-semantic-idle-scheduler-mode 1)
(global-ede-mode)
(global-whitespace-mode)

(semantic-mode 1)

(ede-cpp-root-project "simple-notes-nox"
		      :file "~/Documents/simple-notes-nox/CMakeLists.txt"
		      :include-path '("~/Documents/simple-notes-nox/include/"
				      "~/Documents/simple-notes-nox/shared_source/"
				      "~/Documents/simple-notes-nox/simple_notes/"
				      "~/Documents/simple-notes-nox/simple_notes/Store/"
				      "~/Documents/simple-notes-nox/simple_notes/Models/"
				      "~/Documents/simple-notes-nox/simple_notes/ModelPresenters/"
				      "~/Documents/simple-notes-nox/simple_notes/DataStructures/")
		      :system-include-path '("/usr/include/"
					     "/usr/include/glib-2.0/"
					     "/usr/include/glib-2.0/gio/"
					     "/usr/lib/x86_64-linux-gnu/glib-2.0/include/"))

(set-frame-font "Monospace-13" nil t)
