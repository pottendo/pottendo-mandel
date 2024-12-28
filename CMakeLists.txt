
# for pico under VSCode - run "cmake -G Ninja -B build"
#set(TARGETX pico)
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

elseif(${TARGETX}x STREQUAL "picox")
# == DO NOT EDIT THE FOLLOWING LINES for the Raspberry Pi Pico VS Code Extension to work ==
if(WIN32)
    set(USERHOME $ENV{USERPROFILE})
else()
    set(USERHOME $ENV{HOME})
endif()
set(sdkVersion 2.1.0)
set(toolchainVersion 13_3_Rel1)
set(picotoolVersion 2.1.0)
set(picoVscode ${USERHOME}/.pico-sdk/cmake/pico-vscode.cmake)
if (EXISTS ${picoVscode})
    include(${picoVscode})
endif()
# ====================================================================================
cmake_minimum_required(VERSION 3.10)
set(SDK_PATH $ENV{PICO_SDK_PATH})
if(NOT SDK_PATH)
    message(WARNING "Please Set pico SDK Path. Such as:export PICO_SDK_PATH=/home/user/.pico-sdk/sdk/2.1.0")
    return()
endif()
set(ARCHSRC "mandel-pico.cpp")
set(PICO_BOARD pico2_w CACHE STRING "Board type")
set(PICOLIBS pico_stdlib pico-ili9341xpt2046)
set(TFTLIB "../rpi-hacks/pico/pottendo-pico-ili9341xpt2046/src")
include(pico_sdk_import.cmake)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)
add_compile_definitions(-DPICO)
else()
set(ARCHSRC "mandel-luckfox.cpp")
set(THREAD -DPTHREADS)
endif()

message(STATUS "target = " ${TARGETX} ${PICOLIBS})
add_compile_options(-g -Wall -O3 ${SYSROOTO} ${LINKOPTS} ${LUCKFOX} ${THREAD})
add_link_options(${LINKOPTS})

project(mandel C CXX ASM)

set(SRC_FILES mandel.cpp ${ARCHSRC})
add_executable(${PROJECT_NAME} ${SRC_FILES})

if (NOT ${TARGETX}x STREQUAL "picox")
#Opencv 4 
find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
else()
pico_sdk_init()
add_subdirectory(${TFTLIB} build)

pico_set_program_name(${PROJECT_NAME} ${PROJECT_NAME})
pico_set_program_version(${PROJECT_NAME} "0.1")
pico_add_extra_outputs(${PROJECT_NAME})
endif()


target_link_libraries(${PROJECT_NAME}
    ${PICOLIBS}
    ${OpenCV_LIBS}
    ${TSLIBS}
)

target_include_directories(${PROJECT_NAME} PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${OpenCV_INCLUDE_DIRS}
    ${TFTLIB}
)

if (PICO_CYW43_SUPPORTED)
    target_link_libraries(${PROJECT_NAME} pico_cyw43_arch_none)
endif()
