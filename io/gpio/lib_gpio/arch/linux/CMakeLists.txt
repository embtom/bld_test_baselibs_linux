
lib_gpio_add_architecture("linux")

if("${LIB_GPIO_ARCHITECTURE}" STREQUAL "linux")
    lib_gpio_add_dependencies(EMBTOM::osbase EMBTOM::iotty)
    lib_gpio_add_sourcefile_c(src/gpio_access_linux.c src/gpio_event_linux.c)
    lib_gpio_add_header(include)
    lib_gpio_add_public_definition(LIB_GPIO_LINUX)
endif()
