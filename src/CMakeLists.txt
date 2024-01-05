



add_executable(
    ledblink

    ledblink/main.cpp
    ledblink/system_stm32l4xx.c
    ledblink/startup_stm32l432xx.s
    ledblink/stm32l4xx_hal_msp.c
    ledblink/stm32l4xx_it.c
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

###################################################################

add_executable(
    usart-print

    usart-print/main.cpp
    usart-print/system_stm32l4xx.c
    usart-print/startup_stm32l432xx.s
    usart-print/stm32l4xx_hal_msp.c
    usart-print/stm32l4xx_it.c
)


target_link_libraries(
    usart-print
    PRIVATE

    stm32l4xx_cmsis
    stm32l4xx_hal
    # fmt::fmt
)


target_link_options(usart-print PRIVATE -T ${CMAKE_CURRENT_SOURCE_DIR}/STM32L432KCUX_FLASH.ld)


add_custom_command(
    TARGET usart-print
    POST_BUILD
    BYPRODUCTS usart-print.bin
    DEPENDS usart-print
    COMMENT "generate .bin file"
    COMMAND ${OBJCOPY_BIN} -O binary "$<TARGET_FILE:usart-print>" "$<TARGET_FILE:usart-print>.bin"
    VERBATIM
)

add_custom_target(
    flash-usart-print
    DEPENDS usart-print.bin
    COMMAND st-flash write usart-print.bin 0x08000000
)