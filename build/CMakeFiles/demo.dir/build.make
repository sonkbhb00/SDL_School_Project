# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.31

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
CMAKE_COMMAND = "C:\Program Files\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\CMake\bin\cmake.exe" -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo"

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build"

# Include any dependencies generated for this target.
include CMakeFiles/demo.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/demo.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/demo.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/demo.dir/flags.make

CMakeFiles/demo.dir/codegen:
.PHONY : CMakeFiles/demo.dir/codegen

CMakeFiles/demo.dir/main.cpp.obj: CMakeFiles/demo.dir/flags.make
CMakeFiles/demo.dir/main.cpp.obj: CMakeFiles/demo.dir/includes_CXX.rsp
CMakeFiles/demo.dir/main.cpp.obj: C:/Users/Bui\ Son/OneDrive/Desktop/demo_sdl/demo/main.cpp
CMakeFiles/demo.dir/main.cpp.obj: CMakeFiles/demo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/demo.dir/main.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/demo.dir/main.cpp.obj -MF CMakeFiles\demo.dir\main.cpp.obj.d -o CMakeFiles\demo.dir\main.cpp.obj -c "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\main.cpp"

CMakeFiles/demo.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/demo.dir/main.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\main.cpp" > CMakeFiles\demo.dir\main.cpp.i

CMakeFiles/demo.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/demo.dir/main.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\main.cpp" -o CMakeFiles\demo.dir\main.cpp.s

CMakeFiles/demo.dir/Game.cpp.obj: CMakeFiles/demo.dir/flags.make
CMakeFiles/demo.dir/Game.cpp.obj: CMakeFiles/demo.dir/includes_CXX.rsp
CMakeFiles/demo.dir/Game.cpp.obj: C:/Users/Bui\ Son/OneDrive/Desktop/demo_sdl/demo/Game.cpp
CMakeFiles/demo.dir/Game.cpp.obj: CMakeFiles/demo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/demo.dir/Game.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/demo.dir/Game.cpp.obj -MF CMakeFiles\demo.dir\Game.cpp.obj.d -o CMakeFiles\demo.dir\Game.cpp.obj -c "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\Game.cpp"

CMakeFiles/demo.dir/Game.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/demo.dir/Game.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\Game.cpp" > CMakeFiles\demo.dir\Game.cpp.i

CMakeFiles/demo.dir/Game.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/demo.dir/Game.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\Game.cpp" -o CMakeFiles\demo.dir\Game.cpp.s

CMakeFiles/demo.dir/GameObject.cpp.obj: CMakeFiles/demo.dir/flags.make
CMakeFiles/demo.dir/GameObject.cpp.obj: CMakeFiles/demo.dir/includes_CXX.rsp
CMakeFiles/demo.dir/GameObject.cpp.obj: C:/Users/Bui\ Son/OneDrive/Desktop/demo_sdl/demo/GameObject.cpp
CMakeFiles/demo.dir/GameObject.cpp.obj: CMakeFiles/demo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/demo.dir/GameObject.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/demo.dir/GameObject.cpp.obj -MF CMakeFiles\demo.dir\GameObject.cpp.obj.d -o CMakeFiles\demo.dir\GameObject.cpp.obj -c "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\GameObject.cpp"

CMakeFiles/demo.dir/GameObject.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/demo.dir/GameObject.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\GameObject.cpp" > CMakeFiles\demo.dir\GameObject.cpp.i

CMakeFiles/demo.dir/GameObject.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/demo.dir/GameObject.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\GameObject.cpp" -o CMakeFiles\demo.dir\GameObject.cpp.s

CMakeFiles/demo.dir/TextureManager.cpp.obj: CMakeFiles/demo.dir/flags.make
CMakeFiles/demo.dir/TextureManager.cpp.obj: CMakeFiles/demo.dir/includes_CXX.rsp
CMakeFiles/demo.dir/TextureManager.cpp.obj: C:/Users/Bui\ Son/OneDrive/Desktop/demo_sdl/demo/TextureManager.cpp
CMakeFiles/demo.dir/TextureManager.cpp.obj: CMakeFiles/demo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/demo.dir/TextureManager.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/demo.dir/TextureManager.cpp.obj -MF CMakeFiles\demo.dir\TextureManager.cpp.obj.d -o CMakeFiles\demo.dir\TextureManager.cpp.obj -c "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\TextureManager.cpp"

