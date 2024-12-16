cmake_minimum_required(VERSION 3.10)

set(TARGETX $ENV{TARGET})
if(${TARGETX}x STREQUAL "luckfoxx")
set(SDK_PATH $ENV{LUCKFOX_SDK_PATH})
if(NOT SDK_PATH)
    message(WARNING "Please Set Luckfox-pico SDK Path. Such as:export LUCKFOX_SDK_PATH=/home/user/luckfox-pico")
    return()
endif()
set(LUCKFOX "-DLUCKFOX")
set(CMAKE_C_COMPILER "${SDK_PATH}/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-gcc")
set(CMAKE_CXX_COMPILER "${SDK_PATH}/tools/linux/toolchain/arm-rockchip830-linux-uclibcgnueabihf/bin/arm-rockchip830-linux-uclibcgnueabihf-g++")
set(SYSROOT "${SDK_PATH}/sysdrv/source/buildroot/buildroot-2023.02.6/output/host/arm-buildroot-linux-uclibcgnueabihf/sysroot")
set(SYSROOTO "--sysroot=${SYSROOT}")
set(OpenCV_DIR "/work/src/luckfox/demos/opencv-mobile-4.10.0/build/install/lib/cmake/opencv4")
set(LINKOPTS "-L${SYSROOT}/usr/lib")
set(COPTS "-I${SYSROOT}/include")
set(ARCHSRC "mandel-luckfox.cpp")
set(TSLIBS "ts")
else()
set(ARCHSRC "mandel-luckfox.cpp")
endif()
message(STATUS "target = " ${TARGETX})

project(mandel)
#Opencv 4 
find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
set(SRC_FILES mandel.cpp ${ARCHSRC})

add_compile_options(-g -Wall -O3 ${SYSROOTO} ${LINKOPTS} ${LUCKFOX} -DPTHREADS)
add_link_options(${LINKOPTS})

add_executable(${PROJECT_NAME} ${SRC_FILES})

target_link_libraries(${PROJECT_NAME}
    ${OpenCV_LIBS}
    ${TSLIBS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/
    ${OpenCV_INCLUDE_DIRS}
)

