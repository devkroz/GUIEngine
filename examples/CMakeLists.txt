# Examples CMakeLists

add_executable(guiengine_demo demo.cpp)
target_link_libraries(guiengine_demo PRIVATE GUIEngine)

if(MSVC)
    set_target_properties(guiengine_demo PROPERTIES
        VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    )
endif()
