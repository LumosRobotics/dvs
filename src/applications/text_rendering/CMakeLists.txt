cmake_minimum_required(VERSION 3.14.3 FATAL_ERROR)

project(text_rendering)

# include_directories(${REPO_DIR}/src/interfaces/cpp)
include_directories(${REPO_DIR}/third_party/LumosAlgo/src)


set( GLFW_BUILD_DOCS OFF CACHE BOOL  "GLFW lib only" )
set( GLFW_INSTALL OFF CACHE BOOL  "GLFW lib only" )
set( GLAD_GL "" )

option( GLFW-CMAKE-STARTER-USE-GLFW-GLAD "Use GLAD from GLFW" ON )

if( GLFW-CMAKE-STARTER-USE-GLFW-GLAD )
    include_directories("${GLFW_SOURCE_DIR}/deps")
    set( GLAD_GL "${GLFW_SOURCE_DIR}/deps/glad/gl.h" )
endif()

set( GLFW-CMAKE-STARTER-SRC
        main.cpp
        )

find_package(GLEW REQUIRED)

include_directories($(GLEW_INCLUDE_DIRS))

set(CPP_SOURCE_FILES main.cpp
                     gui_pane.cpp
                     shader.cpp
                     text_rendering.cpp
                     opengl_textrendering/font_atlas.cpp
                     opengl_textrendering/label_text_store.cpp
                     opengl_textrendering/buffers/IndexBuffer.cpp
                     opengl_textrendering/buffers/VertexArray.cpp
                     opengl_textrendering/buffers/VertexBuffer.cpp
                     opengl_textrendering/buffers/VertexBufferLayout.cpp
                     opengl_textrendering/buffers/gBuffers.cpp
                     main_window.cpp)

set(EXAMPLE_APP_FILES main_app.cpp
                      opengl_textrendering/font_atlas.cpp
                      opengl_textrendering/buffers/VertexBufferLayout.cpp
                      opengl_textrendering/buffers/VertexBuffer.cpp
                      opengl_textrendering/buffers/gBuffers.cpp
                      opengl_textrendering/buffers/VertexArray.cpp
                      opengl_textrendering/buffers/IndexBuffer.cpp
                      opengl_textrendering/geom_store.cpp
                      opengl_textrendering/app_window.cpp
                      opengl_textrendering/shaders/shader.cpp
                      opengl_textrendering/label_text_store.cpp)

add_executable(text_rendering ${CPP_SOURCE_FILES})
target_link_libraries(text_rendering ${wxWidgets_LIBRARIES}
                                    ${OPENGL_LIBRARIES}
                                    ${GLUT_LIBRARY}
                                    ${PLATFORM_LIBRARIES}
                                    /Users/danielpi/work/dvs/src/externals/freetype/build/libfreetype.a)
message("GLEW_LIBRARY: ${${GLEW_LIBRARY}}")
add_executable(text_rendering_app ${EXAMPLE_APP_FILES})
target_link_libraries(text_rendering_app 
                                     ${OPENGL_LIBRARIES}
                                     ${GLUT_LIBRARY}
                                     ${PLATFORM_LIBRARIES}
                                     /usr/local/Cellar/glew/2.2.0_1/lib/libGLEW.2.2.dylib
                                     /usr/local/Cellar/glew/2.2.0_1/lib/libGLEW.dylib
                                     glfw
                                     /Users/danielpi/work/dvs/src/externals/freetype/build/libfreetype.a)
target_include_directories(text_rendering_app PUBLIC ${GLEW_INCLUDE_DIRS})
