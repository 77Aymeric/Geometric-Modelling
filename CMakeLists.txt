cmake_minimum_required(VERSION 3.10)
project(MeshViewer)

set(CMAKE_CXX_STANDARD 17)
set(OpenGL_GL_PREFERENCE LEGACY)

find_package(OpenGL REQUIRED)

add_executable(MeshViewer
    main.cpp
    myHalfedge.cpp
    myVector3D.cpp
    myPoint3D.cpp
    myFace.cpp
    myMesh.cpp
    myVertex.cpp
    external/glew/src/glew.c
)

set_source_files_properties(external/glew/src/glew.c PROPERTIES LANGUAGE C)

target_include_directories(MeshViewer PRIVATE
    ${OPENGL_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glew/include
    ${CMAKE_CURRENT_SOURCE_DIR}/external/glm
)

target_link_libraries(MeshViewer
    ${OPENGL_LIBRARIES}
    "-framework GLUT"
)
