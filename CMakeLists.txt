cmake_minimum_required(VERSION 3.18)
project(MeshViewer)

option(MESHVIEWER_USE_APPLE_FRAMEWORK_GLUT
    "macOS: utiliser le GLUT Apple (-framework GLUT). Peut crasher en VM (GLUT 3)." OFF)
option(MESHVIEWER_USE_HOMEBREW_FREEGLUT
    "macOS: lier FreeGLUT Homebrew (X11) au lieu d’un FreeGLUT Cocoa embarqué" OFF)

set(CMAKE_CXX_STANDARD 17)
set(OpenGL_GL_PREFERENCE LEGACY)

if(APPLE)
    add_compile_definitions(GL_SILENCE_DEPRECATION)
endif()

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

# macOS : par défaut, FreeGLUT **Cocoa** (téléchargé + compilé) — pas de X11, pas de GLUT Apple (souvent cassé en VM).
set(MESHVIEWER_FREEGLUT_ROOT "")
if(APPLE AND MESHVIEWER_USE_HOMEBREW_FREEGLUT)
    if(DEFINED ENV{HOMEBREW_PREFIX})
        set(_HB "$ENV{HOMEBREW_PREFIX}")
        if(EXISTS "${_HB}/include/GL/freeglut.h")
            set(MESHVIEWER_FREEGLUT_ROOT "${_HB}")
        endif()
    endif()
    if(MESHVIEWER_FREEGLUT_ROOT STREQUAL "" AND EXISTS "/opt/homebrew/include/GL/freeglut.h")
        set(MESHVIEWER_FREEGLUT_ROOT "/opt/homebrew")
    endif()
    if(MESHVIEWER_FREEGLUT_ROOT STREQUAL "" AND EXISTS "/usr/local/include/GL/freeglut.h")
        set(MESHVIEWER_FREEGLUT_ROOT "/usr/local")
    endif()
    if(NOT MESHVIEWER_FREEGLUT_ROOT STREQUAL "")
        find_library(MESHVIEWER_GLUT_LIB glut PATHS "${MESHVIEWER_FREEGLUT_ROOT}/lib" NO_DEFAULT_PATH)
    endif()
endif()

if(APPLE AND MESHVIEWER_USE_HOMEBREW_FREEGLUT AND NOT MESHVIEWER_FREEGLUT_ROOT STREQUAL "" AND MESHVIEWER_GLUT_LIB)
    message(STATUS "MeshViewer: FreeGLUT Homebrew (X11) : ${MESHVIEWER_GLUT_LIB}")
    target_include_directories(MeshViewer BEFORE PRIVATE "${MESHVIEWER_FREEGLUT_ROOT}/include")
    target_link_libraries(MeshViewer PRIVATE ${OPENGL_LIBRARIES} ${MESHVIEWER_GLUT_LIB})
elseif(APPLE AND MESHVIEWER_USE_APPLE_FRAMEWORK_GLUT)
    message(STATUS "MeshViewer: GLUT Apple (framework) — en cas de crash GLUT 3, reconfigure sans cette option")
    target_compile_definitions(MeshViewer PRIVATE MESHVIEWER_USE_APPLE_GLUT=1)
    target_link_libraries(MeshViewer PRIVATE ${OPENGL_LIBRARIES} "-framework GLUT")
elseif(APPLE)
    include(FetchContent)
    set(FREEGLUT_COCOA ON CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_STATIC_LIBS ON CACHE BOOL "" FORCE)
    set(FREEGLUT_BUILD_DEMOS OFF CACHE BOOL "" FORCE)
    set(FREEGLUT_REPLACE_GLUT ON CACHE BOOL "" FORCE)
    FetchContent_Declare(
        freeglut
        GIT_REPOSITORY https://github.com/FreeGLUT/freeglut.git
        GIT_SHALLOW  TRUE
        GIT_TAG        master
    )
    # MakeAvailable n’existe pas en « pre-patch » : on peuple, on corrige la VM, puis on ajoute.
    FetchContent_GetProperties(freeglut POPULATED _fg_pop)
    if(NOT _fg_pop)
        FetchContent_Populate(freeglut)
    endif()
    set(_fgsrc "${freeglut_SOURCE_DIR}/src/cocoa/fg_window_cocoa.m")
    if(EXISTS "${_fgsrc}")
        file(READ "${_fgsrc}" _fgco)
        if(_fgco MATCHES "NSOpenGLPFAAccelerated")
            string(REPLACE
                "    attrs[attrIndex++]                     = NSOpenGLPFAAccelerated; // choose hardware acceleration"
                "    /* MeshViewer: pas de NSOpenGLPFAAccelerated — requis par les VM sans accél. OpenGL */"
                _fgco
                "${_fgco}"
            )
            file(WRITE "${_fgsrc}" "${_fgco}")
            message(STATUS "MeshViewer: correctif appliqué (FreeGLUT VM — pixel format sans GPU obligatoire)")
        endif()
    endif()
    add_subdirectory("${freeglut_SOURCE_DIR}" "${freeglut_BINARY_DIR}" EXCLUDE_FROM_ALL)
    message(STATUS "MeshViewer: FreeGLUT embarqué (Cocoa, statique) — remplace le GLUT Apple")
    target_include_directories(MeshViewer BEFORE PRIVATE "${freeglut_SOURCE_DIR}/include")
    target_compile_definitions(MeshViewer PRIVATE
        FREEGLUT_STATIC
        MESHVIEWER_FREEGLUT_COCOA_EMBEDDED=1
    )
    target_link_libraries(MeshViewer PRIVATE freeglut_static ${OPENGL_LIBRARIES})
else()
    find_library(MESHVIEWER_GLUT_LIB glut)
    if(MESHVIEWER_GLUT_LIB)
        target_link_libraries(MeshViewer PRIVATE ${OPENGL_LIBRARIES} ${MESHVIEWER_GLUT_LIB})
    else()
        message(FATAL_ERROR "libglut (FreeGLUT) introuvable — installe le paquet glut/freeglut de ta distro.")
    endif()
endif()
