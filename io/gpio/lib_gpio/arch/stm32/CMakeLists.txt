lib_gpio_add_architecture("stm32")

if("${LIB_GPIO_ARCHITECTURE}" STREQUAL "stm32")
    lib_gpio_add_dependencies(lib_convention lib_list lib_log lib_stm32hal lib_isr)
    lib_gpio_add_sourcefile_c(src/gpio_access_stm32.c src/gpio_event_stm32.c)
    lib_gpio_add_header(include)
    lib_gpio_add_public_definition(LIB_GPIO_STM32)
endif()

