# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/abhineet/E/UofA/Thesis/Code/TrackingFramework/C++/MTF

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/abhineet/E/UofA/Thesis/Code/TrackingFramework/C++/MTF/build2

# Utility rule file for all.

# Include the progress variables for this target.
include CMakeFiles/all.dir/progress.make

CMakeFiles/all: runMTF
CMakeFiles/all: createMosaic
CMakeFiles/all: generateSyntheticSeq
CMakeFiles/all: trackUAVTrajectory
CMakeFiles/all: trackMarkers
CMakeFiles/all: extractPatch
CMakeFiles/all: pyMTF.so
CMakeFiles/all: testMTF

all: CMakeFiles/all
all: CMakeFiles/all.dir/build.make
.PHONY : all

# Rule to build all files generated by this target.
CMakeFiles/all.dir/build: all
.PHONY : CMakeFiles/all.dir/build

CMakeFiles/all.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/all.dir/cmake_clean.cmake
.PHONY : CMakeFiles/all.dir/clean

CMakeFiles/all.dir/depend:
	cd /home/abhineet/E/UofA/Thesis/Code/TrackingFramework/C++/MTF/build2 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/abhineet/E/UofA/Thesis/Code/TrackingFramework/C++/MTF /home/abhineet/E/UofA/Thesis/Code/TrackingFramework/C++/MTF /home/abhineet/E/UofA/Thesis/Code/TrackingFramework/C++/MTF/build2 /home/abhineet/E/UofA/Thesis/Code/TrackingFramework/C++/MTF/build2 /home/abhineet/E/UofA/Thesis/Code/TrackingFramework/C++/MTF/build2/CMakeFiles/all.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/all.dir/depend
