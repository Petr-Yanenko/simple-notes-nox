# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.13

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/user/Documents/simple-notes-nox

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/user/Documents/simple-notes-nox/build

# Include any dependencies generated for this target.
include simple_notes_tests/CMakeFiles/simple_notes_tests.dir/depend.make

# Include the progress variables for this target.
include simple_notes_tests/CMakeFiles/simple_notes_tests.dir/progress.make

# Include the compile flags for this target's objects.
include simple_notes_tests/CMakeFiles/simple_notes_tests.dir/flags.make

simple_notes_tests/CMakeFiles/simple_notes_tests.dir/main.c.o: simple_notes_tests/CMakeFiles/simple_notes_tests.dir/flags.make
simple_notes_tests/CMakeFiles/simple_notes_tests.dir/main.c.o: ../simple_notes_tests/main.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Documents/simple-notes-nox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object simple_notes_tests/CMakeFiles/simple_notes_tests.dir/main.c.o"
	cd /home/user/Documents/simple-notes-nox/build/simple_notes_tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/simple_notes_tests.dir/main.c.o   -c /home/user/Documents/simple-notes-nox/simple_notes_tests/main.c

simple_notes_tests/CMakeFiles/simple_notes_tests.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/simple_notes_tests.dir/main.c.i"
	cd /home/user/Documents/simple-notes-nox/build/simple_notes_tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/user/Documents/simple-notes-nox/simple_notes_tests/main.c > CMakeFiles/simple_notes_tests.dir/main.c.i

simple_notes_tests/CMakeFiles/simple_notes_tests.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/simple_notes_tests.dir/main.c.s"
	cd /home/user/Documents/simple-notes-nox/build/simple_notes_tests && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/user/Documents/simple-notes-nox/simple_notes_tests/main.c -o CMakeFiles/simple_notes_tests.dir/main.c.s

# Object files for target simple_notes_tests
simple_notes_tests_OBJECTS = \
"CMakeFiles/simple_notes_tests.dir/main.c.o"

# External object files for target simple_notes_tests
simple_notes_tests_EXTERNAL_OBJECTS =

simple_notes_tests/simple_notes_tests: simple_notes_tests/CMakeFiles/simple_notes_tests.dir/main.c.o
simple_notes_tests/simple_notes_tests: simple_notes_tests/CMakeFiles/simple_notes_tests.dir/build.make
simple_notes_tests/simple_notes_tests: shared_source/libshared.a
simple_notes_tests/simple_notes_tests: simple_notes_tests/CMakeFiles/simple_notes_tests.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/user/Documents/simple-notes-nox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable simple_notes_tests"
	cd /home/user/Documents/simple-notes-nox/build/simple_notes_tests && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/simple_notes_tests.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
simple_notes_tests/CMakeFiles/simple_notes_tests.dir/build: simple_notes_tests/simple_notes_tests

.PHONY : simple_notes_tests/CMakeFiles/simple_notes_tests.dir/build

simple_notes_tests/CMakeFiles/simple_notes_tests.dir/clean:
	cd /home/user/Documents/simple-notes-nox/build/simple_notes_tests && $(CMAKE_COMMAND) -P CMakeFiles/simple_notes_tests.dir/cmake_clean.cmake
.PHONY : simple_notes_tests/CMakeFiles/simple_notes_tests.dir/clean

simple_notes_tests/CMakeFiles/simple_notes_tests.dir/depend:
	cd /home/user/Documents/simple-notes-nox/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/user/Documents/simple-notes-nox /home/user/Documents/simple-notes-nox/simple_notes_tests /home/user/Documents/simple-notes-nox/build /home/user/Documents/simple-notes-nox/build/simple_notes_tests /home/user/Documents/simple-notes-nox/build/simple_notes_tests/CMakeFiles/simple_notes_tests.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : simple_notes_tests/CMakeFiles/simple_notes_tests.dir/depend

