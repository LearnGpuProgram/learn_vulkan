cmake_minimum_required(VERSION 3.8) 

project(VulkanDebug)

link_directories(${CMAKE_SOURCE_DIR}/lib)
include_directories(${CMAKE_SOURCE_DIR}/include)


set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package (GLFW3 REQUIRED)
find_package (GLM REQUIRED)
find_package (Vulkan REQUIRED)


set(SAMPLE_NAME main)
add_executable (${SAMPLE_NAME} ${SAMPLE_NAME}.cpp)
set_target_properties (${SAMPLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${SAMPLE_NAME})
set_target_properties (${SAMPLE_NAME} PROPERTIES CXX_STANDARD 20)
target_link_libraries (${SAMPLE_NAME} Vulkan::Vulkan glfw3)