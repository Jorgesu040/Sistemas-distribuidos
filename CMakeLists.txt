 
cmake_minimum_required(VERSION 3.5)

set(CMAKE_BUILD_TYPE Debug)


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


project(server LANGUAGES CXX)
add_executable(server utils.h utils.cpp server.cpp
        clientManager.cpp
        clientManager.h
)
target_link_libraries(server pthread)


project(client LANGUAGES CXX)
add_executable(client utils.h utils.cpp client.cpp
        clientManager.cpp
        clientManager.h
)
target_link_libraries(client pthread)



# -------------------- CMake Notes --------------------
# 1. cmake_minimum_required(VERSION x.y)
#    - Sets the minimum CMake version required for this project.
#    - Ensures compatibility with older versions.

# 2. set(CMAKE_BUILD_TYPE Debug/Release)
#    - Controls the build type.
#    - Debug: includes debug symbols, no optimization.
#    - Release: optimized build, no debug symbols.

# 3. set(CMAKE_CXX_STANDARD 11/14/17/20)
#    - Sets the C++ standard to use.
#    - set(CMAKE_CXX_STANDARD_REQUIRED ON) enforces this version strictly.

# 4. project(ProjectName LANGUAGES CXX)
#    - Defines a project with the given name.
#    - LANGUAGES CXX specifies C++ (can also include C, etc.).

# 5. add_executable(name file1.cpp file2.cpp ...)
#    - Creates an executable target from the listed source files.
#    - Header files (.h) do not need to be listed, but can be included for IDEs.

# 6. target_link_libraries(target library1 library2 ...)
#    - Links libraries to the target.
#    - e.g., pthread for multithreading on Linux.

# 7. Useful additional CMake commands:
#    - include_directories(path) : add include paths for headers.
#    - link_directories(path)    : add library search paths.
#    - add_subdirectory(dir)      : include another CMake project.
#    - install(TARGETS ...)       : configure installation of executables/libraries.
#    - set(VAR value)             : define custom variables for flexibility.

# 8. Notes on multiple executables:
#    - Usually define one project().
#    - Then use multiple add_executable() calls for server, client, etc.
# -----------------------------------------------------
