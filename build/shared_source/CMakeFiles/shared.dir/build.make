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
include shared_source/CMakeFiles/shared.dir/depend.make

# Include the progress variables for this target.
include shared_source/CMakeFiles/shared.dir/progress.make

# Include the compile flags for this target's objects.
include shared_source/CMakeFiles/shared.dir/flags.make

shared_source/CMakeFiles/shared.dir/global_functions.c.o: shared_source/CMakeFiles/shared.dir/flags.make
shared_source/CMakeFiles/shared.dir/global_functions.c.o: ../shared_source/global_functions.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Documents/simple-notes-nox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object shared_source/CMakeFiles/shared.dir/global_functions.c.o"
	cd /home/user/Documents/simple-notes-nox/build/shared_source && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/shared.dir/global_functions.c.o   -c /home/user/Documents/simple-notes-nox/shared_source/global_functions.c

shared_source/CMakeFiles/shared.dir/global_functions.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/shared.dir/global_functions.c.i"
	cd /home/user/Documents/simple-notes-nox/build/shared_source && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/user/Documents/simple-notes-nox/shared_source/global_functions.c > CMakeFiles/shared.dir/global_functions.c.i

shared_source/CMakeFiles/shared.dir/global_functions.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/shared.dir/global_functions.c.s"
	cd /home/user/Documents/simple-notes-nox/build/shared_source && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/user/Documents/simple-notes-nox/shared_source/global_functions.c -o CMakeFiles/shared.dir/global_functions.c.s

shared_source/CMakeFiles/shared.dir/commands.c.o: shared_source/CMakeFiles/shared.dir/flags.make
shared_source/CMakeFiles/shared.dir/commands.c.o: ../shared_source/commands.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/user/Documents/simple-notes-nox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object shared_source/CMakeFiles/shared.dir/commands.c.o"
	cd /home/user/Documents/simple-notes-nox/build/shared_source && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/shared.dir/commands.c.o   -c /home/user/Documents/simple-notes-nox/shared_source/commands.c

shared_source/CMakeFiles/shared.dir/commands.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/shared.dir/commands.c.i"
	cd /home/user/Documents/simple-notes-nox/build/shared_source && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/user/Documents/simple-notes-nox/shared_source/commands.c > CMakeFiles/shared.dir/commands.c.i

shared_source/CMakeFiles/shared.dir/commands.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/shared.dir/commands.c.s"
	cd /home/user/Documents/simple-notes-nox/build/shared_source && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/user/Documents/simple-notes-nox/shared_source/commands.c -o CMakeFiles/shared.dir/commands.c.s

# Object files for target shared
shared_OBJECTS = \
"CMakeFiles/shared.dir/global_functions.c.o" \
"CMakeFiles/shared.dir/commands.c.o"

# External object files for target shared
shared_EXTERNAL_OBJECTS =

shared_source/libshared.a: shared_source/CMakeFiles/shared.dir/global_functions.c.o
shared_source/libshared.a: shared_source/CMakeFiles/shared.dir/commands.c.o
shared_source/libshared.a: shared_source/CMakeFiles/shared.dir/build.make
shared_source/libshared.a: shared_source/CMakeFiles/shared.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/user/Documents/simple-notes-nox/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C static library libshared.a"
	cd /home/user/Documents/simple-notes-nox/build/shared_source && $(CMAKE_COMMAND) -P CMakeFiles/shared.dir/cmake_clean_target.cmake
	cd /home/user/Documents/simple-notes-nox/build/shared_source && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/shared.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
shared_source/CMakeFiles/shared.dir/build: shared_source/libshared.a

.PHONY : shared_source/CMakeFiles/shared.dir/build

shared_source/CMakeFiles/shared.dir/clean:
	cd /home/user/Documents/simple-notes-nox/build/shared_source && $(CMAKE_COMMAND) -P CMakeFiles/shared.dir/cmake_clean.cmake
.PHONY : shared_source/CMakeFiles/shared.dir/clean

shared_source/CMakeFiles/shared.dir/depend:
	cd /home/user/Documents/simple-notes-nox/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/user/Documents/simple-notes-nox /home/user/Documents/simple-notes-nox/shared_source /home/user/Documents/simple-notes-nox/build /home/user/Documents/simple-notes-nox/build/shared_source /home/user/Documents/simple-notes-nox/build/shared_source/CMakeFiles/shared.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : shared_source/CMakeFiles/shared.dir/depend
