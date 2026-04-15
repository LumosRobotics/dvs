cmake_minimum_required(VERSION 3.14.3 FATAL_ERROR)

project(real-time-gui)

include_directories(${REPO_DIR}/src/interfaces/cpp)

set(CPP_SOURCE_FILES main.cpp
                     rt_main_window.cpp)

add_executable(real-time-gui ${CPP_SOURCE_FILES})
target_link_libraries(real-time-gui ${wxWidgets_LIBRARIES}
                                    ${OPENGL_LIBRARIES}
                                    ${GLUT_LIBRARY}
                                    ${PLATFORM_LIBRARIES}
                                    ${REPO_DIR}/src/externals/freetype/build/libfreetype.a)
