add_subdirectory(raylib)

add_library(args STATIC)
target_sources(args PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES
        args.cppm
)

add_library(render_layer STATIC)
target_sources(render_layer PUBLIC
    FILE_SET cxx_modules TYPE CXX_MODULES
    BASE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}"
    FILES
        RenderLayer.cppm
)
target_link_libraries(render_layer PUBLIC
    raylib_module
)

add_executable(app main.cpp)
target_link_libraries(app PRIVATE 
    args 
    render_layer
    raylib_module
)

enable_std_module(args PUBLIC)
enable_std_module(render_layer PUBLIC)
