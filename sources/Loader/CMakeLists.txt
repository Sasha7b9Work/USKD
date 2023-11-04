# (c) Aleksandr Shevchenko e-mail : Sasha7b9@tut.by

project (Loader)

#set(CMAKE_UNITY_BUILD OFF)

message("------------------------------------------------------------------------ ${PROJECT_NAME}")

include_directories(../common/_VS_KEIL/CMSIS/
                    ../common/_VS_KEIL/std_includes/
                    ../common/GD32F30x_Library/Firmware/GD32F30x_standard_peripheral/Include/
                    ../common/GD32F30x_Library/Firmware/CMSIS/GD/GD32F30x/Include/
                    ../common/
                    ../common/Hardware/HAL/
                    src/
                    )

add_definitions(-DGD32F30X_HD)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -MP -Wall")

add_compile_options(/wd4127)
add_compile_options(/wd4514)    # 'function' : unreferenced inline function has been removed
add_compile_options(/wd4626)    # assignment operator was implicitly defined as deleted
add_compile_options(/wd4710)    # 'xxx' : function not inlined
add_compile_options(/wd4820)    # 'xxx': '4' bytes padding added after data member 'yyy'
add_compile_options(/wd5027)    # move assignment operator was implicitly defined as deleted
add_compile_options(/wd5045)    # Compiler will insert Spectre mitigation for memory load if /Qspectre switch specified

file(GLOB GD32F3                ../common/GD32F30x_Library/Firmware/GD32F30x_standard_peripheral/Include/*.*
                                ../common/GD32F30x_Library/Firmware/GD32F30x_standard_peripheral/Source/*.*)
file(GLOB SRC                   src/*.*
                                ../common/*.h ../common/*.cpp)
file(GLOB HARDWARE                    src/Hardware/*.*
                                ../comm)
file(GLOB HARDWARE_CMSIS              src/Hardware/CMSIS/*.*)
file(GLOB HARDWARE_HAL          ../common/Hardware/HAL/*.*)
file(GLOB MEASURER              ../common/Measurer/*.*)
file(GLOB MODEM                       src/Modem/*.*
                                ../common/Modem/Parser.*
                                ../common/Modem/Modem.*
                                ../common/Modem/SIM868*.*)
file(GLOB UTILS                 ../common/Utils/Math.*)

source_group(GD32F3             FILES ${GD32F3})
source_group(src                FILES ${SRC})
source_group(Hardware           FILES ${HARDWARE})
source_group(Hardware/CMSIS     FILES ${HARDWARE_CMSIS})
source_group(Hardware/HAL       FILES ${HARDWARE_HAL})
source_group(Measurer           FILES ${MEASURER})
source_group(Modem              FILES ${MODEM})
source_group(Utils              FILES ${UTILS})

set_source_files_properties(${GD32F3} ${HARDWARE_CMSIS} ../common/Hardware/HAL/systick.c
                            PROPERTIES HEADER_FILE_ONLY TRUE)

add_library(${PROJECT_NAME}
            ${GD32F3}
            ${SRC}
            ${HARDWARE}
            ${HARDWARE_CMSIS}
            ${HARDWARE_HAL}
            ${MEASURER}
            ${MODEM}
            ${UTILS}
            )
