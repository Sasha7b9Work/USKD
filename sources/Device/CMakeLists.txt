# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Device)

#set(CMAKE_UNITY_BUILD OFF)

message("------------------------------------------------------------------------ ${PROJECT_NAME}")

include_directories(../common/_VS_KEIL/CMSIS/
                    ../common/_VS_KEIL/std_includes
                    ../common/GD32F30x_Library/Firmware/GD32F30x_standard_peripheral/Include/
                    ../common/GD32F30x_Library/Firmware/CMSIS/GD/GD32F30x/Include/
                    ../common/
                    ../common/Hardware/HAL/
                    src/
                    )

add_definitions(-DGD32F30X_HD)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")


add_compile_options(/wd4131)    # 'xxx' : use old-style declarator
add_compile_options(/wd4201)    # nonstandard extension used: nameless struct/union
add_compile_options(/wd4310)    # cast truncates constant value
add_compile_options(/wd4514)    # 'function' : unreferenced inline function has been removed
add_compile_options(/wd4623)    # default constructor was implicitly defined as deleted
add_compile_options(/wd4626)    # assignment operator was implicitly defined as deleted
add_compile_options(/wd4710)    # 'xxx' : function not inlined
add_compile_options(/wd4820)    # 'xxx': '4' bytes padding added after data member 'yyy'
add_compile_options(/wd5027)    # move assignment operator was implicitly defined as deleted
add_compile_options(/wd5045)    # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified
add_compile_options(/wd5204)    # 'type-name': class has virtual functions, but its trivial destructor is not virtual;
                                # instances of objects derived from this class may not be destructed correctly


file(GLOB GD32F3                   ../common/GD32F30x_Library/Firmware/GD32F30x_standard_peripheral/Include/*.*)
file(GLOB SRC                            src/*.*
                                   ../common/*.h
                                   ../common/*.cpp)
file(GLOB DISPLAY                  ../common/Display/*.*)
file(GLOB DISPLAY_FONT             ../common/Display/Font/*.*)
file(GLOB HARDWARE                       src/Hardware/*.*
                                   ../common/Hardware/*.*)
file(GLOB HARDWARE_CMSIS                 src/Hardware/CMSIS/*.*)
file(GLOB HARDWARE_HAL                   src/Hardware/HAL/*.*
                                   ../common/Hardware/HAL/*.*)
file(GLOB MODEM                          src/Modem/*.*
                                   ../common/Modem/*.*)
#file(GLOB MODEM_SWAGGER                  src/Modem/Swagger/*.*)
file(GLOB MODEM_MQTT                     src/Modem/MQTT/*.*)
#file(GLOB MODEM_MQTT_PAHO                src/Modem/MQTT/Paho/*.*)
file(GLOB MODULES_DHT22                  src/Modules/DHT22/*.*)
file(GLOB MODULES_GC777                  src/Modules/GC777/*.*)
file(GLOB MODULES_MPU6050                src/Modules/MPU6050/*.*)
file(GLOB STORAGE                        src/Storage/*.*)
file(GLOB UTILS                    ../common/Utils/*.*)

source_group(GD32F3                   FILES ${GD32F3})
source_group(src                      FILES ${SRC})
source_group(Display                  FILES ${DISPLAY})
source_group(Display/Font             FILES ${DISPLAY_FONT})
source_group(Hardware                 FILES ${HARDWARE})
source_group(Hardware/CMSIS           FILES ${HARDWARE_CMSIS})
source_group(Hardware/HAL             FILES ${HARDWARE_HAL})
source_group(Modem                    FILES ${MODEM})
#source_group(Modem/Swagger            FILES ${MODEM_SWAGGER})
source_group(Modem/MQTT               FILES ${MODEM_MQTT})
#source_group(Modem/MQTT/Paho          FILES ${MODEM_MQTT_PAHO})
source_group(Modules/DHT22            FILES ${MODULES_DHT22})
source_group(Modules/GC777            FILES ${MODULES_GC777})
source_group(Modules/MPU6050          FILES ${MODULES_MPU6050})
source_group(Storage                  FILES ${STORAGE})
source_group(Utils                    FILES ${UTILS})

set_source_files_properties(${STM32F3} ${HARDWARE_CMSIS} ../common/Hardware/HAL/systick.c
                            PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(${PROJECT_NAME}
            ${GD32F3}
            ${SRC}
            ${DISPLAY}
            ${DISPLAY_FONT}
            ${HARDWARE}
            ${HARDWARE_CMSIS}
            ${HARDWARE_HAL}
            ${MODEM}
#            ${MODEM_SWAGGER}
            ${MODEM_MQTT}
#            ${MODEM_MQTT_PAHO}
            ${MODULES_DHT22}
            ${MODULES_GC777}
            ${MODULES_MPU6050}
            ${STORAGE}
            ${UTILS}
)
