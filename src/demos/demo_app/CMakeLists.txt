cmake_minimum_required(VERSION 3.14.3 FATAL_ERROR)

project(demos)

include_directories(${REPO_DIR}/src/externals/liquidfun/liquidfun/Box2D)

include_directories(${REPO_DIR}/src/externals/reactphysics3d/include)

include_directories(${REPO_DIR}/third_party/LumosAlgo/src)
include_directories(${REPO_DIR}/src/main_application)
include_directories(${REPO_DIR}/src/common)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${REPO_DIR}/src/externals/rapidxml)

add_executable(demos main.cpp
                     tests/spring/spring.cpp
                     tests/sphere/sphere.cpp
                     tests/sphere/sphere_subdivision/edge.cpp
                     tests/sphere/sphere_subdivision/face.cpp
                     tests/sphere/sphere_subdivision/point.cpp
                     tests/sphere/sphere_subdivision/subdivisor.cpp
                     tests/particles_density/particles_density.cpp
                     tests/joints/joints.cpp
                     tests/gui/gui.cpp
                     tests/marbles/marbles.cpp
                     tests/sling_plot/sling_plot.cpp
                     tests/boxes/boxes.cpp
                     tests/slam/slam.cpp
                     tests/particle_field/particle_field.cpp
                     tests/ad_dataset/ad_dataset.cpp)

target_link_libraries(demos 
        ${REPO_DIR}/src/externals/liquidfun/liquidfun/Box2D/Box2D/Debug/libliquidfun.a
        ${REPO_DIR}/src/externals/reactphysics3d/build/libreactphysics3d.a
        -lncurses)
