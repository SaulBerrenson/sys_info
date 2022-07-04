cmake_minimum_required(VERSION 3.7)
project(sys_core)

set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON) #lazy export win symbols (only for lib)

file(GLOB FILES_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB FILES_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

add_library(${PROJECT_NAME}
        ${FILES_SRC}
        ${FILES_HDRS}
)

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)       

add_library(sysinfo::core ALIAS ${PROJECT_NAME})

#target_compile_definitions(${PROJECT_NAME} PRIVATE -DPSAPI_VERSION=0)
