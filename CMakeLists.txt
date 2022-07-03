cmake_minimum_required(VERSION 3.22)
project(sys_info)

include(GenerateExportHeader)

set(CMAKE_CXX_STANDARD 14)


file(GLOB FILES_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB FILES_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME}
        ${FILES_SRC}
        ${FILES_HDRS})

#target_compile_definitions(${PROJECT_NAME} PRIVATE -DPSAPI_VERSION=0)
