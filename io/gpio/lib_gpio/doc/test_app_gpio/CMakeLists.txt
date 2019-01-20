project(test_app_gpio)

cmake_minimum_required(VERSION 2.6)

find_package(iotty REQUIRED)
find_package(osbase REQUIRED)

######################################################################################
# Build configurator
######################################################################################
SET(LIB_GPIO_SOURCE_C test_gpio.c)

######################################
#Build target settings
######################################################


add_executable(${PROJECT_NAME} ${LIB_GPIO_SOURCE_C})
target_link_libraries(${PROJECT_NAME} EMBTOM::osbase EMBTOM::iotty lib_gpio)
#target_compile_definitions(${PROJECT_NAME} PRIVATE ${LIB_GPIO_PUBLIC_DEFINITION})
#Generate the static library from the sources-
#add_library(${PROJECT_NAME} STATIC ${SOURCES})
#target_link_libraries(${PROJECT_NAME} lib_convention lib_log lib_stm32hal lib_gpio_STM32F1 lib_isr)
#target_include_directories(${PROJECT_NAME} PUBLIC ${HEADER_FILES})
