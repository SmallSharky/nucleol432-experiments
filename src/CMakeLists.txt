



add_executable(
    ledblink

    ledblink/main.cpp
    ledblink/system_stm32l4xx.c
    ledblink/startup_stm32l432xx.s
    ledblink/stm32l4xx_hal_msp.c
    ledblink/stm32l4xx_it.c
    # target-efm32pg23-obj
)


target_link_libraries(
    ledblink
    PRIVATE

    stm32l4xx_cmsis
    stm32l4xx_hal
)


target_link_options(ledblink PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/STM32L432KCUX_FLASH.ld)


add_custom_command(
    TARGET ledblink
    POST_BUILD
    BYPRODUCTS ledblink.bin
    DEPENDS ledblink
    COMMENT "generate .bin file"
    COMMAND ${OBJCOPY_BIN} -O binary "$<TARGET_FILE:ledblink>" "$<TARGET_FILE:ledblink>.bin"
    VERBATIM
)

add_custom_target(
    flash-ledblink
    DEPENDS ledblink.bin
    COMMAND st-flash write ledblink.bin 0x08000000
)