cmake_minimum_required(VERSION 3.14.3 FATAL_ERROR)

project(duoplot CXX)

set(REPO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/..)

if(UNIX AND NOT APPLE)
    set(PLATFORM_LINUX TRUE)
    add_compile_definitions(PLATFORM_LINUX_M)
elseif(APPLE)
    set(PLATFORM_APPLE TRUE)
    add_compile_definitions(PLATFORM_APPLE_M)
elseif(WIN32)
    set(PLATFORM_WINDOWS TRUE)
    add_compile_definitions(PLATFORM_WINDOWS_M)
else()
    message(FATAL_ERROR "Unknown platform!" )
endif()

if(PLATFORM_LINUX)
    include(cmake/linux.cmake)
else()
    include(cmake/macos.cmake)
endif()

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wno-write-strings -Wno-deprecated \
    -Wall -Wextra -Wno-deprecated-copy -pedantic -fdiagnostics-color")

# if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
    # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 --coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++17")
# endif()

include_directories(${REPO_DIR}/src/)

include_directories(${REPO_DIR}/src/common)

# Googletest
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/externals/googletest/build/lib)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/externals/googletest/googletest/include)

# Qt6
set(CMAKE_PREFIX_PATH "/usr/local/opt/qt/lib/cmake")
find_package(Qt6 COMPONENTS Core Widgets OpenGL OpenGLWidgets QUIET)

# Nlohmann
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/externals/nlohmann/include)

# GLM
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/externals/glm/)

# ****** wxWidgets ******
# find_package(wxWidgets COMPONENTS core base gl adv)

set(WX_CUSTOM_INC_PATH ${REPO_DIR}/src/externals/wxwidgets/include)
set(WX_CUSTOM_LIB_PATH ${REPO_DIR}/src/externals/wxwidgets/lib)

include_directories(${WX_CUSTOM_INC_PATH})
link_directories(${WX_CUSTOM_LIB_PATH})


# ****** freetype ******
include_directories(${REPO_DIR}/src/externals/freetype/build/include)
include_directories(${REPO_DIR}/src/externals/freetype/include)

set(GTEST_LIB_FILES libgtest.a
                    libgtest_main.a
                    pthread)

# ****** OpenGL ******
find_package(OpenGL REQUIRED)
find_package(GLUT REQUIRED)

include_directories(${OPENGL_INCLUDE_DIRS})

# Adding subdirectories
add_subdirectory(applications/gui_testing)
add_subdirectory(applications/text_rendering)
add_subdirectory(applications/real_time_gui)
add_subdirectory(system_test)
add_subdirectory(interfaces/test/c)
add_subdirectory(interfaces/test/cpp)
add_subdirectory(demos/demo_app)
add_subdirectory(demos/plot_stuff)
add_subdirectory(main_application)
add_subdirectory(qt_application)
add_subdirectory(test)
add_subdirectory(applications/module_application)

add_subdirectory(applications/text_rendering/glfw)
