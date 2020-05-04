cmake_minimum_required(VERSION 3.10)

file(GLOB sources "${CMAKE_CURRENT_SOURCE_DIR}/source/*.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/source/*.cxx"
"${CMAKE_CURRENT_SOURCE_DIR}/source/*.c"
"${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
"${CMAKE_CURRENT_SOURCE_DIR}/include/*.hpp")


add_subdirectory("vendor/GLFW")

add_library(dummy INTERFACE)
target_include_directories(dummy INTERFACE "${CMAKE_CURRENT_SOURCE_DIR}/vendor/glm")

add_library(Library "${sources}")

target_include_directories(Library PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/vendor/GLFW/include"
"${CMAKE_CURRENT_SOURCE_DIR}/vendor/Vulkan/Include"
"${CMAKE_CURRENT_SOURCE_DIR}/include"
"${CMAKE_CURRENT_SOURCE_DIR}/vendor/freetype/include")

find_library(Vulkan vulkan-1 "${CMAKE_CURRENT_SOURCE_DIR}/vendor/Vulkan/Lib")
find_library(Freetype freetype "${CMAKE_CURRENT_SOURCE_DIR}/vendor/freetype/win64")

target_link_libraries(Library glfw ${GLFW_LIBRARIES})
target_link_libraries(Library ${Vulkan})
target_link_libraries(Library ${Freetype})
target_link_libraries(Library dummy)
