cmake_minimum_required(VERSION 3.10.0)
project(my_opengl_project VERSION 0.1.0)
cmake_policy(SET CMP0072 NEW)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(assimp REQUIRED)

add_executable(my_opengl_project
    src/glad.c
    src/main.cpp
    src/mesh.cpp
    src/model.cpp
    src/shader_s.cpp
    src/stb_image.cpp
)

target_include_directories(my_opengl_project PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/includes"   
)

target_link_libraries(my_opengl_project PRIVATE
    glfw
    OpenGL::GL
    assimp::assimp
    ${CMAKE_DL_LIBS}
)

# Copy resources (shaders, textures, etc.) next to the built executable
add_custom_command(TARGET my_opengl_project POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/resources"
        "$<TARGET_FILE_DIR:my_opengl_project>/resources"
    COMMENT "Copying resources to output directory"
    VERBATIM
)