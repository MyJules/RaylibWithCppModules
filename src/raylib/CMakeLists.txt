include(FetchContent)

FetchContent_Declare(
    raylib
	SOURCE_DIR ${PROJECT_SOURCE_DIR}/vendor/raylib-6.0
)
FetchContent_MakeAvailable(raylib)

add_library(raylib_module STATIC)
target_sources(raylib_module PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES
        raylib.cppm
)

target_link_libraries(raylib_module PRIVATE raylib)