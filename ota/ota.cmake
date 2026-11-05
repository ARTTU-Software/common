# OTA over CAN: CMake helpers for board applications and bootloaders.
#
#   include(${CMAKE_CURRENT_LIST_DIR}/Drivers/common/ota/ota.cmake)
#
#   ota_add_app(${CMAKE_PROJECT_NAME})          # application: enter/ping hook (ota_app + STM32 glue)
#   ota_add_bootloader(${CMAKE_PROJECT_NAME})   # bootloader:  core + STM32 runner + flash back-end
#
# The STM32 family (G4/H5/L5) is picked from the HAL headers already on the
# target's include path, so all family back-ends are added and only the
# matching one compiles to code.

set(OTA_DIR ${CMAKE_CURRENT_LIST_DIR})

set(OTA_PORT_COMMON_SOURCES
    ${OTA_DIR}/port/ota_stm32_common.c
)

function(_ota_add_includes target)
    target_include_directories(${target} PRIVATE
        ${OTA_DIR}
        ${OTA_DIR}/boards
        ${OTA_DIR}/port
    )
endfunction()

function(ota_add_app target)
    target_sources(${target} PRIVATE
        ${OTA_DIR}/ota_app.c
        ${OTA_PORT_COMMON_SOURCES}
        ${OTA_DIR}/port/ota_app_stm32.c
    )
    _ota_add_includes(${target})
endfunction()

function(ota_add_bootloader target)
    target_sources(${target} PRIVATE
        ${OTA_DIR}/ota_boot.c
        ${OTA_DIR}/ota_crc32.c
        ${OTA_PORT_COMMON_SOURCES}
        ${OTA_DIR}/port/ota_bootloader_stm32.c
        ${OTA_DIR}/port/ota_flash_stm32g4.c
        ${OTA_DIR}/port/ota_flash_stm32h5.c
        ${OTA_DIR}/port/ota_flash_stm32l5.c
    )
    _ota_add_includes(${target})
endfunction()
