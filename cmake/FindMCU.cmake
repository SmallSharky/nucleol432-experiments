
add_definitions(
    # -DSTM32G431xx
    # -DUSE_HAL_DRIVER
    # -DUSE_FULL_LL_DRIVER
    -DSTM32L432xx
    -DARM_MATH_CM4
    -Dflash_layout
    -DSTM32L432C6
)

add_compile_options(
    -mfloat-abi=hard -mfpu=fpv4-sp-d16
    -fno-common
    -mcpu=cortex-m4
)

add_link_options(
    -mfloat-abi=hard -mfpu=fpv4-sp-d16
    -fno-common
    -mcpu=cortex-m4
    -Wl,--no-warn-rwx-segments
)
