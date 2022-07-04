cmake_minimum_required(VERSION 3.7)
project(sys_info)

include(GenerateExportHeader)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)



find_package(Threads REQUIRED)

if(WIN32)
    # pdb files setup for release builds:
    add_definitions(/MP) # Корректно подключает многопоточную сборку в MSVC
    string(APPEND CMAKE_CXX_FLAGS_RELEASE " /Zi")
    string(APPEND CMAKE_EXE_LINKER_FLAGS_RELEASE " /INCREMENTAL:NO /DEBUG /OPT:REF")
    string(APPEND CMAKE_SHARED_LINKER_FLAGS_RELEASE " /INCREMENTAL:NO /DEBUG /OPT:REF")

    add_definitions(-DWIN32_LEAN_AND_MEAN -D_WIN32_WINNT=0x0602)

elseif (CMAKE_COMPILER_IS_GNUCXX OR MINGW OR (CMAKE_CXX_COMPILER_ID MATCHES "Clang"))
    string(APPEND CMAKE_CXX_FLAGS " -Wall -Wextra -Wno-parentheses -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unknown-pragmas -lpthread -ldl")
    string(APPEND CMAKE_CXX_FLAGS_RELEASE " -g")

    if (NOT MINGW)
        string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fno-omit-frame-pointer")

        if(USE_SANITIZER)
            # add address sanitazer check to debug build, and leave release build clean for check (for running under valgrind)
            string(APPEND CMAKE_CXX_FLAGS_DEBUG " -fsanitize=address")
            string(APPEND CMAKE_EXE_LINKER_FLAGS_DEBUG " -fsanitize=address")
            string(APPEND CMAKE_SHARED_LINKER_FLAGS_DEBUG " -fsanitize=address")
        endif()
    endif()
endif()

add_subdirectory(core)

file(GLOB FILES_SRC ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
file(GLOB FILES_HDRS ${CMAKE_CURRENT_SOURCE_DIR}/*.h)

add_executable(${PROJECT_NAME}
        ${FILES_SRC}
        ${FILES_HDRS})

target_link_libraries(${PROJECT_NAME} PUBLIC sysinfo::core PRIVATE Threads::Threads)
