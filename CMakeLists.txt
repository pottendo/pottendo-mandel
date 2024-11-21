cmake_minimum_required(VERSION 3.10)

project(mandel)

#Opencv 4 
#set(OpenCV_DIR "${CMAKE_CURRENT_SOURCE_DIR}/lib/cmake/opencv4")
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS} )

set(SRC_FILES mandel.cpp mandel-luckfox.cpp)

add_compile_options(-g -Wall -DPTHREADS -DLUCKFOX)

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${OpenCV_INCLUDE_DIRS}
)

