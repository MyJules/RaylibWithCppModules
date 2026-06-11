add_subdirectory(raylib)

add_library(args STATIC)
target_sources(args PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES
        args.cppm
)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE 
    args 
    raylib_module
)

enable_std_module(args PUBLIC)
