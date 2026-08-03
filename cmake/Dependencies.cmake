# Dependencies.cmake - Fetch and configure third-party dependencies for GUIEngine

include(FetchContent)

# SDL2
find_package(SDL2 QUIET)
if(NOT SDL2_FOUND)
    FetchContent_Declare(
        SDL2
        GIT_REPOSITORY https://github.com/libsdl-org/SDL.git
        GIT_TAG release-2.28.0
        GIT_SHALLOW TRUE
    )
    set(SDL_SHARED ON CACHE BOOL "" FORCE)
    set(SDL_STATIC OFF CACHE BOOL "" FORCE)
    set(SDL_TEST OFF CACHE BOOL "" FORCE)
    FetchContent_MakeAvailable(SDL2)

    if(NOT TARGET SDL2::Core)
        add_library(SDL2::Core ALIAS SDL2::SDL2)
    endif()
    if(NOT TARGET SDL2::Main)
        add_library(SDL2::Main ALIAS SDL2::SDL2main)
    endif()
endif()

# GLAD - OpenGL loader
FetchContent_Declare(
    glad
    GIT_REPOSITORY https://github.com/Dav1dde/glad.git
    GIT_TAG v0.1.36
    GIT_SHALLOW TRUE
)
FetchContent_GetProperties(glad)
if(NOT glad_POPULATED)
    FetchContent_Populate(glad)
endif()

if(NOT TARGET glad)
    add_library(glad STATIC ${glad_SOURCE_DIR}/src/glad.c)
    target_include_directories(glad PUBLIC ${glad_SOURCE_DIR}/include)
endif()

# STB - Image loading and TrueType font rendering
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
    FetchContent_Populate(stb)
endif()

# stb_image needs a compilation unit with STB_IMAGE_IMPLEMENTATION
if(NOT TARGET stb_image)
    file(WRITE ${CMAKE_BINARY_DIR}/stb_image_impl.c
        "#define STB_IMAGE_IMPLEMENTATION\n"
        "#include \"stb_image.h\"\n"
    )
    add_library(stb_image STATIC ${CMAKE_BINARY_DIR}/stb_image_impl.c)
    target_include_directories(stb_image PUBLIC ${stb_SOURCE_DIR})
endif()

if(NOT TARGET stb_truetype)
    add_library(stb_truetype INTERFACE)
    target_include_directories(stb_truetype INTERFACE ${stb_SOURCE_DIR})
endif()
