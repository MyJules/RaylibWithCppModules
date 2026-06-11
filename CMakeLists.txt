cmake_minimum_required(VERSION 3.30)
project(CppModuleTemplate LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_SCAN_FOR_MODULES ON)

if(CMAKE_CXX_COMPILER_ID STREQUAL "Clang"
   AND NOT MSVC
   AND NOT CMAKE_CXX_SIMULATE_ID STREQUAL "MSVC")
	add_compile_options(-stdlib=libc++ -fexperimental-library)
	add_link_options(-stdlib=libc++)
endif()

include(CTest)

include(${CMAKE_CURRENT_LIST_DIR}/cmake/StdModules.cmake)

add_subdirectory(src)

if(BUILD_TESTING)
	add_subdirectory(tests)
endif()
