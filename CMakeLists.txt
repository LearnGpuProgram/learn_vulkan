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

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

link_directories(${CMAKE_SOURCE_DIR}/thirdpart/lib)
include_directories(${CMAKE_SOURCE_DIR}/thirdpart/include)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(GLFW3 REQUIRED)
find_package(GLM REQUIRED)
find_package(Vulkan REQUIRED)

set(SAMPLE_NAME main)

# 查找 glslangValidator
find_program(GLSLANG_VALIDATOR glslangValidator)

if(NOT GLSLANG_VALIDATOR)
    message(FATAL_ERROR "glslangValidator not found! Make sure Vulkan SDK is installed.")
endif()

# 设置源代码和目标目录
set(SHADER_DIR "${CMAKE_SOURCE_DIR}/media/shaders")
set(SPIRV_DIR "${CMAKE_SOURCE_DIR}/media/shaders")  # 输出目录改为可执行程序目录下的 media/shader

# 创建存放生成的 SPIR-V 文件的目录
file(MAKE_DIRECTORY ${SPIRV_DIR})

# 编译 GLSL 文件为 SPIR-V
file(GLOB VERT_FILES "${SHADER_DIR}/*.vert")
file(GLOB FRAG_FILES "${SHADER_DIR}/*.frag")
set(GLSL_FILES ${VERT_FILES} ${FRAG_FILES})
message(STATUS "Shader directory: ${SHADER_DIR}")
message(STATUS "Spriv directory: ${SPIRV_DIR}")

foreach(SHADER ${GLSL_FILES})
    # 获取文件名（不包括后缀）
    get_filename_component(SHADER_NAME ${SHADER} NAME_WE)

    # 设置目标 SPIR-V 文件路径
    set(SPIRV_FILE "${SPIRV_DIR}/${SHADER_NAME}.spv")

    # 添加自定义命令，使用 glslangValidator 编译 GLSL 为 SPIR-V
    add_custom_command(
        OUTPUT ${SPIRV_FILE}
        COMMAND ${GLSLANG_VALIDATOR} -V ${SHADER} -o ${SPIRV_FILE}
        DEPENDS ${SHADER}
        COMMENT "Compiling GLSL shader ${SHADER} to SPIR-V"
    )

    # 将 SPIR-V 文件作为源文件，加入到编译过程中
    list(APPEND SPIRV_FILES ${SPIRV_FILE})
endforeach()

# 定义自定义目标，确保 GLSL 编译在构建过程中执行
add_custom_target(compile_shaders ALL DEPENDS ${SPIRV_FILES})

# 添加源文件并创建可执行文件
file(GLOB_RECURSE SRC_FILES "${CMAKE_SOURCE_DIR}/*.cpp" "${CMAKE_SOURCE_DIR}/*.h")
list(FILTER SRC_FILES EXCLUDE REGEX "CMakeCXXCompilerId\\.cpp$")

# 在可执行文件创建之后设置输出目录
add_executable(${SAMPLE_NAME} ${SRC_FILES})

# 设置不同构建类型的输出目录
set_target_properties(${SAMPLE_NAME} PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/Debug
    RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/Release
)

# 设置目标属性
set_target_properties(${SAMPLE_NAME} PROPERTIES CXX_STANDARD 17)

# 连接 Vulkan 和 GLFW 库
target_link_libraries(${SAMPLE_NAME} Vulkan::Vulkan glfw3)

# 确保 shader 编译在构建过程中执行
add_dependencies(${SAMPLE_NAME} compile_shaders)

# 将生成的 SPIR-V 文件拷贝到可执行文件的输出目录
foreach(SHADER_FILE ${SPIRV_FILES})
    get_filename_component(SHADER_NAME ${SHADER_FILE} NAME)
    add_custom_command(
        TARGET ${SAMPLE_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy ${SHADER_FILE} $<TARGET_FILE_DIR:${SAMPLE_NAME}>/media/shaders/${SHADER_NAME}
        COMMENT "Copying shader ${SHADER_NAME} to executable directory"
    )
endforeach()
