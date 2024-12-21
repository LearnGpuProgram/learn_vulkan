cmake_minimum_required(VERSION 3.8) 

project(VulkanApp)

# 设置 CMake 构建类型
set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)

# 添加调试日志
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    message(STATUS "Debug mode enabled")
    add_definitions(-DDEBUG_MODE=1)
else()
    add_definitions(-DDEBUG_MODE=0)
endif()

link_directories(${CMAKE_SOURCE_DIR}/thirdpart/lib)
include_directories(${CMAKE_SOURCE_DIR}/thirdpart/include)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package (GLFW3 REQUIRED)
find_package (GLM REQUIRED)
find_package (Vulkan REQUIRED)

file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/*.cpp" "${CMAKE_SOURCE_DIR}/*.h")
list(FILTER SRC_FILES EXCLUDE REGEX "CMakeCXXCompilerId\\.cpp$")

set(SAMPLE_NAME main)
add_executable (${SAMPLE_NAME}  ${SRC_FILES})
set_target_properties (${SAMPLE_NAME} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${SAMPLE_NAME})
set_target_properties (${SAMPLE_NAME} PROPERTIES CXX_STANDARD 20)
target_link_libraries (${SAMPLE_NAME} Vulkan::Vulkan glfw3)