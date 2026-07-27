cmake_minimum_required(VERSION 3.16)

project(FunctionalMapPipeline)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Eigen
include_directories(C:/eigen-3.4.0)

# Enable libigl modules
set(LIBIGL_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(LIBIGL_BUILD_TUTORIALS OFF CACHE BOOL "" FORCE)
set(LIBIGL_INSTALL OFF CACHE BOOL "" FORCE)

set(LIBIGL_OPENGL ON CACHE BOOL "" FORCE)
set(LIBIGL_GLFW ON CACHE BOOL "" FORCE)

add_subdirectory(C:/libigl ${CMAKE_BINARY_DIR}/libigl-build)

# -------------------------
# Stage 2
# -------------------------
add_executable(stage2 src/stage_2.cpp)
target_link_libraries(stage2 PRIVATE igl::core)

# -------------------------
# Stage 3
# -------------------------
add_executable(stage3 src/stage_3.cpp)
target_link_libraries(stage3 PRIVATE igl::core)

# -------------------------
# Stage 4
# -------------------------
add_executable(stage4 src/stage_4.cpp)
target_link_libraries(stage4 PRIVATE igl::core)

# -------------------------
# Stage 5
# -------------------------
add_executable(stage5 src/stage_5.cpp)
target_link_libraries(stage5 PRIVATE igl::core)

# -------------------------
# Stage 6
# -------------------------
add_executable(stage6 src/stage_6.cpp)
target_link_libraries(stage6 PRIVATE igl::glfw)