CMakeFiles/demo.dir/TextureManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/demo.dir/TextureManager.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\TextureManager.cpp" > CMakeFiles\demo.dir\TextureManager.cpp.i

CMakeFiles/demo.dir/TextureManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/demo.dir/TextureManager.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\TextureManager.cpp" -o CMakeFiles\demo.dir\TextureManager.cpp.s

CMakeFiles/demo.dir/TileMap.cpp.obj: CMakeFiles/demo.dir/flags.make
CMakeFiles/demo.dir/TileMap.cpp.obj: CMakeFiles/demo.dir/includes_CXX.rsp
CMakeFiles/demo.dir/TileMap.cpp.obj: C:/Users/Bui\ Son/OneDrive/Desktop/demo_sdl/demo/TileMap.cpp
CMakeFiles/demo.dir/TileMap.cpp.obj: CMakeFiles/demo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/demo.dir/TileMap.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/demo.dir/TileMap.cpp.obj -MF CMakeFiles\demo.dir\TileMap.cpp.obj.d -o CMakeFiles\demo.dir\TileMap.cpp.obj -c "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\TileMap.cpp"

CMakeFiles/demo.dir/TileMap.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/demo.dir/TileMap.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\TileMap.cpp" > CMakeFiles\demo.dir\TileMap.cpp.i

CMakeFiles/demo.dir/TileMap.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/demo.dir/TileMap.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\TileMap.cpp" -o CMakeFiles\demo.dir\TileMap.cpp.s

CMakeFiles/demo.dir/Combat.cpp.obj: CMakeFiles/demo.dir/flags.make
CMakeFiles/demo.dir/Combat.cpp.obj: CMakeFiles/demo.dir/includes_CXX.rsp
CMakeFiles/demo.dir/Combat.cpp.obj: C:/Users/Bui\ Son/OneDrive/Desktop/demo_sdl/demo/Combat.cpp
CMakeFiles/demo.dir/Combat.cpp.obj: CMakeFiles/demo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/demo.dir/Combat.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/demo.dir/Combat.cpp.obj -MF CMakeFiles\demo.dir\Combat.cpp.obj.d -o CMakeFiles\demo.dir\Combat.cpp.obj -c "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\Combat.cpp"

CMakeFiles/demo.dir/Combat.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/demo.dir/Combat.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\Combat.cpp" > CMakeFiles\demo.dir\Combat.cpp.i

CMakeFiles/demo.dir/Combat.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/demo.dir/Combat.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\Combat.cpp" -o CMakeFiles\demo.dir\Combat.cpp.s

CMakeFiles/demo.dir/Physics.cpp.obj: CMakeFiles/demo.dir/flags.make
CMakeFiles/demo.dir/Physics.cpp.obj: CMakeFiles/demo.dir/includes_CXX.rsp
CMakeFiles/demo.dir/Physics.cpp.obj: C:/Users/Bui\ Son/OneDrive/Desktop/demo_sdl/demo/Physics.cpp
CMakeFiles/demo.dir/Physics.cpp.obj: CMakeFiles/demo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_7) "Building CXX object CMakeFiles/demo.dir/Physics.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/demo.dir/Physics.cpp.obj -MF CMakeFiles\demo.dir\Physics.cpp.obj.d -o CMakeFiles\demo.dir\Physics.cpp.obj -c "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\Physics.cpp"

CMakeFiles/demo.dir/Physics.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/demo.dir/Physics.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\Physics.cpp" > CMakeFiles\demo.dir\Physics.cpp.i

CMakeFiles/demo.dir/Physics.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/demo.dir/Physics.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\Physics.cpp" -o CMakeFiles\demo.dir\Physics.cpp.s

CMakeFiles/demo.dir/Enemy.cpp.obj: CMakeFiles/demo.dir/flags.make
CMakeFiles/demo.dir/Enemy.cpp.obj: CMakeFiles/demo.dir/includes_CXX.rsp
CMakeFiles/demo.dir/Enemy.cpp.obj: C:/Users/Bui\ Son/OneDrive/Desktop/demo_sdl/demo/Enemy.cpp
CMakeFiles/demo.dir/Enemy.cpp.obj: CMakeFiles/demo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_8) "Building CXX object CMakeFiles/demo.dir/Enemy.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/demo.dir/Enemy.cpp.obj -MF CMakeFiles\demo.dir\Enemy.cpp.obj.d -o CMakeFiles\demo.dir\Enemy.cpp.obj -c "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\Enemy.cpp"

