# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.14

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
CMAKE_COMMAND = /snap/clion/82/bin/cmake/linux/bin/cmake

# The command to remove a file.
RM = /snap/clion/82/bin/cmake/linux/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/latte/code/cpp/VulcanTutorial

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/latte/code/cpp/VulcanTutorial/cmake-build-release

# Include any dependencies generated for this target.
include CMakeFiles/VulkanTutorial.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/VulkanTutorial.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/VulkanTutorial.dir/flags.make

CMakeFiles/VulkanTutorial.dir/VulkanTutorial.cpp.o: CMakeFiles/VulkanTutorial.dir/flags.make
CMakeFiles/VulkanTutorial.dir/VulkanTutorial.cpp.o: ../VulkanTutorial.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/latte/code/cpp/VulcanTutorial/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/VulkanTutorial.dir/VulkanTutorial.cpp.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/VulkanTutorial.dir/VulkanTutorial.cpp.o -c /home/latte/code/cpp/VulcanTutorial/VulkanTutorial.cpp

CMakeFiles/VulkanTutorial.dir/VulkanTutorial.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/VulkanTutorial.dir/VulkanTutorial.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/latte/code/cpp/VulcanTutorial/VulkanTutorial.cpp > CMakeFiles/VulkanTutorial.dir/VulkanTutorial.cpp.i

CMakeFiles/VulkanTutorial.dir/VulkanTutorial.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/VulkanTutorial.dir/VulkanTutorial.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/latte/code/cpp/VulcanTutorial/VulkanTutorial.cpp -o CMakeFiles/VulkanTutorial.dir/VulkanTutorial.cpp.s

# Object files for target VulkanTutorial
VulkanTutorial_OBJECTS = \
"CMakeFiles/VulkanTutorial.dir/VulkanTutorial.cpp.o"

# External object files for target VulkanTutorial
VulkanTutorial_EXTERNAL_OBJECTS =

VulkanTutorial: CMakeFiles/VulkanTutorial.dir/VulkanTutorial.cpp.o
VulkanTutorial: CMakeFiles/VulkanTutorial.dir/build.make
VulkanTutorial: CMakeFiles/VulkanTutorial.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/latte/code/cpp/VulcanTutorial/cmake-build-release/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable VulkanTutorial"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/VulkanTutorial.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/VulkanTutorial.dir/build: VulkanTutorial

.PHONY : CMakeFiles/VulkanTutorial.dir/build

CMakeFiles/VulkanTutorial.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/VulkanTutorial.dir/cmake_clean.cmake
.PHONY : CMakeFiles/VulkanTutorial.dir/clean

CMakeFiles/VulkanTutorial.dir/depend:
	cd /home/latte/code/cpp/VulcanTutorial/cmake-build-release && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/latte/code/cpp/VulcanTutorial /home/latte/code/cpp/VulcanTutorial /home/latte/code/cpp/VulcanTutorial/cmake-build-release /home/latte/code/cpp/VulcanTutorial/cmake-build-release /home/latte/code/cpp/VulcanTutorial/cmake-build-release/CMakeFiles/VulkanTutorial.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/VulkanTutorial.dir/depend

