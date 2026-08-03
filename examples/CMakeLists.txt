# Exemplos CMakeLists

add_executable(guiengine_demo demo.cpp)
target_link_libraries(guiengine_demo PRIVATE GUIEngine)

# No Arch Linux, libGL e GLEW precisam de ser linkados explicitamente
if(UNIX AND NOT APPLE)
    target_link_libraries(guiengine_demo PRIVATE dl GL GLEW)
endif()

if(MSVC)
    set_target_properties(guiengine_demo PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()
