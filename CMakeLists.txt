cmake_minimum_required(VERSION 3.10)

project(EmptyPunch VERSION 1.0 LANGUAGES C)

add_subdirectory(dependencies)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_subdirectory(source)
add_subdirectory(icon)

add_executable(EmptyPunch
    ${SOURCE_FILES} 
    ${ICON}
    "${CMAKE_SOURCE_DIR}/source/main.c"
)

target_link_libraries(EmptyPunch ${LINK_LIBRARIES})
target_include_directories(EmptyPunch PRIVATE 
    ${INCLUDE_DIRECTORIES}
    ${SOURCE_DIRECTORIES}
)
set(COMPILE_OPTIONS
    -W
    -g
    -Wall
    -Wextra
    -Wpedantic
    -Wshadow
    -Wunused
    -Wno-logical-op-parentheses
    -Wno-parentheses
    -Werror
    -Wno-comment
    -pedantic
    -pedantic-errors
    -D_GLIBCXX_DEBUG
    -D_GLIBCXX_DEBUG_PEDANTIC
    -D_UCRT
)

if (CMAKE_BUILD_TYPE STREQUAL "Release")
    set(COMPILE_OPTIONS 
        ${COMPILE_OPTIONS}
        -O3
    )
    if (WIN32)
        target_link_options(EmptyPunch PRIVATE -mwindows)
    endif()
else()
    set(COMPILE_OPTIONS 
        ${COMPILE_OPTIONS}
        -g
    )
endif()

target_compile_options(EmptyPunch PUBLIC ${COMPILE_OPTIONS})

file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/shaders)

add_custom_target(shaders DEPENDS
    ${CMAKE_SOURCE_DIR}/shaders
    ${CMAKE_SOURCE_DIR}/textures
    ${CMAKE_SOURCE_DIR}/models
)

set(SHADER_SRC "${CMAKE_SOURCE_DIR}/shaders")
set(SHADER_DST "${CMAKE_BINARY_DIR}/shaders")

add_custom_command(TARGET shaders POST_BUILD
    COMMAND glslc ${SHADER_SRC}/shader.vert -o ${SHADER_DST}/vert.spv
    COMMAND glslc ${SHADER_SRC}/shader2.frag -o ${SHADER_DST}/frag2.spv
    COMMAND glslc ${SHADER_SRC}/text2d.vert -o ${SHADER_DST}/text2dV.spv
    COMMAND glslc ${SHADER_SRC}/text.frag -o ${SHADER_DST}/textF.spv
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/textures ${CMAKE_BINARY_DIR}/textures
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/models ${CMAKE_BINARY_DIR}/models
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/fonts ${CMAKE_BINARY_DIR}/fonts
    COMMAND ${CMAKE_COMMAND} -E copy_directory ${CMAKE_SOURCE_DIR}/music ${CMAKE_BINARY_DIR}/music
)

add_dependencies(EmptyPunch shaders)
