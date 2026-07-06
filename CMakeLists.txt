cmake_minimum_required(VERSION 3.16)

project(FunctionalMapPipeline)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Eigen
include_directories(
    /home/student/libs/eigen-3.4.0
)

# libigl
include_directories(
    /home/student/libs/libigl-main/include
)

# -------------------------
# Stage 2
# -------------------------
add_executable(stage2 stage_2.cpp)

# -------------------------
# Stage 3
# -------------------------
add_executable(stage3 stage_3.cpp)

# -------------------------
# Stage 4
# -------------------------
add_executable(stage4 stage_4.cpp)

# -------------------------
# Stage 5
# -------------------------
add_executable(stage5 stage_5.cpp)
