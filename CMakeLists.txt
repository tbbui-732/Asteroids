# CMake v3.31
cmake_minimum_required(VERSION 3.31)

# Asteroids v1.0
set(CMAKE_OSX_DEPLOYMENT_TARGET "15.1")
project(Asteroid VERSION 1.0)

# C99 required
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED True)

# Executables
add_executable(${PROJECT_NAME} src/main.c)

# Libraries
target_link_libraries(${PROJECT_NAME} ${CMAKE_SOURCE_DIR}/lib/libraylib.a)

# Include directory (Headers)
target_include_directories(Asteroid PUBLIC 
                    "${PROJECT_BINARY_DIR}"
                )

# OSX-specific frameworks
if (APPLE)
    target_link_libraries(${PROJECT_NAME} "-framework IOKit")
    target_link_libraries(${PROJECT_NAME} "-framework Cocoa")
    target_link_libraries(${PROJECT_NAME} "-framework OpenGL")
endif()
