
cmake_minimum_required(VERSION 3.0.0)

project(vArtist C CXX)
project(vArtist VERSION 0.0.1)

include_directories(            "${CMAKE_SOURCE_DIR}/src")

find_package(                   SimpleEngine CONFIG REQUIRED)
message(STATUS                  "Found SimpleEngine package, settings at '${SimpleEngine_DIR}'")


set (COMMON_FLAGS               "-fno-strict-aliasing -pthread -ffast-math -mfpmath=sse -ggdb3 -Wall")

set(COMMON_C_FLAGS              "-std=gnu11 -fgnu89-inline")
#set(COMMON_CXX_FLAGS            "-std=gnu++17 -flto")
set(COMMON_CXX_FLAGS            "-std=gnu++17 ")

set(C_WARNING_FLAGS             "-Wignored-qualifiers -Wwrite-strings")
set(CXX_WARNING_FLAGS           "-Wmissing-declarations -Winvalid-pch")

add_definitions(                -D_GNU_SOURCE -D_FILE_OFFSET_BITS=64)
add_definitions(                -DSE_INTERACTIVE)

set(CMAKE_C_FLAGS               "${COMMON_FLAGS} ${COMMON_C_FLAGS} ${C_WARNING_FLAGS}")
set(CMAKE_CXX_FLAGS             "${COMMON_FLAGS} ${COMMON_CXX_FLAGS} ${CXX_WARNING_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS      "${COMMON_FLAGS} -Wl,--hash-style=gnu")

# Release build type
set(CMAKE_C_FLAGS_RELEASE       "${RELEASE_FLAGS}")
set(CMAKE_CXX_FLAGS_RELEASE     "${RELEASE_FLAGS}")
# Debug build type
set(CMAKE_C_FLAGS_DEBUG         "${DEBUG_FLAGS}")
set(CMAKE_CXX_FLAGS_DEBUG       "${DEBUG_FLAGS}")

if (NOT CMAKE_BUILD_TYPE)
        set(CMAKE_BUILD_TYPE    Debug CACHE STRING
                                "Build types: Debug(default) Release"
                                FORCE
                                )
endif()

set(EXECUTABLE_OUTPUT_PATH      "${CMAKE_SOURCE_DIR}")
set(LIBRARY_OUTPUT_PATH         "${CMAKE_SOURCE_DIR}")

set(COMMON_INC_DIRS)
set(COMMON_LIBRARIES_LIST)

### check dependencies
find_package(Boost 1.54.0 REQUIRED COMPONENTS program_options filesystem system)
include_directories (${Boost_INCLUDE_DIRS})
list(APPEND COMMON_LIBRARIES_LIST ${Boost_LIBRARIES})

###

include_directories(${COMMON_INC_DIRS} ${CMAKE_CURRENT_SOURCE_DIR})

set(SOURCES     "src/Main.cpp"
                "src/Scene.cpp"
                "src/ImGuiWrapper.cpp"
                )

add_executable(         vartist
                        ${SOURCES})

target_link_libraries(  vartist
                        ${COMMON_LIBRARIES_LIST}
                        SimpleEngine::SECommon
                        SimpleEngine::SE
                        SimpleEngine::SEImGui)


