cmake_minimum_required(VERSION 3.10)

file(GLOB app_sources "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cxx"
"${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/source/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp"
"${CMAKE_CURRENT_SOURCE_DIR}/include/*.h")

add_executable(Application ${app_sources})

target_include_directories(Application PUBLIC "${CMAKE_PROJECT_SOURCE_DIR}/Library/include"
"${CMAKE_CURRENT_SOURCE_DIR}/include")
target_link_libraries(Application PUBLIC Library )

