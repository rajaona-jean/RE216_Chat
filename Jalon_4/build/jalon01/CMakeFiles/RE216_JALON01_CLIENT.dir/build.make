# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.12

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
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/build

# Include any dependencies generated for this target.
include jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/depend.make

# Include the progress variables for this target.
include jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/progress.make

# Include the compile flags for this target's objects.
include jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/flags.make

jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/client.c.o: jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/flags.make
jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/client.c.o: /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/src/jalon01/client.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/client.c.o"
	cd /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/build/jalon01 && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/RE216_JALON01_CLIENT.dir/client.c.o   -c /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/src/jalon01/client.c

jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/client.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/RE216_JALON01_CLIENT.dir/client.c.i"
	cd /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/build/jalon01 && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/src/jalon01/client.c > CMakeFiles/RE216_JALON01_CLIENT.dir/client.c.i

jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/client.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/RE216_JALON01_CLIENT.dir/client.c.s"
	cd /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/build/jalon01 && /usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/src/jalon01/client.c -o CMakeFiles/RE216_JALON01_CLIENT.dir/client.c.s

# Object files for target RE216_JALON01_CLIENT
RE216_JALON01_CLIENT_OBJECTS = \
"CMakeFiles/RE216_JALON01_CLIENT.dir/client.c.o"

# External object files for target RE216_JALON01_CLIENT
RE216_JALON01_CLIENT_EXTERNAL_OBJECTS =

jalon01/RE216_JALON01_CLIENT: jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/client.c.o
jalon01/RE216_JALON01_CLIENT: jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/build.make
jalon01/RE216_JALON01_CLIENT: jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable RE216_JALON01_CLIENT"
	cd /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/build/jalon01 && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/RE216_JALON01_CLIENT.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/build: jalon01/RE216_JALON01_CLIENT

.PHONY : jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/build

jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/clean:
	cd /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/build/jalon01 && $(CMAKE_COMMAND) -P CMakeFiles/RE216_JALON01_CLIENT.dir/cmake_clean.cmake
.PHONY : jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/clean

jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/depend:
	cd /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/src /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/src/jalon01 /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/build /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/build/jalon01 /home/rajaona/Documents/C/Projet/RE216_Chat/Jalon_4/build/jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : jalon01/CMakeFiles/RE216_JALON01_CLIENT.dir/depend
