cmake_minimum_required(VERSION 3.22)
project(DirectX11-With-Windows-SDK-Extra)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

function(set_project_target_common_properties target_)
    target_compile_options(${target_} PRIVATE
        "/MP"
        "/utf-8"
    )
    target_compile_definitions(${target_} PRIVATE
        _UNICODE
        UNICODE
    )
    set_target_properties(${target_} PROPERTIES
        C_STANDARD 17
        C_STANDARD_REQUIRED ON
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON
        CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:DebugDLL>"
    )
endfunction()

function(project_target_copy_to_output_directory __AFTER_TARGET__ __TARGET__)
    add_custom_command(TARGET ${__AFTER_TARGET__} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_SOURCE_DIR}/bin
        COMMAND ${CMAKE_COMMAND} -E rm -f ${CMAKE_SOURCE_DIR}/bin/"$<TARGET_FILE_NAME:${__TARGET__}>"
        COMMAND ${CMAKE_COMMAND} -E copy  "$<TARGET_FILE:${__TARGET__}>" ${CMAKE_SOURCE_DIR}/bin
    )
endfunction()

add_subdirectory("Project Extra 01-02")
