cmake_minimum_required(VERSION 3.15)

project(grid_generator VERSION 1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")

set(SFML_BUILD_AUDIO FALSE)
include_directories(/usr/include/freetype2)
add_subdirectory(external/SFML)

set(SOURCES
    src/main.cpp
    src/rectangular.cpp
    src/renderer.cpp
    src/triangular.cpp
)

set(HEADERS
    src/geometry.h
    src/rectangular.h
    src/renderer.h
    src/triangular.h
)

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

target_link_libraries(${PROJECT_NAME} PRIVATE sfml-graphics sfml-window sfml-system)

target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_include_directories(${PROJECT_NAME} PRIVATE external/SFML/include)

if (UNIX AND NOT APPLE)
    target_link_libraries(${PROJECT_NAME} PRIVATE sfml-network pthread)
endif()
