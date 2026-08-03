# Dependencies.cmake - Buscar e configurar dependencias de terceiros para a GUIEngine

include(FetchContent)

# SDL2
find_package(SDL2 CONFIG QUIET)
if(NOT SDL2_FOUND)
    find_package(SDL2 QUIET)
endif()

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

# STB - Carregamento de imagens e renderizacao de fontes TrueType
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
cmake_policy(SET CMP0169 OLD)
FetchContent_Populate(stb)

# stb_image precisa de uma unidade de compilacao com STB_IMAGE_IMPLEMENTATION
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
