cmake_minimum_required(VERSION 3.14.3 FATAL_ERROR)

project(basic-tests-c-library C)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(basic-tests-c-library basic_c/tests.c basic_c/other_source_file.c basic_c/gui_test.c)
target_link_libraries(basic-tests-c-library m)
target_include_directories(basic-tests-c-library PUBLIC ${REPO_DIR}/src/interfaces/c)


project(system-test CXX)

add_executable(system-test main.cpp two_way_comm/two_way_comm.cpp)
target_link_libraries(system-test basic-tests-c-library)
if(PLATFORM_WINDOWS)
    # ws2_32: winsock2 (socket, connect, send, recv, …)
    # pthread: winpthreads (pthread_create, pthread_detach used in gui_api.h)
    target_link_libraries(system-test ws2_32 pthread)
endif()
target_include_directories(system-test PUBLIC ${REPO_DIR}/third_party/LumosAlgo/src)