cmake_minimum_required(VERSION 3.14.3 FATAL_ERROR)

project(interface-test-cpp CXX)

include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(interface-test-cpp communication_header_object_test.cpp fillable_uint8_array_test.cpp communication_header_test.cpp plot_properties_test.cpp property_set_test.cpp)
target_link_libraries(interface-test-cpp ${GTEST_LIB_FILES})
target_include_directories(interface-test-cpp PUBLIC ${REPO_DIR}/src/interfaces/cpp)