# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.24

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "D:\CLion 2022.3.2\bin\cmake\win\x64\bin\cmake.exe"

# The command to remove a file.
RM = "D:\CLion 2022.3.2\bin\cmake\win\x64\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = E:\SJTU-Classes\Advanced-Data-Structure\hw5-rbtree

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = E:\SJTU-Classes\Advanced-Data-Structure\hw5-rbtree\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/rbtree.cpp.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/rbtree.cpp.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/rbtree.cpp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/rbtree.cpp.dir/flags.make

CMakeFiles/rbtree.cpp.dir/main.cpp.obj: CMakeFiles/rbtree.cpp.dir/flags.make
CMakeFiles/rbtree.cpp.dir/main.cpp.obj: E:/SJTU-Classes/Advanced-Data-Structure/hw5-rbtree/main.cpp
CMakeFiles/rbtree.cpp.dir/main.cpp.obj: CMakeFiles/rbtree.cpp.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=E:\SJTU-Classes\Advanced-Data-Structure\hw5-rbtree\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/rbtree.cpp.dir/main.cpp.obj"
	"D:\CLion 2022.3.2\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/rbtree.cpp.dir/main.cpp.obj -MF CMakeFiles\rbtree.cpp.dir\main.cpp.obj.d -o CMakeFiles\rbtree.cpp.dir\main.cpp.obj -c E:\SJTU-Classes\Advanced-Data-Structure\hw5-rbtree\main.cpp

CMakeFiles/rbtree.cpp.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/rbtree.cpp.dir/main.cpp.i"
	"D:\CLion 2022.3.2\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E E:\SJTU-Classes\Advanced-Data-Structure\hw5-rbtree\main.cpp > CMakeFiles\rbtree.cpp.dir\main.cpp.i

CMakeFiles/rbtree.cpp.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/rbtree.cpp.dir/main.cpp.s"
	"D:\CLion 2022.3.2\bin\mingw\bin\g++.exe" $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S E:\SJTU-Classes\Advanced-Data-Structure\hw5-rbtree\main.cpp -o CMakeFiles\rbtree.cpp.dir\main.cpp.s

# Object files for target rbtree.cpp
rbtree_cpp_OBJECTS = \
"CMakeFiles/rbtree.cpp.dir/main.cpp.obj"

# External object files for target rbtree.cpp
rbtree_cpp_EXTERNAL_OBJECTS =

rbtree.cpp.exe: CMakeFiles/rbtree.cpp.dir/main.cpp.obj
rbtree.cpp.exe: CMakeFiles/rbtree.cpp.dir/build.make
rbtree.cpp.exe: CMakeFiles/rbtree.cpp.dir/linklibs.rsp
rbtree.cpp.exe: CMakeFiles/rbtree.cpp.dir/objects1.rsp
rbtree.cpp.exe: CMakeFiles/rbtree.cpp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=E:\SJTU-Classes\Advanced-Data-Structure\hw5-rbtree\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable rbtree.cpp.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\rbtree.cpp.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/rbtree.cpp.dir/build: rbtree.cpp.exe
.PHONY : CMakeFiles/rbtree.cpp.dir/build

CMakeFiles/rbtree.cpp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\rbtree.cpp.dir\cmake_clean.cmake
.PHONY : CMakeFiles/rbtree.cpp.dir/clean

CMakeFiles/rbtree.cpp.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" E:\SJTU-Classes\Advanced-Data-Structure\hw5-rbtree E:\SJTU-Classes\Advanced-Data-Structure\hw5-rbtree E:\SJTU-Classes\Advanced-Data-Structure\hw5-rbtree\cmake-build-debug E:\SJTU-Classes\Advanced-Data-Structure\hw5-rbtree\cmake-build-debug E:\SJTU-Classes\Advanced-Data-Structure\hw5-rbtree\cmake-build-debug\CMakeFiles\rbtree.cpp.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/rbtree.cpp.dir/depend

