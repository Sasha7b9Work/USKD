# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Device)

set(CMAKE_UNITY_BUILD ON)

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


add_compile_options(/wd4127)
add_compile_options(/wd4201)    # nonstandard extension used: nameless struct/union
add_compile_options(/wd4514)    # 'function' : unreferenced inline function has been removed
add_compile_options(/wd4623)    # default constructor was implicitly defined as deleted
add_compile_options(/wd4626)    # assignment operator was implicitly defined as deleted
add_compile_options(/wd4820)    # 'xxx': '4' bytes padding added after data member 'yyy'
add_compile_options(/wd5027)    # move assignment operator was implicitly defined as deleted
add_compile_options(/wd5045)    # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified


file(GLOB GD32F3                   ../common/GD32F30x_Library/Firmware/GD32F30x_standard_peripheral/Include/*.*)
#                                   ../common/GD32F30x_Library/Firmware/GD32F30x_standard_peripheral/Source/*.*)
file(GLOB SRC                            src/*.*
                                   ../common/*.h
                                   ../common/*.cpp)
file(GLOB LIBS_CJSON               ../common/libs/cJSON/cJSON.*
                                   ../common/libs/cJSON/cJSON_Utils.*)
file(GLOB DISPLAY                  ../common/Display/*.*)
file(GLOB DISPLAY_FONT             ../common/Display/Font/*.*)
file(GLOB HARDWARE                       src/Hardware/*.*
                                   ../common/Hardware/*.*)
file(GLOB HARDWARE_CMSIS                 src/Hardware/CMSIS/*.*)
file(GLOB HARDWARE_HAL                   src/Hardware/HAL/*.*
                                   ../common/Hardware/HAL/*.*)
file(GLOB HARDWARE_DHT22           ../common/Hardware/DHT22/*.*)
file(GLOB HARDWARE_MPU6050         ../common/Hardware/MPU6050/*.*)
file(GLOB MODEM                          src/Modem/*.*
                                   ../common/Modem/*.*)
file(GLOB MODEM_HTTP                     src/Modem/HTTP/*.*)
file(GLOB MODEM_SENDER             ../common/Modem/Sender/*.*)
file(GLOB UTILS                    ../common/Utils/*.*)

source_group(GD32F3                   FILES ${GD32F3})
source_group(src                      FILES ${SRC})
source_group(libs/cJSON               FILES ${LIBS_CJSON})
source_group(Display                  FILES ${DISPLAY})
source_group(Display/Font             FILES ${DISPLAY_FONT})
source_group(Hardware                 FILES ${HARDWARE})
source_group(Hardware/CMSIS           FILES ${HARDWARE_CMSIS})
source_group(Hardware/HAL             FILES ${HARDWARE_HAL})
source_group(Hardware/DHT22           FILES ${HARDWARE_DHT22})
source_group(Hardware/MPU6050         FILES ${HARDWARE_MPU6050})
source_group(Modem                    FILES ${MODEM})
source_group(Modem/HTTP               FILES ${MODEM_HTTP})
source_group(Utils                    FILES ${UTILS})

set_source_files_properties(${STM32F3} ${HARDWARE_CMSIS} ${LIBS_CJSON} ../common/Hardware/HAL/systick.c
                            PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(${PROJECT_NAME}
            ${GD32F3}
            ${SRC}
            ${LIBS_CJSON}
            ${DISPLAY}
            ${DISPLAY_FONT}
            ${HARDWARE}
            ${HARDWARE_CMSIS}
            ${HARDWARE_HAL}
            ${HARDWARE_DHT22}
            ${HARDWARE_MPU6050}
            ${MODEM}
            ${MODEM_HTTP}
            ${UTILS}
            )