CMakeFiles/demo.dir/Enemy.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/demo.dir/Enemy.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\Enemy.cpp" > CMakeFiles\demo.dir\Enemy.cpp.i

CMakeFiles/demo.dir/Enemy.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/demo.dir/Enemy.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\Enemy.cpp" -o CMakeFiles\demo.dir\Enemy.cpp.s

CMakeFiles/demo.dir/AudioManager.cpp.obj: CMakeFiles/demo.dir/flags.make
CMakeFiles/demo.dir/AudioManager.cpp.obj: CMakeFiles/demo.dir/includes_CXX.rsp
CMakeFiles/demo.dir/AudioManager.cpp.obj: C:/Users/Bui\ Son/OneDrive/Desktop/demo_sdl/demo/AudioManager.cpp
CMakeFiles/demo.dir/AudioManager.cpp.obj: CMakeFiles/demo.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir="C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_9) "Building CXX object CMakeFiles/demo.dir/AudioManager.cpp.obj"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/demo.dir/AudioManager.cpp.obj -MF CMakeFiles\demo.dir\AudioManager.cpp.obj.d -o CMakeFiles\demo.dir\AudioManager.cpp.obj -c "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\AudioManager.cpp"

CMakeFiles/demo.dir/AudioManager.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/demo.dir/AudioManager.cpp.i"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\AudioManager.cpp" > CMakeFiles\demo.dir\AudioManager.cpp.i

CMakeFiles/demo.dir/AudioManager.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/demo.dir/AudioManager.cpp.s"
	C:\msys64\ucrt64\bin\c++.exe $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\AudioManager.cpp" -o CMakeFiles\demo.dir\AudioManager.cpp.s

# Object files for target demo
demo_OBJECTS = \
"CMakeFiles/demo.dir/main.cpp.obj" \
"CMakeFiles/demo.dir/Game.cpp.obj" \
"CMakeFiles/demo.dir/GameObject.cpp.obj" \
"CMakeFiles/demo.dir/TextureManager.cpp.obj" \
"CMakeFiles/demo.dir/TileMap.cpp.obj" \
"CMakeFiles/demo.dir/Combat.cpp.obj" \
"CMakeFiles/demo.dir/Physics.cpp.obj" \
"CMakeFiles/demo.dir/Enemy.cpp.obj" \
"CMakeFiles/demo.dir/AudioManager.cpp.obj"

# External object files for target demo
demo_EXTERNAL_OBJECTS =

demo.exe: CMakeFiles/demo.dir/main.cpp.obj
demo.exe: CMakeFiles/demo.dir/Game.cpp.obj
demo.exe: CMakeFiles/demo.dir/GameObject.cpp.obj
demo.exe: CMakeFiles/demo.dir/TextureManager.cpp.obj
demo.exe: CMakeFiles/demo.dir/TileMap.cpp.obj
demo.exe: CMakeFiles/demo.dir/Combat.cpp.obj
demo.exe: CMakeFiles/demo.dir/Physics.cpp.obj
demo.exe: CMakeFiles/demo.dir/Enemy.cpp.obj
demo.exe: CMakeFiles/demo.dir/AudioManager.cpp.obj
demo.exe: CMakeFiles/demo.dir/build.make
demo.exe: CMakeFiles/demo.dir/linkLibs.rsp
demo.exe: CMakeFiles/demo.dir/objects1.rsp
demo.exe: CMakeFiles/demo.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir="C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build\CMakeFiles" --progress-num=$(CMAKE_PROGRESS_10) "Linking CXX executable demo.exe"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\demo.dir\link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/demo.dir/build: demo.exe
.PHONY : CMakeFiles/demo.dir/build

CMakeFiles/demo.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles\demo.dir\cmake_clean.cmake
.PHONY : CMakeFiles/demo.dir/clean

CMakeFiles/demo.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo" "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo" "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build" "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build" "C:\Users\Bui Son\OneDrive\Desktop\demo_sdl\demo\build\CMakeFiles\demo.dir\DependInfo.cmake" "--color=$(COLOR)"
.PHONY : CMakeFiles/demo.dir/depend

