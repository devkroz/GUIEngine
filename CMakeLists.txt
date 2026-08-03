cmake_minimum_required(VERSION 3.16)

project(GUIEngine
    VERSION 1.0.0
    DESCRIPTION "Uma engine de interface grafica moderna em C++ com SDL2 + OpenGL"
    LANGUAGES C CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(GUIENGINE_BUILD_EXAMPLES "Compilar aplicacoes de exemplo" ON)
option(GUIENGINE_BUILD_SHARED "Compilar como biblioteca compartilhada" OFF)

set(GUIENGINE_SOURCES
    src/Widget.cpp
    src/Window.cpp
    src/Renderer.cpp
    src/Application.cpp
    src/Containers.cpp
    src/Label.cpp
    src/Button.cpp
    src/IconButton.cpp
    src/TextField.cpp
    src/Checkbox.cpp
    src/Slider.cpp
    src/ProgressBar.cpp
    src/Panel.cpp
    src/ScrollView.cpp
    src/Decorations.cpp
    src/WindowWidget.cpp
    src/Dropdown.cpp
    src/TabView.cpp
    src/SplitView.cpp
)

set(GUIENGINE_PUBLIC_HEADERS
    include/GUIEngine/Types.hpp
    include/GUIEngine/Event.hpp
    include/GUIEngine/Layout.hpp
    include/GUIEngine/Style.hpp
    include/GUIEngine/Widget.hpp
    include/GUIEngine/Window.hpp
    include/GUIEngine/Renderer.hpp
    include/GUIEngine/Application.hpp
    include/GUIEngine/Containers.hpp
    include/GUIEngine/Label.hpp
    include/GUIEngine/Button.hpp
    include/GUIEngine/IconButton.hpp
    include/GUIEngine/TextField.hpp
    include/GUIEngine/Checkbox.hpp
    include/GUIEngine/Slider.hpp
    include/GUIEngine/ProgressBar.hpp
    include/GUIEngine/Panel.hpp
    include/GUIEngine/ScrollView.hpp
    include/GUIEngine/Decorations.hpp
    include/GUIEngine/WindowWidget.hpp
    include/GUIEngine/Dropdown.hpp
    include/GUIEngine/TabView.hpp
    include/GUIEngine/SplitView.hpp
    include/GUIEngine/GUI.hpp
    include/GUIEngine/GUIEngine.hpp
)

# Dependencias de terceiros
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/Dependencies.cmake)

# Alvo da biblioteca
if(GUIENGINE_BUILD_SHARED)
    add_library(GUIEngine SHARED ${GUIENGINE_SOURCES} ${GUIENGINE_PUBLIC_HEADERS})
else()
    add_library(GUIEngine STATIC ${GUIENGINE_SOURCES} ${GUIENGINE_PUBLIC_HEADERS})
endif()

target_include_directories(GUIEngine
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third_party
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(GUIEngine
    PUBLIC
        SDL2::Core
        SDL2::Main
    PRIVATE
        glad
        stb_image
        stb_truetype
)

if(UNIX AND NOT APPLE)
    target_link_libraries(GUIEngine PUBLIC pthread dl)
endif()

target_compile_features(GUIEngine PUBLIC cxx_std_17)

if(MSVC)
    target_compile_options(GUIEngine PRIVATE /W3 /WX)
else()
    target_compile_options(GUIEngine PRIVATE -Wall -Wextra -Wpedantic)
endif()

# Instalacao
include(GNUInstallDirs)

install(TARGETS GUIEngine
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

install(DIRECTORY include/GUIEngine
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    FILES_MATCHING PATTERN "*.hpp"
)

# Exemplos
if(GUIENGINE_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
